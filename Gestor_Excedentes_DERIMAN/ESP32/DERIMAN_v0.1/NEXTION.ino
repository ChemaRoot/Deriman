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

// --- Lista Botones Nextion --- //
NexTouch *nex_listen_list[] = {
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

// --- Control Botones Nextion --- //
void Control_Botones_Nextion(){
  nexLoop(nex_listen_list);
}
  
// --- Control_Animaciones_Nextion --- //
void Control_Animaciones_Nextion(){
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

// --- Funcion CRC Nextion --- //
void FF(){
 Serial.write(0xff);
 Serial.write(0xff);
 Serial.write(0xff);
}
