//****************************************************************************************************************
//*Abstract: Code used to display incremental particle concentrations (with percentages) and total concentration 
//           of particles with assumed DENSITY and MAX_PARTICLE_SIZE. This was run using an Arduino Nano.
//
//*Author：Jamie Van de Laar
//*Modified from program originally written by Zuyang @ HUST
//*Last modified：September.28.2016
//
//*Notes: -Green wire is TX on Arduino
//        -"Sleep" pin must be grounded in order to upload code to Arduino while PMS1003 is connected
//****************************************************************************************************************

#include <Arduino.h>

/*********************** Define Constants ************************/ 

#define LENG 31               // 0x42 + 31 bytes equal to 32 bytes
#define DENSITY 3.67e-6       // Bulk density of particulates in ug/um^3, PMS1003 assumes approx 3.67e-6 for PM readings
#define MAX_PARTICLE_SIZE 40  // Max expected particle size in microns - Using 40 for AC Fine Test Dust and 
                              // ISO 12103-1, A2 Fine Test Dust
#define DP 2                  // Decimal places for display of floats


/*********************** Define Globals ************************/

unsigned char buf[LENG];

// Variables for storing sensor-calculated PM values in ug/m^3
int PM01Value = 0;
int PM2_5Value = 0;
int PM10Value = 0;

// Variables for storing number of particles up to each size bin
unsigned long numParticles_over_0_3 = 0;
unsigned long numParticles_over_0_5 = 0;
unsigned long numParticles_over_1 = 0;
unsigned long numParticles_over_2_5 = 0;
unsigned long numParticles_over_5 = 0;
unsigned long numParticles_over_10 = 0;

// Calculated PM values to compare to sensor given values
float myPM01conc = 0;
float myPM2_5conc = 0;
float myPM10conc = 0;

// Variables for concentrations in ug/m^3
float PM005conc = 0;      // Conc between 0.3 and 0.5
float PM01conc = 0;       // Conc between 0.5 and 1.0
float PM2_5conc = 0;      // Conc between 1.0 and 2.5
float PM5conc = 0;        // Conc between 2.5 and 5.0
float PM10conc = 0;       // Conc between 5.0 and 10.0
float PM_Over10conc = 0;  // Conc between 10.0 and MAX_PARTICLE_SIZE
float totalConc = 0;      // Total concentration in ug/m^3
float totalConc_prev = 0; // Previous total concentration, stored so it is known when the sensor has new data


/*********************** Setup Code ************************/

void setup()
{
  Serial.begin(9600);   //use serial0
  Serial.setTimeout(1500);    //set the Timeout to 1500ms, longer than the data transmission periodic time of the sensor
}


/*********************** Main Routine ************************/
 
