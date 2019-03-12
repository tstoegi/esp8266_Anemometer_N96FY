#include <N96FY.h>

N96FY anemometer;

void setup() {
  Serial.begin(9600);
  anemometer.begin();
}

void loop()
{
  Serial.print("Windspeed in Km/h: ");
  Serial.println(anemometer.speedInKMH());
  
  Serial.print("Windspeed in MPH : ");
  Serial.println(anemometer.speedInMPH());
  
  delay(5000);
}
