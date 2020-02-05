
char mqtt_server[] = "makeradmin.ddns.net";
char mqtt_user[] = "maker";
char mqtt_pwd[] = "18arno10";
char http_update[] = "http://tarantl.com/fishfeeder/initial/firmware.bin";

long lastMsg = 0;
char msg[500];
int degrees = 360;
int interval = 10;
int startHour = 6;
int endHour = 18;
int turned = 1;             // set to 0 and it rotates on startup
bool enableWifi = true;     // standalone mode without wifi -> false ... not working at the moment

bool debugMode = false;      
int currentHour = -1;
int currentMinute = -1;
int currentEpochTime = -1;
int loopHour = -1;
int loopMinute = -1;
int loopEpochTime = -1;
int configLoaded = 0;
int celsius = 15;

#define xstr(a) str(a)
#define str(a) #a

#define tempPin D1

#define dirPin D4
#define stepPin D3
#define enablePin D2
#define stepsPerRevolution 200
#define RPM 120
#define MICROSTEPS 1

char clientID[] = xstr(FNAME);
char version[] = xstr(FVERSION);
char out_topic[] = xstr(FOUTTOPIC);
char in_topic[] = xstr(FINTOPIC);
char config_topic[] = xstr(FCONFIGTOPIC);
char reboot_topic[] = xstr(FREBOOTTOPIC);