/*********************************************************************************
 *  MIT License
 *  
 *  Copyright (c) 2020 Gregg E. Berman
 *  
 *  https://github.com/HomeSpan/HomeSpan
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *  
 ********************************************************************************/
 
#include "HomeSpan.h" 
#include "DEV_Identify.h"  
#include "DEV_GarageDoor.h"     
#include "DEV_Sensors.h"
#include "DEV_LockMechanism.h"
#include "DEV_ToggleSwitch.h"

// For manually setting up HomeSpan config
  
int controlPin = 32;
int statusPin = 19;  

// Sensors Inputs
int photoSensorPin = 21;
int reedSensorPin = 23;
int hallSensorPin = 0;
 
// Controlers Outputs
int lockPin = 12; 
int activateDoorPin = 14;
int intercomOpener = 27;
int warnPin = 33;  


void setup() {


  // FIRST: set all Outputs to LOW to make sure there are no unexpected behaviour on rearming

   digitalWrite(activateDoorPin,LOW);
   digitalWrite(lockPin,LOW);
   digitalWrite(intercomOpener,LOW);


   homeSpan.setControlPin(controlPin);
   homeSpan.setStatusPin(statusPin);
   homeSpan.setStatusAutoOff(1);      //turns off status pin after 1 secs of power up, will remain active for control signals
   homeSpan.setHostNameSuffix("");
   homeSpan.enableOTA();   // Enables de OTA updating service

  Serial.begin(115200);
  

  homeSpan.begin(Category::Bridges,"Sakura HomeSpan", "SakuraGarage");  

    Serial.print("Configuring The Bridge \n");                               
    new SpanAccessory();
     new DEV_Identify("Portero","SakuraHouse","20201019","SakuraBridge by HomeSpan","2244.7.3",0); // This calls the function in the DEV_Identify.h file that allows the Accessory identification in HomeKit
     // *NOTE* This is the inizialization of the Bridge         

     Serial.print("Configuring Garage Door Service \n");           // initialization message of the Service
     new SpanAccessory();       //new Service::GarageDoorOpener();
      new DEV_Identify("Portón Garaje","BFT","BT-A-400","Demos Merkal","2244.6.2",0); // This calls the function in the DEV_Identify.h file that allows the Accessory identification in HomeKit
      new DEV_GarageDoor(activateDoorPin, photoSensorPin, hallSensorPin, reedSensorPin, warnPin);    
      //new SpanButton(intercomOpener);    

    Serial.print("Configuring Contact Sensor \n");           // initialization message of the Service
    new SpanAccessory();        //  new Service::ContactSensor();
     new DEV_Identify("Fotocélula Portón","BFT","123-ABC","Photovoltaic Sensor","1.0",0);
     new DEV_ContactSensor(photoSensorPin, 1);      
    
    Serial.print("Configuring Contact Sensor \n");           // initialization message of the Service
    new SpanAccessory();        //  new Service::ContactSensor();
     new DEV_Identify("Limites Portón","SakuraHouse","123-ABC","Hall Sensor","0.2",0);
     new DEV_ContactSensor(hallSensorPin,0);    
    
    Serial.print("Configuring Contact Sensor \n");           // initialization message of the Service  
    new SpanAccessory();        //  new Service::ContactSensor();
     new DEV_Identify("Puerta Acceso","SakuraHouse","123-ABC","Reed Sensor","0.2",0);
     new DEV_ContactSensor(reedSensorPin,0);                             

     
    Serial.print("Configuring LockMechanism \n");           // initialization message of the Service
     new SpanAccessory();        //  new Service::LockMechanism();
     new DEV_Identify("Bloqueo Puerta Garaje","SakuraHouse","123-ABC","Lock","0.1",0);
     new DEV_LockMechanism(lockPin, warnPin);                             
    
    Serial.print("Configuring Door Toggle \n");           // initialization message of the Service
     new SpanAccessory();        //  new Service::Switch();
     new DEV_Identify("Portero Automatico","SakuraHouse","11871-DSH","Door Swwitch","0.1",0);
     new DEV_ToggleSwitch(intercomOpener);     
                                       
     
} // end of setup()

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
  
} // end of loop()