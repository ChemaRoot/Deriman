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
  if(DEBUG)Serial.println(val);
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
  if(DEBUG)Serial.println(VAL);
  EEPROM.write(ADDR,VAL);
}
