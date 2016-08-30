//******************************
 //*Abstract: Code used to log total particle concentration of matter with assumed DENSITY and MAX_PARTICLE_SIZE
 //           using PLX-DAQ software for Excel. Also contains code for using GoBetwino software.
 //
 //*Author：Jamie Van de Laar
 //*Modified from program originally written by Zuyang @ HUST
 //*Date：August.16.2016
 //******************************


 // Green is TX on Arduino!!
 
#include <Arduino.h>
#define LENG 31               //0x42 + 31 bytes equal to 32 bytes
#define DENSITY 1.31e-6       // Density of particulates in ug/um^3, PMS1003 assumes ~3.67 for PM readings
#define MAX_PARTICLE_SIZE 40  // Max expected particle size in microns - USING 40 FOR AC FINE DUST AND ISO DUST!!
#define DP 2                  // Decimal places for display of floats
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
float totalConc_prev = 0;

float sampleTime = 0;

 
void setup()
{
  Serial.begin(9600);   //use serial0
  Serial.setTimeout(1500);    //set the Timeout to 1500ms, longer than the data transmission periodic time of the sensor

  // Create label - only for PLX-DAQ!!!
  Serial.println("LABEL,Time of day,Time Since Start,Conc");
}
 
void loop()
{
  // Sensor seems to get new data every 2.17 seconds... or 1.45 seconds...?
  
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

        // Sum to get total concentration in ug/m^3
        totalConc = /*PM005conc +*/ PM01conc + PM2_5conc + PM5conc + PM10conc + PM_Over10conc;
      }
    } 
  }
 
  static unsigned long OledTimer=millis();  
    if (millis() - OledTimer >= 2000) 
//    if (totalConc != totalConc_prev)
    {
//      sampleTime = float(millis() - OledTimer)/1000;
      OledTimer = millis();

      // GoBetwino Suff
//      Serial.print("#S|PMSLOG|[");
//      Serial.print(float(OledTimer)/1000, DP);
//      Serial.print(",");
//      Serial.print(totalConc, DP);
//      Serial.println("]#");

      // PLX-DAQ Stuff
//      Serial.print("DATA,TIME,");
      Serial.print("DATA,TIME,");
      Serial.print(float(OledTimer)/1000, DP);
      Serial.print(",");
//      Serial.print(sampleTime, DP);
//      Serial.print(",");
      Serial.println(totalConc, DP);

//      totalConc_prev = totalConc;

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
  C = DENSITY*10000*float(numParticles)*(4/3)*M_PI*avgRadius*avgRadius*avgRadius;
  return C;
}

