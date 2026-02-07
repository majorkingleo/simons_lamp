#include <CapacitiveSensor.h>

//const uint8_t CAPACITIVE_SENSOR_SEND_PIN    = 2;
//const uint8_t CAPACITIVE_SENSOR_RECEIVE_PIN = 12;

const uint8_t CAPACITIVE_SENSOR_SEND_PIN    = 6;
const uint8_t CAPACITIVE_SENSOR_RECEIVE_PIN = 7;


const uint8_t SWITCH_PIN = 10;

CapacitiveSensor sensor = CapacitiveSensor( CAPACITIVE_SENSOR_SEND_PIN , CAPACITIVE_SENSOR_RECEIVE_PIN );

bool lamp = 0;
bool lamp_pin_state = 0;
long last_capacitance = 0;
unsigned long next_check = millis() + 5000;

void setup()
{
  pinMode(SWITCH_PIN, OUTPUT);
  digitalWrite(SWITCH_PIN, LOW);
  Serial.begin(9600);

  //sensor.capacitiveSensor(1); // allow one calibration

  // Disable the automatic re-calibration feature of the
  // capacitive sensor library
  sensor.set_CS_AutocaL_Millis(0xFFFFFFFF);  
  //sensor.set_CS_AutocaL_Millis(20);  
}

void loop()
{
  long current_millis = millis();
  
  long capacitance = sensor.capacitiveSensor(30);
  long capacitance_raw = sensor.capacitiveSensorRaw(30);
  
  // Print the result of the sensor reading
  // Note that the capacitance value is an arbitrary number
  // See: https://playground.arduino.cc/Main/CapacitiveSensor/ for details
  char acBuffer[100];
  snprintf( acBuffer, sizeof(acBuffer), "% 4ld Raw: % 4ld diff %4ld", 
            capacitance, 
            capacitance_raw,
            capacitance_raw - capacitance );
            
  Serial.println(acBuffer);

  if(abs(capacitance - last_capacitance) > 50){
    if( next_check < millis() ) {
      if(lamp == 0){
        lamp = 1;
      }
      else{
        lamp = 0;
      }
    }
  }

  last_capacitance = capacitance;

  if( lamp != lamp_pin_state ) {  
    if(lamp == 1){
      Serial.println("lamp on");
      digitalWrite(SWITCH_PIN, HIGH);
    }
    else {
      Serial.println("lamp off");
      digitalWrite(SWITCH_PIN, LOW);
    }

    next_check = millis() + 500;
    lamp_pin_state = lamp;
  }

  // Wait for 50 milliseconds
  while(millis() - current_millis < 150);
}
