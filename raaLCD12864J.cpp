
#include <raaLCD12864J.h>
//	############################################################
//	############################################################
//	############################################################
//	############################################################




void BaseLCD12864J::synchroDelay(){delayMicroseconds(8);}
	
void BaseLCD12864J::setAllPinsOutput(){
	for(int i=0;i<8;i++){
		if(pins.DB[i])pinMode( pins.DB[i], OUTPUT );
	}
	if(pins.RW )pinMode( pins.RW , OUTPUT );
	if(pins.A0 )pinMode( pins.A0 , OUTPUT );
	if(pins.E  )pinMode( pins.E  , OUTPUT );
	if(pins.CS1)pinMode( pins.CS1, OUTPUT );
	if(pins.CS2)pinMode( pins.CS2, OUTPUT );
}
	
void BaseLCD12864J::sendDB(byte _DB){
	for(int i=0;i<8;i++){
		if(pins.DB[i]){
			digitalWrite(   pins.DB[i], (1<<i) & _DB );
		}
	}
}
void BaseLCD12864J::setE(bool flag){
	if(pins.E ){
		synchroDelay();
		digitalWrite(   pins.E , flag );
		synchroDelay();
	}
}
void BaseLCD12864J::setRW(bool flag){
	if(pins.RW){
		digitalWrite(   pins.RW, flag );
	}
}
void BaseLCD12864J::setA0(bool flag){
	if(pins.A0){
		digitalWrite(   pins.A0, flag );
	}
}
void BaseLCD12864J::selectCS(byte chipNumber){
	if(chipNumber==1 && pins.CS1){
		digitalWrite(   pins.CS2, false );
		digitalWrite(   pins.CS1, true );
	}
	if(chipNumber==2 && pins.CS2){
		digitalWrite(   pins.CS1, false );
		digitalWrite(   pins.CS2, true );
	}
}

void BaseLCD12864J::invokeCommand(byte chipNumber, unsigned int command){
	setAllPinsOutput();
	setE(false);
	//			7654321076543210
	//			----12ARdddddddd
	setRW( 0b0000000100000000 & command);
	setA0( 0b0000001000000000 & command);
	selectCS(chipNumber);
	setE(true);
	sendDB(command);
	setE(false);
}


BaseLCD12864J::BaseLCD12864J(  byte _pinCS1, byte _pinCS2,
				  byte _pinE,   byte _pinA0,  byte _pinRW,
				  byte _pinDB7, byte _pinDB6, byte _pinDB5, byte _pinDB4,
				  byte _pinDB3, byte _pinDB2, byte _pinDB1, byte _pinDB0 ){
	pins.CS1=_pinCS1; pins.CS2=_pinCS2;
	pins.E=_pinE;     pins.A0=_pinA0;   pins.RW=_pinRW;
	pins._.DB7=_pinDB7; pins._.DB6=_pinDB6; pins._.DB5=_pinDB5; pins._.DB4=_pinDB4;
	pins._.DB3=_pinDB3; pins._.DB2=_pinDB2; pins._.DB1=_pinDB1; pins._.DB0=_pinDB0;
	setAllPinsOutput();
}

void BaseLCD12864J::setDisplayOnOff(byte chipNumber, bool flag){
	//										7654321076543210
	//										----12ARdddddddd
	invokeCommand( chipNumber,  0b0000000000111110 | (flag?0b1:0b0) );
}
void BaseLCD12864J::setStartLine(byte chipNumber, byte startLine){
	//										7654321076543210
	//										----12ARdddddddd
	invokeCommand( chipNumber,  0b0000000011000000 | (startLine & 0b111111) );
}
void BaseLCD12864J::setPage(byte chipNumber, byte page){
	//										7654321076543210
	//										----12ARdddddddd
	invokeCommand( chipNumber,  0b0000000010111000 | (page & 0b111) );
}
void BaseLCD12864J::setAddress(byte chipNumber, byte address){
	//										7654321076543210
	//										----12ARdddddddd
	invokeCommand( chipNumber,  0b0000000001000000 | (address & 0b111111) );
}
//byte getStatus(byte chipNumber, byte address){
	//										7654321076543210
	//										----12ARdddddddd
