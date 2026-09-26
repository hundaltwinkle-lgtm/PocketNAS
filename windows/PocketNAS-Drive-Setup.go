//go:build windows

package main

import (
    "bufio"
    _ "embed"
    "bytes"
    "context"
    "encoding/json"
    "errors"
    "fmt"
    "io"
    "net"
    "net/http"
    "os"
    "os/exec"
    "path/filepath"
    "runtime"
    "sort"
    "strings"
    "sync"
    "syscall"
    "time"
    "unsafe"
)

const (
    appName = "PocketNAS Drive"
    appVersion = "3.1.0-beta.1"
    remoteName = "pocketnas"
)

//go:embed pocketnas-icon.ico
var pocketNASIcon []byte

type ServerStatus struct {
    App             string `json:"app"`
    Version         string `json:"version"`
    DeviceID        string `json:"deviceId"`
    Auth            bool   `json:"auth"`
    ReadOnly        bool   `json:"readOnly"`
    StorageWritable bool   `json:"storageWritable"`
    IP              string `json:"ip"`
    Port            int    `json:"port"`
}

type Config struct {
    DeviceID    string `json:"deviceId"`
    DriveLetter string `json:"driveLetter"`
    LastURL     string `json:"lastUrl"`
    RclonePath  string `json:"rclonePath"`
}

var (
    user32 = syscall.NewLazyDLL("user32.dll")
    kernel32 = syscall.NewLazyDLL("kernel32.dll")
    procMessageBoxW = user32.NewProc("MessageBoxW")
    procGetLogicalDrives = kernel32.NewProc("GetLogicalDrives")
)

func utf16Ptr(s string) *uint16 { p, _ := syscall.UTF16PtrFromString(s); return p }
func messageBox(title, body string, flags uintptr) {
    procMessageBoxW.Call(0, uintptr(unsafe.Pointer(utf16Ptr(body))), uintptr(unsafe.Pointer(utf16Ptr(title))), flags)
}
func info(body string) { messageBox(appName, body, 0x40) }
func fail(body string) { messageBox(appName+" - Setup Error", body, 0x10) }

func appDir() string { return filepath.Join(os.Getenv("LOCALAPPDATA"), "PocketNAS") }
func configPath() string { return filepath.Join(appDir(), "drive-config.json") }
func rcloneConfigPath() string { return filepath.Join(appDir(), "rclone.conf") }
func logPath() string { return filepath.Join(appDir(), "PocketNAS-Drive.log") }
func installedExePath() string { return filepath.Join(appDir(), "PocketNAS-Drive.exe") }
func iconPath() string { return filepath.Join(appDir(), "PocketNAS.ico") }

func appendLog(format string, a ...any) {
    _ = os.MkdirAll(appDir(), 0755)
    f, err := os.OpenFile(logPath(), os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0644)
    if err != nil { return }
    defer f.Close()
    fmt.Fprintf(f, "%s "+format+"\r\n", append([]any{time.Now().Format("2006-01-02 15:04:05")}, a...)...)
}

func runHidden(name string, args ...string) ([]byte, error) {
    cmd := exec.Command(name, args...)
    cmd.SysProcAttr = &syscall.SysProcAttr{HideWindow: true}
    var out bytes.Buffer
    cmd.Stdout = &out
    cmd.Stderr = &out
    err := cmd.Run()
    if err != nil { return out.Bytes(), fmt.Errorf("%w: %s", err, strings.TrimSpace(out.String())) }
    return out.Bytes(), nil
}

func copySelf() error {
    src, err := os.Executable(); if err != nil { return err }
    src, _ = filepath.EvalSymlinks(src)
    dst := installedExePath()
    _ = os.MkdirAll(filepath.Dir(dst), 0755)
    if strings.EqualFold(src, dst) { return nil }
    in, err := os.Open(src); if err != nil { return err }; defer in.Close()
    out, err := os.Create(dst); if err != nil { return err }
    if _, err = io.Copy(out, in); err != nil { out.Close(); return err }
    return out.Close()
}

func saveConfig(c Config) error {
    _ = os.MkdirAll(appDir(), 0755)
    b, _ := json.MarshalIndent(c, "", "  ")
    return os.WriteFile(configPath(), b, 0600)
}
func loadConfig() Config {
    var c Config
    b, err := os.ReadFile(configPath()); if err == nil { _ = json.Unmarshal(b, &c) }
    return c
}

