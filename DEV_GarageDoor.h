///////////////////////////////////////////////
//   DEVICE-SPECIFIC GARAGE DOOR SERVICES   //
/////////////////////////////////////////////
//
// Based ON: https://github.com/HomeSpan/HomeSpan/blob/master/examples/13-TargetStates/DEV_DoorsWindows.h
// The above logic only manages the interactions with HomeKit, not with the ESP32 I/O System
// Interesting inspiring Project: https://create.arduino.cc/projecthub/DVDMDN/automatic-sliding-door-for-the-garage-c7b1ba?ref=similar&ref_id=477071&offset=3
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

    // Casuisticas: 
    // [0 0 0 0] Puerta abierta Sensor Reed Abierto => STOP               -> [0 1 0 0] Señal cambio a cerrar => [0 0 0 0] No hacer nada. Volver a hacer que el target sea 0
    // [0 0 1 0] Puerta abierta Sensor Reed cerrado => REPOSO ABIERTO     -> [0 1 1 0] Señal cambio a cerrar => [3 1 1 0] Orden cerrar 
    // [1 1 0 -] Puerta cerrada. Sensor Puerta Abierto => STOP            -> [1 0 0 0] Señal cambio a abrir  => [1 1 0 -] No hacer nada. Volver a hacer que el target sea 1
    // [1 1 1 -] Puerta cerrada. Sensor Puerta cerrado => REPOSO CERRADO  -> [1 0 1 -] Señal cambio a abrir  => Abriendo pasar Estado portón a [2 0 1 -]
    // [1 1 1 1] Puerta cerrada. Sensor Foto Cerrado => REPOSO CERRADO    -> [1 0 1 1] Señal cambio a abrir  => Se puede abrir, aunque parece peligroso.
    // [2 0 1 -] Puerta abriendose. Sensor Puerta cerrado => ABRIENDO.    -> [2 0 0 -] Sensor puerta se abre => STOP (When cleared - Esperar un nuevo comando que debería seguir cerrando la puerta)
    // [2 0 1 1] Puerta abriendose. Sensor Foto Cerrado => Continua abriendose. -> [2 1 1 1] Si señal cambio (cerrar) => STOP (ya que es caso de ...)
    

    // En definitiva. Estando en STOP no hay que hacer caso a la señal de trigger, y hay que dejarla como estaba. (pensar con la lógica de la puerta)

    // Idea: Se pueden hacer dos funciones: funcion mandarPulso y funcion mandarDoblePulso.
    // TEST: usar SpanButton() para añadir un pulsador para abrir la puerta peatonal. ¿También para abrir puerta peatonal Garaje?

struct DEV_GarageDoor : Service::GarageDoorOpener {     // A Garage Door Opener

  int OPEN = 0;
  int CLOSED = 1;   
  int OPENING = 2;
  int CLOSING = 3;
  int STOPPED = 4;

  int activationPin;

  int photoSensorPin;
  int hallSensorPin;
  int reedSensorPin;
  int warnPin;

  int photoSensorState;
  int hallSensorState;
  int reedSensorState;

  int OPERATIONTIME = 20000;             // Time for a full door cycle (opening or closing) in ms

  SpanCharacteristic *current;            // reference to the Current Door State Characteristic (specific to Garage Door Openers)
  SpanCharacteristic *target;             // reference to the Target Door State Characteristic (specific to Garage Door Openers)  
  SpanCharacteristic *obstruction;        // reference to the Obstruction Detected Characteristic (specific to Garage Door Openers)


  // The lock can be integrated in the Garage Door, instead of being a separate service. See HAP Documentation.
  //SpanCharacteristic *lockCurrent;        // reference to the Current Lock State Characteristic (optional to Garage Door Openers)
  //SpanCharacteristic *lockTarget;         // reference to the Target Lock State Characteristic (optional to Garage Door Openers)

  DEV_GarageDoor(int activationPin, int photoSensorPin, int hallSensorPin, int reedSensorPin,  int warnPin) : Service::GarageDoorOpener(){       // constructor() method
        
    current=new Characteristic::CurrentDoorState(1);              // initial value of 1 means closed
    target=new Characteristic::TargetDoorState(1);                // initial value of 1 means closed
    obstruction=new Characteristic::ObstructionDetected(false);   // initial value of false means NO obstruction is detected

    this->activationPin=activationPin;                                    // store the activationPin value in the field
    pinMode(activationPin, OUTPUT);
    this->warnPin=warnPin;
    pinMode(warnPin, OUTPUT);
    this->photoSensorPin=photoSensorPin;
    this->photoSensorPin=hallSensorPin;
    this->reedSensorPin=reedSensorPin;
    
    // INITIALIZATION PROCEDURE //
    // Visually check that the Leds are working and config is OK
    
    //LOG0("- Configuring activationPin at PIN ");
    //LOG0(activationPin);
    //LOG0("\n");
    digitalWrite(activationPin,HIGH);                   // turn pin off for relay
    
    //LOG0("- Configuring warnPin at PIN ");
    //LOG0(warnPin);
    //LOG0("\n");
    digitalWrite(warnPin,HIGH);                   // turn pin for warnLed on
    
    // turn off after check
    delay(250);                                     // wait 250 ms
    digitalWrite(warnPin,LOW);                      // turn pin off
    
    Serial.print("Garage Door ready \n");           // initialization message of the Service
  } // end constructor




