
char version[6] = "0.0.1";    // Firmware version

char clientID[] = "FishFeeder_initial";
char mqtt_server[] = "makeradmin.ddns.net";
char mqtt_user[] = "maker";
char mqtt_pwd[] = "18arno10";

char out_topic[] = "fish/initial/telemetry";
char in_topic[] = "fish/initial/command";
char config_topic[] = "fish/initial/config";
char reboot_topic[] = "fish/initial/reboot";

long lastMsg = 0;
char msg[500];
int degrees = 360;
int interval = 10;
int startHour = 6;
int endHour = 18;
int turned = 1;             // set to 0 and it rotates on startup
bool enableWifi = true;     // standalone mode without wifi -> false ... not working at the moment

bool debugMode = true;      
int currentHour = -1;
int currentMinute = -1;
int currentEpochTime = -1;
int loopHour = -1;
int loopMinute = -1;
int loopEpochTime = -1;
