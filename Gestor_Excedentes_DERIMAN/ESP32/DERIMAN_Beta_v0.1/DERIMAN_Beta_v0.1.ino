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

#define WIFI_SSID "XXXXXX"              // --- Colocar Nombre de red WIFI --- //
#define WIFI_PASSWORD "XXXXXXX"         // --- Colocar Contraseña de red WIFI --- //
#define LISTEN_PORT 80
#define AP_SSID "DER_1"                 // --- Colocar el numero de Derivador --- //
#define CLAVE_AP "00001112"
#define Derivador  1                    // --- Colocar el numero de Derivador --- //
#define DEBUG false
String Nombre_Dispositivo = "XXXXXXX" ; // --- Colocar el nombre de Derivador --- //
#define outputPin  2 
#define zerocross  32 //12
#define DIMMER_LIMIT 50                 // --- <limite de Potencia (MAXIMO 90) --- //

// --- DIMMER --- //
dimmerLamp dimmer(outputPin, zerocross); 

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
esp32ModbusTCP Victron_MB_Mul(237, {192, 168, 3, 240}, 502);

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
  21,    //FREQ
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
byte Control_Interruptor = 10 ; // 10 Habilitado,20 Deshabilitado) //
byte Control_Horario = 10 ; // 10 Habilitado,20 Deshabilitado) //
byte cnt_Auto_Off = 0 ;
char Pagina[2550];
uint8_t numSysRegisters = sizeof(Registro_Sys) / sizeof(Registro_Sys[0]);
uint8_t numMulRegisters = sizeof(Registro_Mul) / sizeof(Registro_Mul[0]);
uint8_t cnt_MB = 0;
uint8_t cnt_Reboot_MB = 0;
uint16_t Anyo = 2000 ;
uint16_t i = 1 ;
float PWM_VALUE = 0.0; 
float Out_Freq = 0.0 ;
int32_t Bat_Power = 0 ;
int32_t PV_Power = 0 ;
double value_PWM_alt = 0;
long Contador = 0 ;
String IP ;
String sEstado_Interruptor[15] = "Desconectado" ;

// --- Inicializacion Variables Nextion --- //
bool nxPrimer_Ciclo = true ;
bool nxBoton_Pulsado = false;
bool nxMore_Power = false ;
bool nxEstado_Interruptor = false ;
byte nxDia,nxMes ;
byte nxHora,nxMinuto ;
byte nxPagina = 0 ;
byte nxPagina_Old = 5 ;
int nxPosicion = 0 ;
int nxAnyo = 2000;
int nxValor = 0;
int nxValor_2 = 0;
int eInc = 0 ;
int dInc = 0 ;
int nxBat_Power ;
int nxPV_Power ;
double nxInc ;
double nxInc_2;
double nxInc_3;
double nxP ;
double nxI ;
double nxD ;


// --- Variables Nextion --- //
// -- Paginas -- //
NexPage npPrincipal       = NexPage(0, 0, "Principal");
NexPage npConfiguracion   = NexPage(1, 0, "Configuracion");
NexPage npIncrementos     = NexPage(2, 0, "Incrementos");
NexPage npReg_Posicion    = NexPage(3, 0, "Reg_Posicion");
NexPage npTiempo          = NexPage(4, 0, "Tiempo");
NexPage npConsignas       = NexPage(5, 0, "Consignas");
NexPage npPID             = NexPage(6, 0, "PID");
NexPage npEEPRON          = NexPage(7, 0, "EEPRON");

// -- Pagina 0 -- //
NexNumber nHoras_P0      = NexNumber(0, 14, "nHoras");
NexNumber nMinutos_P0    = NexNumber(0, 13, "nMinutos");
NexNumber nDia_P0        = NexNumber(0, 5, "nDia");
NexNumber nMes_P0        = NexNumber(0, 7, "nMes");
NexNumber nAnio_P0       = NexNumber(0, 8, "nAnio");
NexNumber nBat_Power_P0  = NexNumber(0, 11, "nBat_Power");
NexNumber nPV_Power_P0   = NexNumber(0, 10, "nPV_Power");
NexNumber nPosicion_P0   = NexNumber(0, 4, "nPosicion");
NexProgressBar pbPila    = NexProgressBar(0,15,"pbPila");
NexButton bControl_P0    = NexButton(0, 1, "bControl");
NexButton bConfig_P0     = NexButton(0, 2, "bConfig");
NexText tEstado_P0       = NexText(0, 3, "tEstado");

// -- Pagina 1 -- //
NexNumber nHoras_P1      = NexNumber(1, 8, "nHoras");
NexNumber nMinutos_P1    = NexNumber(1, 10, "nMinutos");
NexNumber nDia_P1        = NexNumber(1, 11, "nDia");
NexNumber nMes_P1        = NexNumber(1, 13, "nMes");
NexNumber nAnio_P1       = NexNumber(1, 15, "nAnio");
NexButton bIncrementos_P1  = NexButton(1, 1, "bIncrementos");
NexButton bReg_Posicion_P1  = NexButton(1, 6, "bReg_Posicion");
NexButton bTiempo_P1     = NexButton(1, 7, "bTiempo");
NexButton bConsignas_P1  = NexButton(1, 16, "bConsignas");
NexButton bPID_P1        = NexButton(1, 17, "bPID");
NexButton bEEPRON_P1     = NexButton(1, 7, "bEEPRON");
NexButton bControl_P1    = NexButton(1, 2, "bControl");
NexButton bVolver_P1     = NexButton(1, 3, "bVolver");
NexText tEstado_P1       = NexText(1, 4, "tEstado");

// -- Pagina 2 -- //
NexNumber nHoras_P2      = NexNumber(2, 22, "nHoras");
NexNumber nMinutos_P2    = NexNumber(2, 24, "nMinutos");
NexNumber nDia_P2        = NexNumber(2, 25, "nDia");
NexNumber nMes_P2        = NexNumber(2, 27, "nMes");
NexNumber nAnio_P2       = NexNumber(2, 29, "nAnio");
NexNumber nInc_P2        = NexNumber(2, 5, "nInc");
NexNumber nInc_2_P2      = NexNumber(2, 7, "nInc_2");
NexNumber nInc_3_P2      = NexNumber(2, 9, "nInc_3");
NexNumber nInc_4_P2      = NexNumber(2, 11, "nInc_4");
NexNumber nInc_5_P2      = NexNumber(2, 12, "nInc_5");
NexNumber nInc_6_P2      = NexNumber(2, 14, "nInc_6");
NexButton bMas_P2        = NexButton(2, 1, "bMas");
NexButton bMenos_P2      = NexButton(2, 2, "bMenos");
NexButton bMas_2_P2      = NexButton(2, 17, "bMas_2");
NexButton bMenos_2_P2    = NexButton(2, 16, "bMenos_2");
NexButton bMas_3_P2      = NexButton(2, 20, "bMas_3");
NexButton bMenos_3_P2    = NexButton(2, 19, "bMenos_3");
NexButton bControl_P2    = NexButton(2, 15, "bControl");
NexButton bVolver_P2     = NexButton(2, 3, "bVolver");
NexText tEstado_P2       = NexText(2, 4, "tEstado");

// -- Pagina 3 -- //
NexNumber nHoras_P3      = NexNumber(3, 12, "nHoras");
NexNumber nMinutos_P3    = NexNumber(3, 14, "nMinutos");
NexNumber nDia_P3        = NexNumber(3, 15, "nDia");
NexNumber nMes_P3        = NexNumber(3, 17, "nMes");
NexNumber nAnio_P3       = NexNumber(3, 19, "nAnio");
NexNumber nReg_Pos_Max_P3      = NexNumber(3, 3, "nReg_Pos_Max");
NexNumber nReg_Pos_Min_P3      = NexNumber(3, 4, "nReg_Pos_Min");
NexButton bMas_P3        = NexButton(3, 7, "bMas");
NexButton bMenos_P3      = NexButton(3, 6, "bMenos");
NexButton bMas_2_P3      = NexButton(3, 10, "bMas_2");
NexButton bMenos_2_P3    = NexButton(3, 9, "bMenos_2");
NexButton bControl_P3    = NexButton(3, 5, "bControl");
NexButton bVolver_P3     = NexButton(3, 1, "bVolver");
NexText tEstado_P3       = NexText(3 ,2, "tEstado");

