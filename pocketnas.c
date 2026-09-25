/* PocketNAS v2.3 - native Android WebDAV Wi-Fi drive
 * NativeActivity UI + foreground-service native core
 * Target: Android 9+ (arm64-v8a), Windows WebDAV clients.
 */

/* ---------- minimal platform types ---------- */
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef signed long long int64_t;
typedef unsigned long long uint64_t;
typedef unsigned long size_t;
typedef long ssize_t;
typedef long off_t;
typedef unsigned int socklen_t;
typedef unsigned short sa_family_t;
typedef unsigned long pthread_t;
#ifndef NULL
#define NULL ((void*)0)
#endif

/* ---------- libc / POSIX declarations ---------- */
extern void *malloc(size_t);
extern void *calloc(size_t,size_t);
extern void *realloc(void*,size_t);
extern void free(void*);
extern void *memcpy(void*,const void*,size_t);
extern void *memmove(void*,const void*,size_t);
extern void *memset(void*,int,size_t);
extern int memcmp(const void*,const void*,size_t);
extern size_t strlen(const char*);
extern int strcmp(const char*,const char*);
extern int strncmp(const char*,const char*,size_t);
extern char *strchr(const char*,int);
extern char *strrchr(const char*,int);
extern char *strstr(const char*,const char*);
extern int snprintf(char*,size_t,const char*,...);
extern int pthread_create(pthread_t*,const void*,void*(*)(void*),void*);
extern int pthread_detach(pthread_t);
extern int usleep(unsigned int);
extern int open(const char*,int,...);
extern ssize_t read(int,void*,size_t);
extern ssize_t write(int,const void*,size_t);
extern off_t lseek(int,off_t,int);
extern int close(int);
extern int mkdir(const char*,unsigned int);
extern int unlink(const char*);
extern int rmdir(const char*);
extern int rename(const char*,const char*);
extern int access(const char*,int);
extern int fsync(int);

typedef struct __dirstream DIR;
struct dirent {
    uint64_t d_ino;
    int64_t d_off;
    uint16_t d_reclen;
    uint8_t d_type;
    char d_name[256];
};
extern DIR *opendir(const char*);
extern struct dirent *readdir(DIR*);
extern int closedir(DIR*);

struct sockaddr { sa_family_t sa_family; char sa_data[14]; };
struct in_addr { uint32_t s_addr; };
struct sockaddr_in {
    sa_family_t sin_family;
    uint16_t sin_port;
    struct in_addr sin_addr;
    uint8_t sin_zero[8];
};
struct ifaddrs {
    struct ifaddrs *ifa_next;
    char *ifa_name;
    unsigned int ifa_flags;
    struct sockaddr *ifa_addr;
    struct sockaddr *ifa_netmask;
    union { struct sockaddr *ifu_broadaddr; struct sockaddr *ifu_dstaddr; } ifa_ifu;
    void *ifa_data;
};
extern int socket(int,int,int);
extern int setsockopt(int,int,int,const void*,socklen_t);
extern int bind(int,const struct sockaddr*,socklen_t);
extern int listen(int,int);
extern int accept(int,struct sockaddr*,socklen_t*);
extern ssize_t recv(int,void*,size_t,int);
extern ssize_t send(int,const void*,size_t,int);
extern int shutdown(int,int);
extern int getifaddrs(struct ifaddrs**);
extern void freeifaddrs(struct ifaddrs*);
extern const char *inet_ntop(int,const void*,char*,socklen_t);

#define AF_INET 2
#define SOCK_STREAM 1
#define SOL_SOCKET 1
#define SO_REUSEADDR 2
#define SHUT_RDWR 2
#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_CREAT 0x40
#define O_TRUNC 0x200
#define SEEK_SET 0
#define SEEK_END 2
#define F_OK 0
#define DT_DIR 4
#define DT_REG 8

/* ---------- Android NativeActivity / window / input ---------- */
typedef struct JavaVM_ JavaVM;
typedef const struct JNINativeInterface *JNIEnv;
typedef void *jobject;
typedef void *jclass;
typedef void *jmethodID;
typedef void *jstring;
typedef void *jthrowable;
typedef int jint;
typedef struct AAssetManager AAssetManager;
typedef struct ANativeWindow ANativeWindow;
typedef struct AInputQueue AInputQueue;
typedef struct AInputEvent AInputEvent;
typedef struct ARect { int32_t left, top, right, bottom; } ARect;
typedef struct ANativeWindow_Buffer {
    int32_t width;
    int32_t height;
    int32_t stride;
    int32_t format;
    void *bits;
    uint32_t reserved[6];
} ANativeWindow_Buffer;
struct ANativeActivityCallbacks;
typedef struct ANativeActivity {
    struct ANativeActivityCallbacks *callbacks;
    JavaVM *vm;
    JNIEnv *env;
    jobject clazz;
    const char *internalDataPath;
    const char *externalDataPath;
    int32_t sdkVersion;
    void *instance;
    AAssetManager *assetManager;
    const char *obbPath;
} ANativeActivity;
typedef struct ANativeActivityCallbacks {
    void (*onStart)(ANativeActivity*);
    void (*onResume)(ANativeActivity*);
    void *(*onSaveInstanceState)(ANativeActivity*,size_t*);
    void (*onPause)(ANativeActivity*);
    void (*onStop)(ANativeActivity*);
    void (*onDestroy)(ANativeActivity*);
    void (*onWindowFocusChanged)(ANativeActivity*,int);
    void (*onNativeWindowCreated)(ANativeActivity*,ANativeWindow*);
    void (*onNativeWindowResized)(ANativeActivity*,ANativeWindow*);
    void (*onNativeWindowRedrawNeeded)(ANativeActivity*,ANativeWindow*);
    void (*onNativeWindowDestroyed)(ANativeActivity*,ANativeWindow*);
    void (*onInputQueueCreated)(ANativeActivity*,AInputQueue*);
    void (*onInputQueueDestroyed)(ANativeActivity*,AInputQueue*);
    void (*onContentRectChanged)(ANativeActivity*,const ARect*);
    void (*onConfigurationChanged)(ANativeActivity*);
    void (*onLowMemory)(ANativeActivity*);
} ANativeActivityCallbacks;

extern void ANativeActivity_finish(ANativeActivity*);
extern void ANativeActivity_setWindowFlags(ANativeActivity*,uint32_t,uint32_t);
extern void ANativeWindow_acquire(ANativeWindow*);
extern void ANativeWindow_release(ANativeWindow*);
extern int32_t ANativeWindow_getWidth(ANativeWindow*);
extern int32_t ANativeWindow_getHeight(ANativeWindow*);
extern int32_t ANativeWindow_setBuffersGeometry(ANativeWindow*,int32_t,int32_t,int32_t);
extern int32_t ANativeWindow_lock(ANativeWindow*,ANativeWindow_Buffer*,ARect*);
extern int32_t ANativeWindow_unlockAndPost(ANativeWindow*);
extern int32_t AInputQueue_hasEvents(AInputQueue*);
extern int32_t AInputQueue_getEvent(AInputQueue*,AInputEvent**);
extern int32_t AInputQueue_preDispatchEvent(AInputQueue*,AInputEvent*);
extern void AInputQueue_finishEvent(AInputQueue*,AInputEvent*,int);
extern int32_t AInputEvent_getType(const AInputEvent*);
extern int32_t AMotionEvent_getAction(const AInputEvent*);
extern float AMotionEvent_getX(const AInputEvent*,size_t);
extern float AMotionEvent_getY(const AInputEvent*,size_t);
extern int32_t AKeyEvent_getAction(const AInputEvent*);
extern int32_t AKeyEvent_getKeyCode(const AInputEvent*);

#define AINPUT_EVENT_TYPE_KEY 1
#define AINPUT_EVENT_TYPE_MOTION 2
#define AMOTION_EVENT_ACTION_MASK 0xff
#define AMOTION_EVENT_ACTION_UP 1
#define AKEY_EVENT_ACTION_UP 1
#define AKEYCODE_BACK 4
#define WINDOW_FORMAT_RGBA_8888 1
#define FLAG_KEEP_SCREEN_ON 0x00000080u

/* ---------- global state ---------- */
static ANativeActivity *g_activity = NULL;
static ANativeWindow *g_window = NULL;
static AInputQueue *g_input = NULL;
static volatile int g_window_lock = 0;
static volatile int g_input_lock = 0;
static volatile int g_state_lock = 0;
static volatile int g_destroyed = 0;
static volatile int g_activity_alive = 0;
static volatile int g_service_active = 0;
static volatile int g_monitor_started = 0;
static volatile int g_config_loaded = 0;
static volatile int g_server_running = 0;
static volatile int g_server_starting = 0;
static volatile int g_server_wanted = 1;
static volatile int g_listen_fd = -1;
static volatile int g_readonly = 0;
static volatile int g_auth = 0;
static volatile int g_clients = 0;
static volatile uint64_t g_requests = 0;
static volatile uint64_t g_bytes_in = 0;
static volatile uint64_t g_bytes_out = 0;
static volatile int g_storage_ok = 0;
static volatile int g_storage_writable = 0;
static char g_root[512] = "/storage/emulated/0";
static char g_ip[64] = "NO LAN IP";
static char g_password[32] = "";
static char g_nonce[40] = "";
static char g_config_path[768] = "";
static char g_status[96] = "STARTING";
static int g_btn_start[4] = {0,0,0,0};
static int g_btn_ro[4] = {0,0,0,0};
static int g_btn_auth[4] = {0,0,0,0};
static int g_btn_setpass[4] = {0,0,0,0};
static volatile int g_password_custom = 0;
static volatile int g_password_edit_mode = 0;
static char g_password_edit[24] = "";
static int g_password_edit_len = 0;
static int g_keypad[13][4];

