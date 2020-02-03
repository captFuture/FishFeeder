const char* clientID = "FishFeeder_032";
//const char* mqtt_server = "makeradmin.ddns.net";
//const char* mqtt_user = "linaro";
//const char* mqtt_pwd = "Che11as!1";

const char *mqtt_server = "192.168.0.105";
const char *mqtt_user = "megara";
const char *mqtt_pwd = "Che11as!1";

const char* out_topic = "fish/032/out";
const char* in_topic = "fish/032/in";
const char* config_topic = "fish/032/config";
const char* reboot_topic = "fish/032/reboot";

long lastMsg = 0;
char msg[50];
int degrees = 360;
int amount = 1;
int startHour = 0;
int endHour = 0;
int turned = 1;             // set to 0 and it rotates on startup
int currentHour = -1;
int loopHour = -1;
bool enableWifi = true;     // standalone mode without wifi -> false

#define dirPin 23
#define stepPin 19
#define enablePin 18