//	return invokeCommand(chipNumber,  0b0000000100000000 | (address & 0b111111) );
//}
void BaseLCD12864J::writeData(byte chipNumber, byte _data){
	//										7654321076543210
	//										----12ARdddddddd
	invokeCommand(chipNumber,   0b0000001000000000 | _data );
}

BufferLCD::IndexAndBits BufferLCD::extractAddressFrom(int X, int Y){
	Y=63-Y;
	if(X<0 || X>=128 || Y<0 || Y>=64)return {-1,0};
	byte cs = X<64?1:2;
	byte addr=X & 0b111111;
	byte page=Y>>3;
	
	return { addr + page*64 + (cs-1)*512, (byte)(Y & 0b111) };
}

BufferLCD::BufferLCD(){
	cleanBuffer();
}
BufferLCD::~BufferLCD(){
	delete lcd;
}

void BufferLCD::cleanBuffer(){
	for(int i=0;i<VIDEO_BUFFER_SIZE;i++){
		video[i]=0;
	}
}

void BufferLCD::initLCD(byte _pinCS1, byte _pinCS2,
				 byte _pinE,   byte _pinA0,  byte _pinRW,
				 byte _pinDB7, byte _pinDB6, byte _pinDB5, byte _pinDB4,
				 byte _pinDB3, byte _pinDB2, byte _pinDB1, byte _pinDB0 ){
	delete lcd;
	lcd=new BaseLCD12864J(_pinCS1, _pinCS2,
								 _pinE,   _pinA0,  _pinRW,
								 _pinDB7, _pinDB6, _pinDB5, _pinDB4,
								 _pinDB3, _pinDB2, _pinDB1, _pinDB0);
	lcd->setStartLine(1,0);
	lcd->setStartLine(2,0);
	sendToLCD();
	lcd->setDisplayOnOff(1,true);
	lcd->setDisplayOnOff(2,true);
}

void BufferLCD::sendToLCD(){
	int count = 0;
	lcd->setAddress(1, 0 );
	lcd->setAddress(2, 0 );
	for(int page=0;page<8;page++){
		lcd->setPage(1, page );
		lcd->setPage(2, page );
		for(int addr=0;addr<64;addr++){
			lcd->writeData(1, video[count] );
			lcd->writeData(2, video[count+VIDEO_BUFFER_SIZE/2] );
			count++;
		}
	}
}

bool BufferLCD::getPixel(int X, int Y){
	BufferLCD::IndexAndBits ind;
	ind = extractAddressFrom( X, Y );
	if(ind.index<0 || ind.index >=VIDEO_BUFFER_SIZE )return false;
	return video[ind.index] & 1<<ind.bitNum;
}

void BufferLCD::setPixel(byte mode, int X, int Y){
	BufferLCD::IndexAndBits ind;
	ind = extractAddressFrom( X, Y );
	if(ind.index<0 || ind.index >=VIDEO_BUFFER_SIZE )return;
	byte old = video[ind.index];
	byte mask = 1<<ind.bitNum;
	switch(mode){
		case CLEAR_PIXEL:
			old = old & ~mask;
			break;
		case SET_PIXEL:
			old = old | mask;
			break;
		case INVERT_PIXEL:
			old = old ^ mask;
			break;
		default:
			;
	}
	video[ind.index]=old;
}

void BufferLCD::setLine(byte mode, int X1, int Y1, int X2, int Y2){
	int dX = X2-X1;
	int dY = Y2-Y1;
	int dL = dX;
	if(abs(dY)>abs(dX))dL=dY;
	int absDL = abs(dL);
	int X; int Y;
	for(int i=0;i<=absDL;i++){
		X = X1 + dX*i/absDL;
		Y = Y1 + dY*i/absDL;
		setPixel( mode, X, Y );
	}
}

void BufferLCD::setRect(byte mode, int X1, int Y1, int X2, int Y2){
	setLine(mode, X1, Y1, X1, Y2);
	setLine(mode, X2, Y2, X1, Y2);
	setLine(mode, X2, Y2, X2, Y1);
	setLine(mode, X1, Y1, X2, Y1);
	setPixel( mode, X1, Y1 );
	setPixel( mode, X1, Y2 );
	setPixel( mode, X2, Y1 );
	setPixel( mode, X2, Y2 );
}
void BufferLCD::setEllipse(byte mode, int X1, int Y1, int X2, int Y2){
	setRect( mode, X1, Y1, X2, Y2);
}




