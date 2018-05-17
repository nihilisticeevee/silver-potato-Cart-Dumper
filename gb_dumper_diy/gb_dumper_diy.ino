// dont forget to set pins for dataread
#include<SPI.h>
//Pin connected to ST_CP of 74HC595
int latchPin = 4;
//Pin connected to SH_CP of 74HC595
int clockPin = 13;
////Pin connected to DS of 74HC595
int MO = 11;
////Pin connected to DS of 74HC165
int MI = 12;

  uint8_t highByte = 0x00; //msb
  uint8_t lowByte =  0x00; //lsb
  uint32_t addr = 0x00000000;
  uint8_t gameName[16]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint8_t MBC  = 0x00;
  uint8_t romSize  = 0x00;
  uint8_t ramSize  = 0x00;
  
  

void setup()
{

 pinMode(latchPin, OUTPUT);
 pinMode(clockPin, OUTPUT);
 pinMode(MO, OUTPUT);

 //pinMode(1, INPUT); // double check if can use these for data pins
  //pinMode(2, INPUT);
   //pinMode(3, INPUT);
    //pinMode(4, INPUT);
     //pinMode(5, INPUT);
      //pinMode(6, INPUT);
       //pinMode(7, INPUT);
        //pinMode(8, INPUT);


SPI.begin ();
SPI.beginTransaction(SPISettings(16000000, LSBFIRST, SPI_MODE0));
 
Serial.begin(9600);
  
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
}

void sendAddr(uint32_t addr)
{
    // if you are daisychaining, add 2nd, 3rd SPI.transfer(next_byte_of_data )

  digitalWrite (latchPin, LOW);
  //need to bit shift and bit mask to send 1 byte at a time
 // SPI.transfer (addr & 00000000000000000000000011111111);
  //SPI.transfer ((addr >> 8) & 00000000000000000000000011111111);
 // SPI.transfer ((addr >> 16) & 00000000000000000000000011111111);
 // SPI.transfer ((addr >> 24) & 00000000000000000000000011111111);

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
   //lets try reading an 8bit byte with port mapping

}
void read16Bit(uint32_t addr)
{
}
void loop()
{
  
 // for (int i =0; i < 100;i++)
  {
    sendAddr(4294967294);
   // shiftOut(MO, clockPin, LSBFIRST, 4);

  }
}
