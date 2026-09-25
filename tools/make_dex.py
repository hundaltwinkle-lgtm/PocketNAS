import os
import struct, os, hashlib, zlib
from pathlib import Path

NO_INDEX=0xFFFFFFFF
# DEX access flags
ACC_PUBLIC=0x1; ACC_PRIVATE=0x2; ACC_PROTECTED=0x4; ACC_STATIC=0x8; ACC_FINAL=0x10
ACC_NATIVE=0x100; ACC_CONSTRUCTOR=0x10000

# Types
T={
 'Main':'Lcom/pocketnas/wifidrive/MainActivity;',
 'Boot':'Lcom/pocketnas/wifidrive/BootReceiver;',
 'Svc':'Lcom/pocketnas/wifidrive/PocketNasService;',
 'NativeActivity':'Landroid/app/NativeActivity;',
 'BroadcastReceiver':'Landroid/content/BroadcastReceiver;',
 'Service':'Landroid/app/Service;',
 'Bundle':'Landroid/os/Bundle;',
 'Context':'Landroid/content/Context;',
 'Intent':'Landroid/content/Intent;',
 'Class':'Ljava/lang/Class;',
 'ComponentName':'Landroid/content/ComponentName;',
 'System':'Ljava/lang/System;',
 'String':'Ljava/lang/String;',
 'CharSequence':'Ljava/lang/CharSequence;',
 'Object':'Ljava/lang/Object;',
 'NotificationChannel':'Landroid/app/NotificationChannel;',
 'NotificationManager':'Landroid/app/NotificationManager;',
 'NotificationBuilder':'Landroid/app/Notification$Builder;',
 'Notification':'Landroid/app/Notification;',
 'IBinder':'Landroid/os/IBinder;',
 'V':'V','I':'I','Z':'Z',
}

# method ref: (class_desc, name, return_desc, params tuple)
def M(cls,name,ret='V',params=()):
    return (cls,name,T.get(ret,ret),tuple(T.get(x,x) for x in params))

# External and internal method references
METHODS=set([
 M(T['NativeActivity'],'<init>'),
 M(T['NativeActivity'],'onCreate','V',('Bundle',)),
 M(T['BroadcastReceiver'],'<init>'),
 M(T['Service'],'<init>'),
 M(T['Service'],'onCreate'),
 M(T['Service'],'onDestroy'),
 M(T['Service'],'startForeground','V',('I','Notification')),
 M(T['Intent'],'<init>','V',('Context','Class')),
 M(T['Context'],'startService','ComponentName',('Intent',)),
 M(T['Context'],'startForegroundService','ComponentName',('Intent',)),
 M(T['Context'],'getSystemService','Object',('String',)),
 M(T['System'],'loadLibrary','V',('String',)),
 M(T['NotificationChannel'],'<init>','V',('String','CharSequence','I')),
 M(T['NotificationManager'],'createNotificationChannel','V',('NotificationChannel',)),
 M(T['NotificationBuilder'],'<init>','V',('Context','String')),
 M(T['NotificationBuilder'],'setContentTitle','NotificationBuilder',('CharSequence',)),
 M(T['NotificationBuilder'],'setContentText','NotificationBuilder',('CharSequence',)),
 M(T['NotificationBuilder'],'setSmallIcon','NotificationBuilder',('I',)),
 M(T['NotificationBuilder'],'setOngoing','NotificationBuilder',('Z',)),
 M(T['NotificationBuilder'],'build','Notification',()),
 # own
 M(T['Main'],'<init>'),
 M(T['Main'],'onCreate','V',('Bundle',)),
 M(T['Boot'],'<init>'),
 M(T['Boot'],'onReceive','V',('Context','Intent')),
 M(T['Svc'],'<init>'),
 M(T['Svc'],'nativeStart'),
 M(T['Svc'],'nativeStop'),
 M(T['Svc'],'onCreate'),
 M(T['Svc'],'onStartCommand','I',('Intent','I','I')),
 M(T['Svc'],'onBind','IBinder',('Intent',)),
 M(T['Svc'],'onDestroy'),
])

LITERALS={
 'pocketnas','pocketnas_service','PocketNAS','notification','PocketNAS server running','Wi-Fi file sharing active'
}

def shorty(ret, params):
    def s(t): return t if len(t)==1 else 'L'
    return s(ret)+''.join(s(x) for x in params)