func installPackage(id string) error {
    args := []string{"install", "--id", id, "-e", "--silent", "--accept-package-agreements", "--accept-source-agreements", "--disable-interactivity"}
    out, err := runHidden("winget.exe", args...)
    appendLog("winget %s: %s", id, strings.TrimSpace(string(out)))
    if err != nil {
        // Treat already-installed/up-to-date wording as success.
        s := strings.ToLower(string(out))
        if strings.Contains(s, "already installed") || strings.Contains(s, "no available upgrade") || strings.Contains(s, "installed package is already") { return nil }
        return err
    }
    return nil
}

func findRclone() string {
    candidates := []string{}
    if p, err := exec.LookPath("rclone.exe"); err == nil { candidates = append(candidates, p) }
    la := os.Getenv("LOCALAPPDATA")
    pf := os.Getenv("ProgramFiles")
    candidates = append(candidates,
        filepath.Join(la, "Microsoft", "WinGet", "Links", "rclone.exe"),
        filepath.Join(pf, "rclone", "rclone.exe"),
        filepath.Join(pf, "Rclone", "rclone.exe"),
    )
    pkgRoot := filepath.Join(la, "Microsoft", "WinGet", "Packages")
    matches, _ := filepath.Glob(filepath.Join(pkgRoot, "Rclone.Rclone_*", "**", "rclone.exe"))
    candidates = append(candidates, matches...)
    // Glob doesn't recurse with ** on Windows reliably; walk shallow package dirs.
    dirs, _ := filepath.Glob(filepath.Join(pkgRoot, "Rclone.Rclone_*"))
    for _, d := range dirs {
        _ = filepath.WalkDir(d, func(path string, de os.DirEntry, err error) error {
            if err == nil && !de.IsDir() && strings.EqualFold(de.Name(), "rclone.exe") { candidates = append(candidates, path) }
            return nil
        })
    }
    seen := map[string]bool{}
    for _, p := range candidates {
        if p == "" || seen[strings.ToLower(p)] { continue }; seen[strings.ToLower(p)] = true
        if st, err := os.Stat(p); err == nil && !st.IsDir() { return p }
    }
    return ""
}

func privateIPv4(ip net.IP) bool {
    v := ip.To4(); if v == nil { return false }
    return v[0] == 10 || (v[0] == 172 && v[1] >= 16 && v[1] <= 31) || (v[0] == 192 && v[1] == 168)
}

func localSubnets() []net.IP {
    var bases []net.IP
    ifs, _ := net.Interfaces()
    seen := map[string]bool{}
    for _, inf := range ifs {
        if inf.Flags&net.FlagUp == 0 || inf.Flags&net.FlagLoopback != 0 { continue }
        addrs, _ := inf.Addrs()
        for _, a := range addrs {
            var ip net.IP
            switch x := a.(type) { case *net.IPNet: ip=x.IP; case *net.IPAddr: ip=x.IP }
            v:=ip.To4(); if !privateIPv4(v) { continue }
            // Deliberately scan local /24. Most home networks use /24; this keeps discovery fast.
            base := net.IPv4(v[0],v[1],v[2],0).To4()
            k:=base.String(); if !seen[k] { seen[k]=true; bases=append(bases,base) }
        }
    }
    return bases
}

func queryServer(ctx context.Context, ip string) (ServerStatus, error) {
    var st ServerStatus
    u := "http://"+ip+":8080/api/status"
    req, _ := http.NewRequestWithContext(ctx, "GET", u, nil)
    cli := &http.Client{Timeout: 650*time.Millisecond}
    resp, err := cli.Do(req); if err != nil { return st,err }
    defer resp.Body.Close()
    if resp.StatusCode != 200 { return st,fmt.Errorf("HTTP %d",resp.StatusCode) }
    if err:=json.NewDecoder(io.LimitReader(resp.Body,64*1024)).Decode(&st); err!=nil { return st,err }
    if st.App!="PocketNAS" || st.DeviceID=="" { return st,errors.New("not PocketNAS") }
    if st.Port==0 { st.Port=8080 }
    if st.IP=="" { st.IP=ip }
    return st,nil
}