  boolean update(){                              // update() method

    LOG1("*** Action requested by changing Target: ");
    // Si la puerta está obstruida, y recibe una orden, debería de obviarse la orden.
    /*
    LOG1(current->getVal());
    LOG1(",");
    LOG1(target->getNewVal());
    LOG1("\n");
    LOG1("Obstruction ");
    LOG1(obstruction->getVal());
    LOG1("\n");
    */

    if(target->getNewVal()==OPEN){                  // if the target-state value is set to 0, HomeKit is requesting the door to be in open position
      // But if the door was already opening and we get a new signal, door must be stopped, not closed (to follow the logic of the Deimos board)
      //With the addition of the Hall sensor, another logic can be built:
      // HallSensor is CLOSED if and only if door OPEN or DOOR CLOSED
      if(current->getVal()==CLOSING){
        LOG1("Garage Door was closing and requested opening. Stop first\n");
        current->setVal(STOPPED);                   // Set currentState to Stopped
        // should trigger a STOP signal
        triggerDoor(activationPin);
        return(true); // ¿should be false?
      }
      LOG1("Opening Garage Door\n");                
      current->setVal(OPENING);                     // set the current-state value to 2, which means "opening"      
      obstruction->setVal(false);                   // clear any prior obstruction detection

    } else {      // TARGET = CLOSE

      if(obstruction->getVal()==true){              // Testing if there is an obstruction. If there is, return to exit the loop, otherwise keep closing
        LOG1("Garage Door obstruction. Can't close\n");
        target->setVal(OPEN);
        return(true); // ¿should be false?
      }
      if(current->getVal()==OPENING){
        LOG1("Garage Door was opening and requested closing. Stop first\n");
        current->setVal(STOPPED);                   // Set currentState to Stopped
        return(true); // ¿should be false?
      }
      LOG1("Closing Garage Door\n");                // else the target-state value is set to 1, and HomeKit is requesting the door to be in the closed position
      current->setVal(CLOSING);                     // set the current-state value to 3, which means "closing" 
      obstruction->setVal(false);                   // clear any prior obstruction detection                            
             
    }
    /*
    LOG1("Trigger the relay");             // TRIGGER TO ACTIVATE THE DOOR
    digitalWrite(activationPin,LOW);               // turn pin off
    delay(250);                                    // wait 250 ms
    digitalWrite(activationPin,HIGH);              // turn pin on
    LOG1("Door Activated \n");
    */
    triggerDoor(activationPin);
    return(true);                                  // return true
  
  } // update


  // Once you read a sensor's values in a loop() method, you need to communicate this back to HomeKit so the new values can be 
  // reflected in the HomeKit Controller. This may be strictly for information purposes (such as a temperature sensor) 
  // or could be used by HomeKit itself to trigger other devices (as might occur implementing a Door Sensor). 
  
  // In order to communicate with HomeKit to update the value os a Characteristic, we use the method called setVal(). 
  // Do not use setVal() unless the value of the Characteristic changes, but do use it to immediately inform HomeKit of something time-sensitive, such as a door opening.

  void loop(){                                      // loop() method
  
     if(obstruction->timeVal()>1000){               // check time elapsed since last update and proceed only if greater than 0,5 seconds
        // LOG1("Read sensor states \n");
        // Read the Reed Sensor (Walking Door)      // The logic of this sensor is reversed!!!
        reedSensorState = digitalRead(reedSensorPin);
        // Read The Photovoltaic Sensor
        photoSensorState = digitalRead(photoSensorPin);

        if(photoSensorState == 1 || reedSensorState == 1){
          obstruction->setVal(true);                   // set obstruction-detected to true
          // LOG1("Turn on the WARNING LED \n");
          // digitalWrite(warnPin,HIGH);                   // turn pin on   
        } else {
          obstruction->setVal(false);                  // set obstruction-detected to false
          // LOG1("Turn off the WARNING LED \n");
          // digitalWrite(warnPin,LOW);                   // turn pin off   
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

    if(current->getVal()!=target->getVal() && target->timeVal()>OPERATIONTIME){ 
      // Además de establecer un tiempo de cierre por seguridad, habría que leer el Sensor Portón para confirmar el cierre de la puerta. 
      // Si el tiempo desde la activación hasta recibir la señal de cierre excede X sg, avisar que puerta bloqueada o parada (o stopped) 
      
      LOG1("Door Inactive \n");
      current->setVal(target->getVal());           // set the current-state to the target-state
      if (target->getVal()==0){
        LOG1("Puerta Abierta \n");
        obstruction->setVal(false);
      } else {
        LOG1("Puerta Cerrada \n");
        obstruction->setVal(false);
      }
    }
  } // loop

  void triggerDoor(int triggerPin){
    //this->activationPin=activationPin;
    LOG1("Trigger the relay: ");             // TRIGGER TO ACTIVATE THE DOOR
    digitalWrite(triggerPin,LOW);               // turn pin off
    delay(250);                                 // wait 250 ms
    digitalWrite(triggerPin,HIGH);              // turn pin on
    LOG1("Door Activated \n");
  }
  
};


////////////////////////////////////