# strings include descriptors, names, literals, shorties
strings=set(LITERALS)
for desc in T.values(): strings.add(desc)
for cls,name,ret,params in METHODS:
    strings.add(cls); strings.add(name); strings.add(ret)
    for p in params: strings.add(p)
    strings.add(shorty(ret,params))
strings=sorted(strings)
str_idx={s:i for i,s in enumerate(strings)}

# type ids sorted by descriptor string index
types=sorted({d for d in T.values()}, key=lambda d:str_idx[d])
type_idx={d:i for i,d in enumerate(types)}

# protos unique, sorted per dex spec (return type idx, parameters lexicographically by type idx)
protos=set((ret,params) for _,_,ret,params in METHODS)
def proto_key(p):
    ret,params=p
    return (type_idx[ret], tuple(type_idx[x] for x in params))
protos=sorted(protos,key=proto_key)
proto_idx={p:i for i,p in enumerate(protos)}

# methods sorted class_idx, name_idx, proto_idx
methods=sorted(METHODS,key=lambda m:(type_idx[m[0]],str_idx[m[1]],proto_idx[(m[2],m[3])]))
method_idx={m:i for i,m in enumerate(methods)}

# class defs sorted by class idx
classes=[
 (T['Main'],T['NativeActivity']),
 (T['Boot'],T['BroadcastReceiver']),
 (T['Svc'],T['Service']),
]
classes=sorted(classes,key=lambda c:type_idx[c[0]])

# fixed section offsets
header_size=112
off=header_size
string_ids_off=off; off+=4*len(strings)
type_ids_off=off; off+=4*len(types)
proto_ids_off=off; off+=12*len(protos)
field_ids_off=off # no fields
method_ids_off=off; off+=8*len(methods)
class_defs_off=off; off+=32*len(classes)
data_off=(off+3)&~3

# Helpers

def uleb(v):
    out=bytearray()
    while True:
        b=v&0x7f; v>>=7
        if v: out.append(b|0x80)
        else: out.append(b); return bytes(out)

def align(buf,n):
    while len(buf)%n: buf.append(0)

def cu(*xs): return list(xs)
# Dalvik instruction encoders (u16 code units)
def op10x(op): return [op]
def op11x(op,a): return [op | (a<<8)]
def op11n(op,a,lit): return [op | (a<<8) | ((lit & 0xf)<<12)]
def op21c(op,a,idx): return [op | (a<<8), idx & 0xffff]
def op21s(op,a,lit): return [op | (a<<8), lit & 0xffff]
def op31i(op,a,lit): return [op | (a<<8), lit & 0xffff, (lit>>16)&0xffff]
def invoke(op,midx,regs):
    if len(regs)>5 or any(r<0 or r>15 for r in regs): raise ValueError(('bad invoke regs',regs))
    rs=list(regs)+[0]*(5-len(regs))
    c,d,e,f,g=rs[0],rs[1],rs[2],rs[3],rs[4]
    return [op | (g<<8) | (len(regs)<<12), midx & 0xffff, c | (d<<4) | (e<<8) | (f<<12)]

OP={
 'move-result-object':0x0c,'return-void':0x0e,'return':0x0f,'return-object':0x11,
 'const/4':0x12,'const/16':0x13,'const':0x14,'const-string':0x1a,'const-class':0x1c,'check-cast':0x1f,'new-instance':0x22,
 'invoke-virtual':0x6e,'invoke-super':0x6f,'invoke-direct':0x70,'invoke-static':0x71,
}

def mi(m): return method_idx[m]
def ti(t): return type_idx[t]
def si(s): return str_idx[s]

def main_ctor():
    ins=[]
    ins+=invoke(OP['invoke-direct'],mi(M(T['NativeActivity'],'<init>')),[0])
    ins+=op10x(OP['return-void'])
    return 1,1,1,ins

def main_oncreate():
    # v2=this, v3=bundle, locals v0/v1
    ins=[]
    ins+=invoke(OP['invoke-super'],mi(M(T['NativeActivity'],'onCreate','V',('Bundle',))),[2,3])
    ins+=op21c(OP['new-instance'],0,ti(T['Intent']))
    ins+=op21c(OP['const-class'],1,ti(T['Svc']))
    ins+=invoke(OP['invoke-direct'],mi(M(T['Intent'],'<init>','V',('Context','Class'))),[0,2,1])
    ins+=invoke(OP['invoke-virtual'],mi(M(T['Context'],'startService','ComponentName',('Intent',))),[2,0])
    ins+=op10x(OP['return-void'])
    return 4,2,3,ins

