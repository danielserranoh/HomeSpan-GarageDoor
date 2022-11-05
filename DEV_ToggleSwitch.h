
//// Define the TogglePin Service ////

struct DEV_ToggleSwitch : Service::Switch{

  int togglePin;                         // store the pin number connected to a hypothetical relay that turns the Toggle Pin on/off
  SpanCharacteristic *power;             // reference to the On Characteristic
  
 DEV_ToggleSwitch(int togglePin) : Service::Switch(){       // constructor() method for TogglePin defined with one parameter.  Note we also call the constructor() method for the Switch Service.

    power=new Characteristic::On(false);             // instantiate the On Characteristic and save it as power

    this->togglePin=togglePin;                  // save the pin number for the hypothetical relay
    pinMode(togglePin,OUTPUT);                  // configure the pin as an output using the standard Arduino pinMode function 

    Serial.print("ConfiguringTogglePin: Pin=");   // initialization message
    Serial.print(togglePin);
    Serial.print("\n");                      
    
  } // end constructor()
  
  boolean update(){                          // update() method

    // NEW! Instead of turning on or off the LED according to newValue, we turn it on and leave it in the off position when finished.

      if(power->getNewVal()){                       // check to ensure HomeKit is requesting we "turn on" this device (else ignore)

      LOG1("Flashing the LED on pin=");
      LOG1(togglePin);
      LOG1("\n");

      digitalWrite(togglePin,HIGH);                   // turn pin on
      delay(250);                                     // wait 1250 ms
      digitalWrite(togglePin,LOW);                    // turn pin off
  
    } // if newVal=true
   
    return(true);                            // return true to let HomeKit (and the Home App Client) know the update was successful
  
  } // end update()
  
  // NEW! Here we implement a very simple loop() method that checks to see if the power Characteristic
  // is "on" for at least 3 seconds.  If so, it resets the value to "off" (false).

  void loop(){

    if(power->getVal() && power->timeVal()>500){   // check that power is true, and that time since last modification is greater than 0,5 seconds 
      LOG1("Resetting ToggleSwitch Control\n");     // log message  
      power->setVal(false);                         // set power to false
    }      
    
  } // loop
  
};