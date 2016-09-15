//******************************
 //*Abstract: Read value of PM1,PM2.5 and PM10 of air quality
 //
 //*Version：V3.1
 //*Author：Zuyang @ HUST
 //*Modified by Cain for Arduino Hardware Serial port compatibility
 //*Date：March.25.2016
 //******************************


 // Green is TX on Arduino!!
 
#include <Arduino.h>
#define LENG 31                     //0x42 + 31 bytes equal to 32 bytes
#define DENSITY 3.67e-6  //0.000001    // Density of particulates in ug/um^3, PMS1003 assumes ~3.67 for PM readings
#define MAX_PARTICLE_SIZE 20        // Max expected particle size in microns
unsigned char buf[LENG];
 
int PM01Value = 0;          //define PM1.0 value of the air detector module
int PM2_5Value = 0;         //define PM2.5 value of the air detector module
int PM10Value = 0;         //define PM10 value of the air detector module
unsigned long numParticles_over_0_3 = 0;
unsigned long numParticles_over_0_5 = 0;
unsigned long numParticles_over_1 = 0;
unsigned long numParticles_over_2_5 = 0;
unsigned long numParticles_over_5 = 0;
unsigned long numParticles_over_10 = 0;

//float myPM01conc = 0;
//float myPM2_5conc = 0;
//float myPM10conc = 0;

float PM005conc = 0;      // Conc between 0.3 and 0.5
float PM01conc = 0;       // Conc between 0.5 and 1.0
float PM2_5conc = 0;      // Conc between 1.0 and 2.5
float PM5conc = 0;        // Conc between 2.5 and 5.0
float PM10conc = 0;       // Conc between 5.0 and 10.0
float PM_Over10conc = 0;  // Conc between 10.0 and MAX_PARTICLE_SIZE
float totalConc = 0;      // Total concentration in ug/m^3
 
void setup()
{
  Serial.begin(9600);   //use serial0
  Serial.setTimeout(1500);    //set the Timeout to 1500ms, longer than the data transmission periodic time of the sensor
 
}
 
