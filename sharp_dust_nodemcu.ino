#include <Chrono.h>
//Chrono sharp;
Chrono wait1(Chrono::MICROS);                                                    // timer for 10 second intervals for publishing level sensor to Serial
Chrono wait2(Chrono::MICROS);

//Dust sensor Specifications
const int   sharpLEDPin         =      5;   // Digital pin 5 connect to sensor LED.
const int   sharpVoPin          =      A0;   // Analog pin 0 connect to sensor Vo.

#define     USE_AVG                          // For averaging last N raw voltage readings.
#ifdef      USE_AVG                               
#define     N                          100
static unsigned long VoRawTotal =      0;
static int  VoRawCount          =      0;
#endif // USE_AVG

static float Voc                 =     0.6;   // Typical output voltage in Volts when there is zero dust.
const float K                    =     0.5;   // Typical sensitivity in units of V per 100ug/m3.

float       dustDensity          =     0;
float       dV                   =     0;
/////////////////////////////////////////////////////////////////////////////////////
void setup() {
    Serial.begin(115200);
}

void loop() {
  GetSHARPData();
}

void GetSHARPData() {  
  digitalWrite(sharpLEDPin, LOW);               // Turn on the dust sensor LED by setting digital pin LOW.
  wait1.restart();                              // Add a 280us delay between sensor readings
  while (wait1.hasPassed(280)) {}
  //delayMicroseconds(280);                     // Wait 0.28ms before taking a reading of the output voltage as per spec.
  int VoRaw = analogRead(sharpVoPin);           // Record the output voltage. This operation takes around 100 microseconds.
  digitalWrite(sharpLEDPin, HIGH);              // Turn the dust sensor LED off by setting digital pin HIGH.
  wait2.restart();                              // Add a 9620us delay between sensor readings
  while (wait2.hasPassed(9620)) {}
  //delayMicroseconds(9620);                    // Wait for remainder of the 10ms cycle = 10000 - 280 - 100 microseconds.
  
  float Vo = VoRaw;                             // Smoothing data by averaging
  #ifdef USE_AVG
  VoRawTotal += VoRaw;
  VoRawCount++;
  if ( VoRawCount >= N ) {
    Vo = 1.0 * VoRawTotal / N;
    VoRawCount = 0;
    VoRawTotal = 0;
  } else {
    return;
  }
  #endif // USE_AVG
  
  Vo = Vo / 1024.0 * 5.0;                        // Compute the output voltage in Volts.
  dV = Vo - Voc;                                 // Convert to Dust Density in units of ug/m3.
  if ( dV < 0 ) {
    dV = 0;
    Voc = Vo;
  }
  dustDensity = dV / K * 100.0;
  //Serial.println("Vo" + String(Vo*1000.0) + "mV");
  //Serial.println("Dust Density" + String(dustDensity) + "ug/m3");
  Serial.println(dustDensity);
}
