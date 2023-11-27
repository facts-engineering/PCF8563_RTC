/*
MIT License

Copyright (c) 2023 FACTS Engineering, LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef PCF8563_H
#define PCF8563_H

#include "Arduino.h"

	#include <time.h>
	#include <Arduino.h>
	#include <Wire.h>

	#define PCF8563_ADDR_R 0x51
	#define PCF8563_ADDR_W  0x51

	#ifndef INTERNAL_RTC_PORT
		#define INTERNAL_RTC_PORT Wire	//Assume using standard I2C port and external device
	#endif	
	
	class PCF8563Class
	{
		public:
			PCF8563Class(TwoWire &wirePort = INTERNAL_RTC_PORT);
			
			TwoWire *rtc_port;

			/* Epoch Functions */
			time_t getEpoch();
			void setEpoch(time_t epoch);
			void setEpoch(char* date, char* time);
 
			void startClock();
			void stopClock();
			
			bool lowVolt();
			
		private:
			bool initialized = false;
			void initialize();
			
			uint8_t dec2bcd(uint8_t inVal);
			uint8_t dec2bcd(uint8_t inVal, uint8_t century);
			uint8_t bcd2dec(uint8_t inVal);
	};

extern PCF8563Class PCF8563_RTC;	
#endif
