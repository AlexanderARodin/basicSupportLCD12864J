#ifndef RAALCD12864J_H
#define RAALCD12864J_H

#include <Arduino.h>
//	############################################################
//	############################################################
//	############################################################
//	############################################################

class BaseLCD12864J{
	struct {
		union{
			struct{
				byte DB0; byte DB1; byte DB2; byte DB3;
				byte DB4; byte DB5; byte DB6; byte DB7;
			}_;
			byte DB[8];
		};
		byte RW; byte A0; byte E; byte CS1; byte CS2;
	}pins;
	
	static void synchroDelay();
	
	void setAllPinsOutput();
	
	void sendDB(byte _DB);
	void setE(bool flag);
	void setRW(bool flag);
	void setA0(bool flag);
	void selectCS(byte chipNumber);
	
	void invokeCommand(byte chipNumber, unsigned int command);
	
	
protected:
	
public:
	//BaseLCD12864J(){};
	BaseLCD12864J(  byte _pinCS1, byte _pinCS2,
					  byte _pinE,   byte _pinA0,  byte _pinRW,
					  byte _pinDB7, byte _pinDB6, byte _pinDB5, byte _pinDB4,
					  byte _pinDB3, byte _pinDB2, byte _pinDB1, byte _pinDB0 );
	
	void setDisplayOnOff(byte chipNumber, bool flag);
	void setStartLine(byte chipNumber, byte startLine);
	void setPage(byte chipNumber, byte page);
	void setAddress(byte chipNumber, byte address);
	//byte getStatus(byte chipNumber, byte address);
	void writeData(byte chipNumber, byte _data);
};

#define		CLEAR_PIXEL		1
#define		SET_PIXEL		2
#define		INVERT_PIXEL	3

#define VIDEO_BUFFER_SIZE 1024
class BufferLCD{
	BaseLCD12864J *lcd;
	byte video[VIDEO_BUFFER_SIZE];
	struct IndexAndBits{
		int index;
		byte bitNum;
	};
	
protected:
	IndexAndBits extractAddressFrom(int X, int Y);
	
public:
	BufferLCD();
	~BufferLCD();
	
	void cleanBuffer();
	void initLCD(byte _pinCS1, byte _pinCS2,
					 byte _pinE,   byte _pinA0,  byte _pinRW,
					 byte _pinDB7, byte _pinDB6, byte _pinDB5, byte _pinDB4,
					 byte _pinDB3, byte _pinDB2, byte _pinDB1, byte _pinDB0 );
	
	void sendToLCD();
	bool getPixel(int X, int Y);
	void setPixel(byte mode, int X, int Y);
	void setLine(byte mode, int X1, int Y1, int X2, int Y2);
	void setRect(byte mode, int X1, int Y1, int X2, int Y2);
	void setEllipse(byte mode, int X1, int Y1, int X2, int Y2);
	
};


//	############################################################
//	############################################################
//	############################################################
//	############################################################
#endif
