


uint8_t CTopData(uint8_t address,int block,  String pattern) //SHIFTED
{

    float t = ((float)address+block*250);

    if (block < 4) // x axis
    {
      if (t>=0 && t<500) // X component of C -- one direction
      {
       float v = (5*pi/1497)*t+pi/6;
       return (uint8_t)(((uint16_t)(1500*cos(v)+2000))>>8);
      }
      if (t>=500 && t<=999)
      {
        float v = (5*pi/1497)*(t-500)+2*pi/3;
        return (uint8_t)(((uint16_t)(1500*sin(v)+2000))>>8);
      }
    }

    else if (block < 8) // y axis
    {
      if (t>=1000 && t<1500) // X component of C -- one direction
      {
       float v = (5*pi/1497)*(t-1000)+pi/6;
       return (uint8_t)(((uint16_t)(1500*sin(v)+2000))>>8);
      }
      if (t>=1500 && t<2000)
      {
        float v = (5*pi/1497)*((t-1000)-500)+2*pi/3;
        return (uint8_t)(((uint16_t)(1500*cos(v)+2000))>>8);
      }
    }
    
}

uint8_t CBottomData(uint8_t address, int block, String pattern)
{
    float t = ((float)address+block*250);

    if (block < 4) // x axis
    {
        if (t>=0 && t<500)
        {
          float v = (5*pi/1497)*t+pi/6;
          return (uint8_t)(1500*cos(v)+2000);
        }
        else if (t>=500 && t<1000)
        {
          float v = (5*pi/1497)*(t-500)+(2*pi/3);
          return (uint8_t)(1500*sin(v)+2000);
        }
    }

    else if (block < 8) // y axis
    {
      if (t>=1000 && t<1500) // X component of C -- one direction
      {
       float v = (5*pi/1497)*(t-1000)+pi/6;
       return (uint8_t)(1500*sin(v)+2000);
      }
      if (t>=1500 && t<2000)
      {
        float v = (5*pi/1497)*((t-1000)-500)+2*pi/3;
        return (uint8_t)(1500*cos(v)+2000);
      }
    }
    
}