// -- Pagina 4 -- //
NexNumber nHoras_P4      = NexNumber(4, 8, "nHoras");
NexNumber nMinutos_P4    = NexNumber(4, 10, "nMinutos");
NexNumber nDia_P4        = NexNumber(4, 11, "nDia");
NexNumber nMes_P4        = NexNumber(4, 13, "nMes");
NexNumber nAnio_P4       = NexNumber(4, 15, "nAnio");
NexNumber nTiempo_P4     = NexNumber(4, 3, "nTiempo");
NexButton bMas_P4        = NexButton(4, 6, "bMas");
NexButton bMenos_P4      = NexButton(4, 5, "bMenos");
NexButton bControl_P4    = NexButton(4, 4, "bControl");
NexButton bVolver_P4     = NexButton(4, 1, "bVolver");
NexText tEstado_P4       = NexText(4 ,2, "tEstado");

// -- Pagina 5 -- //
NexNumber nHoras_P5      = NexNumber(5, 12, "nHoras");
NexNumber nMinutos_P5    = NexNumber(5, 14, "nMinutos");
NexNumber nDia_P5        = NexNumber(5, 15, "nDia");
NexNumber nMes_P5        = NexNumber(5, 17, "nMes");
NexNumber nAnio_P5       = NexNumber(5, 19, "nAnio");
NexButton bControl_P5    = NexButton(5, 5, "bControl");
NexButton bVolver_P5     = NexButton(5, 1, "bVolver");
NexText tEstado_P5       = NexText(5 ,2, "tEstado");
NexNumber nP_Max_P5      = NexNumber(5, 3, "nP_Max");
NexNumber nP_Min_P5      = NexNumber(5, 4, "nP_Min");
NexButton bMas_P5        = NexButton(5, 7, "bMas");
NexButton bMenos_P5      = NexButton(5, 6, "bMenos");
NexButton bMas_2_P5      = NexButton(5, 10, "bMas_2");
NexButton bMenos_2_P5    = NexButton(5, 9, "bMenos_2");

// -- Pagina 6 -- //
NexNumber nHoras_P6      = NexNumber(6, 22, "nHoras");
NexNumber nMinutos_P6    = NexNumber(6, 24, "nMinutos");
NexNumber nDia_P6        = NexNumber(6, 25, "nDia");
NexNumber nMes_P6        = NexNumber(6, 27, "nMes");
NexNumber nAnio_P6       = NexNumber(6, 29, "nAnio");
NexButton bControl_P6    = NexButton(6, 15, "bControl");
NexButton bVolver_P6     = NexButton(6, 3, "bVolver");
NexText tEstado_P6       = NexText(6, 4, "tEstado");
NexNumber nP_P6          = NexNumber(6, 5, "nP");
NexNumber nP2_P6         = NexNumber(6, 7, "nP2");
NexNumber nI_P6          = NexNumber(6, 9, "nI");
NexNumber nI2_P6         = NexNumber(6, 11, "nI2");
NexNumber nD_P6          = NexNumber(6, 12, "nD");
NexNumber nD2_P6         = NexNumber(6, 14, "nD2");
NexButton bMas_P6        = NexButton(6, 1, "bMas");
NexButton bMenos_P6      = NexButton(6, 2, "bMenos");
NexButton bMas_2_P6      = NexButton(6, 16, "bMas_2");
NexButton bMenos_2_P6    = NexButton(6, 15, "bMenos_2");
NexButton bMas_3_P6      = NexButton(6, 19, "bMas_3");
NexButton bMenos_3_P6    = NexButton(6, 18, "bMenos_3");

// -- Pagina 7 -- //
NexNumber nHoras_P7      = NexNumber(7, 8, "nHoras");
NexNumber nMinutos_P7    = NexNumber(7, 10, "nMinutos");
NexNumber nDia_P7        = NexNumber(7, 11, "nDia");
NexNumber nMes_P7        = NexNumber(7, 13, "nMes");
NexNumber nAnio_P7       = NexNumber(7, 15, "nAnio");
NexButton bEscribir_P7   = NexButton(7, 1, "bEscribir");
NexButton bRestaurar_P7  = NexButton(7, 6, "bRestaurar");
NexButton bControl_P7    = NexButton(7, 2, "bControl");
NexButton bVolver_P7     = NexButton(7, 3, "bVolver");
NexText tEstado_P7       = NexText(7, 4, "tEstado");

uint8_t ndt[3] = {255,255,255};

NexTouch *nex_listen_list[] = 
{
    &npPrincipal,
    &npConfiguracion,
    &npIncrementos,
    &npReg_Posicion,
    &npTiempo,
    &npConsignas,
    &npPID,
    &npEEPRON,
    &bControl_P0,
    &bControl_P1,
    &bControl_P2,
    &bControl_P3,
    &bControl_P4,
    &bConfig_P0,
    &bVolver_P1,
    &bVolver_P2,
    &bVolver_P3,
    &bVolver_P4,
    &bIncrementos_P1,
    &bReg_Posicion_P1,
    &bTiempo_P1,
    &bConsignas_P1,
    &bPID_P1,
    &bEEPRON_P1,  
    &bMas_P2,
    &bMas_2_P2,
    &bMas_3_P2,
    &bMas_P3,
    &bMas_2_P3,
    &bMas_P4,   
    &bMas_P5,
    &bMas_2_P5,
    &bMas_P6,
    &bMas_2_P6,
    &bMas_3_P6,  
    &bMenos_P2,
    &bMenos_2_P2,
    &bMenos_3_P2,
    &bMenos_P3,
    &bMenos_2_P3,
    &bMenos_P4,  
    &bMenos_P5,
    &bMenos_2_P5, 
    &bMenos_P6,
    &bMenos_2_P6,  
    &bMenos_3_P6, 
    &bEscribir_P7, 
    &bRestaurar_P7, 
    NULL
};


// --- Funciones Botones --- //
// -- Paginas -- //
void npPrincipal_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxPagina = 0 ;
  nxBoton_Pulsado = true ;
}
void npConfiguracion_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxPagina = 1 ;
  nxBoton_Pulsado = true ;
}
void npIncrementos_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxPagina = 2 ;
  nxBoton_Pulsado = true ;
}
void npReg_Posicion_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxPagina = 3 ;
  nxBoton_Pulsado = true ;
}
void npTiempo_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxPagina = 4 ;
  nxBoton_Pulsado = true ;
}
void npConsignas_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxPagina = 5 ;
  nxBoton_Pulsado = true ;
}
void npPID_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxPagina = 6 ;
  nxBoton_Pulsado = true ;
}
void npEEPRON_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxPagina = 7 ;
  nxBoton_Pulsado = true ;
}

// -- bControl -- //
void bControl_P0_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  if(Estado_Interruptor) Estado_Interruptor = false ;
  else Estado_Interruptor = true ; 
}
void bControl_P1_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  if(Estado_Interruptor) Estado_Interruptor = false ;
  else Estado_Interruptor = true ; 
}
void bControl_P2_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  if(Estado_Interruptor) Estado_Interruptor = false ;
  else Estado_Interruptor = true ; 
}
void bControl_P3_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  if(Estado_Interruptor) Estado_Interruptor = false ;
  else Estado_Interruptor = true ; 
}
void bControl_P4_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  if(Estado_Interruptor) Estado_Interruptor = false ;
  else Estado_Interruptor = true ; 
}
void bControl_P5_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  if(Estado_Interruptor) Estado_Interruptor = false ;
  else Estado_Interruptor = true ; 
}
void bControl_P6_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  if(PID_ON) PID_ON = false ;
  else PID_ON = true ; 
}
void bControl_P7_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  if(Estado_Interruptor) Estado_Interruptor = false ;
  else Estado_Interruptor = true ; 
}

// -- Config y Volver -- //
void bConfig_P0_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  nxPagina = 1 ;
}
void bVolver_P1_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  nxPagina = 0 ;
}
void bVolver_P2_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  nxPagina = 1 ;
}
void bVolver_P3_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  nxPagina = 1 ;
}
void bVolver_P4_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  nxPagina = 1 ;
}
void bVolver_P5_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  nxPagina = 1 ;
}
void bVolver_P6_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  nxPagina = 1 ;
}
void bVolver_P7_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  nxPagina = 1 ;
}

