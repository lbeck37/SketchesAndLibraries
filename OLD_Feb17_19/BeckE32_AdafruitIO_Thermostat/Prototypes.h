// 11/12/18 Beck function protos from sloeber.ino.cpp generated for:
// BeckE32_AdafruitIO_Thermostat.ino
//This is a automatic generated file
//This file has been generated on 2018-11-12 19:19:23

void setup();
void loop();
void  WiFiConnect(void);
void  InitOTA(void);
void  UpdateStats(void);
void UpdateTemperature(void);
char *getTime(bool Short);
char *getDate(bool Short);
void UpTime(char *Buf) ;
char *FormatText(char *Text, int Mode) ;
void AP_Config(void) ;
void DweetPost(void) ;
void GoogleCommand(AdafruitIO_Data *data) ;
void IOSchedule(AdafruitIO_Data *data) ;
void IOCommand(AdafruitIO_Data *data) ;
float Calc_kWh(unsigned long HeatTime) ;
float Calc_HeatingCost(unsigned long HeatTime) ;
char *FusedMAC(void) ;
int rssiToQualityPercentage(void) ;
void renderRSSIIcon(void) ;
bool findService(char *ScanName, int *Count) ;
time_t getNTPTime(void) ;
void ArduinoLogo(int Pause) ;
unsigned int Calc_CRC(void) ;
void  ProcessClient(void) ;
void SendIcon(WiFiClient WebClient) ;
void  WebPageHead(WiFiClient WebClient, char *PageTitle, int ChartType) ;
void  WebPageChart(WiFiClient WebClient, int ChartType) ;
void  SendSchedule(WiFiClient WebClient, bool isValid) ;
float ScheduledTemp(float Default) ;
void  BroadcastSchedule(WiFiClient WebClient, String Line) ;
