/////////////////////////////////////////////////////////////
// ---------------PROGRAMA ESP32---------------------------//
// ---------------Gestor Excedentes------------------------//
// ---------------01/03/2020-------------------------------//
/////////////////////////////////////////////////////////////

// --- INICIALIZACION --- //
#include <WiFi.h>
#include <TimeLib.h> 
#include <esp32ModbusTCP.h>
#include <WebServer.h>
#include <ArduinoOTA.h>
#include "Nextion.h"
#include "EEPROM.h"
#include <PID_v1.h>
#include <RBDdimmer.h>

#define WIFI_SSID "XXXXXXX" 
#define WIFI_PASSWORD "XXXXXXX"
#define LISTEN_PORT 80
#define AP_SSID "DER_1"                  // --- Colocar el numero de Derivador --- //
#define CLAVE_AP "XXXXXXXX"
#define Derivador  1                    // --- Colocar el numero de Derivador --- //
#define DEBUG false
String Nombre_Dispositivo = "XXXXXXX" ; // --- Colocar el nombre de Derivador --- //
#define outputPin  2 
#define zerocross  32
#define Dimmer_LIMIT 50                 // --- <limite de Potencia (MAXIMO 90) --- //
#define Absortion_SOC 95                // --- Estado de Carga en Absorcion ---//

// --- Dimmer --- //
dimmerLamp Dimmer(outputPin, zerocross); 

// --- PID --- //
double KP = 0.0 ;
double KI = 0.0 ;
double KD = 0.0 ;
double SP_PID, IN_PID, OUT_PID;
PID POWER_PID(&IN_PID, &OUT_PID, &SP_PID, KP, KI, KD, REVERSE);

// --- Inicializacion Servidor NTP --- //
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
struct      tm timeinfo;

// --- Inicializacion Modbus TCP --- //
esp32ModbusTCP Victron_MB_Sys(100, {192, 168, 3, 240}, 502);
esp32ModbusTCP Victron_MB_Mul(239, {192, 168, 3, 240}, 502);

struct victronData {
  //const char* name;
  uint16_t address;
  //uint16_t length;
};

victronData Registro_Sys[] = {
  842,    //POT_BAT
  850     //POT PV
};
victronData Registro_Mul[] = {
  30,    //SOC
};
uint32_t MB_Data_Sys = 0 ;
uint32_t MB_Data_Mul = 0 ;

//--- Datos --- //
  boolean Estado_Interruptor = true;
  boolean Reserva[15] ; 
  int p_max = 60;
  int p_min = 20; 
  int reg_pos_max = 60;
  int reg_pos_min = 30; 
  int range_max = 300;
  int range_min = 100;
  int tiempo = 650; 
  int e_inc_max = 2 ; 
  int d_inc_max = 0 ; 
  int e_inc_min = 1 ;
  int d_inc_min = 0 ;
  int e_inc_def = 0 ; 
  int d_inc_def = 5 ;

float inc_max = e_inc_max + float(d_inc_max/10.0) ; 
float inc_min = e_inc_min + float(d_inc_min/10.0) ;
float inc_def = e_inc_def + float(d_inc_def/10.0) ; 

// --- Inicializacion Websever --- //
WebServer server(LISTEN_PORT);

