///////////////////////////////////////////////
//   DEVICE-SPECIFIC GARAGE DOOR SERVICES   //
/////////////////////////////////////////////
//
// Based ON: https://github.com/HomeSpan/HomeSpan/blob/master/examples/13-TargetStates/DEV_DoorsWindows.h
// The above logic only manages the interactions with HomeKit, not with the ESP32 I/O System
// The limitPin is for dev purposes only. Should be removed for production

// GarageDoor Current State options: 
    // 0 = Open
    // 1 = Closed
    // 2 = Opening
    // 3 = Closing
    // 4 = Stopped

// Garage Door TARGET State options:
    // 0 = Open
    // 1 = Close

struct DEV_GarageDoor : Service::GarageDoorOpener {     // A Garage Door Opener

  int OPEN = 0;
  int CLOSED = 1;   
  int OPENING = 2;
  int CLOSING = 3;
  int STOPPED = 4;

  int activationPin;
  int warnPin;
  int photoSensorPin;
  int reedSensorPin;

  int photoSensorState;
  int reedSensorState;

  SpanCharacteristic *current;            // reference to the Current Door State Characteristic (specific to Garage Door Openers)
  SpanCharacteristic *target;             // reference to the Target Door State Characteristic (specific to Garage Door Openers)  
  SpanCharacteristic *obstruction;        // reference to the Obstruction Detected Characteristic (specific to Garage Door Openers)


  // The lock can be integrated in the Garage Door, instead of being a separate service. See HAP Documentation.
  //SpanCharacteristic *lockCurrent;        // reference to the Current Lock State Characteristic (optional to Garage Door Openers)
  //SpanCharacteristic *lockTarget;         // reference to the Target Lock State Characteristic (optional to Garage Door Openers)

  DEV_GarageDoor(int activationPin, int photoSensorPin, int reedSensorPin,  int warnPin) : Service::GarageDoorOpener(){       // constructor() method
        
    current=new Characteristic::CurrentDoorState(1);              // initial value of 1 means closed
    target=new Characteristic::TargetDoorState(1);                // initial value of 1 means closed
    obstruction=new Characteristic::ObstructionDetected(false);   // initial value of false means NO obstruction is detected

    this->activationPin=activationPin;                                    // store the activationPin value in the field
    pinMode(activationPin, OUTPUT);
    this->warnPin=warnPin;
    pinMode(warnPin, OUTPUT);
    this->photoSensorPin=photoSensorPin;
    this->reedSensorPin=reedSensorPin;
    
    // INITIALIZATION PROCEDURE //
    // Visually check that the Leds are working and config is OK
    LOG0("- Configuring activationPin at PIN ");
    LOG0(activationPin);
    LOG0("\n");
    digitalWrite(activationPin,LOW);                   // turn pin off for relay
    
    LOG0("- Configuring warnPin at PIN ");
    LOG0(warnPin);
    LOG0("\n");
    digitalWrite(warnPin,HIGH);                   // turn pin for warnLed on
    
    // turn off after check
    delay(250);                                     // wait 250 ms
    digitalWrite(activationPin,LOW);                // turn pin off
    digitalWrite(warnPin,LOW);                      // turn pin off
    Serial.print("Garage Door ready \n");           // initialization message of the Service
  } // end constructor




  boolean update(){                              // update() method

    LOG1("*** Action requested  by changing Target");

    if(target->getNewVal()==OPEN){                  // if the target-state value is set to 0, HomeKit is requesting the door to be in open position
      // But if the door was already opening and we get a new signal, door must be stopped, not closed (to follow the logic of the Deimos board)
      if(current->getVal()==CLOSING){
        LOG1("Garage Door was closing and requested opening. Stop first\n");
        current->setVal(STOPPED);                   // Set currentState to Stopped
        return(false);
      }
      LOG1("Opening Garage Door\n");                
      current->setVal(OPENING);                     // set the current-state value to 2, which means "opening"      
      obstruction->setVal(false);                   // clear any prior obstruction detection
    } else {
      if(obstruction->getVal()==true){              // Testing if there is an obstruction. If there is, return to exit the loop, otherwise keep closing
      LOG1("Garage Door obstruction. Can't close\n");
      // ¿Shall I change target to 0 to open the door?
        return(false);
      }
      if(current->getVal()==OPENING){
        LOG1("Garage Door was opening and requested closing. Stop first\n");
        current->setVal(STOPPED);                   // Set currentState to Stopped
        return(false);
      }
      LOG1("Closing Garage Door\n");                // else the target-state value is set to 1, and HomeKit is requesting the door to be in the closed position
      current->setVal(CLOSING);                     // set the current-state value to 3, which means "closing" 
      obstruction->setVal(false);                   // clear any prior obstruction detection                            
      LOG1("Turn off the LED on pin=");
      LOG1(warnPin);
      LOG1("\n");
      digitalWrite(warnPin,LOW);                   // turn pin off          
    }
    LOG1("Trigger the relay on pin=");             // TRIGGER TO ACTIVATE THE DOOR
    digitalWrite(activationPin,LOW);               // turn pin off
    delay(250);                                    // wait 250 ms
    digitalWrite(activationPin,HIGH);              // turn pin on
    LOG1("Door Activated \n");
    return(true);                                  // return true
  
  } // update




  void loop(){                                     // loop() method
     if(obstruction->timeVal()>500){     // check time elapsed since last update and proceed only if greater than 0,5 seconds
        // Read the Reed Sensor (Walking Door)
        reedSensorState = digitalRead(reedSensorPin);
        // Read The Photovoltaic Sensor
        photoSensorState = digitalRead(photoSensorPin);
        if(photoSensorState == 0 || reedSensorState == 1){
          obstruction->setVal(true);                   // set obstruction-detected to true
        } else {
          obstruction->setVal(false);                   // set obstruction-detected to true
        }
     }
    
    if(current->getVal()==target->getVal()){
      return;                                       // if current-state matches target-state there is nothing do -- exit loop()
    }        
      

    if(obstruction->getVal()==true && current->getVal()==CLOSING){    // here we dcheck the obstruction obstruction, but only if the door is closing (not opening)
      current->setVal(STOPPED);                     // set the curent-state to 4, which means "stopped"
      digitalWrite(warnPin,HIGH);                   // turn warning pin on
      LOG1("Garage Door Obstruction Detected!\n");
    }

    if(current->getVal()==STOPPED){
      //LOG1("Door stopped\n");  
      return;                                       // if the current-state is stopped, there is nothing more to do - exit loop()  
    }                  
      

    // This last bit of code only gets called if the door is in a state that represents activationly opening or activationly closing.
    // If there is an obstruction, the door is "stopped" and won't start again until the HomeKit Controller requests a new open or close action

    if(current->getVal()!=target->getVal() && target->timeVal()>5000){ // simulate a garage door that takes 5 seconds to operate by monitoring time since target-state was last modified
      // habría que confirmar que el Sensor Portón confirma el cierre de la puerta. 
      // Si el tiempo desde la activación hasta recibir la señal de cierre excede X sg, avidar que puerta bloqueada o parada (o stopped) 
      LOG1("Door Inactive \n");
      current->setVal(target->getVal());           // set the current-state to the target-state
      if (target->getVal()==0){
        LOG1("Puerta Abierta \n");
      } else {
        LOG1("Puerta Cerrada \n");
      }
    }
  } // loop
  
};


////////////////////////////////////
