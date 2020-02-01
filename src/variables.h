
const char* clientID = "FishFeeder_001";
const char* mqtt_server = "makeradmin.ddns.net";
const char* mqtt_user = "linaro";
const char* mqtt_pwd = "Che11as!1";
const char* out_topic = "fish/001/out";
const char* in_topic = "fish/001/in";
const char* config_topic = "fish/001/config";
const char* reboot_topic = "fish/001/reboot";

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