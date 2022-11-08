////////////////////////////////////////
//   DEVICE-SPECIFIC SENSOR SERVICES //
//////////////////////////////////////

struct DEV_AnalogSensor : Service::LightSensor {     // A standalone Reed Sensor

  int SENSORPIN;
  int magneticSignal;

  int OPEN = 0;
  int CLOSED = 1;  
  
  SpanCharacteristic *state;                         // reference to the Current Sensor State Characteristic
  
  
  DEV_AnalogSensor(int SENSORPIN) : Service::LightSensor(){       // constructor() method
    
    // First we instantiate the main Characteristic for a Conctact Sensor, namely the Current State. To set its initial value
    // we would take a reading and initialize it to that value. 
    
    state=new Characteristic::CurrentAmbientLightLevel();             // instantiate the ContactSensorSate Characteristic and save it as state, initial value of 1 means closed
       
    this->sensorPin=sensorPin;                  // save the pin number for the hypothetical relay
    pinMode(sensorPin,INPUT);            // configure the pin as an input using the standard Arduino pinMode function - Set To Pull up to avoid signal floating
        
    magneticSignal = analogRead(sensorPin);                     // read the board pin defined previously
    
    
    state->setVal(magneticSignal);                               // set the real ContactSensorState in HomeKit

    
    Serial.print("- Configuring sensorPin: Pin=");               // initialization message of the Pin Setup
    Serial.print(SENSORPIN);
    Serial.print("\n");

  } // end constructor

  // Next we create the loop() method. This method take no arguments and returns no values. In order to simulate a state change
  // from an actual sensor we will read the current value of the input using the getVal() function, with <float> as the
  // template parameter; 

  // All of the action happens in the setVal() line where we set the value of the temp Characteristic to the new value of temperature.
  // This tells HomeKit to send an Event Notification message to all available Controllers making them aware of the new temperature.
  // Note that setVal() is NOT a template function and does not require you to specify <float> as a template parameter.  This is because
  // setVal() can determine the type from the argument you specify.  If there is any chance of ambiguity, you can always specifically
  // cast the argument such: setVal((float)temperature).

  void loop(){

    Serial.println(analogRead(SENSORPIN));
    delay(1000);
  } // loop
  
};