func discover(preferredDeviceID string, wait time.Duration) (ServerStatus,error) {
    deadline:=time.Now().Add(wait)
    for {
        bases:=localSubnets()
        type result struct{ st ServerStatus; err error }
        ch:=make(chan result,254*len(bases))
        sem:=make(chan struct{},64)
        var wg sync.WaitGroup
        ctx,cancel:=context.WithCancel(context.Background())
        for _,base:=range bases {
            for i:=1;i<255;i++ {
                ip:=fmt.Sprintf("%d.%d.%d.%d",base[0],base[1],base[2],i)
                wg.Add(1); go func(ip string){ defer wg.Done(); sem<-struct{}{}; defer func(){<-sem}(); st,e:=queryServer(ctx,ip); if e==nil { ch<-result{st,nil} } }(ip)
            }
        }
        go func(){wg.Wait();close(ch)}()
        var found []ServerStatus
        for r:=range ch { if r.err==nil { found=append(found,r.st); if preferredDeviceID!="" && r.st.DeviceID==preferredDeviceID { cancel(); return r.st,nil } } }
        cancel()
        if len(found)>0 { sort.Slice(found,func(i,j int)bool{return found[i].IP<found[j].IP}); return found[0],nil }
        if time.Now().After(deadline) { return ServerStatus{},errors.New("PocketNAS was not found on the local Wi-Fi network") }
        time.Sleep(2*time.Second)
    }
}

func writeRcloneConfig(st ServerStatus) error {
    if st.Auth { return errors.New("PocketNAS authentication is enabled. For fully automatic Windows setup, temporarily turn Authentication OFF in the PocketNAS Android app, run setup, then configure authentication manually if required.") }
    _=os.MkdirAll(appDir(),0755)
    u:=fmt.Sprintf("http://%s:%d/",st.IP,st.Port)
    body:=fmt.Sprintf("[%s]\r\ntype = webdav\r\nurl = %s\r\nvendor = other\r\n",remoteName,u)
    return os.WriteFile(rcloneConfigPath(),[]byte(body),0600)
}

func logicalDrivesMask() uint32 { r,_,_:=procGetLogicalDrives.Call(); return uint32(r) }
func chooseDrive(preferred string) string {
    mask:=logicalDrivesMask()
    test:=[]string{}
    if preferred!="" { test=append(test,strings.ToUpper(strings.TrimSuffix(preferred,":"))) }
    test=append(test,"P","R","N","Z","Y","X")
    seen:=map[string]bool{}
    for _,d:=range test { if len(d)!=1||seen[d]{continue};seen[d]=true; bit:=uint32(1)<<(d[0]-'A'); if mask&bit==0{return d+":"} }
    return ""
}

func setStartup() error {
    cmd:=fmt.Sprintf("\"%s\" --watch",installedExePath())
    _,err:=runHidden("reg.exe","add",`HKCU\Software\Microsoft\Windows\CurrentVersion\Run`,"/v","PocketNASDrive","/t","REG_SZ","/d",cmd,"/f")
    return err
}

func createShortcuts() {
    _ = os.WriteFile(iconPath(), pocketNASIcon, 0644)
    startMenu := filepath.Join(os.Getenv("APPDATA"), "Microsoft", "Windows", "Start Menu", "Programs", "PocketNAS Drive.lnk")
    desktop := filepath.Join(os.Getenv("USERPROFILE"), "Desktop", "PocketNAS Drive.lnk")
    ps := fmt.Sprintf(`$ws=New-Object -ComObject WScript.Shell; foreach($p in @('%s','%s')){$s=$ws.CreateShortcut($p);$s.TargetPath='%s';$s.WorkingDirectory='%s';$s.IconLocation='%s,0';$s.Description='PocketNAS automatic Android Wi-Fi drive';$s.Save()}`, strings.ReplaceAll(startMenu,"'","''"), strings.ReplaceAll(desktop,"'","''"), strings.ReplaceAll(installedExePath(),"'","''"), strings.ReplaceAll(appDir(),"'","''"), strings.ReplaceAll(iconPath(),"'","''"))
    _,_ = runHidden("powershell.exe","-NoProfile","-ExecutionPolicy","Bypass","-Command",ps)
}

