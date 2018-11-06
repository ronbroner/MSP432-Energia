uint8_t BTopData(uint8_t address,int block,  String pattern) //SHIFTED
{
 float t = ((float)address+block*250)*0.5;
  

  if (block < 4) // x axis
  {
    if (t >=0 && t < 200)
    {
      float v = (pi/199)*t-pi/2;
      return (uint16_t)(((uint16_t)(1100*cos(v)+1500))>>8);
    }
    if (t >=200 && t < 400)
    {
      float v = (pi/199)*(t-200)-pi/2;
      return (uint8_t)(((uint16_t)(1100*cos(v)+1500))>>8);
    }
    if (t >=400 && t < 500)
    {
      return (uint8_t)(((uint16_t)(1500))>>8);
    }
  }

  else if (block < 8) //y axis
  {
    float s = t-500.0;
    
    if (s >=0 && s < 200)
    {
      float v = (pi/199)*s-pi/2;
      //Serial.println((-750*sin(v)+2750));
      return (uint8_t)(((uint16_t)(-750*sin(v)+2750))>>8);
    }
    if (s >=200 && s < 400)
    {
      float v = (pi/199)*(s-100);
      return (uint8_t)(((uint16_t)(750*sin(v)+1250))>>8);
    }
    if (s >=400 && s < 500)
    {
       
       return (uint16_t)(((uint16_t)(500+1000.0/33.0*(s-400)))>>8);
    }
  }
  
}



uint8_t BBottomData(uint8_t address,int block,  String pattern) 
{
 float t = ((float)address+block*250)*0.5;

  if (block < 4) // x axis
  {
    if (t >=0 && t < 200)
    {
      float v = (pi/199)*t-pi/2;
      return (uint8_t)(1100*cos(v)+1500);
    }
    if (t >=200 && t < 400)
    {
      float v = (pi/199)*(t-200)-pi/2;
      return (uint8_t)(1100*cos(v)+1500);
    }
    if (t >=400 && t < 500)
    {
      return (uint8_t)(1500);
    }
  }

  else if (block < 8) //y axis
  {
    //Serial.println("Block >= 4");
    float s = t-500.0;
    if (s >=0 && s < 200)
    {
      float v = (pi/199)*s-pi/2;
      return (uint8_t)(-750*sin(v)+2750);
    }
    if (s >=200 && s < 400)
    {
      float v = (pi/199)*(s-100);
      return (uint8_t)(750*sin(v)+1250);
    }
    if (s >=400 && s < 500)
    {
       
       return (uint8_t)(500+1000.0/33.0*(s-400));
    }
  }
  
}
