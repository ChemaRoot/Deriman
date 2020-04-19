// --- Control PWM --- //
void Control_PWM(){
  if (Estado_Interruptor == true) {
    if ((Bat_Power > p_max and SOC > Absortion_SOC) or DEBUG) { // Default value is < 60 w from grid an pwm < 100
      if  (Bat_Power > (p_max + range_max) and  PWM_VALUE < (100 - reg_pos_max)) PWM_VALUE+= inc_max; // Increment 15 in range 250
      else if  ( Bat_Power > (p_max + range_min) && PWM_VALUE < (100 - reg_pos_min)) PWM_VALUE+= inc_min; // Increment 3 in range 120
      PWM_VALUE += inc_def ; //Increment value
      if (PWM_VALUE >= Dimmer_LIMIT) PWM_VALUE = Dimmer_LIMIT; 
      More_Power = true ;
    }else{
      if ((Bat_Power < p_min or SOC < Absortion_SOC) and not DEBUG) { // Value is < 90 w from grid an pwm < 125
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
  if (Estado_Interruptor){
    Dimmer.setState(ON);  
    if(Value >= Dimmer_LIMIT) Value = Dimmer_LIMIT ;
    if(Value <= 10){
      Value = 10 ;
      Dimmer.setPower(Value);  // Escribir Salida Dimmer
      Dimmer.setState(OFF);
    }else Dimmer.setPower(Value);  // Escribir Salida Dimmer   
  }else Dimmer.setState(OFF);
}
