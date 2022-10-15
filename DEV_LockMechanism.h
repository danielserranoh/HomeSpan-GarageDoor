////////////////////////////////////////
//   DEVICE-SPECIFIC LOCK SERVICES   //
//////////////////////////////////////
    // Lock Current State options: 
    // 0 = Unsecured
    // 1 = Secured
    // 2 = Jammed
    // 3 = Unknown

    // Lock TARGET State options:
    // 0 = Unsecured
    // 1 = Secured

struct DEV_LockMechanism : Service::LockMechanism {     // A standalone Reed Sensor

  int lockPin;
  int warnPin;
  
  SpanCharacteristic *current;            // reference to the Current Door State Characteristic (specific to Garage Door Openers)
  SpanCharacteristic *target;             // reference to the Target Door State Characteristic (specific to Garage Door Openers)  
  
  
  DEV_LockMechanism(int lockPin, int warnPin) : Service::LockMechanism(){       // constructor() method
    
    // First we instantiate the main Characteristic for a Conctact Sensor, namely the Current State. To set its initial value
    // we would take a reading and initialize it to that value. 
    
    current=new Characteristic::LockCurrentState(0);             // instantiate the LockCurrentSate Characteristic and save it as state, initial value of 1 means secured
    target=new Characteristic::LockTargetState(0);   
    
    this->lockPin=lockPin;                  // save the pin number for the hypothetical relay
    pinMode(lockPin, OUTPUT);             // configure the pin as an input using the standard Arduino pinMode function
    this->warnPin=warnPin;                  // save the pin number for the warning led
        

    
    Serial.print("Configuring lockPin: Pin=");               // initialization message of the Pin Setup
    Serial.print(lockPin);
    Serial.print("\n");

  } // end constructor


  boolean update(){                              // update() method

    // see HAP Documentation for details on what each value represents
    LOG1("*** The requested Target is ");
    LOG1(target->getNewVal());
    LOG1("\n");
    LOG1("*** The current state is ");
    LOG1(current->getVal());
    LOG1("\n");

    if(target->getNewVal()==0){                     // if the target-state value is set to 0, HomeKit is requesting the lock to be in open position
      LOG1("Opening Lock\n");
      current->setVal(1);                           // set the current-state value to 1, which means "open"
      digitalWrite(lockPin,LOW);                    // turn pin off
    } else {
      LOG1("Closing Lock\n");                       // else the target-state value is set to 0, and HomeKit is requesting the lock to be in the closed position
      current->setVal(0);                           // set the current-state value to 0, which means "closed" 
      digitalWrite(lockPin,HIGH);                   // turn pin on     
    }
    
    return(true);                               // return true
  
  } // update

  void loop(){                                     // loop() method

    if(current->getVal()==target->getVal())        // if current-state matches target-state there is nothing do -- exit loop()
      return;

    if(current->getVal()==2){    
      //current->setVal(4);                             // if our simulated obstruction is triggered, set the curent-state to 4, which means "stopped"
      digitalWrite(warnPin,HIGH);                     // turn pin on
      LOG1("Lock Jammed!\n");
    }
    if(current->getVal()!=target->getVal()){
      current->setVal(target->getVal());           // set the current-state to the target-state
    }

  } // loop
  
};

