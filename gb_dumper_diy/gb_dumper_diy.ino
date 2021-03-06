#include<SPI.h>
//Pin connected to ST_CP of 74HC595
//int latchPin = 4;
//Pin connected to SH_CP of 74HC595
//int clockPin = 13;
////Pin connected to DS of 74HC595
//int MO = 11;
////Pin connected to DS of 74HC165
//int MI = 12;
//^for pro mini, switching to mega


//check pins for arduino mega 2560 R3
int latchPin = 10;   // D4
int clockPin = 52; //D52
int MO = 51;  //D51
int MI = 50;  //D50
int rdPin = 49;
int wrPin = 47;
int csPin = 48;


//gb pins
//VCC – Power (5 volts)
//CLK – Clock signal (not used)
//~WR – if low(grounded) and if RD is low, we can write to the SRAM and load a ROM or SRAM bank
//~RD – if low (grounded) and if WR is high, we can read the ROM and SRAM
//CS_SRAM – if enabled, selects the SRAM

  uint8_t highByte = 0x00; //msb
  uint8_t lowByte =  0x00; //lsb
  uint32_t addr = 0x00000000;
  uint8_t gameName[16]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint8_t MBC  = 0x00;
  uint8_t romSize  = 0x00;
  uint8_t ramSize  = 0x00;
  
  int ramBanks  = 0x00;
  int romBanks  = 0x00;

  

void setup()
{

 pinMode(latchPin, OUTPUT);
 pinMode(clockPin, OUTPUT);
 pinMode(MO, OUTPUT);
 pinMode(rdPin, OUTPUT);
 pinMode(wrPin, OUTPUT);
 pinMode(wrPin, OUTPUT);


 SPI.begin ();
 SPI.beginTransaction(SPISettings(16000000, LSBFIRST, SPI_MODE0));
 Serial.begin(9600);
{
//PORTA: Pins 22-29 (bits 0-7)
//PORTC: Pins 30-37 (bits 7-0)
//PORTF: Pins A0-A7 (bits 0-7)
//PORTK: Pins A8-A15 (bits 0-7)
//PORTL: Pins 42-49 (bits 7-0)

//Note that some ports (C and L) map bits to pins in reverse order.

//Port   D PinName   Function
//PF 0       54       A0
//PF 1       55       A1
//PF 2       56       A2
//PF 3       57       A3
//PF 4       58       A4
//PF 5       59       A5
//PF 6       60       A6
//PF 7       61       A7
//PK 0       62       A8
//PK 1       63       A9
//PK 2       64       A10
//PK 3       65       A11
//PK 4       66       A12
//PK 5       67       A13
//PK 6       68       A14
//PK 7       69       A15

}

//needed to read from card
  digitalWrite(rdPin, HIGH); // RD 1
  digitalWrite(wrPin, HIGH); // WR 1
  digitalWrite(csPin, HIGH); // RD 1
 
 Serial.println("START"); // Send the start command

 //input ports
   DDRF = 0x00; // All pins in PORTF are inputs A0-A7
   DDRK = 0x00; // All pins in PORTK are inputs A8-A18
   //pull down with 10k resisters or something.
  
}
void readHeader()
{
  
  // start at adress hx0134
     for(uint8_t i = 0x0134; i <= 0x0143;i++) // get title from addr 0134-0143 
     {
      int j = 0;
      j++;
      read8Bit(i);
      gameName[j] = lowByte;
      Serial.print(gameName[i]);
      Serial.print("\n");
     }
     
     read8Bit(0x0147); // read mbc
     MBC = lowByte;
     Serial.print(MBC);
     Serial.print("\n");

     read8Bit(0x0148); // read romsize
     romSize = lowByte;
     Serial.print(romSize);
     Serial.print("\n");

     read8Bit(0x0149); // read ramsize
     ramSize = lowByte;
     Serial.print(ramSize);
     Serial.print("\n");    

    // romBanks = romSize / 16;
    // ramBanks = ramSize /8;
}

void sendAddr(uint32_t addr)
{
 //faster if works SPI.transfer(buffer, size) 
  SPI.transfer(&addr, 4);
  digitalWrite (latchPin, HIGH);
  delay (500);
}

void read8Bit(uint32_t addr)
{
  // 2 hex numbers is 1 byte(8bits)
 
 sendAddr(addr);
 //now write read
 // use ports https://www.arduino.cc/en/uploads/Hacking/PinMap2560big.png
  lowByte = PINK; //read port in to low byte
  Serial.print(lowByte);
  Serial.print("\n");
  

}
void read16Bit(uint32_t addr)
{
 
   sendAddr(addr);
  lowByte = PINK; //read port in to low byte
  highByte = PINF; //read port in to low byte
}

void gbReadRomOnly()
{
  for(uint8_t i = 0x0000; i<=0x7FFF; i++)
  {
    read8Bit(i);
  }
  
}
void mbc1()
{
	//read bank 0
    for(uint8_t i = 0x0000; i<=0x3FFF; i++)
  {
    read8Bit(i);
  }

	//will overdump set to actual size later
	for (uint8_t i = 0x01; i <= 0x7f; i++) 
	{
		bankSwitch(i);
		for (uint8_t i = 0x4000; i <= 0x7FFF; i++)
		{
			read8Bit(i);
		}
	}


}
  


void bankSwitch(uint8_t bank)
{
  digitalWrite(rdPin, 1); 
    digitalWrite(wrPin, 0); 
  DDRK = B11111111; //set data to output
  
  if(MBC == 0x01 || MBC == 0x02 || MBC == 0x03 || MBC == 0xFF) //mbc1
  {
	 if (bank != 20 || bank != 40 || bank != 60)
	 {
		 if (ramSize == 0x03)
		 {
			 sendAddr(0x7000); //set 32k ram mode
			 PORTK = 0x01;

			 sendAddr(0x3000); //set bank number
			 PORTK = bank;

		 }
		 else
		 {
			 sendAddr(0x7000);
			 PORTK = 0x00;  //8k  ram mode

			 sendAddr(0x3000); //set bank number
			 PORTK = bank & 0x1F; //send lower 5 bits
			 sendAddr(0x4000); //set bank number
			 PORTK = bank >> 5; //send last 2 bits
		 }
	 }
   
 
  }

  if (MBC == 0x05 || MBC == 0x06) //mmb2, maybe add 3 and 5 later
  {
	  sendAddr(0x3000); //set bank number
	  PORTK = bank;
  }

  digitalWrite(rdPin, LOW); // RD 0
  digitalWrite(wrPin, HIGH); // WR 1
  DDRF = 0x00; //set data to read again

}

void loop()
{
  readHeader();
  gbReadRomOnly();
}
