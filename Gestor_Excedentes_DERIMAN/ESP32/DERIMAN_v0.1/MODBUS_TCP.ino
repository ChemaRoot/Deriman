
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
  SOC = MB_Data_Mul/10 ;
}
