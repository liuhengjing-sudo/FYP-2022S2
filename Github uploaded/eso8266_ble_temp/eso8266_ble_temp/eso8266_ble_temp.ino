#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <math.h>
SoftwareSerial LoRaSerial(5, 4); // RX, TX
//SoftwareSerial LoRaSerial(13, 15); // RX, TX

int tmp;
String content="";
String contentlora="";
// WiFi
const char *ssid = "Vodafone-B0D74"; // Enter your WiFi name
const char *password = "nTkg6wyHksad5439";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "192.168.1.10";
const char *topic = "/home/esp8266/switch/command";
const char *temp1 = "/home/esp8266/temp1/value";
const char *temp2 = "/home/esp8266/temp2/value";
const char *rssivalue="/home/esp8266/lorarssi/value";
const char *location="/home/esp8266/room/value";

const char *mqtt_username = "openhabian";
const char *mqtt_password = "openhabian";
const int mqtt_port = 1883;

#define load1 2
#define input1 4
#define input2 0
int last_pub_time=0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(load1, OUTPUT);
  pinMode(input1, INPUT_PULLUP); 
  // Set software serial baud to 115200;
  Serial.begin(9600);
  while(!Serial){}       // Loop until USB is ready
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("Public emqx mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
  // publish and subscribe
  //client.publish(topic2,temperatureptr );
  client.subscribe(topic);


  //mySerial.begin(9600);
  //mySerial.println("hello world");
  LoRaSerial.begin(9600);
  

}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
      Serial.print((char) payload[i]);
  }
  
  if ((char)payload[0] == '1'){
    Serial.print("\n payload = ");
    Serial.print((char) payload[0]);
    digitalWrite(load1, LOW);
    Serial.print(digitalRead(load1));

  }
  else{
    Serial.print("\n payload = ");
    Serial.print((char) payload[0]);
    digitalWrite(load1, HIGH);
    Serial.print(digitalRead(load1));
  }
  

  Serial.println();
  Serial.println("-----------------------");
}

void loop() {
  /*
  *****************************************************************************
  //  if (!digitalRead(input2)) {
  //  if(!digitalRead(input1) && (millis()- last_pub_time > 3000)){
  //         Serial.print("last_pub_time");
  //         digitalWrite(load1, HIGH);

  //         last_pub_time=millis();
  //         client.publish("/home/esp8266/switch/state","0");   
  // }
  // else if (digitalRead(input2)){
  //             digitalWrite(load1, LOW);
  //             client.publish("/home/esp8266/switch/state","1");   


  //   }
  //  }
 ******************************************************************************
 */
  client.loop();
  if (LoRaSerial.available()) {
     //Serial.print(mySerial.read());        //Serial.println(data);
    content = LoRaSerial.readStringUntil('\n');
    Serial.println(content);
    //int RSIstring = LoRaSerial.parseInt();
    String RSIstring=content.substring(0,5);
    //if (millis()- last_pub_time < 10000){
    if (RSIstring =="RSSI:"){
        char valR[5];
        String content_temp= content.substring(6);
        
        sprintf(valR,"%d",atoi(content_temp.c_str()));
        client.publish(rssivalue,valR);
        Serial.print("publish the rssi = ");
        Serial.println(valR);
        last_pub_time=millis();

      }



        String Templora=content.substring(0,9);
    if (Templora == "Temp1lora"){
          String temp1_stringlora= content.substring(10); 
         
         float numlora = temp1_stringlora.toFloat();
         if (numlora >0){
          char tempvallora[5];
          sprintf(tempvallora,"%f",numlora);
          client.publish(temp1,tempvallora);
          Serial.print("publish the temp1 = ");
          Serial.println(tempvallora);
          last_pub_time=millis();

      }
      }


    
    String IRstrlora=content.substring(0,6);

     if(IRstrlora == "IRlora"){
          String temp2_stringlora= content.substring(7); 
         float num3 = temp2_stringlora.toFloat();
          Serial.print("num3 = ");
         Serial.println(num3);
         if (num3 >0){
         //*****
          char sensorvallora[10];
          sprintf(sensorvallora,"%f",num3);
          client.publish(temp2,sensorvallora);
          Serial.print("publish the topic IR = ");
          Serial.println(sensorvallora); 
          last_pub_time=millis();
 
         }  
      }
    
    String Temp=content.substring(0,5);
    
    if (Temp == "Temp1"){
          String temp1_string= content.substring(6); 
         
         float num = temp1_string.toFloat();
          char tempval[5];
           if (num >0){

          sprintf(tempval,"%f",num);
          client.publish(temp1,tempval);
          Serial.print("publish the temp1 = ");
          Serial.println(tempval);
          client.publish(location,"THIS BOX IS IN ROOM 1");
          last_pub_time=millis();

          
      }
    }
    String IRstr=content.substring(0,2);

     if(IRstr == "IR"){
          String temp2_string= content.substring(3); 
         float num2 = temp2_string.toFloat();
          Serial.print("num2 = ");
         Serial.println(num2);
         if (num2 > 0 ){
         //*****
          char sensorval[10];
          sprintf(sensorval,"%f",num2);
          client.publish(temp2,sensorval);
          Serial.print("publish the topic IR = ");
          Serial.println(sensorval);  
          client.publish(location,"THE BOX IS IN ROOM 1");
           last_pub_time=millis();

  
      }   
     }
    

       
    }

}
     

    /***************************************
    if (content.substring(0,5)=="Temp1"){
          Serial.println(content.substring(6)); 
          String temp1_string= content.substring(6);     
          client.publish(temp1,temp1_string.c_str());

      }
      if (content.substring(0,5)=="Temp2"){
          Serial.println(content.substring(6)); 
          String temp2_string= content.substring(6);     
          client.publish(temp2,temp2_string.c_str());

      }
      
  }


*************************/


  /*
  if (Serial.available()) {
     //Serial.print(mySerial.read());        //Serial.println(data);
    contentlora = Serial.read();
  
    Serial.println("contentlora is ");  
    Serial.println(Serial.parseInt()); 
      //Serial.println(data);
//    String c=String(temp);
//    client.publish(topic2,c.c_str());

    //Serial.write(mySerial.read());
    //String data = mySerial.readStringUntil('\n');
    //Serial.println(data);
    /**********
    temperature = mySerial.read();
    Serial.println("temperature.toInt()");

    Serial.println(temperature.toInt());

    int b= temperature.toInt();

    String c=String(b);
    client.publish(topic2,c.c_str());
    ******/
/*
    }
    */
    