// --- INICIALIZACION 2--- //
bool Marca = false;
bool WIFI_OK = false ;
bool PID_ON = false ;
bool Startup = false ;
bool Horario = false ;
bool More_Power = false ;
byte Hora = 0 ;
byte Hora_Old = 0 ;
byte Minuto = 0 ;
byte Minuto_Old = 0 ;
byte Segundo = 0 ;
byte Segundo_Old = 0 ;
byte Segundo_Old_2 = 0 ;
byte Mes = 1 ;
byte Dia = 1 ;
byte Dia_Semana = 1 ;
byte Hora_Encender[] = {0,0} ;
byte Hora_Apagar[] = {0,0}  ;
byte cnt_Auto_Off = 0 ;
char Pagina[2550];
uint8_t numSysRegisters = sizeof(Registro_Sys) / sizeof(Registro_Sys[0]);
uint8_t numMulRegisters = sizeof(Registro_Mul) / sizeof(Registro_Mul[0]);
uint8_t cnt_MB = 0;
uint8_t cnt_Reboot_MB = 0;
uint16_t Anyo = 2000 ;
uint16_t i = 1 ;
float PWM_VALUE = 0.0; 
float SOC = 0.0 ;
int32_t Bat_Power = 0 ;
int32_t PV_Power = 0 ;
double value_PWM_alt = 0;
long Contador = 0 ;
String IP ;
String sEstado_Interruptor[15] = "Desconectado" ;

// --- SETUP --- //
void setup(){
  Serial.begin(9600);
  Conexion_Wifi() ;
  Init_EEPRON();
  Conexion_OTA(); 
  Init_Modbus_Sys();
  Init_Modbus_Mul();
  //POWER_PID.SetMode(AUTOMATIC);
  Dimmer.begin(NORMAL_MODE, OFF);  //Inicializacion Dimmer: name.begin(MODE, STATE) 
  Control_Dimmer(2);              // Escribir Salida Dimmer 
  Init_Nextion();
  delay(1000);
  Lectura_EEPRON();
}

// --- Main Loop --- //
void loop() {
  // --- Salida Dimmer Inicial --- //
  if(PWM_VALUE < 2)Control_Dimmer(2);             // Escribir Salida Dimmer 
  
  // --- ACTUALIZAR HORA --- //
  if (Hora != timeinfo.tm_hour) Hora = timeinfo.tm_hour ;
  if (Minuto != timeinfo.tm_min) Minuto = timeinfo.tm_min ;
  if (Segundo != timeinfo.tm_sec) Segundo = timeinfo.tm_sec ;
  if (Dia != timeinfo.tm_mday) Dia = timeinfo.tm_mday ;
  if (Dia_Semana != timeinfo.tm_wday) Dia_Semana = timeinfo.tm_wday ;
  if (Mes != timeinfo.tm_mon) Mes = timeinfo.tm_mon;
  if (Anyo != timeinfo.tm_year) Anyo = timeinfo.tm_year ;

  // --- Mantener Activo OTA --- //
  ArduinoOTA.handle();

  // --- Mantener Activo Weberver --- //
  server.handleClient();

  // --- Control Botones Nextion --- //
  Control_Botones_Nextion();
     
  // ---MARCA DE CICLO --- // 
  if ((millis() - tiempo) > Contador ){ 
    Contador = millis();
    Marca = true;
  }
  else Marca = false;

  // --- Marca Ciclo 1s --- //
  if(Segundo != Segundo_Old){
    Segundo_Old = Segundo ;
  }
    
  // --- Control Horario --- //
  if(Horario){
    if (Hora_Encender[0] == Hora and Hora_Encender[1] == Minuto){
      Estado_Interruptor = true ;
    }
    if (Hora_Apagar[0] == Hora and Hora_Apagar[1] == Minuto){
      Estado_Interruptor = false ;
    }
  } 

  // --- Lectura Modbus y Control PWM --- //
  static uint32_t lastMillis = 0;  //
  if ((millis() - lastMillis > tiempo and WIFI_OK)) {
    lastMillis = millis();
    
    // -- Leer Modbus_Sys -- //  
    Leer_Modbus_Sys();

    // -- Leer Modbus_Mul -- //  
    Leer_Modbus_Mul();

    // -- Control PWM -- //
    if(cnt_MB ==1)Control_PWM();
  }
  
  // --- Limites --- //
  Limites();

  // --- Generar Pagina WEBSERVER --- //
  Generar_WEB() ;

  // --- Control Animaciones Nextion --- /
  Control_Animaciones_Nextion();
}
