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

int statusPin = 19;    
int activateDoorPin = 17;
int photoSensorPin = 13;
int reedSensorPin = 23;
int closedSensorPin = 0;
int warnPin = 33;    
int lockPin = 22; 




void setup() {

   //homeSpan.setControlPin(controlPin)
   homeSpan.setStatusPin(statusPin);
   homeSpan.setStatusAutoOff(5);      //turns off status pin after 5 secs of power up, will remain active for control signals
   homeSpan.setHostNameSuffix("");
   homeSpan.enableOTA();   // Enables de OTA updating service

  Serial.begin(115200);
  

  homeSpan.begin(Category::Bridges,"HomeSpan Bridge", "SakuraGarage");  

    Serial.print("Configuring The Bridge \n");                               
    new SpanAccessory();
     new DEV_Identify("Portero","SakuraHouse","20141114","SakuraBridge by HomeSpan","1.6.0",0); // This calls the function in the DEV_Identify.h file that allows the Accessory identification in HomeKit
     // *NOTE* This is the inizialization of the Bridge         

     Serial.print("Configuring Garage Door Service \n");           // initialization message of the Service
     new SpanAccessory();       //new Service::GarageDoorOpener();
      new DEV_Identify("Puerta Garaje","BFT","BT-A-400","Demos Merkal","0.9",0); // This calls the function in the DEV_Identify.h file that allows the Accessory identification in HomeKit
      new DEV_GarageDoor(activateDoorPin, warnPin);        

    Serial.print("Configuring Contact Sensor \n");           // initialization message of the Service
    new SpanAccessory();        //  new Service::ContactSensor();
     new DEV_Identify("Fotovoltaico Portón","BFT","123-ABC","Photovoltaic Sensor","1.0",0);
     new DEV_ContactSensor(photoSensorPin);      

    Serial.print("Configuring Contact Sensor \n");           // initialization message of the Service
    new SpanAccessory();        //  new Service::ContactSensor();
     new DEV_Identify("Cierre Portón","SakuraHouse","123-ABC","Reed Sensor","0.2",0);
     new DEV_ContactSensor(closedSensorPin);    
    
    Serial.print("Configuring Contact Sensor \n");           // initialization message of the Service  
    new SpanAccessory();        //  new Service::ContactSensor();
     new DEV_Identify("Cierre Puerta","SakuraHouse","123-ABC","Reed Sensor","0.2",0);
     new DEV_ContactSensor(reedSensorPin);                             

     
    Serial.print("Configuring LockMechanism \n");           // initialization message of the Service
     new SpanAccessory();        //  new Service::LockMechanism();
     new DEV_Identify("Bloqueo Puerta Garaje","SakuraHouse","123-ABC","Lock","0.1",0);
     new DEV_LockMechanism(lockPin, warnPin);                             
                                       
     
} // end of setup()

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
  
} // end of loop()