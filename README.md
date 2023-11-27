# PCF8563 RTC Arduino Library

Arduino library for the P1AM-200 PCF8563 RTC chip.

Use the Arduino Library Manager to install.

## Example
Here is a simple example which shows the capabilities of the library
```cpp
#include <PCF8563.h>

void setup(){
  Serial.begin(9600);
  while(!Serial){
    ;
  }
  Serial.println(PCF8563_RTC.getEpoch());	//Print RTC's epoch time
}

void loop(){
}
```