static void spin_lock(volatile int *p) {
    while (__atomic_exchange_n(p,1,__ATOMIC_ACQUIRE)) usleep(1000);
}
static void spin_unlock(volatile int *p) { __atomic_store_n(p,0,__ATOMIC_RELEASE); }
static void str_copy(char *dst,size_t cap,const char *src) {
    if (!cap) return;
    size_t i=0; if (src) while (src[i] && i+1<cap) { dst[i]=src[i]; i++; }
    dst[i]=0;
}
static void set_status(const char *s) {
    spin_lock(&g_state_lock); str_copy(g_status,sizeof(g_status),s); spin_unlock(&g_state_lock);
}
static int ascii_tolower(int c) { return (c>='A'&&c<='Z')?c+32:c; }
static int streq_ci_n(const char *a,const char *b,size_t n) {
    for(size_t i=0;i<n;i++){ if(!a[i]||!b[i]) return a[i]==b[i]; if(ascii_tolower(a[i])!=ascii_tolower(b[i])) return 0; } return 1;
}
static uint16_t bswap16(uint16_t x){return (uint16_t)((x<<8)|(x>>8));}
static uint32_t bswap32(uint32_t x){return ((x&0xffu)<<24)|((x&0xff00u)<<8)|((x>>8)&0xff00u)|((x>>24)&0xffu);}
static uint16_t htons16(uint16_t x){return bswap16(x);}
static uint32_t ntohl32(uint32_t x){return bswap32(x);}

/* ---------- compact MD5 implementation ---------- */
typedef struct { uint32_t h[4]; uint64_t len; uint8_t buf[64]; uint32_t used; } MD5Ctx;
static uint32_t rol32(uint32_t x,uint32_t n){return (x<<n)|(x>>(32-n));}
static const uint32_t md5_k[64]={
0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,0x6b901122,0xfd987193,0xa679438e,0x49b40821,
0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,
0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391};
static const uint8_t md5_s[64]={7,12,17,22,7,12,17,22,7,12,17,22,7,12,17,22,5,9,14,20,5,9,14,20,5,9,14,20,5,9,14,20,4,11,16,23,4,11,16,23,4,11,16,23,4,11,16,23,6,10,15,21,6,10,15,21,6,10,15,21,6,10,15,21};
static void md5_block(MD5Ctx *c,const uint8_t *p){
    uint32_t m[16]; for(int i=0;i<16;i++) m[i]=(uint32_t)p[i*4]|((uint32_t)p[i*4+1]<<8)|((uint32_t)p[i*4+2]<<16)|((uint32_t)p[i*4+3]<<24);
    uint32_t a=c->h[0],b=c->h[1],cc=c->h[2],d=c->h[3];
    for(int i=0;i<64;i++){uint32_t f,g;if(i<16){f=(b&cc)|((~b)&d);g=i;}else if(i<32){f=(d&b)|((~d)&cc);g=(5*i+1)&15;}else if(i<48){f=b^cc^d;g=(3*i+5)&15;}else{f=cc^(b|(~d));g=(7*i)&15;}uint32_t t=d;d=cc;cc=b;b=b+rol32(a+f+md5_k[i]+m[g],md5_s[i]);a=t;}
    c->h[0]+=a;c->h[1]+=b;c->h[2]+=cc;c->h[3]+=d;
}
static void md5_init(MD5Ctx *c){c->h[0]=0x67452301;c->h[1]=0xefcdab89;c->h[2]=0x98badcfe;c->h[3]=0x10325476;c->len=0;c->used=0;}
static void md5_update(MD5Ctx *c,const void *data,size_t n){const uint8_t*p=(const uint8_t*)data;c->len+=(uint64_t)n*8;while(n){size_t take=64-c->used;if(take>n)take=n;memcpy(c->buf+c->used,p,take);c->used+=(uint32_t)take;p+=take;n-=take;if(c->used==64){md5_block(c,c->buf);c->used=0;}}}
static void md5_final(MD5Ctx*c,uint8_t out[16]){uint64_t bits=c->len;uint8_t one=0x80;md5_update(c,&one,1);uint8_t zero=0;while(c->used!=56)md5_update(c,&zero,1);uint8_t l[8];for(int i=0;i<8;i++)l[i]=(uint8_t)(bits>>(8*i));md5_update(c,l,8);for(int i=0;i<4;i++){out[i*4]=(uint8_t)c->h[i];out[i*4+1]=(uint8_t)(c->h[i]>>8);out[i*4+2]=(uint8_t)(c->h[i]>>16);out[i*4+3]=(uint8_t)(c->h[i]>>24);}}
static void md5_hex(const char *s,char out[33]){MD5Ctx c;uint8_t d[16];static const char*h="0123456789abcdef";md5_init(&c);md5_update(&c,s,strlen(s));md5_final(&c,d);for(int i=0;i<16;i++){out[i*2]=h[d[i]>>4];out[i*2+1]=h[d[i]&15];}out[32]=0;}

/* ---------- config / random ---------- */
static int random_bytes(uint8_t *p,size_t n){int fd=open("/dev/urandom",O_RDONLY);if(fd<0)return 0;size_t got=0;while(got<n){ssize_t r=read(fd,p+got,n-got);if(r<=0)break;got+=(size_t)r;}close(fd);return got==n;}
static void make_password(void){
    static const char al[]="ABCDEFGHJKLMNPQRSTUVWXYZ23456789";uint8_t r[12];if(!random_bytes(r,sizeof(r))){for(int i=0;i<12;i++)r[i]=(uint8_t)(i*17+41);}for(int i=0;i<10;i++)g_password[i]=al[r[i]%32];g_password[10]=0;
    uint8_t n[16];if(!random_bytes(n,sizeof(n)))for(int i=0;i<16;i++)n[i]=(uint8_t)(r[i%12]^i*31);static const char hx[]="0123456789abcdef";for(int i=0;i<16;i++){g_nonce[i*2]=hx[n[i]>>4];g_nonce[i*2+1]=hx[n[i]&15];}g_nonce[32]=0;
}
static void save_config(void){
    if(!g_config_path[0])return;char b[320];int n=snprintf(b,sizeof(b),"CONFIG_VERSION=20\nPASSWORD=%s\nPASSWORD_CUSTOM=%d\nAUTH=%d\nREADONLY=%d\n",g_password,g_password_custom?1:0,g_auth?1:0,g_readonly?1:0);int fd=open(g_config_path,O_WRONLY|O_CREAT|O_TRUNC,0600);if(fd>=0){write(fd,b,(size_t)n);fsync(fd);close(fd);}
}
static void load_config(void){
    /* v1.4 starts with authentication OFF. A password becomes permanent only
       after the user explicitly saves one in the on-screen password editor. */
    make_password();g_auth=0;g_password_custom=0;if(!g_config_path[0])return;int fd=open(g_config_path,O_RDONLY);if(fd<0){save_config();return;}char b[640];ssize_t n=read(fd,b,sizeof(b)-1);close(fd);if(n<=0)return;b[n]=0;
    int is_v20=strstr(b,"CONFIG_VERSION=20")!=NULL;
    int had_custom_marker=strstr(b,"PASSWORD_CUSTOM=1")!=NULL;
    char *p=strstr(b,"PASSWORD=");if(p){p+=9;char *e=strchr(p,'\n');size_t l=e?(size_t)(e-p):strlen(p);if(l>0&&l<sizeof(g_password)){memcpy(g_password,p,l);g_password[l]=0;}}
    p=strstr(b,"PASSWORD_CUSTOM=");if(p)g_password_custom=(p[16]=='1');
    p=strstr(b,"READONLY=");if(p)g_readonly=(p[9]=='1');
    /* Existing v1.0-v1.3 configs are migrated with auth disabled so an update
       cannot unexpectedly lock Windows out. */
    if(is_v20){p=strstr(b,"AUTH=");if(p)g_auth=(p[5]=='1');}
    else{g_auth=0;if(had_custom_marker)g_password_custom=1;save_config();}
}
static void ensure_config_loaded(void){
    if(g_config_loaded)return;
    if(!g_config_path[0])str_copy(g_config_path,sizeof(g_config_path),"/data/user/0/com.pocketnas.wifidrive/files/pocketnas.conf");
    load_config();
    g_config_loaded=1;
}
static void derive_root(const char *ext){
    if(!ext||!ext[0])return;const char *m=strstr(ext,"/Android/");if(m){size_t n=(size_t)(m-ext);if(n>0&&n<sizeof(g_root)){memcpy(g_root,ext,n);g_root[n]=0;}}
}
static void refresh_storage(void){
    DIR*d=opendir(g_root);if(!d){g_storage_ok=0;g_storage_writable=0;return;}closedir(d);g_storage_ok=1;
    /* On modern Android, listing shared storage can succeed even when broad
       write/delete access has not been granted. Probe a tiny temporary file so
       the UI can distinguish read-only/limited storage from full access. */
    char t[768];const char *tag=g_nonce[0]?g_nonce:"pnas";
    snprintf(t,sizeof(t),"%s/.pocketnas_access_%.8s.tmp",g_root,tag);
    int f=open(t,O_WRONLY|O_CREAT|O_TRUNC,0600);if(f>=0){char c='P';ssize_t w=write(f,&c,1);fsync(f);close(f);if(w==1&&unlink(t)==0)g_storage_writable=1;else{unlink(t);g_storage_writable=0;}}else g_storage_writable=0;
}

/* ---------- networking helpers ---------- */
static int ip_private(uint32_t n){uint32_t h=ntohl32(n);uint8_t a=(uint8_t)(h>>24),b=(uint8_t)(h>>16);return a==10||(a==172&&b>=16&&b<=31)||(a==192&&b==168);}
static int name_score(const char*n){if(!n)return 0;if(strstr(n,"wlan")||strstr(n,"wifi"))return 5;if(strstr(n,"swlan")||strstr(n,"ap"))return 4;if(strstr(n,"eth"))return 3;return 1;}
static int find_lan_ip(char out[64],uint32_t *addr_out){
    struct ifaddrs *ifs=NULL;if(getifaddrs(&ifs)!=0||!ifs)return 0;int best=0;uint32_t ba=0;char bt[64]="";
    for(struct ifaddrs*i=ifs;i;i=i->ifa_next){if(!i->ifa_addr||i->ifa_addr->sa_family!=AF_INET)continue;struct sockaddr_in*s=(struct sockaddr_in*)i->ifa_addr;uint32_t h=ntohl32(s->sin_addr.s_addr);if((h>>24)==127)continue;int sc=name_score(i->ifa_name);if(ip_private(s->sin_addr.s_addr))sc+=10;if(sc>best){char t[64];if(inet_ntop(AF_INET,&s->sin_addr,t,sizeof(t))){best=sc;ba=s->sin_addr.s_addr;str_copy(bt,sizeof(bt),t);}}}
    freeifaddrs(ifs);if(!best)return 0;str_copy(out,64,bt);if(addr_out)*addr_out=ba;return 1;
}
static int send_all(int fd,const void *buf,size_t n){const uint8_t*p=(const uint8_t*)buf;size_t off=0;while(off<n){ssize_t r=send(fd,p+off,n-off,0);if(r<=0)return 0;off+=(size_t)r;__atomic_fetch_add(&g_bytes_out,(uint64_t)r,__ATOMIC_RELAXED);}return 1;}
static int send_str(int fd,const char*s){return send_all(fd,s,strlen(s));}
static void http_simple(int fd,int code,const char*reason,const char*extra,const char*body){char h[1024];size_t bl=body?strlen(body):0;int n=snprintf(h,sizeof(h),"HTTP/1.1 %d %s\r\nServer: PocketNAS/2.3\r\nConnection: close\r\n%sContent-Length: %lu\r\n\r\n",code,reason,extra?extra:"",(unsigned long)bl);send_all(fd,h,(size_t)n);if(bl)send_all(fd,body,bl);}
static void http_no_body(int fd,int code,const char*reason,const char*extra){http_simple(fd,code,reason,extra,NULL);}