// -- Incrementos,Reg_Posicion,Tiempo,Consignas,PID,EEPRON -- //
void bIncrementos_P1_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  nxPagina = 2 ;
}
void bReg_Posicion_P1_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  nxPagina = 3 ;
}
void bTiempo_P1_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  nxPagina = 4 ;
}
void bConsignas_P1_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  nxPagina = 5 ;
}
void bPID_P1_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  nxPagina = 6 ;
}
void bEEPRON_P1_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  nxPagina = 7 ;
}

// -- Mas y Menos -- //
void bMas_P2_Funcion(void *ptr){
  inc_max = inc_max + 0.1 ;
}
void bMas_2_P2_Funcion(void *ptr){
  inc_min = inc_min + 0.1 ;
}
void bMas_3_P2_Funcion(void *ptr){
  inc_def = inc_def + 0.1 ;
}
void bMenos_P2_Funcion(void *ptr){
  inc_max = inc_max - 0.1 ;
}
void bMenos_2_P2_Funcion(void *ptr){
  inc_min = inc_min - 0.1 ;
}
void bMenos_3_P2_Funcion(void *ptr){
  inc_def = inc_def - 0.1 ;
}
void bMas_P3_Funcion(void *ptr){
  reg_pos_max = reg_pos_max + 10 ;
  if(reg_pos_max >= 100) reg_pos_max = 100 ;
}
void bMas_2_P3_Funcion(void *ptr){
  reg_pos_min = reg_pos_min + 10 ;
  if(reg_pos_min >= reg_pos_max) reg_pos_min = reg_pos_max - 10 ;
}
void bMenos_P3_Funcion(void *ptr){
  reg_pos_max = reg_pos_max - 10 ;
  if(reg_pos_max <= reg_pos_min) reg_pos_max = reg_pos_min + 10 ;
}
void bMenos_2_P3_Funcion(void *ptr){
  reg_pos_min = reg_pos_min - 10 ;
  if(reg_pos_min <= 0) reg_pos_min = 0 ;
}
void bMas_P4_Funcion(void *ptr){
  tiempo = tiempo + 10 ;
  if(tiempo >= 3000)tiempo = 3000 ;
}
void bMenos_P4_Funcion(void *ptr){
  tiempo = tiempo - 10 ;
  if(tiempo <= 100)tiempo = 100 ;
}

void bMas_P5_Funcion(void *ptr){
  p_max = p_max + 1 ;
}
void bMas_2_P5_Funcion(void *ptr){
  p_min = p_min + 1 ;
}
void bMenos_P5_Funcion(void *ptr){
  p_max = p_max - 1 ;
}
void bMenos_2_P5_Funcion(void *ptr){
  p_min = p_min - 1 ;
}
void bMas_P6_Funcion(void *ptr){
  KP = KP + 0.01 ;
  POWER_PID.SetTunings(KP, KI, KD) ; 
}
void bMas_2_P6_Funcion(void *ptr){
  KI = KI + 0.01 ;
  POWER_PID.SetTunings(KP, KI, KD) ;
}
void bMas_3_P6_Funcion(void *ptr){
  KD = KD + 0.01 ;
  POWER_PID.SetTunings(KP, KI, KD) ;
}
void bMenos_P6_Funcion(void *ptr){
  KP = KP - 0.01 ;
  POWER_PID.SetTunings(KP, KI, KD) ;
}
void bMenos_2_P6_Funcion(void *ptr){
  KI = KI - 0.01 ;
  POWER_PID.SetTunings(KP, KI, KD) ;
}
void bMenos_3_P6_Funcion(void *ptr){
  KD = KD - 0.01 ;
  POWER_PID.SetTunings(KP, KI, KD) ;
}

// -- Funciones EEPRON -- //
void bEscribir_P7_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  Escritura_EEPRON();
}
void bRestaurar_P7_Funcion(void *ptr){ 
  NexButton *btn = (NexButton *)ptr;
  nxBoton_Pulsado = true ;
  Valores_Fabrica();
}

// --- Conexion Wifi ---//
void Conexion_Wifi(){
  WiFi.disconnect(true);
  delay(1000);
  /*
  IPAddress wifiIp(192, 168, 3, 5 + Derivador);
  IPAddress wifiNet(255, 255, 255, 0);
  IPAddress wifiGW(192, 168, 3, 1);
  IPAddress wifiDNS1(8, 8, 8, 8);
  IPAddress wifiDNS2(8, 8, 4, 4);
  WiFi.config(wifiIp,wifiDNS1,wifiGW,wifiNet); */
  
  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.print("WiFi connected. IP: ");
    //Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
    IP = WiFi.localIP().toString() ;
    Serial.println(IP);
    WIFI_OK = true;

    // --- Obtener Hora --- //
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    Escribir_Hora();
    setTime(timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec,
    timeinfo.tm_mday,timeinfo.tm_mon,timeinfo.tm_year);
    
    // --- Control Webserver --- //
    Control_WEBSERVER();
    
  }, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info){
    Serial.print("WiFi lost connection. Reason: ");
    Serial.println(info.disconnected.reason);
    WiFi.disconnect();
    WIFI_OK = false;
    delay(3000);
    ESP.restart(); 
  }, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println();
  Serial.println("Connecting to WiFi... ");
}