void loop()
{
  // Start to read from sensor when detect 0x42
  if(Serial.find(0x42)){
    
    Serial.readBytes(buf, LENG);
    if(buf[0] == 0x4d){
      if(checkValue(buf, LENG)){
        
        // Return sensor calculated PM values
        PM01Value = returnData(buf, 3);
        PM2_5Value = returnData(buf, 5);
        PM10Value = returnData(buf, 7);
        
        // Return number of particles over each threshold
        numParticles_over_0_3 = returnData(buf, 15);
        numParticles_over_0_5 = returnData(buf, 17);
        numParticles_over_1 = returnData(buf, 19);
        numParticles_over_2_5 = returnData(buf, 21);
        numParticles_over_5 = returnData(buf, 23);
        numParticles_over_10 = returnData(buf, 25);

        // Calculate PM1/PM2.5/PM10 values to compare to sensor given values, uncomment to use
//        myPM01conc = return_ugConc(numParticles_over_0_3 - numParticles_over_1, (0.3 + 1)/2/2);
//        myPM2_5conc = return_ugConc(numParticles_over_1 - numParticles_over_2_5, (1 + 2.5)/2/2);
//        myPM10conc = return_ugConc(numParticles_over_2_5 - numParticles_over_10, (2.5 + 10)/2/2);

        // Calculate particle concentration in each size bin
        PM005conc = return_ugConc(numParticles_over_0_3 - numParticles_over_0_5, (0.3 + 0.5)/2/2);
        PM01conc = return_ugConc(numParticles_over_0_5 - numParticles_over_1, (0.5 + 1)/2/2);
        PM2_5conc = return_ugConc(numParticles_over_1 - numParticles_over_2_5, (1 + 2.5)/2/2);
        PM5conc = return_ugConc(numParticles_over_2_5 - numParticles_over_5, (2.5 + 5)/2/2);
        PM10conc = return_ugConc(numParticles_over_5 - numParticles_over_10, (5 + 10)/2/2);
        PM_Over10conc = return_ugConc(numParticles_over_10, (10 + MAX_PARTICLE_SIZE)/2/2);

        // Sum to get total concentration in ug/m^3
        totalConc = PM005conc + PM01conc + PM2_5conc + PM5conc + PM10conc + PM_Over10conc;
      }
    } 
  }
 
  if (totalConc != totalConc_prev)  // If the sensor has updated the data, display the new data
  {
    // Uncomment following code to compare calculated PM concentration to sensor given values
    
//    Serial.print("PM1.0:             ");  
//    Serial.print(PM01Value);
//    Serial.println("  ug/m3");            
//   
//    Serial.print("PM2.5:             ");  
//    Serial.print(PM2_5Value);
//    Serial.println("  ug/m3");     
//     
//    Serial.print("PM10:              ");  
//    Serial.print(PM10Value);
//    Serial.println("  ug/m3");   
//
//    Serial.print("My PM01 conc:      ");  
//    Serial.print(myPM01conc, DP);
//    Serial.println("  ug/m3");
//
//    Serial.print("My PM2.5 conc:     ");  
//    Serial.print(myPM2_5conc, DP);
//    Serial.println("  ug/m3");
//
//    Serial.print("My PM10 conc:      ");  
//    Serial.print(myPM10conc, DP);
//    Serial.println("  ug/m3");
//    Serial.println();


    Serial.print("Total conc:             ");
    Serial.print(totalConc, DP);   // In ug/m3
    Serial.println("  ug/m3"); 
//    Serial.print(totalConc/1000, DP);   // In mg/m3
//    Serial.println("  mg/m3");

    Serial.print("0.03 to 0.05 um conc:   ");  
    Serial.print(PM005conc, DP);
    Serial.print("  ug/m3,   ");
    Serial.print((PM005conc/totalConc)*100, DP);
    Serial.println("%");

    Serial.print("0.05 to 0.1 um conc:    ");  
    Serial.print(PM01conc, DP);
    Serial.print("  ug/m3,   ");  
    Serial.print((PM01conc/totalConc)*100, DP);
    Serial.println("%");

    Serial.print("0.1 to 2.5 um conc:     ");  
    Serial.print(PM2_5conc, DP);
    Serial.print("  ug/m3,   ");  
    Serial.print((PM2_5conc/totalConc)*100, DP);
    Serial.println("%");

    Serial.print("2.5 to 5 um conc:       ");  
    Serial.print(PM5conc, DP);
    Serial.print("  ug/m3,   ");  
    Serial.print((PM5conc/totalConc)*100, DP);
    Serial.println("%");

    Serial.print("5 to 10 um conc:        ");  
    Serial.print(PM10conc, DP);
    Serial.print("  ug/m3,   ");  
    Serial.print((PM10conc/totalConc)*100, DP);
    Serial.println("%");  

    Serial.print(">10 um conc:            ");  
    Serial.print(PM_Over10conc, DP);
    Serial.print("  ug/m3,   ");  
    Serial.print((PM_Over10conc/totalConc)*100, DP);
    Serial.println("%");  

    Serial.print("Over 10.0 in 0.1L: ");  
    Serial.print(numParticles_over_10);
    Serial.println("  particles");
    
    Serial.println();
    Serial.println();

    totalConc_prev = totalConc;
  }
   
}


/*********************** Define Functions ************************/

// Check the data from the sensor is valid
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


// Returns 16 bit data from the specified starting index in the buffer (index of the first byte)
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