/* ---------- URL/path helpers ---------- */
static int hexv(int c){if(c>='0'&&c<='9')return c-'0';if(c>='a'&&c<='f')return c-'a'+10;if(c>='A'&&c<='F')return c-'A'+10;return -1;}
static int url_decode(const char *in,char*out,size_t cap){size_t j=0;for(size_t i=0;in[i];i++){unsigned char c=(unsigned char)in[i];if(c=='?'||c=='#')break;if(c=='%'&&in[i+1]&&in[i+2]){int a=hexv(in[i+1]),b=hexv(in[i+2]);if(a>=0&&b>=0){c=(unsigned char)((a<<4)|b);i+=2;}}if(c==0)return 0;if(j+1>=cap)return 0;out[j++]=(char)c;}out[j]=0;return 1;}
static int path_safe(const char*p){if(!p||p[0]!='/')return 0;const char*s=p;while(*s){while(*s=='/')s++;const char*e=s;while(*e&&*e!='/')e++;size_t n=(size_t)(e-s);if(n==2&&s[0]=='.'&&s[1]=='.')return 0;s=e;}return 1;}
static int map_path(const char *url,char*out,size_t cap){char dec[1024];if(!url_decode(url,dec,sizeof(dec))||!path_safe(dec))return 0;size_t a=strlen(g_root),b=strlen(dec);if(a+b+2>cap)return 0;memcpy(out,g_root,a);if(b==1&&dec[0]=='/'){out[a]=0;return 1;}memcpy(out+a,dec,b+1);return 1;}
static int append_char(char*out,size_t cap,size_t *j,char c){if(*j+1>=cap)return 0;out[(*j)++]=c;out[*j]=0;return 1;}
static int url_encode_name(const char*in,char*out,size_t cap){static const char*h="0123456789ABCDEF";size_t j=0;for(size_t i=0;in[i];i++){unsigned char c=(unsigned char)in[i];if((c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='-'||c=='_'||c=='.'||c=='~'){if(!append_char(out,cap,&j,(char)c))return 0;}else{if(j+3>=cap)return 0;out[j++]='%';out[j++]=h[c>>4];out[j++]=h[c&15];out[j]=0;}}return 1;}
static void xml_escape(const char*in,char*out,size_t cap){size_t j=0;for(size_t i=0;in[i]&&j+1<cap;i++){const char*r=NULL;switch(in[i]){case'&':r="&amp;";break;case'<':r="&lt;";break;case'>':r="&gt;";break;case'\"':r="&quot;";break;case'\'':r="&apos;";break;}if(r){size_t n=strlen(r);if(j+n>=cap)break;memcpy(out+j,r,n);j+=n;}else out[j++]=in[i];}out[j]=0;}
static const char *basename_ptr(const char*p){const char*s=strrchr(p,'/');return s?s+1:p;}
static int is_dir_path(const char*p){DIR*d=opendir(p);if(d){closedir(d);return 1;}return 0;}
static long long file_size(const char*p){int fd=open(p,O_RDONLY);if(fd<0)return -1;off_t n=lseek(fd,0,SEEK_END);close(fd);return (long long)n;}

/* ---------- request parsing ---------- */
typedef struct {char method[16];char uri[1024];char authz[2048];char depth[32];char destination[2048];char overwrite[8];char range[128];long long content_length;int chunked;int expect_continue;size_t body_off;size_t have;} HttpReq;
static char *find_header_end(char*b,size_t n){if(n<4)return NULL;for(size_t i=0;i+3<n;i++)if(b[i]=='\r'&&b[i+1]=='\n'&&b[i+2]=='\r'&&b[i+3]=='\n')return b+i+4;return NULL;}
static void trim_copy(char*out,size_t cap,const char*s,size_t n){while(n&&(*s==' '||*s=='\t')){s++;n--;}while(n&&(s[n-1]==' '||s[n-1]=='\t'||s[n-1]=='\r'))n--;if(n>=cap)n=cap-1;memcpy(out,s,n);out[n]=0;}
static long long parse_dec(const char*s){long long v=0;while(*s>='0'&&*s<='9'){v=v*10+(*s-'0');s++;}return v;}
static int parse_request(char*b,size_t n,HttpReq*r){memset(r,0,sizeof(*r));r->content_length=0;char*e=find_header_end(b,n);if(!e)return 0;r->body_off=(size_t)(e-b);r->have=n-r->body_off;char*line=b;char*nl=strstr(line,"\r\n");if(!nl)return 0;char*sp=strchr(line,' ');if(!sp||sp>nl)return 0;size_t ml=(size_t)(sp-line);if(ml>=sizeof(r->method))ml=sizeof(r->method)-1;memcpy(r->method,line,ml);r->method[ml]=0;char*sp2=strchr(sp+1,' ');if(!sp2||sp2>nl)return 0;size_t ul=(size_t)(sp2-(sp+1));if(ul>=sizeof(r->uri))ul=sizeof(r->uri)-1;memcpy(r->uri,sp+1,ul);r->uri[ul]=0;line=nl+2;while(line<e-2){nl=strstr(line,"\r\n");if(!nl)break;if(nl==line)break;char*colon=strchr(line,':');if(colon&&colon<nl){size_t kn=(size_t)(colon-line);const char*v=colon+1;size_t vn=(size_t)(nl-v);if(kn==13&&streq_ci_n(line,"Authorization",13))trim_copy(r->authz,sizeof(r->authz),v,vn);else if(kn==5&&streq_ci_n(line,"Depth",5))trim_copy(r->depth,sizeof(r->depth),v,vn);else if(kn==11&&streq_ci_n(line,"Destination",11))trim_copy(r->destination,sizeof(r->destination),v,vn);else if(kn==9&&streq_ci_n(line,"Overwrite",9))trim_copy(r->overwrite,sizeof(r->overwrite),v,vn);else if(kn==5&&streq_ci_n(line,"Range",5))trim_copy(r->range,sizeof(r->range),v,vn);else if(kn==14&&streq_ci_n(line,"Content-Length",14)){char t[64];trim_copy(t,sizeof(t),v,vn);r->content_length=parse_dec(t);}else if(kn==17&&streq_ci_n(line,"Transfer-Encoding",17)){char t[128];trim_copy(t,sizeof(t),v,vn);if(strstr(t,"chunked")||strstr(t,"Chunked"))r->chunked=1;}else if(kn==6&&streq_ci_n(line,"Expect",6)){char t[128];trim_copy(t,sizeof(t),v,vn);if(strstr(t,"100-continue")||strstr(t,"100-Continue"))r->expect_continue=1;}}
        line=nl+2;
    }return 1;}
static int recv_headers(int fd,char*b,size_t cap,size_t*out_n){size_t n=0;while(n+1<cap){ssize_t r=recv(fd,b+n,cap-n-1,0);if(r<=0)return 0;n+=(size_t)r;__atomic_fetch_add(&g_bytes_in,(uint64_t)r,__ATOMIC_RELAXED);b[n]=0;if(find_header_end(b,n)){*out_n=n;return 1;}}return 0;}

/* ---------- Digest auth ---------- */
static int digest_field(const char*s,const char*key,char*out,size_t cap){size_t kl=strlen(key);const char*p=s;while(*p){while(*p==' '||*p==',')p++;if(streq_ci_n(p,key,kl)&&p[kl]=='='){p+=kl+1;if(*p=='\"'){p++;const char*e=strchr(p,'\"');if(!e)return 0;trim_copy(out,cap,p,(size_t)(e-p));return 1;}else{const char*e=p;while(*e&&*e!=','&&*e!=' ')e++;trim_copy(out,cap,p,(size_t)(e-p));return 1;}}const char*c=strchr(p,',');if(!c)break;p=c+1;}return 0;}
static int user_allowed(const char *user){
    const char *u=user;const char *bs=strrchr(user,'\\');if(bs&&bs[1])u=bs+1;
    size_t n=strlen(u);return n==9&&streq_ci_n(u,"pocketnas",9);
}
static int auth_ok(const HttpReq*r){
    if(!g_auth)return 1;
    if(strncmp(r->authz,"Digest ",7)!=0)return 0;
    const char*s=r->authz+7;char user[128],realm[64],nonce[80],uri[1024],resp[80],qop[32],nc[32],cnonce[128];
    if(!digest_field(s,"username",user,sizeof(user))||!digest_field(s,"realm",realm,sizeof(realm))||!digest_field(s,"nonce",nonce,sizeof(nonce))||!digest_field(s,"uri",uri,sizeof(uri))||!digest_field(s,"response",resp,sizeof(resp)))return 0;
    if(!user_allowed(user)||strcmp(realm,"PocketNAS")||strcmp(nonce,g_nonce))return 0;
    char a1[320],a2[1400],ha1[33],ha2[33],combo[1800],want[33];
    /* Digest HA1 must use the exact username token supplied by the client. */
    snprintf(a1,sizeof(a1),"%s:PocketNAS:%s",user,g_password);md5_hex(a1,ha1);
    snprintf(a2,sizeof(a2),"%s:%s",r->method,uri);md5_hex(a2,ha2);
    if(digest_field(s,"qop",qop,sizeof(qop))&&qop[0]){
        if(!digest_field(s,"nc",nc,sizeof(nc))||!digest_field(s,"cnonce",cnonce,sizeof(cnonce)))return 0;
        snprintf(combo,sizeof(combo),"%s:%s:%s:%s:%s:%s",ha1,nonce,nc,cnonce,qop,ha2);
    }else snprintf(combo,sizeof(combo),"%s:%s:%s",ha1,nonce,ha2);
    md5_hex(combo,want);return streq_ci_n(want,resp,32)&&strlen(resp)==32;
}
static void auth_challenge(int fd){char e[256];snprintf(e,sizeof(e),"WWW-Authenticate: Digest realm=\"PocketNAS\", nonce=\"%s\", algorithm=MD5, qop=\"auth\"\r\n",g_nonce);http_simple(fd,401,"Unauthorized",e,"Authentication required");}

/* ---------- file type / range helpers ---------- */
static int ends_ci(const char*s,const char*suf){size_t a=strlen(s),b=strlen(suf);if(a<b)return 0;return streq_ci_n(s+a-b,suf,b);}
static const char *mime_type(const char *p){
    if(ends_ci(p,".txt")||ends_ci(p,".log")||ends_ci(p,".ini")||ends_ci(p,".cfg")||ends_ci(p,".md")||ends_ci(p,".csv"))return "text/plain; charset=utf-8";
    if(ends_ci(p,".html")||ends_ci(p,".htm"))return "text/html; charset=utf-8";
    if(ends_ci(p,".css"))return "text/css; charset=utf-8";
    if(ends_ci(p,".js")||ends_ci(p,".mjs"))return "application/javascript";
    if(ends_ci(p,".json"))return "application/json";
    if(ends_ci(p,".xml"))return "application/xml";
    if(ends_ci(p,".pdf"))return "application/pdf";
    if(ends_ci(p,".jpg")||ends_ci(p,".jpeg"))return "image/jpeg";
    if(ends_ci(p,".png"))return "image/png";
    if(ends_ci(p,".gif"))return "image/gif";
    if(ends_ci(p,".webp"))return "image/webp";
    if(ends_ci(p,".bmp"))return "image/bmp";
    if(ends_ci(p,".svg"))return "image/svg+xml";
    if(ends_ci(p,".heic")||ends_ci(p,".heif"))return "image/heic";
    if(ends_ci(p,".mp4")||ends_ci(p,".m4v"))return "video/mp4";
    if(ends_ci(p,".mkv"))return "video/x-matroska";
    if(ends_ci(p,".avi"))return "video/x-msvideo";
    if(ends_ci(p,".mov"))return "video/quicktime";
    if(ends_ci(p,".webm"))return "video/webm";
    if(ends_ci(p,".3gp"))return "video/3gpp";
    if(ends_ci(p,".mp3"))return "audio/mpeg";
    if(ends_ci(p,".m4a")||ends_ci(p,".aac"))return "audio/mp4";
    if(ends_ci(p,".wav"))return "audio/wav";
    if(ends_ci(p,".flac"))return "audio/flac";
    if(ends_ci(p,".ogg")||ends_ci(p,".opus"))return "audio/ogg";
    if(ends_ci(p,".zip"))return "application/zip";
    if(ends_ci(p,".7z"))return "application/x-7z-compressed";
    if(ends_ci(p,".rar"))return "application/vnd.rar";
    if(ends_ci(p,".tar"))return "application/x-tar";
    if(ends_ci(p,".gz")||ends_ci(p,".tgz"))return "application/gzip";
    if(ends_ci(p,".apk"))return "application/vnd.android.package-archive";
    if(ends_ci(p,".doc"))return "application/msword";
    if(ends_ci(p,".docx"))return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    if(ends_ci(p,".xls"))return "application/vnd.ms-excel";
    if(ends_ci(p,".xlsx"))return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    if(ends_ci(p,".ppt"))return "application/vnd.ms-powerpoint";
    if(ends_ci(p,".pptx"))return "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    if(ends_ci(p,".db")||ends_ci(p,".sqlite")||ends_ci(p,".sqlite3"))return "application/vnd.sqlite3";
    if(ends_ci(p,".epub"))return "application/epub+zip";
    if(ends_ci(p,".ttf"))return "font/ttf";
    if(ends_ci(p,".otf"))return "font/otf";
    return "application/octet-stream";
}
static int parse_range_header(const char *h,long long sz,long long *start,long long *end){
    if(!h||!h[0]||strncmp(h,"bytes=",6)!=0||sz<=0)return 0;const char*p=h+6;if(strchr(p,','))return -1;
    const char*dash=strchr(p,'-');if(!dash)return -1;
    if(dash==p){long long n=parse_dec(dash+1);if(n<=0)return -1;if(n>sz)n=sz;*start=sz-n;*end=sz-1;return 1;}
    long long a=parse_dec(p);long long b=(dash[1]?parse_dec(dash+1):sz-1);if(a<0||a>=sz||b<a)return -1;if(b>=sz)b=sz-1;*start=a;*end=b;return 1;
}

/* ---------- WebDAV XML streaming ---------- */
static int chunk_send(int fd,const char*s){size_t n=strlen(s);char h[32];int k=snprintf(h,sizeof(h),"%lx\r\n",(unsigned long)n);if(!send_all(fd,h,(size_t)k))return 0;if(n&&!send_all(fd,s,n))return 0;return send_str(fd,"\r\n");}
static int chunk_end(int fd){return send_str(fd,"0\r\n\r\n");}
static void href_for(const char*uri,const char*child,int isdir,char*out,size_t cap){char base[1024];str_copy(base,sizeof(base),uri);char*q=strchr(base,'?');if(q)*q=0;if(!base[0])str_copy(base,sizeof(base),"/");if(child){size_t n=strlen(base);if(n==0||base[n-1]!='/'){if(n+1<sizeof(base)){base[n++]='/';base[n]=0;}}char enc[768];url_encode_name(child,enc,sizeof(enc));snprintf(out,cap,"%s%s%s",base,enc,isdir?"/":"");}else{size_t n=strlen(base);if(isdir&&n&&base[n-1]!='/')snprintf(out,cap,"%s/",base);else str_copy(out,cap,base);}}
static void dav_one(int fd,const char*href,const char*display,const char*path,int isdir,long long size){char hesc[2048],desc[1024],b[4608];xml_escape(href,hesc,sizeof(hesc));xml_escape(display,desc,sizeof(desc));if(isdir)snprintf(b,sizeof(b),"<D:response><D:href>%s</D:href><D:propstat><D:prop><D:displayname>%s</D:displayname><D:resourcetype><D:collection/></D:resourcetype><D:getcontentlength>0</D:getcontentlength><D:getlastmodified>Thu, 01 Jan 1970 00:00:00 GMT</D:getlastmodified><D:creationdate>1970-01-01T00:00:00Z</D:creationdate><D:supportedlock><D:lockentry><D:lockscope><D:exclusive/></D:lockscope><D:locktype><D:write/></D:locktype></D:lockentry></D:supportedlock><D:lockdiscovery/></D:prop><D:status>HTTP/1.1 200 OK</D:status></D:propstat></D:response>",hesc,desc);else snprintf(b,sizeof(b),"<D:response><D:href>%s</D:href><D:propstat><D:prop><D:displayname>%s</D:displayname><D:resourcetype/><D:getcontentlength>%lld</D:getcontentlength><D:getcontenttype>%s</D:getcontenttype><D:getlastmodified>Thu, 01 Jan 1970 00:00:00 GMT</D:getlastmodified><D:creationdate>1970-01-01T00:00:00Z</D:creationdate><D:getetag>\"%lld\"</D:getetag><D:supportedlock><D:lockentry><D:lockscope><D:exclusive/></D:lockscope><D:locktype><D:write/></D:locktype></D:lockentry></D:supportedlock><D:lockdiscovery/></D:prop><D:status>HTTP/1.1 200 OK</D:status></D:propstat></D:response>",hesc,desc,size,mime_type(path),size);chunk_send(fd,b);}
static void handle_propfind(int fd,const HttpReq*r,const char*path){int dir=is_dir_path(path);if(!dir&&access(path,F_OK)!=0){http_no_body(fd,404,"Not Found",NULL);return;}send_str(fd,"HTTP/1.1 207 Multi-Status\r\nServer: PocketNAS/2.3\r\nContent-Type: application/xml; charset=utf-8\r\nTransfer-Encoding: chunked\r\nConnection: close\r\nDAV: 1,2\r\n\r\n");chunk_send(fd,"<?xml version=\"1.0\" encoding=\"utf-8\"?><D:multistatus xmlns:D=\"DAV:\">");char href[2048];href_for(r->uri,NULL,dir,href,sizeof(href));const char*bn=basename_ptr(path);if(!bn[0])bn="PocketNAS";dav_one(fd,href,bn,path,dir,dir?0:file_size(path));int depth1=(r->depth[0]==0||r->depth[0]=='1');if(dir&&depth1){DIR*d=opendir(path);if(d){struct dirent*de;while((de=readdir(d))){if(!strcmp(de->d_name,".")||!strcmp(de->d_name,".."))continue;char cp[1536];snprintf(cp,sizeof(cp),"%s/%s",path,de->d_name);int cd=(de->d_type==DT_DIR)?1:(de->d_type==DT_REG?0:is_dir_path(cp));long long sz=cd?0:file_size(cp);href_for(r->uri,de->d_name,cd,href,sizeof(href));dav_one(fd,href,de->d_name,cp,cd,sz<0?0:sz);}closedir(d);}}chunk_send(fd,"</D:multistatus>");chunk_end(fd);}

/* ---------- filesystem operations ---------- */
static int file_exists_any(const char*p){DIR*d=opendir(p);if(d){closedir(d);return 1;}int f=open(p,O_RDONLY);if(f>=0){close(f);return 1;}return 0;}
static int delete_recursive(const char*p){
    /* Try a normal file first. This avoids treating unreadable files as folders. */
    if(unlink(p)==0)return 1;
    DIR*d=opendir(p);if(!d)return 0;
    struct dirent*de;int ok=1;while((de=readdir(d))){if(!strcmp(de->d_name,".")||!strcmp(de->d_name,".."))continue;char cp[1536];snprintf(cp,sizeof(cp),"%s/%s",p,de->d_name);
        int child_dir=(de->d_type==DT_DIR)?1:(de->d_type==DT_REG?0:is_dir_path(cp));
        if(child_dir){if(!delete_recursive(cp))ok=0;}else if(unlink(cp)!=0){/* Some Android/FUSE files report unknown type; retry as directory. */if(!delete_recursive(cp))ok=0;}
    }closedir(d);if(rmdir(p)!=0)ok=0;return ok;
}
static int copy_file(const char*a,const char*b){int in=open(a,O_RDONLY);if(in<0)return 0;int out=open(b,O_WRONLY|O_CREAT|O_TRUNC,0660);if(out<0){close(in);return 0;}char*buf=(char*)malloc(65536);if(!buf){close(in);close(out);return 0;}int ok=1;for(;;){ssize_t n=read(in,buf,65536);if(n==0)break;if(n<0){ok=0;break;}size_t o=0;while(o<(size_t)n){ssize_t w=write(out,buf+o,(size_t)n-o);if(w<=0){ok=0;break;}o+=(size_t)w;}if(!ok)break;}fsync(out);free(buf);close(in);close(out);return ok;}
static int copy_recursive(const char*a,const char*b){if(!is_dir_path(a))return copy_file(a,b);if(mkdir(b,0770)!=0&&access(b,F_OK)!=0)return 0;DIR*d=opendir(a);if(!d)return 0;int ok=1;struct dirent*de;while((de=readdir(d))){if(!strcmp(de->d_name,".")||!strcmp(de->d_name,".."))continue;char x[1536],y[1536];snprintf(x,sizeof(x),"%s/%s",a,de->d_name);snprintf(y,sizeof(y),"%s/%s",b,de->d_name);if(!copy_recursive(x,y))ok=0;}closedir(d);return ok;}
static int destination_path(const char*d,char*out,size_t cap){if(!d||!d[0])return 0;const char*p=d;const char*sch=strstr(d,"://");if(sch){p=sch+3;p=strchr(p,'/');if(!p)p="/";}return map_path(p,out,cap);}

/* ---------- PUT body ---------- */
static int write_put_body(int fd,const HttpReq*r,const char*raw,size_t raw_n,const char*path){int out=open(path,O_WRONLY|O_CREAT|O_TRUNC,0660);if(out<0)return 0;int ok=1;if(r->chunked){/* conservative chunked parser: buffer stream incrementally */size_t cap=131072;char*b=(char*)malloc(cap);if(!b){close(out);return 0;}size_t n=0;if(r->have){if(r->have>cap)n=cap;else n=r->have;memcpy(b,raw+r->body_off,n);}for(;;){/* ensure a full chunk-size line */char*le=NULL;for(size_t i=0;i+1<n;i++)if(b[i]=='\r'&&b[i+1]=='\n'){le=b+i;break;}while(!le&&n+1<cap){ssize_t z=recv(fd,b+n,cap-n,0);if(z<=0){ok=0;break;}n+=(size_t)z;__atomic_fetch_add(&g_bytes_in,(uint64_t)z,__ATOMIC_RELAXED);for(size_t i=0;i+1<n;i++)if(b[i]=='\r'&&b[i+1]=='\n'){le=b+i;break;}}if(!ok||!le)break;unsigned long cs=0;for(char*q=b;q<le;q++){int v=hexv(*q);if(v<0)break;cs=cs*16u+(unsigned)v;}size_t hdr=(size_t)(le-b)+2;while(n<hdr+cs+2){if(n==cap){ok=0;break;}ssize_t z=recv(fd,b+n,cap-n,0);if(z<=0){ok=0;break;}n+=(size_t)z;__atomic_fetch_add(&g_bytes_in,(uint64_t)z,__ATOMIC_RELAXED);}if(!ok)break;if(cs==0)break;size_t o=0;while(o<cs){ssize_t w=write(out,b+hdr+o,cs-o);if(w<=0){ok=0;break;}o+=(size_t)w;}if(!ok)break;size_t used=hdr+cs+2;if(used<n)memmove(b,b+used,n-used);n-=used;}free(b);}else{long long remain=r->content_length;size_t first=r->have;if((long long)first>remain)first=(size_t)remain;size_t o=0;while(o<first){ssize_t w=write(out,raw+r->body_off+o,first-o);if(w<=0){ok=0;break;}o+=(size_t)w;}remain-=(long long)first;char*b=(char*)malloc(65536);if(!b){ok=0;}while(ok&&remain>0){size_t want=remain>65536?65536:(size_t)remain;ssize_t z=recv(fd,b,want,0);if(z<=0){ok=0;break;}__atomic_fetch_add(&g_bytes_in,(uint64_t)z,__ATOMIC_RELAXED);size_t q=0;while(q<(size_t)z){ssize_t w=write(out,b+q,(size_t)z-q);if(w<=0){ok=0;break;}q+=(size_t)w;}remain-=z;}if(b)free(b);}fsync(out);close(out);if(!ok)unlink(path);return ok;}

/* ---------- WebDAV handlers ---------- */
static void handle_get(int fd,const HttpReq*r,const char*path,int head_only){
    if(is_dir_path(path)){const char*body="<html><body><h2>PocketNAS</h2><p>This is a WebDAV folder. Add this address as a Windows network location.</p></body></html>";http_simple(fd,200,"OK","Content-Type: text/html; charset=utf-8\r\n",head_only?NULL:body);return;}
    int f=open(path,O_RDONLY);if(f<0){http_no_body(fd,404,"Not Found",NULL);return;}off_t osz=lseek(f,0,SEEK_END);if(osz<0){close(f);http_no_body(fd,500,"Read Failed",NULL);return;}long long sz=(long long)osz,start=0,end=sz?sz-1:0;int rr=parse_range_header(r->range,sz,&start,&end);if(rr<0){char ex[128];snprintf(ex,sizeof(ex),"Content-Range: bytes */%lld\r\n",sz);close(f);http_no_body(fd,416,"Range Not Satisfiable",ex);return;}long long count=(sz==0)?0:(rr?end-start+1:sz);if(rr&&lseek(f,(off_t)start,SEEK_SET)<0){close(f);http_no_body(fd,500,"Seek Failed",NULL);return;}else if(!rr)lseek(f,0,SEEK_SET);
    char h[1024];int n;if(rr)n=snprintf(h,sizeof(h),"HTTP/1.1 206 Partial Content\r\nServer: PocketNAS/2.3\r\nContent-Type: %s\r\nContent-Length: %lld\r\nContent-Range: bytes %lld-%lld/%lld\r\nConnection: close\r\nAccept-Ranges: bytes\r\n\r\n",mime_type(path),count,start,end,sz);else n=snprintf(h,sizeof(h),"HTTP/1.1 200 OK\r\nServer: PocketNAS/2.3\r\nContent-Type: %s\r\nContent-Length: %lld\r\nConnection: close\r\nAccept-Ranges: bytes\r\n\r\n",mime_type(path),count);send_all(fd,h,(size_t)n);
    if(!head_only&&count>0){char*b=(char*)malloc(65536);if(b){long long left=count;while(left>0){size_t want=left>65536?65536:(size_t)left;ssize_t z=read(f,b,want);if(z<=0)break;if(!send_all(fd,b,(size_t)z))break;left-=z;}free(b);}}close(f);
}
static void handle_lock(int fd,const HttpReq*r){(void)r;char body[1024];snprintf(body,sizeof(body),"<?xml version=\"1.0\" encoding=\"utf-8\"?><D:prop xmlns:D=\"DAV:\"><D:lockdiscovery><D:activelock><D:locktype><D:write/></D:locktype><D:lockscope><D:exclusive/></D:lockscope><D:depth>Infinity</D:depth><D:timeout>Second-3600</D:timeout><D:locktoken><D:href>opaquelocktoken:%s</D:href></D:locktoken></D:activelock></D:lockdiscovery></D:prop>",g_nonce);char ex[256];snprintf(ex,sizeof(ex),"Content-Type: application/xml; charset=utf-8\r\nLock-Token: <opaquelocktoken:%s>\r\n",g_nonce);http_simple(fd,200,"OK",ex,body);}
static void handle_request(int fd,char*raw,size_t raw_n,HttpReq*r){__atomic_fetch_add(&g_requests,1,__ATOMIC_RELAXED);if(!strcmp(r->method,"OPTIONS")){http_no_body(fd,200,"OK","DAV: 1,2\r\nMS-Author-Via: DAV\r\nAllow: OPTIONS, GET, HEAD, PUT, DELETE, MKCOL, MOVE, COPY, PROPFIND, PROPPATCH, LOCK, UNLOCK\r\n");return;}if(g_auth&&!auth_ok(r)){auth_challenge(fd);return;}char path[1536];if(!map_path(r->uri,path,sizeof(path))){http_no_body(fd,400,"Bad Request",NULL);return;}if(!strcmp(r->method,"PROPFIND")){handle_propfind(fd,r,path);return;}if(!strcmp(r->method,"GET")){handle_get(fd,r,path,0);return;}if(!strcmp(r->method,"HEAD")){handle_get(fd,r,path,1);return;}if(!strcmp(r->method,"LOCK")){handle_lock(fd,r);return;}if(!strcmp(r->method,"UNLOCK")){http_no_body(fd,204,"No Content",NULL);return;}if(!strcmp(r->method,"PROPPATCH")){http_simple(fd,207,"Multi-Status","Content-Type: application/xml; charset=utf-8\r\n","<?xml version=\"1.0\"?><D:multistatus xmlns:D=\"DAV:\"></D:multistatus>");return;}if(g_readonly){http_no_body(fd,403,"Forbidden",NULL);return;}if(!strcmp(r->method,"PUT")){int existed=(access(path,F_OK)==0);if(r->expect_continue)send_str(fd,"HTTP/1.1 100 Continue\r\n\r\n");if(write_put_body(fd,r,raw,raw_n,path))http_no_body(fd,existed?204:201,existed?"No Content":"Created",NULL);else{refresh_storage();set_status(g_storage_writable?"WRITE FAILED":"WRITE FAILED - GRANT ALL FILES ACCESS");http_no_body(fd,403,"Write Failed",NULL);}return;}if(!strcmp(r->method,"MKCOL")){if(mkdir(path,0770)==0)http_no_body(fd,201,"Created",NULL);else if(access(path,F_OK)==0)http_no_body(fd,405,"Method Not Allowed",NULL);else http_no_body(fd,409,"Conflict",NULL);return;}if(!strcmp(r->method,"DELETE")){int existed=file_exists_any(path);if(delete_recursive(path)){refresh_storage();set_status("DELETE OK");http_no_body(fd,204,"No Content",NULL);}else if(!existed){http_no_body(fd,404,"Not Found",NULL);}else{refresh_storage();set_status(g_storage_writable?"DELETE FAILED":"DELETE FAILED - GRANT ALL FILES ACCESS");http_no_body(fd,403,"Forbidden",g_storage_writable?NULL:"X-PocketNAS-Storage: limited\r\n");}return;}if(!strcmp(r->method,"MOVE")){char dst[1536];if(!destination_path(r->destination,dst,sizeof(dst))){http_no_body(fd,400,"Bad Destination",NULL);return;}int existed=(access(dst,F_OK)==0);if(existed&&r->overwrite[0]=='F'){http_no_body(fd,412,"Precondition Failed",NULL);return;}if(existed)delete_recursive(dst);if(rename(path,dst)==0)http_no_body(fd,existed?204:201,existed?"No Content":"Created",NULL);else http_no_body(fd,409,"Conflict",NULL);return;}if(!strcmp(r->method,"COPY")){char dst[1536];if(!destination_path(r->destination,dst,sizeof(dst))){http_no_body(fd,400,"Bad Destination",NULL);return;}int existed=(access(dst,F_OK)==0);if(existed&&r->overwrite[0]=='F'){http_no_body(fd,412,"Precondition Failed",NULL);return;}if(existed)delete_recursive(dst);if(copy_recursive(path,dst))http_no_body(fd,existed?204:201,existed?"No Content":"Created",NULL);else http_no_body(fd,409,"Conflict",NULL);return;}http_no_body(fd,405,"Method Not Allowed","Allow: OPTIONS, GET, HEAD, PUT, DELETE, MKCOL, MOVE, COPY, PROPFIND, PROPPATCH, LOCK, UNLOCK\r\n");}

static void *client_thread(void *arg){int fd=(int)(long)arg;__atomic_fetch_add(&g_clients,1,__ATOMIC_RELAXED);char*buf=(char*)malloc(131072);if(buf){size_t n=0;if(recv_headers(fd,buf,131072,&n)){HttpReq r;if(parse_request(buf,n,&r))handle_request(fd,buf,n,&r);else http_no_body(fd,400,"Bad Request",NULL);}free(buf);}shutdown(fd,SHUT_RDWR);close(fd);__atomic_fetch_sub(&g_clients,1,__ATOMIC_RELAXED);return NULL;}
static void server_stop(void);
static void *server_thread(void*unused){(void)unused;char ip[64];uint32_t addr=0;if(!find_lan_ip(ip,&addr)){set_status("NO WIFI OR LAN IP");g_server_starting=0;g_server_running=0;return NULL;}str_copy(g_ip,sizeof(g_ip),ip);int s=socket(AF_INET,SOCK_STREAM,0);if(s<0){set_status("SOCKET ERROR");g_server_starting=0;g_server_running=0;return NULL;}int one=1;setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&one,sizeof(one));struct sockaddr_in a;memset(&a,0,sizeof(a));a.sin_family=AF_INET;a.sin_port=htons16(8080);a.sin_addr.s_addr=addr;if(bind(s,(struct sockaddr*)&a,sizeof(a))!=0){close(s);set_status("PORT 8080 BIND FAILED");g_server_starting=0;g_server_running=0;return NULL;}if(listen(s,16)!=0){close(s);set_status("LISTEN FAILED");g_server_starting=0;g_server_running=0;return NULL;}g_listen_fd=s;g_server_starting=0;g_server_running=1;set_status("SERVER RUNNING");while(g_server_wanted){int c=accept(s,NULL,NULL);if(c<0)break;pthread_t t;if(pthread_create(&t,NULL,client_thread,(void*)(long)c)==0)pthread_detach(t);else close(c);}if(g_listen_fd==s){g_listen_fd=-1;close(s);}g_server_starting=0;g_server_running=0;if(g_server_wanted)set_status("SERVER STOPPED");return NULL;}
static void server_start(void){if(g_server_running||__atomic_exchange_n(&g_server_starting,1,__ATOMIC_ACQ_REL))return;g_server_wanted=1;pthread_t t;if(pthread_create(&t,NULL,server_thread,NULL)==0)pthread_detach(t);else{g_server_starting=0;set_status("THREAD ERROR");}}
static void server_stop(void){g_server_wanted=0;int s=g_listen_fd;if(s>=0){g_listen_fd=-1;shutdown(s,SHUT_RDWR);close(s);}g_server_running=0;set_status("SERVER STOPPED");}