def boot_ctor():
    ins=[]; ins+=invoke(OP['invoke-direct'],mi(M(T['BroadcastReceiver'],'<init>')),[0]); ins+=op10x(OP['return-void']); return 1,1,1,ins

def boot_onreceive():
    # regs=5 ins=3 => v2=this,v3=context,v4=intent
    ins=[]
    ins+=op21c(OP['new-instance'],0,ti(T['Intent']))
    ins+=op21c(OP['const-class'],1,ti(T['Svc']))
    ins+=invoke(OP['invoke-direct'],mi(M(T['Intent'],'<init>','V',('Context','Class'))),[0,3,1])
    ins+=invoke(OP['invoke-virtual'],mi(M(T['Context'],'startForegroundService','ComponentName',('Intent',))),[3,0])
    ins+=op10x(OP['return-void'])
    return 5,3,3,ins

def svc_ctor():
    ins=[]; ins+=invoke(OP['invoke-direct'],mi(M(T['Service'],'<init>')),[0]); ins+=op10x(OP['return-void']); return 1,1,1,ins

def svc_oncreate():
    # regs 7, p0=v6
    ins=[]
    ins+=invoke(OP['invoke-super'],mi(M(T['Service'],'onCreate')),[6])
    ins+=op21c(OP['const-string'],0,si('pocketnas'))
    ins+=invoke(OP['invoke-static'],mi(M(T['System'],'loadLibrary','V',('String',))),[0])
    # IMPORTANT: enter foreground before starting native networking. This prevents
    # Android's foreground-service watchdog from killing the process while the
    # server initializes.
    ins+=op21c(OP['const-string'],0,si('pocketnas_service'))
    ins+=op21c(OP['const-string'],1,si('PocketNAS'))
    ins+=op11n(OP['const/4'],2,2)
    ins+=op21c(OP['new-instance'],3,ti(T['NotificationChannel']))
    ins+=invoke(OP['invoke-direct'],mi(M(T['NotificationChannel'],'<init>','V',('String','CharSequence','I'))),[3,0,1,2])
    ins+=op21c(OP['const-string'],4,si('notification'))
    ins+=invoke(OP['invoke-virtual'],mi(M(T['Context'],'getSystemService','Object',('String',))),[6,4])
    ins+=op11x(OP['move-result-object'],4)
    ins+=op21c(OP['check-cast'],4,ti(T['NotificationManager']))
    ins+=invoke(OP['invoke-virtual'],mi(M(T['NotificationManager'],'createNotificationChannel','V',('NotificationChannel',))),[4,3])
    ins+=op21c(OP['new-instance'],3,ti(T['NotificationBuilder']))
    ins+=invoke(OP['invoke-direct'],mi(M(T['NotificationBuilder'],'<init>','V',('Context','String'))),[3,6,0])
    ins+=op21c(OP['const-string'],1,si('PocketNAS server running'))
    ins+=invoke(OP['invoke-virtual'],mi(M(T['NotificationBuilder'],'setContentTitle','NotificationBuilder',('CharSequence',))),[3,1]); ins+=op11x(OP['move-result-object'],3)
    ins+=op21c(OP['const-string'],1,si('Wi-Fi file sharing active'))
    ins+=invoke(OP['invoke-virtual'],mi(M(T['NotificationBuilder'],'setContentText','NotificationBuilder',('CharSequence',))),[3,1]); ins+=op11x(OP['move-result-object'],3)
    ins+=op31i(OP['const'],1,17301651) # android.R.drawable.sym_def_app_icon
    ins+=invoke(OP['invoke-virtual'],mi(M(T['NotificationBuilder'],'setSmallIcon','NotificationBuilder',('I',))),[3,1]); ins+=op11x(OP['move-result-object'],3)
    ins+=op11n(OP['const/4'],1,1)
    ins+=invoke(OP['invoke-virtual'],mi(M(T['NotificationBuilder'],'setOngoing','NotificationBuilder',('Z',))),[3,1]); ins+=op11x(OP['move-result-object'],3)
    ins+=invoke(OP['invoke-virtual'],mi(M(T['NotificationBuilder'],'build','Notification',())),[3]); ins+=op11x(OP['move-result-object'],4)
    ins+=op21s(OP['const/16'],5,1001)
    ins+=invoke(OP['invoke-virtual'],mi(M(T['Service'],'startForeground','V',('I','Notification'))),[6,5,4])
    ins+=invoke(OP['invoke-static'],mi(M(T['Svc'],'nativeStart')),[])
    ins+=op10x(OP['return-void'])
    return 7,1,4,ins

