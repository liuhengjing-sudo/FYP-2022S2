#include <SPI.h>
#include <RH_RF95.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>
#include <SharpIR.h>
SharpIR sensor( SharpIR::GP2Y0A41SK0F, A3 );

int counter = 0;
int counter1 = 0;
int loraflag = 1;
SoftwareSerial mySerial(14, 15); // RX, TX


// Singleton instance of the radio driver
RH_RF95 rf95;
//RH_RF95 rf95(5, 2); // Rocket Scream Mini Ultra Pro with the RFM95W
//RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM95

// Need this on Arduino Zero with SerialUSB port (eg RocketScream Mini Ultra Pro)
//#define Serial SerialUSB
char temp1[10];
 char IR[10];
 char temp1_buffer[30]; char IR_buffer[30];
char a[2][14];
char lorabuffer[251];

 
void timerIsr()
{
counter1++;
}
float Thermister(int RawADC) {
float Temp;
Temp = log(((10240000 / RawADC) - 10000));
Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp );
Temp = Temp - 273.15; // Convert Kelvin to Celcius
return Temp;
}
void setup()
{
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  //  driver.setTxPower(14, true);
  Timer1.initialize(1000000);
Timer1.attachInterrupt( timerIsr );
pinMode(16,OUTPUT);
pinMode(3,INPUT);
pinMode(4,INPUT);
 mySerial.begin(9600);




}

void loop()
{

   Serial.print("counter1= ");
Serial.println(counter1);

 if (counter1 >= 30){
 counter1 = 0;
 loraflag= !loraflag;
 digitalWrite(16,!digitalRead(16));
 }


   if(loraflag == 1 ){  

  Serial.println("Sending to rf95_server");
  // Send a message to rf95_server
  uint8_t data[] = "Hello I'm from lora!";
    rf95.send(lorabuffer, 251);

  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.waitAvailableTimeout(3000))
  {
    // Should be a reply message for us now
    if (rf95.recv(buf, &len))
    {
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply, is rf95_server running?");
  }
  delay(400);
   }



   
if (mySerial.available()){

 String command = mySerial.readStringUntil('\n');

Serial.println(command);

 
}


 char data[10];
 char data_buffer[20];
  int distance = analogRead(3); //Calculate the distance in centimeters and store the value in a variable
  float stock_level = map (analogRead(3),0,600,0,100);
 int precision =1;

dtostrf(Thermister(analogRead(4)),precision+3,precision,temp1);
dtostrf(stock_level,precision+3,precision,IR);
sprintf(lorabuffer, "Temp1lora=%s\nIRlora=%s", temp1,IR);


sprintf(temp1_buffer, "Temp1=%s", temp1);
sprintf(IR_buffer, "IR=%s ", IR);
mySerial.println(temp1_buffer);
delay(500);

mySerial.println(IR_buffer);
delay(500);

Serial.println(temp1_buffer);
Serial.println(IR_buffer);












}