/* ---------- tiny 5x7 font ---------- */
static const uint8_t *glyph_rows(char c){
    static const uint8_t blank[7]={0,0,0,0,0,0,0};
    static const uint8_t A[7]={14,17,17,31,17,17,17},B[7]={30,17,17,30,17,17,30},C[7]={14,17,16,16,16,17,14},D[7]={30,17,17,17,17,17,30},E[7]={31,16,16,30,16,16,31},F[7]={31,16,16,30,16,16,16},G[7]={14,17,16,23,17,17,15},H[7]={17,17,17,31,17,17,17},I[7]={14,4,4,4,4,4,14},J[7]={7,2,2,2,18,18,12},K[7]={17,18,20,24,20,18,17},L[7]={16,16,16,16,16,16,31},M[7]={17,27,21,21,17,17,17},N[7]={17,25,21,19,17,17,17},O[7]={14,17,17,17,17,17,14},P[7]={30,17,17,30,16,16,16},Q[7]={14,17,17,17,21,18,13},R[7]={30,17,17,30,20,18,17},S[7]={15,16,16,14,1,1,30},T[7]={31,4,4,4,4,4,4},U[7]={17,17,17,17,17,17,14},V[7]={17,17,17,17,17,10,4},W[7]={17,17,17,21,21,21,10},X[7]={17,17,10,4,10,17,17},Y[7]={17,17,10,4,4,4,4},Z[7]={31,1,2,4,8,16,31};
    static const uint8_t N0[7]={14,17,19,21,25,17,14},N1[7]={4,12,4,4,4,4,14},N2[7]={14,17,1,2,4,8,31},N3[7]={30,1,1,14,1,1,30},N4[7]={2,6,10,18,31,2,2},N5[7]={31,16,16,30,1,1,30},N6[7]={14,16,16,30,17,17,14},N7[7]={31,1,2,4,8,8,8},N8[7]={14,17,17,14,17,17,14},N9[7]={14,17,17,15,1,1,14};
    static const uint8_t DOT[7]={0,0,0,0,0,6,6},COL[7]={0,6,6,0,6,6,0},SL[7]={1,2,2,4,8,8,16},BSL[7]={16,8,8,4,2,2,1},DASH[7]={0,0,0,31,0,0,0},US[7]={0,0,0,0,0,0,31},GT[7]={16,8,4,2,4,8,16},LT[7]={1,2,4,8,4,2,1},LP[7]={2,4,8,8,8,4,2},RP[7]={8,4,2,2,2,4,8},EQ[7]={0,31,0,31,0,0,0},EX[7]={4,4,4,4,4,0,4},QM[7]={14,17,1,2,4,0,4},HASH[7]={10,31,10,10,31,10,0},PLUS[7]={0,4,4,31,4,4,0},STAR[7]={0,21,14,31,14,21,0};
    if(c>='a'&&c<='z')c=(char)(c-32);if(c>='A'&&c<='Z'){static const uint8_t*arr[26];arr[0]=A;arr[1]=B;arr[2]=C;arr[3]=D;arr[4]=E;arr[5]=F;arr[6]=G;arr[7]=H;arr[8]=I;arr[9]=J;arr[10]=K;arr[11]=L;arr[12]=M;arr[13]=N;arr[14]=O;arr[15]=P;arr[16]=Q;arr[17]=R;arr[18]=S;arr[19]=T;arr[20]=U;arr[21]=V;arr[22]=W;arr[23]=X;arr[24]=Y;arr[25]=Z;return arr[c-'A'];}if(c>='0'&&c<='9'){static const uint8_t*arrn[10];arrn[0]=N0;arrn[1]=N1;arrn[2]=N2;arrn[3]=N3;arrn[4]=N4;arrn[5]=N5;arrn[6]=N6;arrn[7]=N7;arrn[8]=N8;arrn[9]=N9;return arrn[c-'0'];}switch(c){case'.':return DOT;case':':return COL;case'/':return SL;case'\\':return BSL;case'-':return DASH;case'_':return US;case'>':return GT;case'<':return LT;case'(':return LP;case')':return RP;case'=':return EQ;case'!':return EX;case'?':return QM;case'#':return HASH;case'+':return PLUS;case'*':return STAR;default:return blank;}}
