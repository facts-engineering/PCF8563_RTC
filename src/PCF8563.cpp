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

#include <PCF8563.h>

PCF8563Class::PCF8563Class(TwoWire &wirePort){
	rtc_port = &wirePort;
}
PCF8563Class PCF8563_RTC;

	
	
/*******************************************************************************
Description: Sets the RTC to the provided Unix time stamp

Parameters: -unix time stamp

Returns: 	-none
*******************************************************************************/

	void PCF8563Class::setEpoch(time_t epoch)
	{
		struct tm * ptm;	//storage UTC time
		uint8_t century = 0;
		uint8_t temp = 0;
		uint8_t month = 0;
		
		if(initialized == false){
		  initialize();
	    }
		
		ptm = gmtime(&epoch);
		
		if(ptm->tm_year < 70 || ptm->tm_year > 199){
			Serial.println("Invalid start year. ");
			return;
		}
		
		if(ptm->tm_year >= 100){
			temp = ptm->tm_year % 100;
			century = 1;
		}
		else{
			temp = ptm->tm_year % 100;
			century = 0;
		}
		
		month = dec2bcd(ptm->tm_mon, century);
		
		stopClock();
		rtc_port->beginTransmission(PCF8563_ADDR_W);
		rtc_port->write(0x02);  // Second Register
		rtc_port->write(dec2bcd(ptm->tm_sec) & 0x7F);
		rtc_port->write(dec2bcd(ptm->tm_min) & 0x7F);
		rtc_port->write(dec2bcd(ptm->tm_hour) & 0x3F);
		rtc_port->write(dec2bcd(ptm->tm_mday) & 0x3F);
		rtc_port->write(dec2bcd(ptm->tm_wday) & 0x03);
		rtc_port->write(month);
		rtc_port->write(dec2bcd(temp));
		rtc_port->endTransmission();
		startClock();
	}
	
/*******************************************************************************
Description: Sets the RTC to the provided english time and date string provided

Parameters: -Date and time in the following format
			 Example date and time: "Jan 13 2021 Wed", "07:27:45"

Returns: 	-none
*******************************************************************************/
	void PCF8563Class::setEpoch(char* date, char* time)
	{
		uint8_t day, month, hour, minute, second, weekday, century;
		uint16_t year;
		
		if(initialized == false){
		  initialize();
	    }
		
		switch (date[0]) {
			case 'J': 
				if(date[1] == 'a'){
					month = 0;	//set month to Jan
					break;
				}
				else{
					if(date[2] == 'n'){
						month = 5;	//set month to June
						break;
					}
					else{
						month = 6;	//set month to July
						break;
					}
				}
			case 'F': 
				month = 1;
				break;
			case 'A': 
				if(date[2] == 'r'){
					month = 3;
					break;
				}
				else {
					month = 7;
					break;
				}
			case 'M': 
				if(date[2] == 'r'){
					month = 2;
					break;
				}
				else{
					month = 4;
					break;
				}
			case 'S': 
				month = 8;
				break;
			case 'O':
				month = 9;
				break;
			case 'N': 
				month = 10;
				break;
			case 'D': 
				month = 11;
				break;
		}
		
		switch (date[12]) {
			case 'M':
				weekday = 1;
				break;
			case 'T':
				if(date[13] == 'u'){
					weekday = 2;
					break;
				}
				else{
					weekday = 4;
					break;
				}
			case 'W':
				weekday = 3;
				break;
			case 'F':
				weekday = 5;
				break;
			case 'S':
				if(date[13] == 'a'){
					weekday = 6;
					break;
				}
				else{
					weekday = 0;
					break;
				}
		}
		
		second = atoi(time + 6);
		minute = atoi(time + 3);
		hour = atoi(time);
		day = atoi(date + 4);
		year = atoi(date + 7);
		if(year < 1970 || year > 2099){
			Serial.println("Invalid start year. ");
			return;
		}
		
		if(year >= 2000){
			year = year % 100;
			century = 1;
		}
		else{
			year = year % 100;
			century = 0;
		}
		
		stopClock();
		rtc_port->beginTransmission(PCF8563_ADDR_W);
		rtc_port->write(0x02);  // Second Register
		rtc_port->write(dec2bcd(second) & 0x7F);
		rtc_port->write(dec2bcd(minute) & 0x7F);
		rtc_port->write(dec2bcd(hour) & 0x3F);
		rtc_port->write(dec2bcd(day) & 0x3F);
		rtc_port->write(dec2bcd(weekday) & 0x03);
		rtc_port->write(dec2bcd(month, century));
		rtc_port->write(dec2bcd(year));
		rtc_port->endTransmission();
		startClock();
	}