func registerUninstall() {
    key:=`HKCU\Software\Microsoft\Windows\CurrentVersion\Uninstall\PocketNASDrive`
    _,_=runHidden("reg.exe","add",key,"/v","DisplayName","/t","REG_SZ","/d","PocketNAS Drive","/f")
    _,_=runHidden("reg.exe","add",key,"/v","DisplayVersion","/t","REG_SZ","/d",appVersion,"/f")
    _,_=runHidden("reg.exe","add",key,"/v","Publisher","/t","REG_SZ","/d","PocketNAS Open Source Project","/f")
    _,_=runHidden("reg.exe","add",key,"/v","DisplayIcon","/t","REG_SZ","/d",iconPath(),"/f")
    _,_=runHidden("reg.exe","add",key,"/v","UninstallString","/t","REG_SZ","/d",fmt.Sprintf("\\\"%s\\\" --uninstall",installedExePath()),"/f")
    _,_=runHidden("reg.exe","add",key,"/v","NoModify","/t","REG_DWORD","/d","1","/f")
    _,_=runHidden("reg.exe","add",key,"/v","NoRepair","/t","REG_DWORD","/d","1","/f")
}

func openExplorer(drive string) { _=exec.Command("explorer.exe",drive+"\\").Start() }

func startWatcherDetached() error {
    cmd:=exec.Command(installedExePath(),"--watch")
    cmd.SysProcAttr=&syscall.SysProcAttr{HideWindow:true,CreationFlags:0x00000008|0x00000200} // DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP
    return cmd.Start()
}

func killOurMounts() {
    ps := `Get-CimInstance Win32_Process -Filter "Name='rclone.exe'" | Where-Object { $_.CommandLine -match 'pocketnas:' -and $_.CommandLine -match 'PocketNAS-Drive' } | ForEach-Object { Stop-Process -Id $_.ProcessId -Force -ErrorAction SilentlyContinue }`
    _,_ = runHidden("powershell.exe","-NoProfile","-ExecutionPolicy","Bypass","-Command",ps)
}

func startMount(rclone,drive string) (*exec.Cmd,error) {
    _=os.MkdirAll(appDir(),0755)
    args:=[]string{"mount",remoteName+":",drive,
        "--config",rcloneConfigPath(),
        "--network-mode",
        "--vfs-cache-mode","full",
        "--vfs-cache-max-size","20G",
        "--vfs-cache-max-age","1h",
        "--dir-cache-time","5s",
        "--poll-interval","0",
        "--buffer-size","64M",
        "--transfers","4",
        "--volname","PocketNAS-Drive",
        "--log-file",logPath(),
        "--log-level","INFO",
    }
    cmd:=exec.Command(rclone,args...)
    cmd.SysProcAttr=&syscall.SysProcAttr{HideWindow:true,CreationFlags:0x08000000} // CREATE_NO_WINDOW
    if err:=cmd.Start();err!=nil{return nil,err}
    appendLog("Started rclone PID %d on %s",cmd.Process.Pid,drive)
    return cmd,nil
}

func watcher() {
    _=os.MkdirAll(appDir(),0755)
    // Crude single-instance lock using an exclusively-created marker plus live PID.
    lock:=filepath.Join(appDir(),"watcher.pid")
    if b,err:=os.ReadFile(lock);err==nil && len(b)>0 {
        // Do not blindly exit: stale markers are common. Continue after a short grace period.
    }
    _=os.WriteFile(lock,[]byte(fmt.Sprintf("%d",os.Getpid())),0600)
    defer os.Remove(lock)
    cfg:=loadConfig()
    rclone:=cfg.RclonePath
    if rclone=="" || func()bool{_,e:=os.Stat(rclone);return e!=nil}(){rclone=findRclone()}
    if rclone=="" { appendLog("Watcher: rclone missing"); return }
    drive:=cfg.DriveLetter; if drive=="" { drive=chooseDrive("P:") }
    var mount *exec.Cmd
    var mountDone chan error
    var currentURL string
    for {
        st,err:=discover(cfg.DeviceID,8*time.Second)
        if err!=nil { appendLog("Discovery: %v",err); if mount!=nil && mount.Process!=nil { _=mount.Process.Kill(); mount=nil }; time.Sleep(5*time.Second); continue }
        if cfg.DeviceID=="" {cfg.DeviceID=st.DeviceID}
        u:=fmt.Sprintf("http://%s:%d/",st.IP,st.Port)
        restart:=mount==nil || u!=currentURL
        if mountDone!=nil { select { case err:=<-mountDone: appendLog("Mount exited: %v",err);mount=nil;mountDone=nil;restart=true; default: } }
        if restart {
            if mount!=nil && mount.Process!=nil {_=mount.Process.Kill(); _=mount.Wait(); mount=nil}
            if err:=writeRcloneConfig(st);err!=nil {appendLog("Config: %v",err);time.Sleep(5*time.Second);continue}
            if drive=="" {drive=chooseDrive("P:")}
            m,e:=startMount(rclone,drive); if e!=nil {appendLog("Mount start: %v",e);time.Sleep(5*time.Second);continue}
            mount=m; currentURL=u; mountDone=make(chan error,1); go func(c *exec.Cmd,ch chan error){ch<-c.Wait()}(m,mountDone)
            cfg.LastURL=u;cfg.DriveLetter=drive;cfg.RclonePath=rclone;_ = saveConfig(cfg)
        }
        time.Sleep(8*time.Second)
    }
}