static uint32_t px(uint8_t r,uint8_t g,uint8_t b){return 0xff000000u|((uint32_t)b<<16)|((uint32_t)g<<8)|r;}
static void fill_rect(uint32_t*p,int stride,int w,int h,int x,int y,int rw,int rh,uint32_t c){if(x<0){rw+=x;x=0;}if(y<0){rh+=y;y=0;}if(x+rw>w)rw=w-x;if(y+rh>h)rh=h-y;if(rw<=0||rh<=0)return;for(int yy=y;yy<y+rh;yy++){uint32_t*q=p+yy*stride+x;for(int xx=0;xx<rw;xx++)q[xx]=c;}}
static void draw_char(uint32_t*p,int stride,int w,int h,int x,int y,char c,int s,uint32_t col){const uint8_t*r=glyph_rows(c);for(int yy=0;yy<7;yy++)for(int xx=0;xx<5;xx++)if(r[yy]&(1u<<(4-xx)))fill_rect(p,stride,w,h,x+xx*s,y+yy*s,s,s,col);}
static void draw_text(uint32_t*p,int stride,int w,int h,int x,int y,const char*s,int sc,uint32_t col){int ox=x;for(size_t i=0;s[i];i++){if(s[i]=='\n'){x=ox;y+=9*sc;continue;}draw_char(p,stride,w,h,x,y,s[i],sc,col);x+=6*sc;}}
static void draw_button(uint32_t*p,int stride,int w,int h,int *rect,int x,int y,int rw,int rh,const char*text,int s,uint32_t bg,uint32_t fg){fill_rect(p,stride,w,h,x,y,rw,rh,bg);rect[0]=x;rect[1]=y;rect[2]=x+rw;rect[3]=y+rh;int tw=(int)strlen(text)*6*s;int tx=x+(rw-tw)/2;int ty=y+(rh-7*s)/2;draw_text(p,stride,w,h,tx,ty,text,s,fg);}
static int hit(const int*r,int x,int y){return x>=r[0]&&x<r[2]&&y>=r[1]&&y<r[3];}

