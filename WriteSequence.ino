
#undef P2_OUT_REG // LED's
#undef P2_DIR_REG
#define P2_OUT_REG (0x40004C03)
#define P2_DIR_REG (0x40004C05)

#undef P4_OUT_REG // ADDRESS
#undef P4_DIR_REG
#define P4_OUT_REG (0x40004C23)
#define P4_DIR_REG (0x40004C25)


#undef P6_OUT_REG // Mode 6.4, 6.5 and  WCLK1  (6.0), NO LONGER WCKLKE/EN
#undef P6_DIR_REG
#define P6_OUT_REG (0x40004C43)
#define P6_DIR_REG (0x40004C45)

#undef P7_OUT_REG // DATA TOP 8
#undef P7_DIR_REG
#undef P7_SEL1_REG
#define P7_OUT_REG (0x40004C62)
#define P7_DIR_REG (0x40004C64)
#define P7_SEL1_REG (0x40004C6A)

#undef P8_OUT_REG  // DOES NOTHING, use if needed
#undef P8_DIR_REG
#define P8_OUT_REG (0x40004C63)
#define P8_DIR_REG (0x40004C65)

#undef P9_OUT_REG // DATA BOTTOM 8
#undef P9_DIR_REG
#define P9_OUT_REG (0x40004C82)
#define P9_DIR_REG (0x40004C84)


const float pi = 3.14159; 



uint8_t *p2_out = (uint8_t*)P2_OUT_REG;
uint8_t *p2_dir = (uint8_t*)P2_DIR_REG;
uint8_t *p4_out = (uint8_t*)P4_OUT_REG;
uint8_t *p4_dir = (uint8_t*)P4_DIR_REG;
uint8_t *p6_out = (uint8_t*)P6_OUT_REG;
uint8_t *p6_dir = (uint8_t*)P6_DIR_REG;
uint8_t *p7_out = (uint8_t*)P7_OUT_REG;
uint8_t *p7_dir = (uint8_t*)P7_DIR_REG;
uint8_t *p8_out = (uint8_t*)P8_OUT_REG;
uint8_t *p8_dir = (uint8_t*)P8_DIR_REG;
uint8_t *p9_out = (uint8_t*)P9_OUT_REG;
uint8_t *p9_dir = (uint8_t*)P9_DIR_REG;


uint8_t *p7_SEL1 = (uint8_t*)P7_SEL1_REG;




//Write specific variables
int mode = 0; // 0 = NO-WRITE MODE   1 = WRITE MODE
int adr = 0;
int count = 0;
bool fin = false;
int blockNum = 8; // number of blocks to write to (normally 8, less for testing purposes)

//General variables
int buttonState1 = 0;         // variable for reading the pushbutton status
int buttonState2 = 0;         // variable for reading the pushbutton status
unsigned long previousMillis = 0; // for pause without delay for Buttons
unsigned long previousMillis2 = 0; // for pause without delay for LED (two because they conflict with one another)
unsigned long letterPreviousMillis = 0;
int letterInterval = 4000;
int interval = 1000;
int currentlyWriting = 0; // lets you know if button1 was pressed and write has started

int letter = 0;

void setup() {
 
    Serial.begin(9600);
    Serial.print("test");
    pinMode(PUSH1, INPUT_PULLUP); //buttons are only thing we use pinMode for, eveything else is memory mapped io
    pinMode(PUSH2, INPUT_PULLUP);

    *p2_dir = 0xFF;
    *p4_dir = 0xFF;
    *p6_dir = 0xFF; // STEP 1: setup all write ports 
    *p7_dir = 0xFF;
    *p8_dir = 0xFF;
    *p9_dir = 0xFF;
    
    *p6_out = 0b00010000;


    *p7_SEL1 = 0x00;
}