def svc_onstartcommand():
    # regs5, ins4 => v1=this v2=intent v3=flags v4=startId, local v0
    ins=[]; ins+=invoke(OP['invoke-static'],mi(M(T['Svc'],'nativeStart')),[]); ins+=op11n(OP['const/4'],0,1); ins+=op11x(OP['return'],0); return 5,4,0,ins

def svc_onbind():
    # regs3 ins2 -> v1=this v2=intent
    ins=[]; ins+=op11n(OP['const/4'],0,0); ins+=op11x(OP['return-object'],0); return 3,2,0,ins

def svc_ondestroy():
    ins=[]; ins+=invoke(OP['invoke-static'],mi(M(T['Svc'],'nativeStop')),[]); ins+=invoke(OP['invoke-super'],mi(M(T['Service'],'onDestroy')),[0]); ins+=op10x(OP['return-void']); return 1,1,1,ins

# Own method definitions: method-ref -> (flags, code generator or None)
DEFS={
 M(T['Main'],'<init>'):(ACC_PUBLIC|ACC_CONSTRUCTOR,main_ctor),
 M(T['Main'],'onCreate','V',('Bundle',)):(ACC_PROTECTED,main_oncreate),
 M(T['Boot'],'<init>'):(ACC_PUBLIC|ACC_CONSTRUCTOR,boot_ctor),
 M(T['Boot'],'onReceive','V',('Context','Intent')):(ACC_PUBLIC,boot_onreceive),
 M(T['Svc'],'<init>'):(ACC_PUBLIC|ACC_CONSTRUCTOR,svc_ctor),
 M(T['Svc'],'nativeStart'):(ACC_PRIVATE|ACC_STATIC|ACC_NATIVE,None),
 M(T['Svc'],'nativeStop'):(ACC_PRIVATE|ACC_STATIC|ACC_NATIVE,None),
 M(T['Svc'],'onCreate'):(ACC_PUBLIC,svc_oncreate),
 M(T['Svc'],'onStartCommand','I',('Intent','I','I')):(ACC_PUBLIC,svc_onstartcommand),
 M(T['Svc'],'onBind','IBinder',('Intent',)):(ACC_PUBLIC,svc_onbind),
 M(T['Svc'],'onDestroy'):(ACC_PUBLIC,svc_ondestroy),
}

# Build data section
data=bytearray()
# string data
string_data_offsets={}
first_string_data_off=None
for s in strings:
    abs_off=data_off+len(data)
    if first_string_data_off is None:first_string_data_off=abs_off
    string_data_offsets[s]=abs_off
    b=s.encode('utf-8')
    # ASCII/non-BMP not used; UTF-16 len == Python len for our strings
    data+=uleb(len(s))+b+b'\0'

# type lists for proto parameters
param_lists={}
first_type_list_off=None
for params in sorted({p[1] for p in protos if p[1]}, key=lambda ps:tuple(type_idx[x] for x in ps)):
    align(data,4); abs_off=data_off+len(data)
    if first_type_list_off is None:first_type_list_off=abs_off
    param_lists[params]=abs_off
    data+=struct.pack('<I',len(params))
    data+=b''.join(struct.pack('<H',type_idx[x]) for x in params)

# code items: sorted by method index for deterministic output
code_offsets={}; first_code_off=None; code_count=0
for m in sorted(DEFS,key=lambda x:method_idx[x]):
    flags,gen=DEFS[m]
    if gen is None: continue
    regs,ins_size,outs_size,insns=gen()
    align(data,4); abs_off=data_off+len(data)
    if first_code_off is None:first_code_off=abs_off
    code_offsets[m]=abs_off; code_count+=1
    data+=struct.pack('<HHHHII',regs,ins_size,outs_size,0,0,len(insns))
    data+=b''.join(struct.pack('<H',x&0xffff) for x in insns)