static void draw_password_editor(uint32_t*p,int stride,int W,int H,int S){
    uint32_t bg=px(15,23,42),fg=px(241,245,249),mut=px(148,163,184),teal=px(13,148,136),red=px(239,68,68),panel=px(30,41,59),blue=px(37,99,235);
    fill_rect(p,stride,W,H,0,0,W,H,bg);int x=16*S,y=24*S;draw_text(p,stride,W,H,x,y,"SET PERMANENT PASSWORD",2*S,fg);y+=30*S;draw_text(p,stride,W,H,x,y,"4-16 DIGITS. SAVED ACROSS REBOOTS/UPDATES.",S,mut);y+=22*S;
    fill_rect(p,stride,W,H,12*S,y-4*S,W-24*S,42*S,panel);char line[96];if(g_password_edit_len)snprintf(line,sizeof(line),"PASSWORD: %s",g_password_edit);else str_copy(line,sizeof(line),"PASSWORD: _");draw_text(p,stride,W,H,x,y+8*S,line,S,fg);y+=54*S;
    int gap=8*S,left=24*S,bw=(W-48*S-2*gap)/3,bh=42*S;const char*labels[9]={"1","2","3","4","5","6","7","8","9"};int k=0;
    for(int r=0;r<3;r++){for(int c=0;c<3;c++){int bx=left+c*(bw+gap),by=y+r*(bh+gap);draw_button(p,stride,W,H,g_keypad[k],bx,by,bw,bh,labels[k],2*S,blue,fg);k++;}}
    y+=3*(bh+gap);draw_button(p,stride,W,H,g_keypad[9],left,y,bw,bh,"0",2*S,blue,fg);draw_button(p,stride,W,H,g_keypad[10],left+bw+gap,y,bw,bh,"BACK",S,panel,fg);draw_button(p,stride,W,H,g_keypad[11],left+2*(bw+gap),y,bw,bh,"SAVE",S,teal,fg);y+=bh+gap;
    draw_button(p,stride,W,H,g_keypad[12],left,y,W-48*S,bh,"CANCEL",S,red,fg);y+=bh+14*S;draw_text(p,stride,W,H,x,y,"AUTHENTICATION STAYS OFF UNTIL YOU TURN IT ON.",S,mut);
}