/*******************************************************************************
Description: Reads epoch from RTC and formats it into Unix time stamp

Parameters: -None

Returns: 	-Current Unix time stamp
*******************************************************************************/
	time_t PCF8563Class::getEpoch()
	{
		uint8_t second;
		uint8_t minute;
		uint8_t hour;
		uint8_t day;
		uint8_t week;
		uint8_t month;
		uint16_t year;
		uint8_t century;
		
		if(initialized == false){
		  initialize();
	    }
		
		rtc_port->beginTransmission(PCF8563_ADDR_W);
		rtc_port->write(0x02);
		rtc_port->endTransmission();
		rtc_port->requestFrom(PCF8563_ADDR_R, 7);
		second = bcd2dec(rtc_port->read() & 0x7F);
		minute = bcd2dec(rtc_port->read() & 0x7F);
		hour = bcd2dec(rtc_port->read() & 0x3F);
		day = bcd2dec(rtc_port->read() & 0x3F);
		week = bcd2dec(rtc_port->read() & 0x07);
		month = rtc_port->read() & 0x9F;
		year = bcd2dec(rtc_port->read() & 0xFF);
		rtc_port->endTransmission();
		
		century = (month & 0x80) >> 7;
		month = bcd2dec(month & 0x1F);
		
		time_t epoch;
		struct tm epoch_tm;

		epoch_tm.tm_sec = second;
		epoch_tm.tm_min = minute;
		epoch_tm.tm_hour = hour;
		epoch_tm.tm_wday = week;
		epoch_tm.tm_mday = day;
		epoch_tm.tm_mon = month - 1;
		epoch_tm.tm_year = year + (100 * (century));

		epoch = mktime (&epoch_tm);

		return (epoch);
	}
	
/*******************************************************************************
Description: Clears the RTC stop bit so the RTC will operate.

Parameters: -None

Returns: 	-None
*******************************************************************************/
	
	void PCF8563Class::startClock(void)
	{
		uint8_t data;
		
		if(initialized == false){
		  initialize();
	    }

		rtc_port->beginTransmission(PCF8563_ADDR_W);
		rtc_port->write(0x00);
		rtc_port->endTransmission();

		rtc_port->requestFrom(PCF8563_ADDR_R, 1);
		data = rtc_port->read();

		bitClear(data, 5);
		
		rtc_port->beginTransmission(PCF8563_ADDR_W);
		rtc_port->write(0x00);
		rtc_port->write(data);
		rtc_port->endTransmission();
	}
	
/*******************************************************************************
Description: Sets the RTC stop bit so the RTC will not count.

Parameters: -None

Returns: 	-None
*******************************************************************************/

	void PCF8563Class::stopClock(void)
	{
	  uint8_t data;
	  
	  if(initialized == false){
		initialize();
	  }

	  rtc_port->beginTransmission(PCF8563_ADDR_W);
	  rtc_port->write(0x00);
	  rtc_port->endTransmission();

	  rtc_port->requestFrom(PCF8563_ADDR_R, 1);
	  data = rtc_port->read();
	  
	  bitSet(data, 5);

	  rtc_port->beginTransmission(PCF8563_ADDR_W);
	  rtc_port->write(0x00);
	  rtc_port->write(data);
	  rtc_port->endTransmission();
	}
	
/*******************************************************************************
Description: Checks low voltage bit on PCF8563. Indicates if power has fallen 
			 below the threshold and time is no longer valid.

Parameters: -None

Returns: 	-Boolean True if low voltage is detected.
			 Boolean False if voltage is fine.
*******************************************************************************/
	
	bool PCF8563Class::lowVolt(void)
	{
	  uint8_t data;
	  
	  if(initialized == false){
		initialize();
	  }

	  rtc_port->beginTransmission(PCF8563_ADDR_W);
	  rtc_port->write(0x02);
	  rtc_port->endTransmission();

	  rtc_port->requestFrom(PCF8563_ADDR_R, 1);
	  data = rtc_port->read();
	  
	  if(bitRead(data, 7) == 1)
	  {
		return true;
	  }
	  return false;
	}
	
/*******************************************************************************
Description: Private functions to convert from BCD to Binary and vice versa.
			 Initialize starts i2c communication for the PCF8563 RTC. 
*******************************************************************************/
	
	void PCF8563Class::initialize (void)
	{
		INTERNAL_EEPROM_PORT.begin();
		initialized = true;
	}
	
	uint8_t PCF8563Class::dec2bcd(uint8_t inVal){
		uint8_t bcdVal = 0;
		uint8_t loByte = 0;
		uint8_t hiByte = 0;
		
		loByte = inVal % 10;
		hiByte = inVal / 10;
		
		bcdVal = (hiByte << 4) | loByte;
		return bcdVal;
	}
	
	uint8_t PCF8563Class::dec2bcd(uint8_t inVal, uint8_t century){
		uint8_t bcdVal = 0;
		uint8_t loByte = 0;
		uint8_t hiByte = 0;
		
		loByte = (inVal % 10) + 1;
		hiByte = inVal / 10;
		
		bcdVal = (hiByte << 4) | loByte;
		
		bcdVal = bcdVal | (century << 7);
		bcdVal = bcdVal & 0x9F;
		
		return bcdVal;
	}
	
	uint8_t PCF8563Class::bcd2dec(uint8_t inVal){
		uint8_t decVal = 0;
		uint8_t loByte = 0;
		uint8_t hiByte = 0;
		
		loByte = inVal & 0x0F;
		hiByte = (inVal & 0xF0) >> 4;
		
		decVal = ((hiByte * 10) + loByte);
		
		return decVal;
	}
