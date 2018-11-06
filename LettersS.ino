//const float pi = 3.14159; 

uint8_t STopData(uint8_t address,int block,  String pattern) //SHIFTED
{

    int t = (address+block*250);

    if (block < 4) // x axis
    {
      if (t>=0 && t<250) 
      {
       float v = (4*pi/747)*t+pi/6;
       return (uint8_t)(((uint16_t)(750*cos(v)+2000))>>8);
      }
      if (t>=250 && t<500)
      {
        float v = (4*pi/747)*(t-250);
        return (uint8_t)(((uint16_t)(750*sin(v)+2000))>>8);
      }
      if (t>=500 && t<750)
      {
        float v = (4*pi/747)*(t-500)-5*pi/6;
        return ((uint8_t)(((uint16_t)(750*cos(v)+2000))>>8));
      }
      if (t>=750 && t<1000)
      {
        float v = (4*pi/747)*(t-750)+pi;
        return ((uint8_t)(((uint16_t)(750*sin(v)+2000))>>8));
      }
    }

    else if (block < 8) // y axis
    {
      float s = t-1000;
      if (s>=0 && s<250) 
      {
       float v = (4*pi/747)*t+5*pi/6;
       return (uint8_t)(((uint16_t)(750*sin(v)+2750))>>8);
      }
      if (s>=250 && s<500)
      {
        float v = (4*pi/747)*(s-250);
        return (uint8_t)(((uint16_t)(750*cos(v)+1250))>>8);
      }
      if (s>=500 && s<750)
      {
        float v = (4*pi/747)*(s-500)-5*pi/6;
        return ((uint8_t)(((uint16_t)(750*sin(v)+1250))>>8));
      }
      if (s>=750 && s<1000)
      {
        float v = (4*pi/747)*(s-750)+pi;
        return ((uint8_t)(((uint16_t)(750*cos(v)+2750))>>8));
      }
    }
   
}

uint8_t SBottomData(uint8_t address, int block, String pattern)
{
    
    int t = (address+block*250);

    if (block < 4) // x axis
    {
      if (t>=0 && t<250) 
      {
       float v = (4*pi/747)*t+pi/6;
       return (uint8_t)(((uint16_t)(750*cos(v)+2000)));
      }
      if (t>=250 && t<500)
      {
        float v = (4*pi/747)*(t-250);
        return (uint8_t)(((uint16_t)(750*sin(v)+2000)));
      }
      if (t>=500 && t<750)
      {
        float v = (4*pi/747)*(t-500)-5*pi/6;
        return ((uint8_t)(((uint16_t)(750*cos(v)+2000))));
      }
      if (t>=750 && t<1000)
      {
        float v = (4*pi/747)*(t-750)+pi;
        return ((uint8_t)(((uint16_t)(750*sin(v)+2000))));
      }
    }

    else if (block < 8) // y axis
    {
      float s = t-1000;
       if (s>=0 && s<250) 
      {
       float v = (4*pi/747)*t+5*pi/6;
       return (uint8_t)(((uint16_t)(750*sin(v)+2750)));
      }
      if (s>=250 && s<500)
      {
        float v = (4*pi/747)*(s-250);
        return (uint8_t)(((uint16_t)(750*cos(v)+1250)));
      }
      if (s>=500 && s<750)
      {
        float v = (4*pi/747)*(s-500)-5*pi/6;
        return ((uint8_t)(((uint16_t)(750*sin(v)+1250))));
      }
      if (s>=750 && s<1000)
      {
        float v = (4*pi/747)*(s-750)+pi;
        return ((uint8_t)(((uint16_t)(750*cos(v)+2750))));
      }
    }
    
}