static void draw_ui(ANativeWindow*w){if(!w)return;ANativeWindow_setBuffersGeometry(w,0,0,WINDOW_FORMAT_RGBA_8888);ANativeWindow_Buffer b;if(ANativeWindow_lock(w,&b,NULL)!=0||!b.bits)return;uint32_t*p=(uint32_t*)b.bits;int W=b.width,H=b.height,S=W/360;if(S<1)S=1;if(H/S<640)S=H/640;if(S<1)S=1;uint32_t bg=px(15,23,42),fg=px(241,245,249),mut=px(148,163,184),green=px(34,197,94),red=px(239,68,68),blue=px(37,99,235),amber=px(245,158,11),panel=px(30,41,59),teal=px(13,148,136);fill_rect(p,b.stride,W,H,0,0,W,H,bg);
    if(g_password_edit_mode){draw_password_editor(p,b.stride,W,H,S);ANativeWindow_unlockAndPost(w);return;}
    int x=16*S,y=18*S;draw_text(p,b.stride,W,H,x,y,"POCKETNAS",2*S,fg);y+=24*S;draw_text(p,b.stride,W,H,x,y,"WIFI FILE DRIVE FOR WINDOWS",S,mut);y+=24*S;
    char line[256],st[96];spin_lock(&g_state_lock);str_copy(st,sizeof(st),g_status);spin_unlock(&g_state_lock);snprintf(line,sizeof(line),"SERVER: %s   CLIENTS: %d",g_server_running?"RUNNING":"STOPPED",(int)g_clients);draw_text(p,b.stride,W,H,x,y,line,S,g_server_running?green:red);y+=16*S;snprintf(line,sizeof(line),"STATUS: %s",st);draw_text(p,b.stride,W,H,x,y,line,S,mut);y+=22*S;
    refresh_storage();if(!g_storage_ok)draw_text(p,b.stride,W,H,x,y,"STORAGE: PERMISSION REQUIRED",S,amber);else if(!g_storage_writable)draw_text(p,b.stride,W,H,x,y,"STORAGE: LIMITED - ENABLE ALL FILES ACCESS",S,amber);else draw_text(p,b.stride,W,H,x,y,"STORAGE: FULL READ/WRITE",S,green);y+=24*S;
    draw_text(p,b.stride,W,H,x,y,"WINDOWS ADDRESS",S,mut);y+=12*S;snprintf(line,sizeof(line),"HTTP://%s:8080/",g_ip);draw_text(p,b.stride,W,H,x,y,line,S,fg);y+=22*S;
    if(g_auth){snprintf(line,sizeof(line),"USER: pocketnas   PASS: %s",g_password);draw_text(p,b.stride,W,H,x,y,line,S,fg);}else{draw_text(p,b.stride,W,H,x,y,"AUTH: OFF - NO USER/PASSWORD REQUIRED",S,amber);}y+=16*S;draw_text(p,b.stride,W,H,x,y,g_password_custom?"PERMANENT PASSWORD: SAVED":"PERMANENT PASSWORD: NOT SET",S,g_password_custom?green:mut);y+=24*S;
    fill_rect(p,b.stride,W,H,12*S,y-4*S,W-24*S,82*S,panel);draw_text(p,b.stride,W,H,x,y,"WINDOWS 11 SETUP",S,fg);y+=14*S;draw_text(p,b.stride,W,H,x,y,"THIS PC > ADD A NETWORK LOCATION",S,mut);y+=12*S;draw_text(p,b.stride,W,H,x,y,"ENTER THE HTTP ADDRESS ABOVE.",S,mut);y+=12*S;draw_text(p,b.stride,W,H,x,y,"PHONE AND PC MUST USE SAME WIFI.",S,mut);y+=32*S;
    int bw=W-32*S,bh=30*S;draw_button(p,b.stride,W,H,g_btn_start,16*S,y,bw,bh,g_server_running?"STOP SERVER":"START SERVER",S,g_server_running?red:blue,fg);y+=38*S;draw_button(p,b.stride,W,H,g_btn_ro,16*S,y,bw,bh,g_readonly?"READ ONLY: ON":"READ ONLY: OFF",S,g_readonly?amber:teal,fg);y+=38*S;draw_button(p,b.stride,W,H,g_btn_setpass,16*S,y,bw,bh,g_password_custom?"CHANGE PERMANENT PASSWORD":"SET PERMANENT PASSWORD",S,blue,fg);y+=38*S;draw_button(p,b.stride,W,H,g_btn_auth,16*S,y,bw,bh,g_auth?"AUTHENTICATION: ON":"AUTHENTICATION: OFF",S,g_auth?teal:amber,fg);y+=44*S;
    if(!g_storage_ok||!g_storage_writable){draw_text(p,b.stride,W,H,x,y,"GRANT FULL STORAGE ACCESS:",S,amber);y+=13*S;draw_text(p,b.stride,W,H,x,y,"SETTINGS > SPECIAL APP ACCESS >",S,fg);y+=12*S;draw_text(p,b.stride,W,H,x,y,"ALL FILES ACCESS > POCKETNAS > ALLOW",S,fg);y+=12*S;draw_text(p,b.stride,W,H,x,y,"THEN RETURN TO THIS APP.",S,mut);}else{snprintf(line,sizeof(line),"REQUESTS: %llu  TRANSFER: %llu MB",(unsigned long long)g_requests,(unsigned long long)((g_bytes_in+g_bytes_out)/(1024ull*1024ull)));draw_text(p,b.stride,W,H,x,y,line,S,mut);y+=14*S;draw_text(p,b.stride,W,H,x,y,"FILES: ALL TYPES / RANGE / COPY / UPLOAD / DELETE",S,mut);}
    ANativeWindow_unlockAndPost(w);
}

/* ---------- foreground-service launcher from framework NativeActivity ---------- */
/*
 * Keep the launcher activity as android.app.NativeActivity (the stable v1.0 path)
 * and request the Java foreground service through JNI. The foreground service runs in the same app process so it shares the proven
 * v1 WebDAV core and does not start a second competing server on port 8080.
 */
