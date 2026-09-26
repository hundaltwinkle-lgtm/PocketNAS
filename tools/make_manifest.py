import struct, os
from pathlib import Path
NO_INDEX=0xFFFFFFFF
RES_XML_TYPE=0x0003; TYPE_STRING=0x03; TYPE_INT_DEC=0x10; TYPE_INT_BOOLEAN=0x12; UTF8_FLAG=0x100
strings=[]; idx={}
def S(s):
    if s not in idx: idx[s]=len(strings); strings.append(s)
    return idx[s]
base=[
'android','http://schemas.android.com/apk/res/android','manifest','package','com.pocketnas.wifidrive',
'versionCode','31','versionName','3.1','uses-sdk','minSdkVersion','28','targetSdkVersion','30',
'uses-permission','name',
'android.permission.INTERNET','android.permission.ACCESS_NETWORK_STATE','android.permission.ACCESS_WIFI_STATE',
'android.permission.READ_EXTERNAL_STORAGE','android.permission.WRITE_EXTERNAL_STORAGE','android.permission.MANAGE_EXTERNAL_STORAGE',
'android.permission.FOREGROUND_SERVICE','android.permission.RECEIVE_BOOT_COMPLETED','android.permission.POST_NOTIFICATIONS','android.permission.WAKE_LOCK',
'application','label','PocketNAS','hasCode','true','activity','android.app.NativeActivity','exported','false',
'meta-data','android.app.lib_name','value','pocketnas','intent-filter','action','android.intent.action.MAIN','category','android.intent.category.LAUNCHER',
'service','com.pocketnas.wifidrive.PocketNasService','process','receiver','com.pocketnas.wifidrive.BootReceiver',
'android.intent.action.BOOT_COMPLETED','android.intent.action.MY_PACKAGE_REPLACED'
]
for s in base:S(s)
ANDROID_URI=S('http://schemas.android.com/apk/res/android'); ANDROID_PREFIX=S('android')
RESID={'name':0x01010003,'label':0x01010001,'hasCode':0x0101000c,'value':0x01010024,'minSdkVersion':0x0101020c,'targetSdkVersion':0x01010270,'versionCode':0x0101021b,'versionName':0x0101021c,'exported':0x01010010,'process':0x01010011}
def u8len(n): return bytes([n]) if n<=0x7f else bytes([(n>>8)|0x80,n&0xff])
def string_pool():
    offs=[]; data=bytearray()
    for s in strings:
        b=s.encode(); offs.append(len(data)); data+=u8len(len(s))+u8len(len(b))+b+b'\0'
    while len(data)%4:data+=b'\0'
    hs=28; ss=hs+4*len(strings); size=ss+len(data)
    return struct.pack('<HHI',1,hs,size)+struct.pack('<IIIII',len(strings),0,UTF8_FLAG,ss,0)+b''.join(struct.pack('<I',x) for x in offs)+data
def resource_map():
    vals=[0]*len(strings)
    for n,r in RESID.items(): vals[S(n)]=r
    return struct.pack('<HHI',0x180,8,8+4*len(vals))+b''.join(struct.pack('<I',v) for v in vals)
def nh(t,size,line=1): return struct.pack('<HHIII',t,16,size,line,NO_INDEX)
def sns(): return nh(0x100,24)+struct.pack('<II',ANDROID_PREFIX,ANDROID_URI)
def ens(): return nh(0x101,24)+struct.pack('<II',ANDROID_PREFIX,ANDROID_URI)
def attr(name,value,android=False,typ='string'):
    ns=ANDROID_URI if android else NO_INDEX; ni=S(name)
    if typ=='string': vi=S(str(value)); return ns,ni,vi,TYPE_STRING,vi
    if typ=='int': return ns,ni,S(str(value)),TYPE_INT_DEC,int(value)
    if typ=='bool': return ns,ni,S('true' if value else 'false'),TYPE_INT_BOOLEAN,0xffffffff if value else 0
    raise ValueError
def se(name,attrs=()):
    size=36+20*len(attrs); out=bytearray(nh(0x102,size)); out+=struct.pack('<IIHHHHHH',NO_INDEX,S(name),20,20,len(attrs),0,0,0)
    for ns,ni,raw,dt,data in attrs: out+=struct.pack('<IIIHBBI',ns,ni,raw,8,0,dt,data)
    return bytes(out)
def ee(name): return nh(0x103,24)+struct.pack('<II',NO_INDEX,S(name))
chunks=[sns(),
 se('manifest',[attr('package','com.pocketnas.wifidrive'),attr('versionCode',31,True,'int'),attr('versionName','3.1',True)]),
 se('uses-sdk',[attr('minSdkVersion',28,True,'int'),attr('targetSdkVersion',30,True,'int')]),ee('uses-sdk')]
perms=['android.permission.INTERNET','android.permission.ACCESS_NETWORK_STATE','android.permission.ACCESS_WIFI_STATE','android.permission.READ_EXTERNAL_STORAGE','android.permission.WRITE_EXTERNAL_STORAGE','android.permission.MANAGE_EXTERNAL_STORAGE','android.permission.FOREGROUND_SERVICE','android.permission.RECEIVE_BOOT_COMPLETED','android.permission.POST_NOTIFICATIONS','android.permission.WAKE_LOCK']
for p in perms:
    chunks += [se('uses-permission',[attr('name',p,True)]),ee('uses-permission')]
chunks += [
 se('application',[attr('label','PocketNAS',True),attr('hasCode',True,True,'bool')]),
 se('activity',[attr('name','android.app.NativeActivity',True),attr('label','PocketNAS',True),attr('exported',True,True,'bool')]),
 se('meta-data',[attr('name','android.app.lib_name',True),attr('value','pocketnas',True)]),ee('meta-data'),
 se('intent-filter'),se('action',[attr('name','android.intent.action.MAIN',True)]),ee('action'),se('category',[attr('name','android.intent.category.LAUNCHER',True)]),ee('category'),ee('intent-filter'),
 ee('activity'),
 se('service',[attr('name','com.pocketnas.wifidrive.PocketNasService',True),attr('exported',False,True,'bool')]),ee('service'),
 se('receiver',[attr('name','com.pocketnas.wifidrive.BootReceiver',True),attr('exported',False,True,'bool')]),
 se('intent-filter'),
 se('action',[attr('name','android.intent.action.BOOT_COMPLETED',True)]),ee('action'),
 se('action',[attr('name','android.intent.action.MY_PACKAGE_REPLACED',True)]),ee('action'),
 ee('intent-filter'),ee('receiver'),
 ee('application'),ee('manifest'),ens()]
body=string_pool()+resource_map()+b''.join(chunks)
out=struct.pack('<HHI',RES_XML_TYPE,8,8+len(body))+body
path=Path(os.environ.get('POCKETNAS_MANIFEST_OUT','build/apkroot/AndroidManifest.xml')); path.parent.mkdir(parents=True,exist_ok=True); path.write_bytes(out)
print('WROTE',path,len(out),'strings',len(strings))
