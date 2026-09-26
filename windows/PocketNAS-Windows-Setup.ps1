# PocketNAS v3 Windows Auto Setup
# Creates a reliable large-file PocketNAS drive using rclone + WinFsp.
# This avoids the size/cache limits of Windows' built-in WebClient redirector.

param(
    [string]$InitialUrl = "",
    [string]$PreferredDrive = "P:"
)

$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"

function Write-Step([string]$Text) {
    Write-Host "[PocketNAS] $Text" -ForegroundColor Cyan
}

function Test-Administrator {
    $id = [Security.Principal.WindowsIdentity]::GetCurrent()
    $p = [Security.Principal.WindowsPrincipal]::new($id)
    return $p.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

if (-not (Test-Administrator)) {
    Write-Step "Administrator permission is required once to install the Windows drive components."
    $args = "-NoProfile -ExecutionPolicy Bypass -File `"$PSCommandPath`" -InitialUrl `"$InitialUrl`" -PreferredDrive `"$PreferredDrive`""
    Start-Process powershell.exe -Verb RunAs -ArgumentList $args
    exit
}

Write-Host ""
Write-Host "PocketNAS v3 - Windows Auto Setup" -ForegroundColor Green
Write-Host "This setup installs WinFsp and rclone, creates an automatic PocketNAS drive," 
Write-Host "and configures Windows WebDAV as a compatibility fallback."
Write-Host ""

# 1) Keep Windows native WebDAV usable as a fallback.
Write-Step "Configuring Windows WebClient fallback settings..."
$webClientKey = "HKLM:\SYSTEM\CurrentControlSet\Services\WebClient\Parameters"
New-ItemProperty -Path $webClientKey -Name FileSizeLimitInBytes -PropertyType DWord -Value 4294967295 -Force | Out-Null
New-ItemProperty -Path $webClientKey -Name SendReceiveTimeoutInSec -PropertyType DWord -Value 600 -Force | Out-Null
New-ItemProperty -Path $webClientKey -Name LocalServerTimeoutInSec -PropertyType DWord -Value 120 -Force | Out-Null
New-ItemProperty -Path $webClientKey -Name FileAttributesLimitInBytes -PropertyType DWord -Value 20000000 -Force | Out-Null
try { Restart-Service WebClient -Force -ErrorAction Stop } catch { }

# 2) Install large-file drive prerequisites.
if (-not (Get-Command winget.exe -ErrorAction SilentlyContinue)) {
    throw "Windows Package Manager (winget) was not found. Update/install 'App Installer' from Microsoft Store, then run this setup again."
}

Write-Step "Installing/updating WinFsp..."
winget install -e --id WinFsp.WinFsp --accept-package-agreements --accept-source-agreements --silent | Out-Host
Write-Step "Installing/updating rclone..."
winget install -e --id Rclone.Rclone --accept-package-agreements --accept-source-agreements --silent | Out-Host

# Refresh PATH after winget installs.
$env:Path = [Environment]::GetEnvironmentVariable("Path", "Machine") + ";" + [Environment]::GetEnvironmentVariable("Path", "User")
$rclone = (Get-Command rclone.exe -ErrorAction SilentlyContinue).Source
if (-not $rclone) {
    $candidate = Join-Path $env:LOCALAPPDATA "Microsoft\WinGet\Links\rclone.exe"
    if (Test-Path $candidate) { $rclone = $candidate }
}
if (-not $rclone) { throw "rclone was installed but rclone.exe could not be located. Sign out/in or reboot Windows, then run the setup again." }

$baseDir = Join-Path $env:LOCALAPPDATA "PocketNAS"
New-Item -ItemType Directory -Path $baseDir -Force | Out-Null
$configFile = Join-Path $baseDir "rclone.conf"
$watcherFile = Join-Path $baseDir "PocketNAS-AutoMount.ps1"
$logFile = Join-Path $baseDir "rclone.log"
$settingsFile = Join-Path $baseDir "settings.txt"

function Normalize-Url([string]$Url) {
    if (-not $Url) { return $null }
    $u = $Url.Trim()
    if (-not $u.EndsWith('/')) { $u += '/' }
    return $u
}

function Get-PocketNasStatus([string]$Url, [int]$TimeoutSec = 2) {
    try {
        $u = (Normalize-Url $Url) + "api/status"
        $r = Invoke-RestMethod -Uri $u -TimeoutSec $TimeoutSec -UseBasicParsing
        if ($r.app -eq "PocketNAS") { return $r }
    } catch { }
    return $null
}

function Test-Port8080([string]$Ip, [int]$TimeoutMs = 80) {
    $c = [Net.Sockets.TcpClient]::new()
    try {
        $ar = $c.BeginConnect($Ip, 8080, $null, $null)
        if (-not $ar.AsyncWaitHandle.WaitOne($TimeoutMs)) { return $false }
        $c.EndConnect($ar)
        return $true
    } catch { return $false }
    finally { $c.Dispose() }
}

function Find-PocketNas([string]$FirstUrl, [string]$WantedDeviceId = "") {
    $first = Normalize-Url $FirstUrl
    if ($first) {
        $s = Get-PocketNasStatus $first 1
        if ($s -and ((-not $WantedDeviceId) -or $s.deviceId -eq $WantedDeviceId)) {
            return [pscustomobject]@{ Url = $first; Status = $s }
        }
    }

    # Fast candidates from the ARP cache.
    $seen = @{}
    $arpText = (arp -a 2>$null) -join "`n"
    foreach ($m in [regex]::Matches($arpText, '(?m)\b(?:10|192\.168|172\.(?:1[6-9]|2\d|3[01]))(?:\.\d{1,3}){2}\b')) {
        $ip = $m.Value
        if ($seen[$ip]) { continue }
        $seen[$ip] = $true
        if (Test-Port8080 $ip 120) {
            $u = "http://$ip`:8080/"
            $s = Get-PocketNasStatus $u 1
            if ($s -and ((-not $WantedDeviceId) -or $s.deviceId -eq $WantedDeviceId)) {
                return [pscustomobject]@{ Url = $u; Status = $s }
            }
        }
    }

    # Fallback: scan local /24 networks. Port probe is deliberately short.
    $prefixes = Get-NetIPAddress -AddressFamily IPv4 -ErrorAction SilentlyContinue |
        Where-Object { $_.IPAddress -notmatch '^(127\.|169\.254\.)' } |
        ForEach-Object {
            $p = $_.IPAddress.Split('.')
            if ($p.Count -eq 4) { "$($p[0]).$($p[1]).$($p[2])" }
        } | Sort-Object -Unique

    foreach ($prefix in $prefixes) {
        foreach ($n in 1..254) {
            $ip = "$prefix.$n"
            if ($seen[$ip]) { continue }
            if (-not (Test-Port8080 $ip 45)) { continue }
            $u = "http://$ip`:8080/"
            $s = Get-PocketNasStatus $u 1
            if ($s -and ((-not $WantedDeviceId) -or $s.deviceId -eq $WantedDeviceId)) {
                return [pscustomobject]@{ Url = $u; Status = $s }
            }
        }
    }
    return $null
}

Write-Step "Finding PocketNAS on the local network..."
$found = Find-PocketNas $InitialUrl
if (-not $found) { throw "PocketNAS was not found. Make sure the phone and PC are on the same Wi-Fi and PocketNAS shows SERVER: RUNNING." }
$deviceId = [string]$found.Status.deviceId
$serverUrl = [string]$found.Url
Write-Host "Found PocketNAS $deviceId at $serverUrl" -ForegroundColor Green

# Choose a free drive letter.
$drive = $PreferredDrive.ToUpper()
if ($drive -notmatch '^[D-Z]:$') { $drive = 'P:' }
if (Get-PSDrive -Name $drive.Substring(0,1) -ErrorAction SilentlyContinue) {
    foreach ($letter in @('P','Q','R','S','T','U','V','W','X','Y','Z')) {
        if (-not (Get-PSDrive -Name $letter -ErrorAction SilentlyContinue)) { $drive = "$letter`:"; break }
    }
}

# Optional PocketNAS Digest credentials.
$userLine = ""
$passLine = ""
if ([bool]$found.Status.auth) {
    Write-Host "PocketNAS authentication is ON." -ForegroundColor Yellow
    $secure = Read-Host "Enter the permanent PocketNAS password" -AsSecureString
    $ptr = [Runtime.InteropServices.Marshal]::SecureStringToBSTR($secure)
    try { $plain = [Runtime.InteropServices.Marshal]::PtrToStringBSTR($ptr) }
    finally { [Runtime.InteropServices.Marshal]::ZeroFreeBSTR($ptr) }
    $obscured = (& $rclone obscure $plain).Trim()
    $plain = $null
    $userLine = "user = pocketnas"
    $passLine = "pass = $obscured"
}

function Write-RcloneConfig([string]$Url) {
    $lines = @(
        '[pocketnas]',
        'type = webdav',
        "url = $Url",
        'vendor = other'
    )
    if ($userLine) { $lines += $userLine }
    if ($passLine) { $lines += $passLine }
    Set-Content -Path $configFile -Value $lines -Encoding ASCII
}
Write-RcloneConfig $serverUrl

# Save values used by the background watcher.
@(
    "INITIAL_URL=$serverUrl",
    "DEVICE_ID=$deviceId",
    "DRIVE=$drive",
    "RCLONE=$rclone",
    "CONFIG=$configFile",
    "LOG=$logFile"
) | Set-Content -Path $settingsFile -Encoding UTF8

$watcherTemplate = @'
$ErrorActionPreference = "SilentlyContinue"
$settings = @{}
Get-Content "@@SETTINGS@@" | ForEach-Object {
    $p = $_.IndexOf('=')
    if ($p -gt 0) { $settings[$_.Substring(0,$p)] = $_.Substring($p+1) }
}
$InitialUrl = $settings['INITIAL_URL']
$DeviceId   = $settings['DEVICE_ID']
$Drive      = $settings['DRIVE']
$Rclone     = $settings['RCLONE']
$Config     = $settings['CONFIG']
$Log        = $settings['LOG']

function Normalize-Url([string]$Url) { if(!$Url){return $null}; $u=$Url.Trim(); if(!$u.EndsWith('/')){$u+='/'}; return $u }
function Get-Status([string]$Url) {
    try { $r=Invoke-RestMethod -Uri ((Normalize-Url $Url)+'api/status') -TimeoutSec 1 -UseBasicParsing; if($r.app -eq 'PocketNAS' -and $r.deviceId -eq $DeviceId){return $r} } catch {}
    return $null
}
function Test-Port([string]$Ip,[int]$Ms=60){$c=[Net.Sockets.TcpClient]::new();try{$a=$c.BeginConnect($Ip,8080,$null,$null);if(!$a.AsyncWaitHandle.WaitOne($Ms)){return $false};$c.EndConnect($a);return $true}catch{return $false}finally{$c.Dispose()}}
function Find-Server {
    if(Get-Status $InitialUrl){return (Normalize-Url $InitialUrl)}
    $arp=(arp -a 2>$null)-join "`n"; $seen=@{}
    foreach($m in [regex]::Matches($arp,'(?m)\b(?:10|192\.168|172\.(?:1[6-9]|2\d|3[01]))(?:\.\d{1,3}){2}\b')){$ip=$m.Value;if($seen[$ip]){continue};$seen[$ip]=$true;if(Test-Port $ip 100){$u="http://$ip`:8080/";if(Get-Status $u){return $u}}}
    $prefixes=Get-NetIPAddress -AddressFamily IPv4 -ErrorAction SilentlyContinue|?{$_.IPAddress -notmatch '^(127\.|169\.254\.)'}|%{$p=$_.IPAddress.Split('.');if($p.Count -eq 4){"$($p[0]).$($p[1]).$($p[2])"}}|sort -Unique
    foreach($pre in $prefixes){foreach($n in 1..254){$ip="$pre.$n";if($seen[$ip]){continue};if(Test-Port $ip 40){$u="http://$ip`:8080/";if(Get-Status $u){return $u}}}}
    return $null
}
function Update-Config([string]$Url){$c=Get-Content $Config; $c=$c|%{if($_ -like 'url = *'){"url = $Url"}else{$_}}; Set-Content -Path $Config -Value $c -Encoding ASCII}
$rcloneProc=$null; $currentUrl=''
while($true){
    $url=Find-Server
    if($url){
        if($currentUrl -ne $url -or !$rcloneProc -or $rcloneProc.HasExited){
            if($rcloneProc -and !$rcloneProc.HasExited){try{$rcloneProc.Kill()}catch{};Start-Sleep -Seconds 2}
            Update-Config $url
            $args=@('mount','pocketnas:',$Drive,'--config',$Config,'--vfs-cache-mode','writes','--vfs-cache-max-age','1h','--dir-cache-time','5s','--poll-interval','0','--network-mode','--log-file',$Log,'--log-level','INFO')
            $rcloneProc=Start-Process -FilePath $Rclone -ArgumentList $args -WindowStyle Hidden -PassThru
            $currentUrl=$url
            Start-Sleep -Seconds 5
        }
    }
    Start-Sleep -Seconds 15
}
'@
$watcher = $watcherTemplate.Replace('@@SETTINGS@@', $settingsFile.Replace("'", "''"))
Set-Content -Path $watcherFile -Value $watcher -Encoding UTF8

# Start the watcher automatically whenever this Windows user signs in.
$startup = [Environment]::GetFolderPath('Startup')
$launcher = Join-Path $startup "PocketNAS Auto Mount.cmd"
$cmd = '@echo off' + "`r`n" + 'start "" /min powershell.exe -NoProfile -ExecutionPolicy Bypass -WindowStyle Hidden -File "' + $watcherFile + '"' + "`r`n"
Set-Content -Path $launcher -Value $cmd -Encoding ASCII

# Stop any previous watcher from an older setup, then launch the new one.
Get-CimInstance Win32_Process -Filter "Name='powershell.exe'" -ErrorAction SilentlyContinue |
    Where-Object { $_.CommandLine -like "*$watcherFile*" } |
    ForEach-Object { try { Stop-Process -Id $_.ProcessId -Force } catch {} }
Start-Process powershell.exe -ArgumentList @('-NoProfile','-ExecutionPolicy','Bypass','-WindowStyle','Hidden','-File',$watcherFile) -WindowStyle Hidden

Write-Host ""
Write-Host "PocketNAS Windows Drive setup is complete." -ForegroundColor Green
Write-Host "Drive letter: $drive"
Write-Host "The drive will auto-discover this phone and reconnect after Windows sign-in or phone IP changes."
Write-Host "Large transfers use rclone/WinFsp instead of the Windows WebClient redirector."
Write-Host ""
Write-Step "Waiting for the drive to appear..."
for($i=0;$i -lt 20;$i++){
    Start-Sleep -Seconds 1
    if(Test-Path "$drive\") { Start-Process explorer.exe "$drive\"; break }
}
if(-not (Test-Path "$drive\")){
    Write-Host "The drive did not appear yet. WinFsp may require a Windows restart after first installation." -ForegroundColor Yellow
    Write-Host "After restarting Windows, PocketNAS Auto Mount will start automatically."
}
Read-Host "Press Enter to close"