static int jni_exception_clear(JNIEnv *env){
    if(!env||!*env)return 0;void **t=(void **)(*env);
    typedef jthrowable (*FExc)(JNIEnv*); typedef void (*FClear)(JNIEnv*);
    FExc occurred=(FExc)t[15]; FClear clear=(FClear)t[17];
    jthrowable e=occurred?occurred(env):NULL;if(e){if(clear)clear(env);return 1;}return 0;
}
static int request_foreground_service(ANativeActivity *a){
    if(!a||!a->env||!a->clazz||!*a->env)return 0;
    JNIEnv *env=a->env;void **t=(void **)(*env);
    typedef jclass (*FFindClass)(JNIEnv*,const char*);
    typedef jmethodID (*FGetMethodID)(JNIEnv*,jclass,const char*,const char*);
    typedef jobject (*FNewObject)(JNIEnv*,jclass,jmethodID,...);
    typedef jclass (*FGetObjectClass)(JNIEnv*,jobject);
    typedef jobject (*FCallObject)(JNIEnv*,jobject,jmethodID,...);
    typedef jstring (*FNewStringUTF)(JNIEnv*,const char*);
    typedef void (*FDeleteLocalRef)(JNIEnv*,jobject);
    FFindClass FindClass=(FFindClass)t[6]; FGetMethodID GetMethodID=(FGetMethodID)t[33];
    FNewObject NewObject=(FNewObject)t[28]; FGetObjectClass GetObjectClass=(FGetObjectClass)t[31];
    FCallObject CallObject=(FCallObject)t[34]; FNewStringUTF NewStringUTF=(FNewStringUTF)t[167];
    FDeleteLocalRef DeleteLocalRef=(FDeleteLocalRef)t[23];
    if(!FindClass||!GetMethodID||!NewObject||!GetObjectClass||!CallObject||!NewStringUTF)return 0;
    jclass ic=FindClass(env,"android/content/Intent");if(jni_exception_clear(env)||!ic)return 0;
    jmethodID ctor=GetMethodID(env,ic,"<init>","()V");if(jni_exception_clear(env)||!ctor)return 0;
    jobject intent=NewObject(env,ic,ctor);if(jni_exception_clear(env)||!intent)return 0;
    jmethodID setcn=GetMethodID(env,ic,"setClassName","(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;");
    if(jni_exception_clear(env)||!setcn)return 0;
    jstring pkg=NewStringUTF(env,"com.pocketnas.wifidrive");
    jstring cls=NewStringUTF(env,"com.pocketnas.wifidrive.PocketNasService");
    if(jni_exception_clear(env)||!pkg||!cls)return 0;
    (void)CallObject(env,intent,setcn,pkg,cls);if(jni_exception_clear(env))return 0;
    jclass ac=GetObjectClass(env,a->clazz);if(jni_exception_clear(env)||!ac)return 0;
    jmethodID start=GetMethodID(env,ac,"startForegroundService","(Landroid/content/Intent;)Landroid/content/ComponentName;");
    if(jni_exception_clear(env)||!start){
        start=GetMethodID(env,ac,"startService","(Landroid/content/Intent;)Landroid/content/ComponentName;");
        if(jni_exception_clear(env)||!start)return 0;
    }
    (void)CallObject(env,a->clazz,start,intent);int bad=jni_exception_clear(env);
    if(DeleteLocalRef){DeleteLocalRef(env,cls);DeleteLocalRef(env,pkg);DeleteLocalRef(env,ac);DeleteLocalRef(env,intent);DeleteLocalRef(env,ic);}
    return bad?0:1;
}

/* ---------- activity loop / callbacks ---------- */
static void process_input(void){
    spin_lock(&g_input_lock);AInputQueue*q=g_input;if(!q){spin_unlock(&g_input_lock);return;}while(AInputQueue_hasEvents(q)>0){AInputEvent*e=NULL;if(AInputQueue_getEvent(q,&e)<0||!e)break;int handled=0;int type=AInputEvent_getType(e);
        if(type==AINPUT_EVENT_TYPE_MOTION){int a=AMotionEvent_getAction(e)&AMOTION_EVENT_ACTION_MASK;if(a==AMOTION_EVENT_ACTION_UP){int x=(int)AMotionEvent_getX(e,0),y=(int)AMotionEvent_getY(e,0);
            if(g_password_edit_mode){
                for(int i=0;i<10;i++)if(hit(g_keypad[i],x,y)){if(g_password_edit_len<16){char d=(i==9)?'0':(char)('1'+i);g_password_edit[g_password_edit_len++]=d;g_password_edit[g_password_edit_len]=0;}handled=1;break;}
                if(!handled&&hit(g_keypad[10],x,y)){if(g_password_edit_len>0)g_password_edit[--g_password_edit_len]=0;handled=1;}
                else if(!handled&&hit(g_keypad[11],x,y)){if(g_password_edit_len>=4){str_copy(g_password,sizeof(g_password),g_password_edit);g_password_custom=1;g_password_edit_mode=0;save_config();set_status("PERMANENT PASSWORD SAVED");}else set_status("PASSWORD MUST BE 4-16 DIGITS");handled=1;}
                else if(!handled&&hit(g_keypad[12],x,y)){g_password_edit_mode=0;g_password_edit_len=0;g_password_edit[0]=0;set_status("PASSWORD CHANGE CANCELLED");handled=1;}
            }else if(hit(g_btn_start,x,y)){if(g_server_running)server_stop();else server_start();handled=1;}
            else if(hit(g_btn_ro,x,y)){g_readonly=!g_readonly;save_config();handled=1;}
            else if(hit(g_btn_setpass,x,y)){g_password_edit_mode=1;g_password_edit_len=0;g_password_edit[0]=0;set_status("ENTER PERMANENT PASSWORD");handled=1;}
            else if(hit(g_btn_auth,x,y)){if(g_auth){g_auth=0;save_config();set_status("AUTHENTICATION OFF");}else if(!g_password_custom){g_password_edit_mode=1;g_password_edit_len=0;g_password_edit[0]=0;set_status("SET PASSWORD BEFORE ENABLING AUTH");}else{g_auth=1;save_config();set_status("AUTHENTICATION ON");}handled=1;}
        }}else if(type==AINPUT_EVENT_TYPE_KEY){if(AKeyEvent_getAction(e)==AKEY_EVENT_ACTION_UP&&AKeyEvent_getKeyCode(e)==AKEYCODE_BACK){if(g_password_edit_mode){g_password_edit_mode=0;g_password_edit_len=0;g_password_edit[0]=0;set_status("PASSWORD CHANGE CANCELLED");}else ANativeActivity_finish(g_activity);handled=1;}}
        AInputQueue_finishEvent(q,e,handled);
    }spin_unlock(&g_input_lock);
}
static void *monitor_loop(void*u){(void)u;char previous[64]="";while(1){char now[64];uint32_t a;if(find_lan_ip(now,&a)){int changed=strcmp(now,previous)!=0;str_copy(g_ip,sizeof(g_ip),now);str_copy(previous,sizeof(previous),now);if(changed&&g_server_running){server_stop();g_server_wanted=1;}if(g_server_wanted&&!g_server_running&&!g_server_starting)server_start();}else{str_copy(g_ip,sizeof(g_ip),"NO LAN IP");previous[0]=0;if(g_server_running){server_stop();g_server_wanted=1;}}usleep(1000000);}return NULL;}
static void ensure_monitor(void){if(__atomic_exchange_n(&g_monitor_started,1,__ATOMIC_ACQ_REL))return;pthread_t t;if(pthread_create(&t,NULL,monitor_loop,NULL)==0)pthread_detach(t);else g_monitor_started=0;}
static void *app_loop(void*u){(void)u;while(g_activity_alive){process_input();spin_lock(&g_window_lock);ANativeWindow*w=g_window;if(w)ANativeWindow_acquire(w);spin_unlock(&g_window_lock);if(w){draw_ui(w);ANativeWindow_release(w);}usleep(100000);}return NULL;}
static void cb_resume(ANativeActivity*a){(void)a;g_activity_alive=1;refresh_storage();ensure_monitor();if(g_server_wanted&&!g_server_running)server_start();}
static void cb_destroy(ANativeActivity*a){(void)a;g_activity_alive=0;g_activity=NULL;}
static void cb_window_created(ANativeActivity*a,ANativeWindow*w){(void)a;spin_lock(&g_window_lock);if(g_window)ANativeWindow_release(g_window);g_window=w;if(g_window)ANativeWindow_acquire(g_window);spin_unlock(&g_window_lock);}
static void cb_window_resized(ANativeActivity*a,ANativeWindow*w){(void)a;(void)w;}
static void cb_window_redraw(ANativeActivity*a,ANativeWindow*w){(void)a;draw_ui(w);}
static void cb_window_destroyed(ANativeActivity*a,ANativeWindow*w){(void)a;spin_lock(&g_window_lock);if(g_window==w){ANativeWindow_release(g_window);g_window=NULL;}spin_unlock(&g_window_lock);}
static void cb_input_created(ANativeActivity*a,AInputQueue*q){(void)a;spin_lock(&g_input_lock);g_input=q;spin_unlock(&g_input_lock);}
static void cb_input_destroyed(ANativeActivity*a,AInputQueue*q){(void)a;(void)q;spin_lock(&g_input_lock);g_input=NULL;spin_unlock(&g_input_lock);}

__attribute__((visibility("default"))) void ANativeActivity_onCreate(ANativeActivity *a,void *saved,size_t savedSize){(void)saved;(void)savedSize;g_activity=a;g_destroyed=0;g_activity_alive=1;derive_root(a->externalDataPath);if(a->internalDataPath){snprintf(g_config_path,sizeof(g_config_path),"%s/pocketnas.conf",a->internalDataPath);}ensure_config_loaded();refresh_storage();find_lan_ip(g_ip,NULL);a->callbacks->onResume=cb_resume;a->callbacks->onDestroy=cb_destroy;a->callbacks->onNativeWindowCreated=cb_window_created;a->callbacks->onNativeWindowResized=cb_window_resized;a->callbacks->onNativeWindowRedrawNeeded=cb_window_redraw;a->callbacks->onNativeWindowDestroyed=cb_window_destroyed;a->callbacks->onInputQueueCreated=cb_input_created;a->callbacks->onInputQueueDestroyed=cb_input_destroyed;ANativeActivity_setWindowFlags(a,FLAG_KEEP_SCREEN_ON,0);g_server_wanted=1;ensure_monitor();server_start();int bg=request_foreground_service(a);if(!bg)set_status("SERVER RUNNING - BG SERVICE DEFERRED");pthread_t t;if(pthread_create(&t,NULL,app_loop,NULL)==0)pthread_detach(t);}

/* JNI entry points used by PocketNasService.  Parameters are intentionally opaque: no JNI calls are needed. */
__attribute__((visibility("default"))) void Java_com_pocketnas_wifidrive_PocketNasService_nativeStart(void *env,void *clazz){(void)env;(void)clazz;g_service_active=1;g_destroyed=0;ensure_config_loaded();refresh_storage();g_server_wanted=1;ensure_monitor();server_start();}
__attribute__((visibility("default"))) void Java_com_pocketnas_wifidrive_PocketNasService_nativeStop(void *env,void *clazz){(void)env;(void)clazz;g_service_active=0;g_server_wanted=0;server_stop();}
