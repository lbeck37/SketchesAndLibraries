//********************************************************************************
// Set Wifi AP Mode config
//********************************************************************************
void WifiAPconfig()
{
  Serial << LOG0 << " WifiAPconfig(): Begin" << endl;
  // create and store unique AP SSID/PW to prevent ESP from starting AP mode with default SSID and No password!
  char ap_ssid[20];
  ap_ssid[0] = 0;
  strcpy(ap_ssid, "ESP_");
  sprintf_P(ap_ssid, PSTR("%s%u"), ap_ssid, Settings.Unit);

  // setup ssid for AP Mode when needed
  Serial << LOG0 << " WifiAPconfig(): Call WiFi.softAP( "<< ap_ssid << ", " << Settings.WifiAPKey
         << ")" << endl;
  WiFi.softAP(ap_ssid, Settings.WifiAPKey);

  // We start in STA mode
  Serial << LOG0 << " WifiAPconfig(): Call WiFi.mode(WIFI_STA)" << endl;
  WiFi.mode(WIFI_STA);
} //WifiAPconfig


//********************************************************************************
// Set Wifi AP Mode
//********************************************************************************
void WifiAPMode(boolean state)
{
  Serial << LOG0 << " WifiAPMode(): Begin" << endl;
  if (state)
  {
    AP_Mode = true;
    char ap_ssid[20];
    ap_ssid[0] = 0;
    strcpy(ap_ssid, "ESP_");
    sprintf_P(ap_ssid, PSTR("%s%u"), ap_ssid, Settings.Unit);
    Serial.println(F("WIFI : Starting AP Mode"));
    WiFi.softAP(ap_ssid, Settings.WifiAPKey);
    WiFi.mode(WIFI_AP_STA);
  }
  else
  {
    AP_Mode = false;
    Serial.println(F("WIFI : Ending AP Mode"));
    WiFi.mode(WIFI_STA);
  }
} //WifiAPMode


//********************************************************************************
// Connect to Wifi AP
//********************************************************************************
boolean WifiConnect()
{
  Serial << LOG0 << " WifiConnect(): Begin" << endl;
  //Serial.println(F("WIFI : Connecting..."));
  if (WiFi.status() != WL_CONNECTED)
  {
    if ((Settings.WifiSSID[0] != 0) && (strcasecmp(Settings.WifiSSID, "ssid") != 0))
    {
      Serial << LOG0 << " WifiConnect(): Call WiFi.begin( "<< Settings.WifiSSID << ", "
             << Settings.WifiKey << ")" << endl;
      WiFi.begin(Settings.WifiSSID, Settings.WifiKey);
      for (byte x = 0; x < 10; x++)
      {
        if (WiFi.status() != WL_CONNECTED)
        {
          delay(500);
          Serial.print(".");
        } //if(WiFi.status()!=WL_CONNECTED)
        else {
          break;
        } //else
      } //for
      Serial.println();

      // fix ip if last octet is set
      if (Settings.IP_Octet != 0 && Settings.IP_Octet != 255)
      {
        IPAddress ip = WiFi.localIP();
        IPAddress gw = WiFi.gatewayIP();
        IPAddress subnet = WiFi.subnetMask();
        ip[3] = Settings.IP_Octet;
        Serial.print(F("IP   : Fixed IP :"));
        Serial.println(ip);
        WiFi.config(ip, gw, subnet);
      } //if(Settings.IP_Octet!=0

      if (Settings.IP[0] != 0 && Settings.IP[0] != 255)
      {
        Serial.print(F("IP   : Static IP :"));
        char str[20];
        sprintf_P(str, PSTR("%u.%u.%u.%u"), Settings.IP[0], Settings.IP[1], Settings.IP[2], Settings.IP[3]);
        Serial.println(str);
        IPAddress ip = Settings.IP;
        IPAddress gw = Settings.Gateway;
        IPAddress subnet = Settings.Subnet;
        WiFi.config(ip, gw, subnet);
      } //if(Settings.IP[0]!=0...
    } //if((Settings.WifiSSID[0]!=0)&&...
    else
    {
      Serial.println(F("WIFI : No SSID!"));
      NC_Count = 1;
      WifiAPMode(true);
    } //if((Settings.WifiSSID[0]!=0)&&...else
  } //if (WiFi.status() != WL_CONNECTED)
  Serial << LOG0 << " WifiConnect(): Exit" << endl;
  //return;
} //WifiConnect


//********************************************************************************
// Disconnect from Wifi AP
//********************************************************************************
boolean WifiDisconnect()
{
  Serial << LOG0 << " WifiDisconnect(): Begin" << endl;
  WiFi.disconnect();
} //WifiDisconnect


//********************************************************************************
// Scan all Wifi Access Points
//********************************************************************************
void WifiScan()
{
  Serial << LOG0 << " WifiScan(): Begin" << endl;
  Serial.println(F("WIFI : SSID Scan start"));
  int n = WiFi.scanNetworks();
  Serial.println(F("WIFI : Scan done"));
  if (n == 0)
    Serial.println(F("WIFI : No networks found"));
  else
  {
    Serial.print(F("WIFI : "));
    Serial.print(n);
    Serial.println(F(" networks found"));
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(F("WIFI : "));
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      //Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      Serial.println("");
      delay(10);
    }
  }
  Serial.println("");
} //WifiScan


//********************************************************************************
// Check if we are still connected to a Wifi AP
//********************************************************************************
void WifiCheck()
{
  //Serial << LOG0 << " WifiCheck(): Begin" << endl;
  if (WiFi.status() != WL_CONNECTED)
  {
    NC_Count++;
    if (NC_Count > 10 && !AP_Mode)
    {
      C_Count = 0;
      WifiAPMode(true);
    }
  }
  else
  {
    C_Count++;
    NC_Count = 0;
    if (C_Count > 60)
    {
      byte wifimode = wifi_get_opmode();
      if (wifimode == 2 || wifimode == 3) //apmode is active
      {
        Serial.println(F("WIFI : Return to STA mode"));
        WifiAPMode(false);
      }
    }
  }
} //WifiCheck
//Last line.