void loop()
{
  if(Serial.find(0x42)){    //start to read when detect 0x42
    Serial.readBytes(buf,LENG);
//    Serial.println("buf");
//    Serial.println(*buf);
    if(buf[0] == 0x4d){
      if(checkValue(buf,LENG)){
        PM01Value = returnData(buf, 3);   //count PM1.0 value of the air detector module
        PM2_5Value = returnData(buf, 5);  //count PM2.5 value of the air detector module
        PM10Value = returnData(buf, 7);   //count PM10 value of the air detector module 
        numParticles_over_0_3 = returnData(buf, 15);
        numParticles_over_0_5 = returnData(buf, 17);
        numParticles_over_1 = returnData(buf, 19);
        numParticles_over_2_5 = returnData(buf, 21);
        numParticles_over_5 = returnData(buf, 23);
        numParticles_over_10 = returnData(buf, 25);
        
//        myPM01conc = return_ugConc(numParticles_over_0_3 - numParticles_over_1, (0.3 + 1)/2/2);
//        myPM2_5conc = return_ugConc(numParticles_over_0_3 - numParticles_over_2_5, (0.3 + 2.5)/2/2);
//        myPM10conc = return_ugConc(numParticles_over_2_5 - numParticles_over_10, (2.5 + 10)/2/2);

        PM005conc = return_ugConc(numParticles_over_0_3 - numParticles_over_0_5, (0.3 + 0.5)/2/2);
        PM01conc = return_ugConc(numParticles_over_0_5 - numParticles_over_1, (0.5 + 1)/2/2);
        PM2_5conc = return_ugConc(numParticles_over_1 - numParticles_over_2_5, (1 + 2.5)/2/2);
        PM5conc = return_ugConc(numParticles_over_2_5 - numParticles_over_5, (2.5 + 5)/2/2);
        PM10conc = return_ugConc(numParticles_over_5 - numParticles_over_10, (5 + 10)/2/2);
        PM_Over10conc = return_ugConc(numParticles_over_10, (10 + MAX_PARTICLE_SIZE)/2/2);

        totalConc = PM005conc + PM01conc + PM2_5conc + PM5conc + PM10conc + PM_Over10conc;
      }
    } 
  }
 
  static unsigned long OledTimer=millis();  
    if (millis() - OledTimer >=1000) 
    {
      OledTimer=millis(); 
       
      Serial.print("PM1.0:             ");  
      Serial.print(PM01Value);
      Serial.println("  ug/m3");            
     
      Serial.print("PM2.5:             ");  
      Serial.print(PM2_5Value);
      Serial.println("  ug/m3");     
       
      Serial.print("PM10:              ");  
      Serial.print(PM10Value);
      Serial.println("  ug/m3");   

//      Serial.print("My PM01 conc:      ");  
//      Serial.print(myPM01conc, 2);
//      Serial.println("  ug/m3");
//
//      Serial.print("My PM2.5 conc:     ");  
//      Serial.print(myPM2_5conc, 2);
//      Serial.println("  ug/m3");
//
//      Serial.print("My PM10 conc:      ");  
//      Serial.print(myPM10conc, 2);
//      Serial.println("  ug/m3");
//      Serial.println();

      Serial.print("Total conc:        ");
      Serial.print(totalConc);   // In ug/m3
      Serial.println("  ug/m3"); 
//      Serial.print(totalConc/1000);   // In mg/m3
//      Serial.println("  mg/m3");   

      Serial.print("PM0.05 conc:       ");  
      Serial.print(PM005conc);
      Serial.println("  ug/m3");  

      Serial.print("PM0.1 conc:        ");  
      Serial.print(PM01conc);
      Serial.println("  ug/m3");  

      Serial.print("PM2.5 conc:        ");  
      Serial.print(PM2_5conc);
      Serial.println("  ug/m3");  

      Serial.print("PM5 conc:          ");  
      Serial.print(PM5conc);
      Serial.println("  ug/m3");  

      Serial.print("PM10 conc:         ");  
      Serial.print(PM10conc);
      Serial.println("  ug/m3");  

      Serial.print("Over 10.0 in 0.1L: ");  
      Serial.print(numParticles_over_10);
      Serial.println("  particles");
//
//      for (int j=0; j <= LENG; j++) {
//        Serial.print(buf[j]);
//        Serial.print(" ");
//      }
      
      Serial.println();
      Serial.println();
    }
   
}


char checkValue(unsigned char *thebuf, char leng)
{  
  char receiveflag=0;
  int receiveSum=0;
 
  for(int i=0; i<(leng-2); i++){
  receiveSum=receiveSum+thebuf[i];
  }
  receiveSum=receiveSum + 0x42;
  
  if(receiveSum == ((thebuf[leng-2]<<8)+thebuf[leng-1]))  //check the serial data 
  {
    receiveSum = 0;
    receiveflag = 1;
  }
  return receiveflag;
}

//// OLD FUNCTION, NOW UNUSED!!
//int transmitPM01(unsigned char *thebuf)
//{
//  int PM01Val;
//  PM01Val=((thebuf[3]<<8) + thebuf[4]); //count PM1.0 value of the air detector module
//  return PM01Val;
//}

unsigned long returnData(unsigned char *thebuf, int index1)
{
  unsigned long particleVal;
  particleVal = ((thebuf[index1]<<8) + thebuf[index1 + 1]); //count specified particles
  return particleVal;
}

// Return concentration in ug/m^3 for given number of particles with given average radius in microns
float return_ugConc(unsigned long numParticles, float avgRadius)
{
  float C;
  C = DENSITY*10000*numParticles*(4/3)*M_PI*avgRadius*avgRadius*avgRadius;
  return C;
}