// --- Escribir_Hora --- //
void Escribir_Hora(){
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

// --- Conexion OTA --- //
void Conexion_OTA(){
  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname(AP_SSID);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
}

// --- Modbus_Sys --- //
void Init_Modbus_Sys(){
  Victron_MB_Sys.onData([](uint16_t packet, uint8_t slave, esp32Modbus::FunctionCode fc , uint8_t* data , uint16_t len) {
    for (uint8_t i = 0; i < numSysRegisters; ++i) {
      uint32_t value = 0;
      value = (data[0] << 8) | data[1];
      if(value > 32668){
        value = 65535 - value ;
        value = value * -1 ;   
        if(DEBUG)Serial.printf("%i\n", value);
      }else{
        if(DEBUG)Serial.printf("%u\n", value);
      }
      MB_Data_Sys = value ; 
      cnt_Reboot_MB = 0 ;      
      return;
    }
  });
  Victron_MB_Sys.onError([](uint16_t packet, esp32Modbus::Error e) { 
    if(e != 0){
      Serial.printf("Error packet %u: %02x\n", packet, e);
      cnt_Reboot_MB++;
      if(cnt_Reboot_MB >= 60)ESP.restart();
    }  
  });
  Serial.println("Modbus_Sys Inicializado");
}

// --- Modbus_Mul --- //
void Init_Modbus_Mul(){
  Victron_MB_Mul.onData([](uint16_t packet, uint8_t slave, esp32Modbus::FunctionCode fc , uint8_t* data , uint16_t len) {
    for (uint8_t i = 0; i < numMulRegisters; ++i) {
      uint32_t value = 0;
      value = (data[0] << 8) | data[1];
      if(value > 32668){
        value = 65535 - value ;
        value = value * -1 ;   
        if(DEBUG)Serial.printf("%i\n", value);
      }else{
        if(DEBUG)Serial.printf("%u\n", value);
      }
      MB_Data_Mul = value ; 
      cnt_Reboot_MB = 0 ;      
      return;
    }
  });
  Victron_MB_Mul.onError([](uint16_t packet, esp32Modbus::Error e) { 
    if(e != 0){
      Serial.printf("Error packet %u: %02x\n", packet, e);
      cnt_Reboot_MB++;
      if(cnt_Reboot_MB >= 60)ESP.restart();
    }  
  });
  Startup = true ;
  Serial.println("Modbus_Mul Inicializado");
}

// --- Leer Registros Modbus_Sys --- //
void Leer_Modbus_Sys(){
  uint16_t packetId = 0 ; 
  switch(cnt_MB){
    case 0: 
      packetId = Victron_MB_Sys.readHoldingRegisters(Registro_Sys[0].address, 1);
      PV_Power = MB_Data_Sys ;
      break;     
    case 1: 
      packetId = Victron_MB_Sys.readHoldingRegisters(Registro_Sys[1].address, 1);
      Bat_Power = MB_Data_Sys ;
      break;
  }
  if(cnt_MB <  numSysRegisters) cnt_MB++ ;      
  else cnt_MB = 0;
}

// --- Leer Registros Modbus_Mul --- //
void Leer_Modbus_Mul(){
  uint16_t packetId = 0 ; 
  packetId = Victron_MB_Mul.readHoldingRegisters(Registro_Mul[0].address, 1);
  Out_Freq = MB_Data_Mul/100 ;
}

// --- Control_WEBSERVER --- //
void Control_WEBSERVER(){
  // -- Server Update -- //
  //httpUpdater.setup(&server);

  // -- Control Webserver -- //
  server.on("/",[](){
    Capturar_Datos_WEBSERVER();
    server.send(200, "text/html", Pagina) ;
  });
  server.on("/ENCENDER",[](){
    Estado_Interruptor = true ;
    Capturar_Datos_WEBSERVER();
    server.send(200, "text/html", Pagina) ;
  });
  server.on("/APAGAR",[]() {
    Estado_Interruptor = false ;
    Capturar_Datos_WEBSERVER();
    server.send(200, "text/html", Pagina) ;
  });
  server.on("/HORARIO/ON",[](){
    Horario = true ;
    Capturar_Datos_WEBSERVER();
    server.send(200, "text/html", Pagina) ;
  }); 
  server.on("/HORARIO/OFF",[](){
    Horario = false ;
    Capturar_Datos_WEBSERVER();
    server.send(200, "text/html", Pagina) ;
  }); 
  server.on("/GUARDAR",[](){
    Escritura_EEPRON(); 
    server.send(200, "text/html", Pagina) ;
  });
  server.on("/CARGAR",[](){
    Lectura_EEPRON();
    server.send(200, "text/html", Pagina) ;
  });
  server.on("/REINICIAR",[]() {server.send(200, "text/plain", "Reiniciando...");ESP.restart();}); 
  server.on("/online", []() {server.send(200, "text/plain","OK");});
  server.onNotFound([](){server.send(200, "text/html", Pagina) ;});
  
  // - Iniciar Webserver - //
  server.begin();
  Serial.println("WEBSERVER Iniciado");
}

// --- Generar WEBSERVER --- //
void Generar_WEB(){  
// -- Conversiones A Caracteres -- //
char* C_WIFI = "WiFi" ;
char* C_Estado_Wifi = "NOK" ;
if (WIFI_OK) C_Estado_Wifi = "OK" ;
char* C_Estado_Interruptor = "Desconectado" ;
if (Estado_Interruptor) C_Estado_Interruptor = "Conectado" ;
char* C_Horario = "Deshabilitado" ;
if (Horario) C_Horario = "Habilitado" ;
int str_len = IP.length() + 1;  
char char_IP[str_len];
IP.toCharArray(char_IP, str_len);
int str_len_2 = Nombre_Dispositivo.length() + 1;  
char C_Nombre_Dispositivo[str_len_2];
Nombre_Dispositivo.toCharArray(C_Nombre_Dispositivo, str_len_2);

snprintf(Pagina,2550,  
"<!doctype html>\
<html>\
<head>\
<meta charset='utf-8'>\
<title>WEB_SERVER_DERIVADOR</title>\
</head>\
  <h1 class='titulo_principal' style='background-color: antiquewhite' align='center'> WEBSERVER Derivador %s %01d </h1>\
<body bgcolor='#7F18A0'>\
<table width='1500' border='5' align='center' bgcolor='#A6A6A6'>\
  <tbody>\
    <tr>\
      <th>Variables</th>\
      <th>Estado</th>\
    </tr>\
    <tr>\
      <td align='center'>Nombre del Dispositivo</td>\
      <td align='center'>%s</td>\
    </tr>\
    <tr>\
      <td align='center'>Estado del WIFI</td>\
      <td align='center'>%s</td>\
    </tr>\
    <tr>\
      <td align='center'>Hora de Dispositivo</td>\
      <td align='center'>%02d:%02d</td>\
    </tr>\
    <tr>\
      <td align='center'>Nobre de la Red</td>\
      <td align='center'>%s</td>\
    </tr>\
    <tr>\
      <td align='center'>IP</td>\
      <td align='center'>%s</td>\
    </tr>\
    <tr>\
      <td align='center'>Estado Interruptor</td>\
      <td align='center'>%s</td>\
    </tr>\
    <tr>\
      <td align='center'>Salida</td>\
      <td align='center'>%.2f</td>\
    </tr>\
    <tr>\
      <td align='center'>Bat Power</td>\
      <td align='center'>%02d</td>\
    </tr>\
    <tr>\
      <td align='center'>PV Power</td>\
      <td align='center'>%02d</td>\
    </tr>\
    <tr>\
      <td align='center'>Out Freq</td>\
      <td align='center'>%.2f</td>\
    </tr>\
    <tr>\
      <td align='center'>Horario</td>\
      <td align='center'>%s</td>\
    </tr>\
    <tr>\
      <td align='center'>Hora de Encendido</td>\
      <td align='center'> <form  name='frm1'  method='post'><input type='text' maxlength='5' size='5' value=%02d:%02d name='Encendido'></form></td>\
    </tr>\
    <tr>\
      <td align='center'>Hora de Apagado</td>\
      <td align='center'> <form  name='frm2'  method='post'><input type='text' maxlength='5' size='5' value=%02d:%02d name='Apagado'></form></td>\
    </tr>\
    <tr>\
      <td align='center'> <button type='button' onClick=location.href='/ENCENDER'> ENCENDER </button></td>\
      <td align='center'> <button type='button' onClick=location.href='/APAGAR'> APAGAR </button></td>\
    </tr>\
    <tr>\
      <td align='center'> <button type='button' onClick=location.href='/HORARIO/ON'> HABILITAR HORARIO </button></td>\
      <td align='center'> <button type='submit' onClick=location.href='/HORARIO/OFF'> DESHABILITAR HORARIO </button></td>\
    </tr>\ 
    <tr>\
      <td align='center'> <button type='submit' onClick=location.href='/GUARDAR'> GUARDAR EEPRON </button></td>\
      <td align='center'> <button type='submit' onClick=location.href='/CARGAR'> CARGAR EEPRON </button></td>\
    </tr>\   
    <tr>\
      <td align='center'> Reiniciar Dispositivo </td>\
      <td align='center'> <button type='submit' onClick=location.href='/REINICIAR'> REINICIAR </button></td>\
    </tr>\
  </tbody>\    
</table>\
</body>\
</html>",C_WIFI,Derivador,C_Nombre_Dispositivo,C_Estado_Wifi,Hora,Minuto,WIFI_SSID,char_IP,C_Estado_Interruptor,PWM_VALUE,Bat_Power,PV_Power,Out_Freq,C_Horario,Hora_Encender[0],Hora_Encender[1],Hora_Apagar[0],Hora_Apagar[1]);
}

void Capturar_Datos_WEBSERVER(){ 
  // -- Horario Encendido -- //
  if(server.hasArg("Encendido") && (server.arg("Encendido").length()>0)){
    String S_Encender = server.arg("Encendido");
    char C_Encender[5] ;
    S_Encender.toCharArray(C_Encender, S_Encender.length() + 1);   
    if(isDigit(C_Encender[0]) and isDigit(C_Encender[1]) and isDigit(C_Encender[3]) and isDigit(C_Encender[4])){
      Hora_Encender[0] = (C_Encender[0] - 48)*10 + C_Encender[1] - 48 ;
      Hora_Encender[1] = (C_Encender[3]-  48)*10 + C_Encender[4] - 48 ;      
    }
  }
  // -- Horario Apagado -- // 
  if(server.hasArg("Apagado") && (server.arg("Apagado").length()>0)){
    String S_Apagar = server.arg("Apagado");
    char C_Apagar[5] ;
    S_Apagar.toCharArray(C_Apagar, S_Apagar.length() + 1);
    if(isDigit(C_Apagar[0]) and isDigit(C_Apagar[1]) and isDigit(C_Apagar[3]) and isDigit(C_Apagar[4])){
      Hora_Apagar[0] = (C_Apagar[0] - 48)*10 + C_Apagar[1] - 48 ;
      Hora_Apagar[1] = (C_Apagar[3]-  48)*10 + C_Apagar[4] - 48 ;
    }
  }
  // -- Limites Horarios -- //
  if(Hora_Encender[0] >= 24 || Hora_Encender[0] <= 0) Hora_Encender[0] = 0 ;
  if(Hora_Encender[1] >= 60 || Hora_Encender[1] <= 0) Hora_Encender[1] = 0 ;
  if(Hora_Apagar[0] >= 24 || Hora_Apagar[0] <= 0) Hora_Apagar[0] = 0 ;
  if(Hora_Apagar[1] >= 60 || Hora_Apagar[1] <= 0) Hora_Apagar[1] = 0 ;
   
  // --- Generar Pagina WEBSERVER --- //
  Generar_WEB() ;
}

// --- Control PWM --- //
void Control_PWM(){
  if (Estado_Interruptor == true) {
    if ( Bat_Power > p_max or DEBUG) { // Default value is < 60 w from grid an pwm < 100
      if  (Bat_Power > (p_max + range_max) and  PWM_VALUE < (100 - reg_pos_max)) PWM_VALUE+= inc_max; // Increment 15 in range 250
      else if  ( Bat_Power > (p_max + range_min) && PWM_VALUE < (100 - reg_pos_min)) PWM_VALUE+= inc_min; // Increment 3 in range 120
      PWM_VALUE += inc_def ; //Increment value
      if (PWM_VALUE >= DIMMER_LIMIT) PWM_VALUE = DIMMER_LIMIT; 
      More_Power = true ;
    }else{
      if (Bat_Power < p_min and not DEBUG) { // Value is < 90 w from grid an pwm < 125
        if  (Bat_Power < (p_max - range_max)  && PWM_VALUE > reg_pos_max) PWM_VALUE-= inc_max; // Decrement 15 in range 250
        else if  ( Bat_Power < (p_max - range_min)  && PWM_VALUE > reg_pos_min) PWM_VALUE-= inc_min; // Decrement 3 in range 120
        PWM_VALUE-= inc_def ; //Decrement value 
        if (PWM_VALUE <= 2) PWM_VALUE = 2;
        More_Power = false ;
      }
   }
   Control_Dimmer(PWM_VALUE); 
  }else{
    PWM_VALUE = 2;
    Control_Dimmer(PWM_VALUE);
  }
}

// --- Control Dimmer --- //
void Control_Dimmer(double Value){
  if(Value >= DIMMER_LIMIT) Value = DIMMER_LIMIT ;
  if(Value <= 2){
    Value = 2 ;
    dimmer.setPower(Value);  // Escribir Salida Dimmer
    digitalWrite(outputPin,LOW);
  }else dimmer.setPower(Value);  // Escribir Salida Dimmer 
}

// --- Inicializacion Nextion --- //
void Init_Nextion(){
  nexInit();  // --- Inicializacion de Nextion --- // 
  npPrincipal.attachPop(npPrincipal_Funcion, &npPrincipal); 
  npConfiguracion.attachPop(npConfiguracion_Funcion, &npConfiguracion); 
  npIncrementos.attachPop(npIncrementos_Funcion, &npIncrementos); 
  npReg_Posicion.attachPop(npReg_Posicion_Funcion, &npReg_Posicion); 
  npTiempo.attachPop(npPrincipal_Funcion, &npTiempo); 
  npConsignas.attachPop(npConsignas_Funcion, &npConsignas); 
  npPID.attachPop(npPID_Funcion, &npPID); 
  npEEPRON.attachPop(npEEPRON_Funcion, &npEEPRON); 
  
  bControl_P0.attachPop(bControl_P0_Funcion, &bControl_P0);
  bControl_P1.attachPop(bControl_P1_Funcion, &bControl_P1);
  bControl_P2.attachPop(bControl_P2_Funcion, &bControl_P2);
  bControl_P3.attachPop(bControl_P3_Funcion, &bControl_P3);
  bControl_P4.attachPop(bControl_P4_Funcion, &bControl_P4);
  bControl_P5.attachPop(bControl_P5_Funcion, &bControl_P5);
  bControl_P6.attachPop(bControl_P6_Funcion, &bControl_P6);
  bControl_P7.attachPop(bControl_P7_Funcion, &bControl_P7);
  
  bConfig_P0.attachPop(bConfig_P0_Funcion, &bConfig_P0);
  bVolver_P1.attachPop(bVolver_P1_Funcion, &bVolver_P1);
  bVolver_P2.attachPop(bVolver_P2_Funcion, &bVolver_P2);
  bVolver_P3.attachPop(bVolver_P3_Funcion, &bVolver_P3);
  bVolver_P4.attachPop(bVolver_P4_Funcion, &bVolver_P4);
  bVolver_P5.attachPop(bVolver_P5_Funcion, &bVolver_P5);
  bVolver_P6.attachPop(bVolver_P6_Funcion, &bVolver_P6);
  bVolver_P7.attachPop(bVolver_P7_Funcion, &bVolver_P7);
  
  bIncrementos_P1.attachPop(bIncrementos_P1_Funcion, &bIncrementos_P1); 
  bReg_Posicion_P1.attachPop(bReg_Posicion_P1_Funcion, &bReg_Posicion_P1);
  bTiempo_P1.attachPop(bTiempo_P1_Funcion, &bTiempo_P1); 
  bConsignas_P1.attachPop(bConsignas_P1_Funcion, &bConsignas_P1); 
  bPID_P1.attachPop(bPID_P1_Funcion, &bPID_P1);
  bEEPRON_P1.attachPop(bEEPRON_P1_Funcion, &bEEPRON_P1); 
  
  bMas_P2.attachPop(bMas_P2_Funcion, &bMas_P2); 
  bMas_2_P2.attachPop(bMas_2_P2_Funcion, &bMas_2_P2);
  bMas_3_P2.attachPop(bMas_3_P2_Funcion, &bMas_3_P2);
  bMenos_P2.attachPop(bMenos_P2_Funcion, &bMenos_P2); 
  bMenos_2_P2.attachPop(bMenos_2_P2_Funcion, &bMenos_2_P2); 
  bMenos_3_P2.attachPop(bMenos_3_P2_Funcion, &bMenos_3_P2); 
  bMas_P3.attachPop(bMas_P3_Funcion, &bMas_P3); 
  bMas_2_P3.attachPop(bMas_2_P3_Funcion, &bMas_2_P3);
  bMenos_P3.attachPop(bMenos_P3_Funcion, &bMenos_P3); 
  bMenos_2_P3.attachPop(bMenos_2_P3_Funcion, &bMenos_2_P3); 
  bMas_P4.attachPop(bMas_P4_Funcion, &bMas_P4); 
  bMenos_P4.attachPop(bMenos_P4_Funcion, &bMenos_P4); 
  bMas_P5.attachPop(bMas_P5_Funcion, &bMas_P5); 
  bMas_2_P5.attachPop(bMas_2_P5_Funcion, &bMas_2_P5);
  bMenos_P5.attachPop(bMenos_P5_Funcion, &bMenos_P5); 
  bMenos_2_P5.attachPop(bMenos_2_P5_Funcion, &bMenos_2_P5); 
  bMas_P6.attachPop(bMas_P6_Funcion, &bMas_P6); 
  bMas_2_P6.attachPop(bMas_2_P6_Funcion, &bMas_2_P6);
  bMas_3_P6.attachPop(bMas_3_P6_Funcion, &bMas_2_P6);
  bMenos_P6.attachPop(bMenos_P6_Funcion, &bMenos_P6); 
  bMenos_2_P6.attachPop(bMenos_2_P6_Funcion, &bMenos_2_P6);
  bMenos_3_P6.attachPop(bMenos_3_P6_Funcion, &bMenos_3_P6);
  bEscribir_P7.attachPop(bEscribir_P7_Funcion, &bEscribir_P7);
  bRestaurar_P7.attachPop(bRestaurar_P7_Funcion, &bRestaurar_P7);
}

// --- Control_Nextion --- //
void Control_Nextion(){
  // -- Control Apagado Pantalla -- //
  if(nxPagina != nxPagina_Old or nxPrimer_Ciclo){
    Serial.print("thsp=60");
    Serial.write(ndt,3);
    Serial.print("thup=1");
    Serial.write(ndt,3);  
    nxPagina_Old = nxPagina ; 
  }
  // -- Control de Brillo -- //
  /*if(cnt_nxBrillo >= 30){
    Serial.print("dim=50");
    Serial.write(ndt,3);
    cnt_nxBrillo = 0 ;
  }
  if(nxBoton_Pulsado){
    Serial.print("dim=100");
    Serial.write(ndt,3);    
    cnt_nxBrillo = 0 ;
  }*/
  
  if(nxPrimer_Ciclo){
    Serial.print("dims=100");
    Serial.write(ndt,3); 
    nxPrimer_Ciclo = false ;         
  }

  // -- Control Pantallas -- //
  switch(nxPagina){
    // --- Pagina 0 --- //
    case 0:
      // -- Estado Interruptor -- // 
      if(nxEstado_Interruptor != Estado_Interruptor or nxBoton_Pulsado){    
        if(Estado_Interruptor) bControl_P0.setText("APAGAR") ;
        else bControl_P0.setText("ENCENDER") ;    
        nxEstado_Interruptor = Estado_Interruptor ;
      }

      // -- Hora y Fecha -- //  
      if(nxDia != Dia or nxBoton_Pulsado){
        nDia_P0.setValue(Dia);
        nxDia = Dia ;
      }
      if(nxMes != Mes or nxBoton_Pulsado){
        nMes_P0.setValue(Mes);
        nxMes = Mes ;
      }
      if(nxAnyo != Anyo or nxBoton_Pulsado){
        nAnio_P0.setValue(Anyo);
        nxAnyo = Anyo ;
      }
      if(nxHora != Hora or nxBoton_Pulsado){
        nHoras_P0.setValue(Hora);
        nxHora = Hora ;
      }
      if(nxMinuto != Minuto or nxBoton_Pulsado){
        nMinutos_P0.setValue(Minuto);
        nxMinuto = Minuto ;
      }

      // -- Potencias -- //
      if(((Bat_Power >= (nxBat_Power + 30) or Bat_Power <= (nxBat_Power - 30)) and Bat_Power != nxBat_Power) or nxBoton_Pulsado){
        FF();
        nBat_Power_P0.setValue(Bat_Power);
        nxBat_Power = Bat_Power ;
        FF();
      }
      if(((PV_Power >= (nxPV_Power + 30) or PV_Power <= (nxPV_Power - 30)) and PV_Power != nxPV_Power) or nxBoton_Pulsado){
        FF();
        nPV_Power_P0.setValue(PV_Power);
        nxPV_Power = PV_Power ;
        FF();
      }  

      // -- Posicion -- //
      if(((PWM_VALUE >= (nxPosicion + 1) or PWM_VALUE<= (nxPosicion- 1)) and PWM_VALUE != nxPosicion) or nxBoton_Pulsado){
        FF();
        nPosicion_P0.setValue(PWM_VALUE);
        FF();
        pbPila.setValue(PWM_VALUE);
        FF();
        nxPosicion = PWM_VALUE ;
      } 
       
      // -- Estado -- //
      if(nxMore_Power != More_Power or Estado_Interruptor != nxEstado_Interruptor or nxBoton_Pulsado){
        if (Estado_Interruptor){
          if (More_Power)tEstado_P0.setText("Subiendo..."); 
          else tEstado_P0.setText("Bajando...");           
        }else tEstado_P0.setText("Sist.Apagado");            
        nxMore_Power = More_Power ;
        nxEstado_Interruptor = Estado_Interruptor ;
      }
      nxBoton_Pulsado = false;
      break; 

    // --- Pagina 1 --- //
    case 1:
      // -- Estado Interruptor -- // 
      if(nxEstado_Interruptor != Estado_Interruptor or nxBoton_Pulsado){    
        if(Estado_Interruptor) bControl_P1.setText("APAGAR") ;
        else bControl_P1.setText("ENCENDER") ;    
        nxEstado_Interruptor = Estado_Interruptor ;
      }

      // -- Hora y Fecha -- //  
      if(nxDia != Dia or nxBoton_Pulsado){
        nDia_P1.setValue(Dia);
        nxDia = Dia ;
      }
      if(nxMes != Mes or nxBoton_Pulsado){
        nMes_P1.setValue(Mes);
        nxMes = Mes ;
      }
      if(nxAnyo != Anyo or nxBoton_Pulsado){
        nAnio_P1.setValue(Anyo);
        nxAnyo = Anyo ;
      }
      if(nxHora != Hora or nxBoton_Pulsado){
        nHoras_P1.setValue(Hora);
        nxHora = Hora ;
      }
      if(nxMinuto != Minuto or nxBoton_Pulsado){
        nMinutos_P1.setValue(Minuto);
        nxMinuto = Minuto ;
      }

      // -- Estado -- //
      if(nxMore_Power != More_Power or Estado_Interruptor != nxEstado_Interruptor or nxBoton_Pulsado){
        if (Estado_Interruptor){
          if (More_Power)tEstado_P0.setText("Subiendo..."); 
          else tEstado_P0.setText("Bajando...");           
        }else tEstado_P0.setText("Sist.Apagado");            
        nxMore_Power = More_Power ;
        nxEstado_Interruptor = Estado_Interruptor ;
      }
      nxBoton_Pulsado = false;
      break; 

    // --- Pagina 2 --- //
    case 2:
      // -- Estado Interruptor -- // 
      if(nxEstado_Interruptor != Estado_Interruptor or nxBoton_Pulsado){    
        if(Estado_Interruptor) bControl_P2.setText("APAGAR") ;
        else bControl_P2.setText("ENCENDER") ;    
        nxEstado_Interruptor = Estado_Interruptor ;
      }

      // -- Hora y Fecha -- //  
      if(nxDia != Dia or nxBoton_Pulsado){
        nDia_P2.setValue(Dia);
        nxDia = Dia ;
      }
      if(nxMes != Mes or nxBoton_Pulsado){
        nMes_P2.setValue(Mes);
        nxMes = Mes ;
      }
      if(nxAnyo != Anyo or nxBoton_Pulsado){
        nAnio_P2.setValue(Anyo);
        nxAnyo = Anyo ;
      }
      if(nxHora != Hora or nxBoton_Pulsado){
        nHoras_P2.setValue(Hora);
        nxHora = Hora ;
      }
      if(nxMinuto != Minuto or nxBoton_Pulsado){
        nMinutos_P2.setValue(Minuto);
        nxMinuto = Minuto ;
      }
            
      // -- Incrementos -- //
      if(nxInc != inc_max or nxBoton_Pulsado){
        eInc = int(inc_max) ;
        dInc = int((inc_max - eInc) * 10) ;
        nInc_P2.setValue(eInc);
        nInc_2_P2.setValue(dInc);
        nxInc = inc_max ;
      } 
      if(nxInc_2 != inc_min or nxBoton_Pulsado){
        eInc = int(inc_min) ;
        dInc = int((inc_min - eInc) * 10) ;
        nInc_3_P2.setValue(eInc);
        nInc_4_P2.setValue(dInc);
        nxInc_2 = inc_min ;
      }
      if(nxInc_3 != inc_def or nxBoton_Pulsado){
        eInc = int(inc_def) ;
        dInc = int((inc_def - eInc) * 10) ;
        nInc_5_P2.setValue(eInc);
        nInc_6_P2.setValue(dInc);
        nxInc_3 = inc_def ;
      }      
  
      // -- Estado -- //
      if(nxMore_Power != More_Power or Estado_Interruptor != nxEstado_Interruptor or nxBoton_Pulsado){
        if (Estado_Interruptor){
          if (More_Power)tEstado_P0.setText("Subiendo..."); 
          else tEstado_P0.setText("Bajando...");           
        }else tEstado_P0.setText("Sist.Apagado");            
        nxMore_Power = More_Power ;
        nxEstado_Interruptor = Estado_Interruptor ;
      }
      nxBoton_Pulsado = false;
      break;  

    // --- Pagina 3 --- //
    case 3:
      // -- Estado Interruptor -- // 
      if(nxEstado_Interruptor != Estado_Interruptor or nxBoton_Pulsado){    
        if(Estado_Interruptor) bControl_P3.setText("APAGAR") ;
        else bControl_P3.setText("ENCENDER") ;    
        nxEstado_Interruptor = Estado_Interruptor ;
      }

      // -- Hora y Fecha -- //  
      if(nxDia != Dia or nxBoton_Pulsado){
        nDia_P3.setValue(Dia);
        nxDia = Dia ;
      }
      if(nxMes != Mes or nxBoton_Pulsado){
        nMes_P3.setValue(Mes);
        nxMes = Mes ;
      }
      if(nxAnyo != Anyo or nxBoton_Pulsado){
        nAnio_P3.setValue(Anyo);
        nxAnyo = Anyo ;
      }
      if(nxHora != Hora or nxBoton_Pulsado){
        nHoras_P3.setValue(Hora);
        nxHora = Hora ;
      }
      if(nxMinuto != Minuto or nxBoton_Pulsado){
        nMinutos_P3.setValue(Minuto);
        nxMinuto = Minuto ;
      }

      // -- Valores -- //
      if(nxValor != reg_pos_max or nxBoton_Pulsado){
        nReg_Pos_Max_P3.setValue(reg_pos_max);
        nxValor = reg_pos_max ;
      } 
      if(nxValor_2 != reg_pos_min or nxBoton_Pulsado){
        nReg_Pos_Min_P3.setValue(reg_pos_min);
        nxValor_2 = reg_pos_min ;
      } 
      
      // -- Estado -- //
      if(nxMore_Power != More_Power or Estado_Interruptor != nxEstado_Interruptor or nxBoton_Pulsado){
        if (Estado_Interruptor){
          if (More_Power)tEstado_P0.setText("Subiendo..."); 
          else tEstado_P0.setText("Bajando...");           
        }else tEstado_P0.setText("Sist.Apagado");            
        nxMore_Power = More_Power ;
        nxEstado_Interruptor = Estado_Interruptor ;
      }
      nxBoton_Pulsado = false;
      break;

    // --- Pagina 4 --- //
    case 4:
      // -- Estado Interruptor -- // 
      if(nxEstado_Interruptor != Estado_Interruptor or nxBoton_Pulsado){    
        if(Estado_Interruptor) bControl_P4.setText("APAGAR") ;
        else bControl_P4.setText("ENCENDER") ;    
        nxEstado_Interruptor = Estado_Interruptor ;
      }

      // -- Hora y Fecha -- //  
      if(nxDia != Dia or nxBoton_Pulsado){
        nDia_P4.setValue(Dia);
        nxDia = Dia ;
      }
      if(nxMes != Mes or nxBoton_Pulsado){
        nMes_P4.setValue(Mes);
        nxMes = Mes ;
      }
      if(nxAnyo != Anyo or nxBoton_Pulsado){
        nAnio_P4.setValue(Anyo);
        nxAnyo = Anyo ;
      }
      if(nxHora != Hora or nxBoton_Pulsado){
        nHoras_P4.setValue(Hora);
        nxHora = Hora ;
      }
      if(nxMinuto != Minuto or nxBoton_Pulsado){
        nMinutos_P4.setValue(Minuto);
        nxMinuto = Minuto ;
      }

      // -- Valores -- //
      if(nxValor != tiempo or nxBoton_Pulsado){
        nTiempo_P4.setValue(tiempo);
        nxValor = tiempo ;
      } 
      
      // -- Estado -- //
      if(nxMore_Power != More_Power or Estado_Interruptor != nxEstado_Interruptor or nxBoton_Pulsado){
        if (Estado_Interruptor){
          if (More_Power)tEstado_P0.setText("Subiendo..."); 
          else tEstado_P0.setText("Bajando...");           
        }else tEstado_P0.setText("Sist.Apagado");            
        nxMore_Power = More_Power ;
        nxEstado_Interruptor = Estado_Interruptor ;
      }
      nxBoton_Pulsado = false;
      break; 

    // --- Pagina 5 --- //
    case 5:
      // -- Estado Interruptor -- // 
      if(nxEstado_Interruptor != Estado_Interruptor or nxBoton_Pulsado){    
        if(Estado_Interruptor) bControl_P5.setText("APAGAR") ;
        else bControl_P5.setText("ENCENDER") ;    
        nxEstado_Interruptor = Estado_Interruptor ;
      }

      // -- Hora y Fecha -- //  
      if(nxDia != Dia or nxBoton_Pulsado){
        nDia_P5.setValue(Dia);
        nxDia = Dia ;
      }
      if(nxMes != Mes or nxBoton_Pulsado){
        nMes_P5.setValue(Mes);
        nxMes = Mes ;
      }
      if(nxAnyo != Anyo or nxBoton_Pulsado){
        nAnio_P5.setValue(Anyo);
        nxAnyo = Anyo ;
      }
      if(nxHora != Hora or nxBoton_Pulsado){
        nHoras_P5.setValue(Hora);
        nxHora = Hora ;
      }
      if(nxMinuto != Minuto or nxBoton_Pulsado){
        nMinutos_P5.setValue(Minuto);
        nxMinuto = Minuto ;
      }

      // -- Valores -- //
      if(nxValor != p_max or nxBoton_Pulsado){
        nP_Max_P5.setValue(p_max);
        nxValor = p_max ;
      } 
      if(nxValor_2 != p_min or nxBoton_Pulsado){
        nP_Min_P5.setValue(p_min);
        nxValor_2 = p_min ;
      }
      
      // -- Estado -- //
      if(nxMore_Power != More_Power or Estado_Interruptor != nxEstado_Interruptor or nxBoton_Pulsado){
        if (Estado_Interruptor){
          if (More_Power)tEstado_P0.setText("Subiendo..."); 
          else tEstado_P0.setText("Bajando...");           
        }else tEstado_P0.setText("Sist.Apagado");            
        nxMore_Power = More_Power ;
        nxEstado_Interruptor = Estado_Interruptor ;
      }
      nxBoton_Pulsado = false;
      break;
      
    // --- Pagina 6 --- //
    case 6:
      // -- Estado Interruptor -- // 
      if(nxEstado_Interruptor != Estado_Interruptor or nxBoton_Pulsado){    
        if(Estado_Interruptor) bControl_P6.setText("APAGAR") ;
        else bControl_P6.setText("ENCENDER") ;    
        nxEstado_Interruptor = Estado_Interruptor ;
      }

      // -- Hora y Fecha -- //  
      if(nxDia != Dia or nxBoton_Pulsado){
        nDia_P6.setValue(Dia);
        nxDia = Dia ;
      }
      if(nxMes != Mes or nxBoton_Pulsado){
        nMes_P6.setValue(Mes);
        nxMes = Mes ;
      }
      if(nxAnyo != Anyo or nxBoton_Pulsado){
        nAnio_P6.setValue(Anyo);
        nxAnyo = Anyo ;
      }
      if(nxHora != Hora or nxBoton_Pulsado){
        nHoras_P6.setValue(Hora);
        nxHora = Hora ;
      }
      if(nxMinuto != Minuto or nxBoton_Pulsado){
        nMinutos_P6.setValue(Minuto);
        nxMinuto = Minuto ;
      }

      // -- PID -- //
      if(nxP != KP or nxBoton_Pulsado){
        eInc = int(KP) ;
        dInc = int((KP - eInc) * 100) ;
        nP_P6.setValue(eInc);
        nP2_P6.setValue(dInc);
        nxP = KP ;
      } 
      if(nxI != KI or nxBoton_Pulsado){
        eInc = int(KI) ;
        dInc = int((KI - eInc) * 100) ;
        nI_P6.setValue(eInc);
        nI2_P6.setValue(dInc);
        nxI = KI ;
      }
      if(nxD != KD or nxBoton_Pulsado){
        eInc = int(KD) ;
        dInc = int((KD - eInc) * 100) ;
        nD_P6.setValue(eInc);
        nD2_P6.setValue(dInc);
        nxD = KD ;
      }   
      
      // -- Estado -- //
      if(nxMore_Power != More_Power or Estado_Interruptor != nxEstado_Interruptor or nxBoton_Pulsado){
        if (Estado_Interruptor){
          if (More_Power)tEstado_P0.setText("Subiendo..."); 
          else tEstado_P0.setText("Bajando...");           
        }else tEstado_P0.setText("Sist.Apagado");            
        nxMore_Power = More_Power ;
        nxEstado_Interruptor = Estado_Interruptor ;
      }
      nxBoton_Pulsado = false;
      break;  

    // --- Pagina 7 --- //
    case 7:
      // -- Estado Interruptor -- // 
      if(nxEstado_Interruptor != Estado_Interruptor or nxBoton_Pulsado){    
        if(Estado_Interruptor) bControl_P7.setText("APAGAR") ;
        else bControl_P7.setText("ENCENDER") ;    
        nxEstado_Interruptor = Estado_Interruptor ;
      }

      // -- Hora y Fecha -- //  
      if(nxDia != Dia or nxBoton_Pulsado){
        nDia_P7.setValue(Dia);
        nxDia = Dia ;
      }
      if(nxMes != Mes or nxBoton_Pulsado){
        nMes_P7.setValue(Mes);
        nxMes = Mes ;
      }
      if(nxAnyo != Anyo or nxBoton_Pulsado){
        nAnio_P7.setValue(Anyo);
        nxAnyo = Anyo ;
      }
      if(nxHora != Hora or nxBoton_Pulsado){
        nHoras_P7.setValue(Hora);
        nxHora = Hora ;
      }
      if(nxMinuto != Minuto or nxBoton_Pulsado){
        nMinutos_P7.setValue(Minuto);
        nxMinuto = Minuto ;
      }   
      
      // -- Estado -- //
      if(nxMore_Power != More_Power or Estado_Interruptor != nxEstado_Interruptor or nxBoton_Pulsado){
        if (Estado_Interruptor){
          if (More_Power)tEstado_P0.setText("Subiendo..."); 
          else tEstado_P0.setText("Bajando...");           
        }else tEstado_P0.setText("Sist.Apagado");            
        nxMore_Power = More_Power ;
        nxEstado_Interruptor = Estado_Interruptor ;
      }
      nxBoton_Pulsado = false;
      break;                                                                                                                                                                                                            
  }
}

// --- Init EEPRON --- //
void Init_EEPRON(){
    Serial.println("Inicializando EEPROM...");
    while(!EEPROM.begin(512)){
      Serial.print(".");
      delay(1000); 
    }
    Serial.println("EEPROM correcta");     
}

// --- Lectura EEPRON --- //
void Lectura_EEPRON(){
  Serial.println("Leyendo EEPROM...");
  byte EE_Estado_Interruptor = Leer_EEPRON(5);
  if(EE_Estado_Interruptor == 10) Estado_Interruptor = true;
  else Estado_Interruptor = false; 
  p_max = Leer_EEPRON(10); 
  p_min = Leer_EEPRON(15);
  reg_pos_max = Leer_EEPRON(20) ;
  reg_pos_min = Leer_EEPRON(25) ;
  byte EE_range_max = Leer_EEPRON(30) * 10 ;
  range_max = Leer_EEPRON(35) + EE_range_max ;
  byte EE_range_min = Leer_EEPRON(40) * 10 ;
  range_min = Leer_EEPRON(45) + EE_range_min ;
  byte EE_tiempo = Leer_EEPRON(50) * 10 ;
  tiempo = Leer_EEPRON(55) + EE_tiempo ;
  e_inc_max = Leer_EEPRON(60) ; 
  d_inc_max = Leer_EEPRON(65) ;
  e_inc_min = Leer_EEPRON(70) ; 
  d_inc_min = Leer_EEPRON(75) ; 
  e_inc_def = Leer_EEPRON(80) ; 
  d_inc_def = Leer_EEPRON(85) ;
  inc_max = e_inc_max + float(d_inc_max / 10.0) ; 
  inc_min = e_inc_min + float(d_inc_min / 10.0) ;
  inc_def = e_inc_def + float(d_inc_def / 10.0) ;
  Serial.println("Lectura de EEPROM correcta");
}
byte Leer_EEPRON(int ADDR){
  byte val = EEPROM.read(ADDR);
  /*if(DEBUG)*/Serial.println(val);
  return val ;
}
  
// --- Escribir EEPRON --- //
void Escritura_EEPRON(){
  Serial.println("Escribiendo EEPROM...");
  byte EE_Estado_Interruptor = 0;
  if(Estado_Interruptor) EE_Estado_Interruptor = 10;
  else EE_Estado_Interruptor = 0;
  Escribir_EEPRON(5, EE_Estado_Interruptor); 
  Escribir_EEPRON(10, p_max); 
  Escribir_EEPRON(15, p_min);
  Escribir_EEPRON(20, reg_pos_max) ;
  Escribir_EEPRON(25, reg_pos_min) ;
  byte EE_range_max = int(range_max/10) ;
  Escribir_EEPRON(30, EE_range_max) ;
  EE_range_max = (range_max - (EE_range_max * 10)) ;
  Escribir_EEPRON(35, EE_range_max) ;
  byte EE_range_min = int(range_min/10) ;
  Escribir_EEPRON(40, EE_range_min) ;
  EE_range_min = (range_min - (EE_range_min * 10)) ;
  Escribir_EEPRON(45, EE_range_min) ;
  byte EE_tiempo = int(tiempo/10) ;
  Escribir_EEPRON(50, EE_tiempo) ;
  EE_tiempo = (tiempo - (EE_tiempo * 10)) ;
  Escribir_EEPRON(55, EE_tiempo) ;
  e_inc_max = int(inc_max) ;
  d_inc_max = (inc_max - e_inc_max) * 10 ; 
  Escribir_EEPRON(60, e_inc_max) ; 
  Escribir_EEPRON(65, d_inc_max) ;
  e_inc_min = int(inc_min) ;
  d_inc_min = (inc_min - e_inc_min) * 10 ; 
  Escribir_EEPRON(70, e_inc_min) ; 
  Escribir_EEPRON(75, d_inc_min) ;
  e_inc_def = int(inc_def) ;
  d_inc_def = (inc_def - e_inc_def) * 10 ; 
  Escribir_EEPRON(80, e_inc_def) ; 
  Escribir_EEPRON(85, d_inc_def) ;
  EEPROM.commit();
  Serial.println("Escritura de EEPROM correcta");
}
void Escribir_EEPRON(int ADDR, byte VAL){
  /*if(DEBUG)*/Serial.println(VAL);
  EEPROM.write(ADDR,VAL);
}

// --- Valores de Fabrica --- //
void Valores_Fabrica(){
  p_max = 60;
  p_min = 20; 
  range_max = 250;
  range_min = 120;
  inc_max = 2.0 ; 
  inc_min = 1.0 ;
  inc_def = 0.5 ; 
  Hora_Encender[0] = 7 ;
  Hora_Encender[1] = 30 ;
  Hora_Apagar[0] = 19 ;
  Hora_Apagar[1] = 30 ;
}

// --- Limites --- //
void Limites(){
  if(p_max >= 2000)p_max = 2000;
  else if (p_max <= p_min) p_max = p_min + 10 ;
  if(p_min <= 10)p_min = 10;
  else if(p_min >= p_max) p_min = p_min + 10;  
  if(range_max >= 1000)range_max = 1000;
  else if(range_max <= range_min)range_max = range_min + 100; 
  if(range_min <= 100)range_min = 100;
  else if(range_min >= range_max)range_min = range_max + 100; 
  if(inc_max >= 5.0) inc_max = 5.0 ;
  else if(inc_max <= inc_min) inc_max = inc_min + 0.1 ;
  if(inc_min <= inc_def) inc_min = inc_def + 0.1 ;
  else if(inc_min >= inc_max) inc_min = inc_max - 0.1 ; 
  if(inc_def <= 0.1)inc_def = 0.1 ;
  else if(inc_def >= inc_min) inc_def = inc_min - 0.1 ;
}
// --- Funcion CRC Nextion --- //
void FF(){
 Serial.write(0xff);
 Serial.write(0xff);
 Serial.write(0xff);
}

// --- SETUP --- //
void setup(){
  Init_EEPRON();
  Serial.begin(9600);
  Init_Nextion();
  Conexion_Wifi() ;
  Conexion_OTA(); 
  Init_Modbus_Sys();
  //Init_Modbus_Mul();
  //POWER_PID.SetMode(AUTOMATIC);
  dimmer.begin(NORMAL_MODE, ON);  //dimmer initialisation: name.begin(MODE, STATE) 
  Control_Dimmer(2);              // Escribir Salida Dimmer 
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
  nexLoop(nex_listen_list);
     
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
    //Leer_Modbus_Mul();
    
    // -- Leer Modbus_Sys -- //  
    Leer_Modbus_Sys();

    // -- Control PWM -- //
    if(cnt_MB ==1)Control_PWM();
  }
  
  // --- Limites --- //
  //Limites();

  // --- Generar Pagina WEBSERVER --- //
  Generar_WEB() ;

  // --- Control Nextion --- /
  Control_Nextion();
}
