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

const unsigned long POWEROFF_TIMEOUT = 1000uL * 60uL * 60uL;
const unsigned long STARTUP_DELAY    = 5000uL;
const unsigned long TOGGLE_DELAY     = 500uL;

// Deadlines are initialised in setup(); millis() is not reliable yet
// while the static initialisers are running.
unsigned long next_check;
unsigned long poweroff;

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

  next_check = millis() + STARTUP_DELAY;
  poweroff   = millis() + POWEROFF_TIMEOUT;
}

#undef max

void loop()
{
  unsigned long current_millis = millis();
  
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
            
 // Serial.println(acBuffer);

  if(labs(capacitance - last_capacitance) > 50){
    // Rollover-safe "has the deadline passed?" test
    if( (long)(millis() - next_check) >= 0 ) {
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
      poweroff = millis() + POWEROFF_TIMEOUT;
    }
    else {
      Serial.println("lamp off");
      digitalWrite(SWITCH_PIN, LOW);
    }

    next_check = millis() + TOGGLE_DELAY;
    lamp_pin_state = lamp;
  }

  // Auto power-off after POWEROFF_TIMEOUT.
  // Rollover-safe: the (long) cast makes the subtraction still correct when
  // millis() wraps around (it overflows every ~49.7 days on AVR).
  if( (long)(millis() - poweroff) >= 0 ) {
    lamp = 0;
  }

  // Wait for 50 milliseconds
  while(millis() - current_millis < 150);
}