void loop() {
  
 
  buttonState1 = digitalRead(PUSH1);
  buttonState2 = digitalRead(PUSH2);


  if (letter>3)
    letter = 0;

  /*unsigned long letterCurrentMillis = millis();
  if (letterCurrentMillis - letterPreviousMillis >= letterInterval) 
    {
      letterPreviousMillis = letterCurrentMillis;
      mode = 1; // begin write process
      letter++;
      
    }*/
    

  if (buttonState1 == LOW)
  {

      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
       // save the last time you blinked the LED
        previousMillis = currentMillis;
        mode = 1; // begin write process
        letter++;
        }
  }
  
  if (buttonState2 == LOW)
  {
        unsigned long currentMillis = millis();

        if (currentMillis - previousMillis >= interval) {
       // save the last time you blinked the LED
        previousMillis = currentMillis;
        mode = 0; // revert to no write mode
        }
  }

  if (mode == 0 && (*p6_out>>4 == 2)) // mode is for energia code use, P6 pins will be used by FPGA: 0b##01####;(>>6 = 1) = mode 0, no write     0b##10####(>>6 = 2) = mode 1, write
  { // Bit shift is because we are using port 6 for both the mode and the write clock, ignore write clock when change mode
    *p6_out = (*p6_out|0b00010000)^0b00100000;
  
  }
  else if (mode == 1 && (*p6_out>>4 == 1))
  {
    *p6_out = (*p6_out|0b00100000)^0b00010000;

  }

 
  if (mode == 0) //In not write mode
  {
   *p2_out = 0b00000010; // Turn green on solid
   *p6_out &= 0b11111110; // reset clockwrite
   *p6_out ^= 0b00000001;
   adr = 0; // Resets block and adress. If youre in this mode, it means you should be ready to write from the beginning at any moment. 
   count = 0; // so DELETE this if you dont want a reset capability
   fin = false;
   currentlyWriting = 0; //finished writing, could now do everything again
  }
  else if (mode == 1) // turn to write mode and start writing
  {
    if (currentlyWriting == 0)
    {
      *p2_out &= 0b11111101; // Turn green off, everything else remains the same as it was
      currentlyWriting = 1;
      initializeWrite(); 
    }
    writeLoop();
  }
  
  
}

void  initializeWrite()
{
  Serial.println("Writing Initialized -- Commencing Write process");
  //*p6_out = 0b00000011; p6 used to be for clock enable
}

void writeLoop()
{
  while (count < blockNum)
  {
    *p2_out = 0b00000001; // light stays red during writing;
    *p6_out &= 0b11111110; // STEP 7: WRITE WCLK 0, eveyting else stays as it was before
    *p4_out = adr; // STEP 4: write to adress port, start at 0->249
    
    
    
    //*p7_out = topData(*p4_out,count, "DEFAULT"); // STEP 5: WRITE DATA TOP BITS TO PORT 7       ----- TEST FUNCTION CIRCLE top bits
    //*p9_out = bottomData(*p4_out,count, "DEFAULT"); // STEP 6: WRITE DATA BOTTOM BITS TO PORT 9    ----- TEST FUNCTION CIRCLE bottom bits
    if (letter == 1)
    {
      *p7_out = UTopData(*p4_out,count, "DEFAULT"); // STEP 5: WRITE DATA TOP BITS TO PORT 7       ----- U FUNCTION top bits
      *p9_out = UBottomData(*p4_out,count, "DEFAULT"); // STEP 6: WRITE DATA BOTTOM BITS TO PORT 9    ----- U FUNCTION  bottom bits
    }
    else if (letter == 2)
    {
      *p7_out = CTopData(*p4_out,count, "DEFAULT"); // STEP 5: WRITE DATA TOP BITS TO PORT 7       ----- C FUNCTION top bits
      *p9_out = CBottomData(*p4_out,count, "DEFAULT"); // STEP 6: WRITE DATA BOTTOM BITS TO PORT 9    ----- C FUNCTION  bottom bits
    }
     else if (letter == 3)
     {
      *p7_out = STopData(*p4_out,count, "DEFAULT"); // STEP 5: WRITE DATA TOP BITS TO PORT 7       ----- S FUNCTION top bits
      *p9_out = SBottomData(*p4_out,count, "DEFAULT"); // STEP 6: WRITE DATA BOTTOM BITS TO PORT 9    ----- S FUNCTION  bottom bits
     }
     else if (letter == 4)
     {
      *p7_out = BTopData(*p4_out,count, "DEFAULT"); // STEP 5: WRITE DATA TOP BITS TO PORT 7       ----- B FUNCTION top bits
      *p9_out = BBottomData(*p4_out,count, "DEFAULT"); // STEP 6: WRITE DATA BOTTOM BITS TO PORT 9    ----- B FUNCTION  bottom bits
     }
    
    
    
    *p6_out ^= 0b00000001; // STEP 7: WRITE WCLK 1, eveyting else stays as it was before
    if (adr < 250)
    {
      adr++; // STEP 9: INCREMENT ADDRESS
    }
    else
    {
      adr = 0; // Step 10: REPEAT FOR ALL BLOCKS OF RAM
      count++;
    }
  }
  if (count >=blockNum && mode == 1 ) // Finished writing and still in write mode
  {
    if (fin == false)
      {
      Serial.println("DONE");
      *p6_out &= 0b11111110; // STEP 7: Finish writing with write clock off

      fin = true;
      }
   // currentlyWriting = 0; //finished writing, could now do everything again
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis2 >= interval/2) // Blinking to notify writing done but still in write mode
    { 
      previousMillis2 = currentMillis;
      *p2_out ^= 0b00000001; // flicker red led
    }
      
  }

}

