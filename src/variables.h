char clientID[] = "FishFeeder_initial";
//const char* mqtt_server = "makeradmin.ddns.net";
//const char* mqtt_user = "linaro";
//const char* mqtt_pwd = "Che11as!1";

char mqtt_server[] = "mrtarantl.ddns.net";
char mqtt_user[] = "megara";
char mqtt_pwd[] = "Che11as!1";

char out_topic[] = "fish/initial/out";
char in_topic[] = "fish/initial/in";
char config_topic[] = "fish/initial/config";
char reboot_topic[] = "fish/initial/reboot";


long lastMsg = 0;
char msg[300];
int degrees = 360;
int amount = 1;
int startHour = 0;
int endHour = 0;
int turned = 1;             // set to 0 and it rotates on startup
int currentHour = -1;
int currentMinute = -1;
int loopHour = -1;
int loopMinute = -1;
bool enableWifi = true;     // standalone mode without wifi -> false

bool SDpresent = true;

#define stepsPerRevolution 200
#define RPM 120
#define MICROSTEPS 1
#define dirPin 23
#define stepPin 19
#define enablePin 18