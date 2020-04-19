// --- Conexion Wifi ---//
void Conexion_Wifi(){
  // -- IP Estatica -- //
  /*IPAddress wifiIp(192, 168, 3, 150 + Derivador);
  IPAddress wifiNet(255, 255, 255, 0);
  IPAddress wifiGW(192, 168, 3, 1);
  IPAddress wifiDNS1(8, 8, 8, 8);
  IPAddress wifiDNS2(8, 8, 4, 4);
  WiFi.config(wifiIp,wifiDNS1,wifiGW,wifiNet); */
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Conectando WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  WIFI_OK = true;
  Serial.println("Wifi Conectado");
  IP = WiFi.localIP().toString() ;
  Serial.println(IP);

  // --- Obtener Hora --- //
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Escribir_Hora();
  setTime(timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec,
  timeinfo.tm_mday,timeinfo.tm_mon,timeinfo.tm_year);
    
  // --- Control Webserver --- //
  Control_WEBSERVER();
}
