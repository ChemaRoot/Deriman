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
