/*
 * ----------------------------------------------------------------------------------------
 * Pin Yang di Gunakan :
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             48         D9         RESET/ICSP-5     RST
 * SPI IRQ     IRQ          8             49
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 * SPI SS      SDA(SS)      10            53        D10        10               10
 *-------------------------------------------------------------------------------------------
 */
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <dht.h>

#define RST_PIN         48          // Pin ini dapat di atur sesuka kallian
#define SS_PIN          53          // Pin ini dapat di atur sesuka kallian
#define dataPin         4           //pin dht22

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo servo, servo2;
dht DHT;   

String read_rfid;                   
String ok_rfid_1="20f8d21e";        // RFID 1
String ok_rfid_2="43a5e52ec5780";    // RFID 2
String ok_rfid_3="466892acf2e80";         // RFID 3
int lock = 2;                       // Relay

const int buttonPin = 8; //pin sensor LDR
int gnd = 9;             //pin led low
int led = 10;            //pin led high
int buttonState = 0;

int r1 = 5;   //pin motor dc1
int l1 = 6;   //pin motor dc2
int pwm = 7;  //pin pengatur kecepan motor dc


void setup() {
    Serial.begin(9600);         
    while (!Serial);           
    SPI.begin();                
    mfrc522.PCD_Init();        

    pinMode(lock, OUTPUT);
    digitalWrite(lock,HIGH);

    servo.attach(3);
    servo.write(180);
 
    Serial.print("Letakkan Kartu Anda ...");
    Serial.println();

    pinMode(buttonPin, INPUT_PULLUP); //Fungsi PULLUP = mengganti resistor (menaikan tegangan)
    pinMode(led, OUTPUT);
    pinMode(gnd, OUTPUT);
    digitalWrite(gnd,LOW);
    digitalWrite(led,HIGH);
    servo2.attach(11);
    servo2.write(180);

    /*pinMode(r1,OUTPUT);
    pinMode(l1,OUTPUT);
    pinMode(pwm,OUTPUT);
    analogWrite(pwm,0);
    digitalWrite(r1,HIGH);
    digitalWrite(l1,LOW);
    delay(500);*/
}

void dump_byte_array(byte *buffer, byte bufferSize) 
  {
    read_rfid="";
    for (byte i = 0; i < bufferSize; i++) 
    {
      read_rfid=read_rfid + String(buffer[i], HEX);
    }
  }

void open_lock() {
  digitalWrite(lock,LOW);
  delay(500);
  servo.write(135);
  delay(1000);
  servo.write(180);
  delay(1000);
  digitalWrite(lock,HIGH);
}

void pintu()
{
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;
    
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println(read_rfid);
    if ((read_rfid==ok_rfid_1) || (read_rfid==ok_rfid_3) || (read_rfid==ok_rfid_2)) {
      open_lock();
      //Serial.println("Akses Diterima");     
    }
    /*else {
      Serial.println("Akses Ditolak");
    }*/
}

void lampu()
{
   buttonState = digitalRead(buttonPin);
   if (buttonState == HIGH) 
   {
      digitalWrite(led, HIGH);
      servo2.write(0);
      delay(1000);
   }
   else 
   {
      digitalWrite(led, LOW);
      servo2.write(180);    
   }
}

void kipeh()
{
  int readData = DHT.read22(dataPin);//baca data dari sensor
  float t = DHT.temperature;         // ambil nilai suhu
  float h = DHT.humidity;            //ambil nilai kelembaban

  if(t<20) {
    analogWrite(pwm,0);
    digitalWrite(r1,LOW);
    digitalWrite(l1,LOW); 
  }
  if(t >= 20 && t <= 30) {
     analogWrite(pwm,100);
     digitalWrite(r1,HIGH);
     digitalWrite(l1,LOW);
  }
  
  if(t > 30) {
     analogWrite(pwm,180);
     digitalWrite(r1,HIGH);
     digitalWrite(l1,LOW);
  } 
}

void loop() {
  pintu();
  lampu();
  kipeh();
}
