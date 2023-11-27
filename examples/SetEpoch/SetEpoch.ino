/*
  Example: SetEpochUnix

  This example shows how to set the RTC to a given date and time using char arrays.
  The current Unix time stamp is measured as seconds since January 1st, 1970 and can be
  found here. https://www.unixtimestamp.com/index.php

  This example will set the time to 8:00am Janurary 1st, 2021 and read the date and time from 
  the RTC every 5 seconds. This Unix time stamp is 1609488000.

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
  
  PCF8563_RTC.setEpoch("Jan 01 2021 Fri", "08:00:00");   //set time to 8:00am Janurary 1st, 2021
  delay(1000);    //delay 1 second
}

unsigned int current_time = 0;

void loop(){  // the loop routine runs over and over again forever:
  current_time = PCF8563_RTC.getEpoch();  //get current time from PCF8563 RTC
  Serial.println(current_time);       //print time
  Serial.println();

  delay(5000);   //delay 5 seconds
}