# class data items
class_data_offsets={}; first_class_data_off=None
for cls,supercls in classes:
    direct=[]; virtual=[]
    for m,(flags,gen) in DEFS.items():
        if m[0]!=cls: continue
        # constructors, static, native are direct; the rest virtual
        if m[1]=='<init>' or (flags & (ACC_STATIC|ACC_PRIVATE)) or (flags & ACC_NATIVE): direct.append(m)
        else: virtual.append(m)
    direct.sort(key=lambda x:method_idx[x]); virtual.sort(key=lambda x:method_idx[x])
    abs_off=data_off+len(data)
    if first_class_data_off is None:first_class_data_off=abs_off
    class_data_offsets[cls]=abs_off
    data+=uleb(0)+uleb(0)+uleb(len(direct))+uleb(len(virtual))
    for group in (direct,virtual):
        prev=0
        for j,m in enumerate(group):
            idx=method_idx[m]; diff=idx if j==0 else idx-prev; prev=idx
            flags,gen=DEFS[m]
            data+=uleb(diff)+uleb(flags)+uleb(code_offsets.get(m,0))

# map list at aligned end
align(data,4)
map_off=data_off+len(data)
map_entries=[]
def addmap(typ,size,ofs):
    if size: map_entries.append((ofs,typ,size))
addmap(0x0000,1,0)
addmap(0x0001,len(strings),string_ids_off)
addmap(0x0002,len(types),type_ids_off)
addmap(0x0003,len(protos),proto_ids_off)
addmap(0x0005,len(methods),method_ids_off)
addmap(0x0006,len(classes),class_defs_off)
addmap(0x2002,len(strings),first_string_data_off)
addmap(0x1001,len(param_lists),first_type_list_off or 0)
addmap(0x2001,code_count,first_code_off or 0)
addmap(0x2000,len(classes),first_class_data_off)
addmap(0x1000,1,map_off)
map_entries.sort()
mapbuf=bytearray(struct.pack('<I',len(map_entries)))
for ofs,typ,size in map_entries:
    mapbuf+=struct.pack('<HHII',typ,0,size,ofs)
data+=mapbuf

# Build fixed sections
string_ids=b''.join(struct.pack('<I',string_data_offsets[s]) for s in strings)
type_ids=b''.join(struct.pack('<I',str_idx[t]) for t in types)
proto_ids=bytearray()
for ret,params in protos:
    proto_ids+=struct.pack('<III',str_idx[shorty(ret,params)],type_idx[ret],param_lists.get(params,0))
method_ids=bytearray()
for cls,name,ret,params in methods:
    method_ids+=struct.pack('<HHI',type_idx[cls],proto_idx[(ret,params)],str_idx[name])
class_defs=bytearray()
for cls,supercls in classes:
    class_defs+=struct.pack('<IIIIIIII',type_idx[cls],ACC_PUBLIC,type_idx[supercls],0,NO_INDEX,0,class_data_offsets[cls],0)

# Header placeholder then file
fixed=string_ids+type_ids+proto_ids+method_ids+class_defs
if header_size+len(fixed)>data_off: raise AssertionError('fixed overflow')
fixed+=b'\0'*(data_off-(header_size+len(fixed)))
file_size=header_size+len(fixed)+len(data)
data_size=file_size-data_off
hdr=bytearray(112)
hdr[0:8]=b'dex\n035\0'
# checksum/signature later
vals=[file_size,112,0x12345678,0,0,map_off,len(strings),string_ids_off,len(types),type_ids_off,len(protos),proto_ids_off,0,0,len(methods),method_ids_off,len(classes),class_defs_off,data_size,data_off]
struct.pack_into('<20I',hdr,32,*vals)
out=hdr+fixed+data
# SHA-1 signature bytes 32.. ; checksum bytes 8..
sig=hashlib.sha1(out[32:]).digest(); out[12:32]=sig
chk=zlib.adler32(out[12:])&0xffffffff; struct.pack_into('<I',out,8,chk)
path=Path(os.environ.get('POCKETNAS_DEX_OUT','build/apkroot/classes.dex')); path.parent.mkdir(parents=True,exist_ok=True); path.write_bytes(out)
print('WROTE',path,len(out),'strings',len(strings),'types',len(types),'protos',len(protos),'methods',len(methods),'classes',len(classes),'map',hex(map_off))
print('SHA1',sig.hex(),'ADLER',hex(chk))
print('Method IDs:')
for i,m in enumerate(methods): print(i,m)