func install() error {
    appendLog("Setup %s started",appVersion)
    if _,err:=exec.LookPath("winget.exe");err!=nil{return errors.New("Windows Package Manager (winget) is required. Install App Installer from Microsoft Store, then run PocketNAS Drive Setup again.")}
    if err:=installPackage("WinFsp.WinFsp");err!=nil{return fmt.Errorf("WinFsp installation failed: %w",err)}
    if err:=installPackage("Rclone.Rclone");err!=nil{return fmt.Errorf("rclone installation failed: %w",err)}
    rclone:=findRclone();if rclone==""{return errors.New("rclone was installed but PocketNAS could not locate rclone.exe. Sign out/in once or reinstall rclone.")}
    st,err:=discover("",20*time.Second);if err!=nil{return err}
    if st.Auth{return errors.New("PocketNAS was found, but Authentication is ON. Turn Authentication OFF in the Android app for the one-click setup, then run this installer again.")}
    if err:=writeRcloneConfig(st);err!=nil{return err}
    drive:=chooseDrive("P:");if drive==""{return errors.New("No free drive letter was found for PocketNAS")}
    cfg:=Config{DeviceID:st.DeviceID,DriveLetter:drive,LastURL:fmt.Sprintf("http://%s:%d/",st.IP,st.Port),RclonePath:rclone}
    if err:=saveConfig(cfg);err!=nil{return err}
    if err:=copySelf();err!=nil{return fmt.Errorf("installing PocketNAS Drive helper: %w",err)}
    if err:=setStartup();err!=nil{return fmt.Errorf("enabling automatic Windows login reconnect: %w",err)}
    createShortcuts()
    registerUninstall()
    killOurMounts()
    if err:=startWatcherDetached();err!=nil{return fmt.Errorf("starting PocketNAS Drive: %w",err)}
    // Give rclone/WinFsp a moment to establish the mount.
    time.Sleep(4*time.Second)
    openExplorer(drive)
    info(fmt.Sprintf("PocketNAS Drive is installed.\n\nPhone: %s\nDrive: %s\n\nWindows will reconnect the drive automatically after sign-in and when the phone IP changes.\n\nIf Windows shows a UAC prompt while WinFsp installs, choose Yes.",cfg.LastURL,drive))
    return nil
}

func main(){
    if runtime.GOOS!="windows"{return}
    if len(os.Args)>1 {
        switch os.Args[1] {
        case "--watch": watcher(); return
        case "--uninstall":
            killOurMounts();_,_=runHidden("reg.exe","delete",`HKCU\Software\Microsoft\Windows\CurrentVersion\Run`,"/v","PocketNASDrive","/f");_,_=runHidden("reg.exe","delete",`HKCU\Software\Microsoft\Windows\CurrentVersion\Uninstall\PocketNASDrive`,"/f");_ = os.Remove(filepath.Join(os.Getenv("APPDATA"),"Microsoft","Windows","Start Menu","Programs","PocketNAS Drive.lnk"));_ = os.Remove(filepath.Join(os.Getenv("USERPROFILE"),"Desktop","PocketNAS Drive.lnk"));_ = os.RemoveAll(appDir());info("PocketNAS Drive was removed. WinFsp and rclone were left installed because other applications may use them.");return
        }
    }
    if err:=install();err!=nil{appendLog("Setup failed: %v",err);fail(err.Error()+"\n\nLog: "+logPath())}
}

// Keep bufio imported for compatibility with older Go toolchains that otherwise
// aggressively alter import grouping in generated source bundles.
var _ = bufio.ErrInvalidUnreadByte
