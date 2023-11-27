/*
  Example: CheckLowVoltageBit

  This example checks the voltage low bit of the RTC.
  The voltage low bit in the RTC is set when the RTC voltage falls below the low voltage threshold
  (0.9V).
  The current Unix time stamp is measured as seconds since January 1st, 1970 and can be
  found here. https://www.unixtimestamp.com/index.php

  This example will check the low voltage bit of the PCF8563 RTC. In the case that the low voltage 
  bit is set the time is invalid. Otherwise the saved time will be displayed.  

  Written by FACTS Engineering
  Copyright (c) 2023 FACTS Engineering, LLC
  Licensed under the MIT license.
*/

#include <PCF8563.h>


void setup(){ // the setup routine runs once:
    
  Serial.begin(9600);   //start serial monitor connection for display
  while(!Serial){   //wait for connection to serial monitor
    ;
  }

  bool checkVolt = PCF8563_RTC.lowVolt();
  Serial.print("Is saved time valid? ");
  if(PCF8563_RTC.lowVolt()){
    Serial.println("No. ");
  }
  else{
    Serial.print("Yes: ");
    Serial.println(PCF8563_RTC.getEpoch());   //get saved time
  }
  delay(1000);    //delay 1 second
}


void loop(){  // the loop routine runs over and over again forever
}
