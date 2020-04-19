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
      <td align='center'>SOC</td>\
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
</html>",C_WIFI,Derivador,C_Nombre_Dispositivo,C_Estado_Wifi,Hora,Minuto,WIFI_SSID,char_IP,C_Estado_Interruptor,PWM_VALUE,Bat_Power,PV_Power,SOC,C_Horario,Hora_Encender[0],Hora_Encender[1],Hora_Apagar[0],Hora_Apagar[1]);
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