uint8_t topData(uint8_t address,int block,  String pattern)
{
  if (block < 4)
       return (uint8_t)((uint16_t)(2047 + floor(1900*sin((2*3.1415926535/1000)*(address+block*250)))) >> 8);
   else 
       return (uint8_t)((uint16_t)(2047 + floor(1900*sin((2*3.1415926535/1000)*(address+block*250)+3.1415926535/2))) >> 8);

}

uint8_t bottomData(uint8_t address, int block, String pattern)
{
  if (block < 4)
    return (uint8_t)(2047 + floor(1900*sin((2*3.1415926535/1000)*(address+block*250))));
  else 
    return (uint8_t)(2047 + floor(1900*sin((2*3.1415926535/1000)*(address+block*250)+3.1415926535/2)));
}


/*




uint8_t UTopData(uint8_t address,int block,  String pattern)
{

    int t = (address+block*250);
    if (block < 4) // x axis
    {
        if (t>=0 && t<150) // LINE 1
        {
         // Serial.println(500);
          return (uint8_t)(((uint16_t)(500))>>8);
        }
        else if (t>=150 && t<350)
        {
          float v = (pi/199)*(t-150) + pi;
          //Serial.println((uint16_t)(1500*cos(v)+2000));
          return (uint8_t)(((uint16_t)(1500*cos(v)+2000))>>8);
        }
        else if (t>=350 && t<500)
        {
          //Serial.println(((uint16_t)(3500)));
          return (uint8_t)(((uint16_t)(3500))>>8);
        }
        else if (t>=500 && t<650)
        {
          return (uint8_t)(((uint16_t)(3500))>>8);
        }
        else if (t>= 650 && t<850)
        {
          float v = (pi/199)*(t-650);
          return (uint8_t)(((uint16_t)(1500*cos(v)+2000))>>8);
        }
        else if (t>=850 && t<1000)
        {
          return (uint8_t)(((uint16_t)(500))>>8);
        }
    }

    else if (block < 8) // y axis
    {
       float s = t-1000;
       if (s>=0 && s<150) // LINE 1
        {
          //Serial.println((3000-5*(t-1000)));
          return (uint8_t)((uint16_t)(3500-10*s)>>8);
        }
        else if (s>=150 && s<350)
        {
          float v = (pi/199)*(s-150) + pi;
          //Serial.println(((uint16_t)(1500*sin(v)+1500)));
          return (uint8_t)(((uint16_t)(1500*sin(v)+2010))>>8);
        }
        else if (s>=350 && s<500)
        {
          //Serial.println((-1995+5*(t-1000)));
          return (uint8_t)(((uint16_t)(10*s-1490))>>8);
        }
         else if (s>=500 && s<650)
        {
          return (uint8_t)(((uint16_t)(8500-10*s))>>8);
        }
        else if (s>= 650 && s<850)
        {
          float v = (pi/199)*(s-650);
          return (uint8_t)(((uint16_t)(-1500*sin(v)+2010))>>8);
        }
        else if (s>=850 && s<1000)
        {
         return (uint8_t)(((uint16_t)(10*s-6490))>>8);
        }
    }
    

  

}

uint8_t UBottomData(uint8_t address, int block, String pattern)
{
    int t = (address+block*250);

     if (block < 4) // x axis
    {
        if (t>=0 && t<150) // LINE 1
        {
          //Serial.print(t);
          //Serial.print((uint8_t)(((uint16_t)(500))));
          //Serial.println(";");
          return (uint8_t)(((uint16_t)(500)));
        }
        else if (t>=150 && t<350)
        {
          //Serial.print(t);
          float v = (pi/199)*(t-150) + pi;
          //Serial.print((uint8_t)(((uint16_t)(1500*cos(v)+2000))));
          //Serial.println(";");
          return (uint8_t)(((uint16_t)(1500*cos(v)+2000)));
        }
        else if (t>=350 && t<500)
        {
          //Serial.print(t);
          //Serial.print((uint8_t)(((uint16_t)(3500))));
          //Serial.println(";");
          return (uint8_t)(((uint16_t)(3500)));
        }
        else if (t>=500 && t<650)
        {
          //Serial.print(t);
          //Serial.print((uint8_t)(((uint16_t)(3500))));
          //Serial.println(";");
          return (uint8_t)(((uint16_t)(3500)));
        }
        else if (t>= 650 && t<850)
        {
           float v = (pi/199)*(t-650);
          //Serial.print(t);
          //Serial.print((uint8_t)(((uint16_t)(1500*cos(v)+2000))));
          //Serial.println(";");
          return (uint8_t)(((uint16_t)(1500*cos(v)+2000)));
        }
        else if (t>=850 && t<1000)
        {
          //Serial.print(t);
          //Serial.print((uint8_t)(((uint16_t)(500))));
          //Serial.println(";");
          return (uint8_t)(((uint16_t)(500)));
        }
        
    }

    else if (block < 8) // y axis
    {
       float s = t-1000;
       if (s>=0 && s<150) // LINE 1
        {
          //Serial.println(s);
          //Serial.println((3000-5*(t-1000)));
          return (uint8_t)((uint16_t)(3500-10*s));
        }
        else if (s>=150 && s<350)
        {
          //Serial.println(s);
          float v = (pi/199)*(s-150) + pi;
          //Serial.println(((uint16_t)(1500*sin(v)+1500)));
          return (uint8_t)(((uint16_t)(1500*sin(v)+2010)));
        }
        else if (s>=350 && s<500)
        {
          //Serial.println(s);
          //Serial.println((-1995+5*(t-1000)));
          return (uint8_t)(((uint16_t)(10*s-1490)));
        }
         else if (s>=500 && s<650)
        {
          //Serial.println(s);
          return (uint8_t)(((uint16_t)(8500-10*s)));
        }
        else if (s>= 650 && s<850)
        {
          //Serial.println(s);
          float v = (pi/199)*(s-650);
          return (uint8_t)(((uint16_t)(-1500*sin(v)+2010)));
        }
        else if (s>=850 && s<1000)
        {
         //Serial.println(s);
         return (uint8_t)(((uint16_t)(10*s-6490)));
        }
    }
    

  
}
























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

*/
