String agAdi = "Ardad";                   //Ağımızın adını buraya yazıyoruz.    
String agSifresi = "Arda2000";             //Ağımızın şifresini buraya yazıyoruz.
int IN1 = 3, IN2 = 4, IN3 = 5, IN4 = 6; //Step motor pinlerini tanımlıyoruz.
int sure = 3;
int feedcount=0;
#include "NewPing.h"
#define TRIGGER_PIN 9
#define ECHO_PIN 10
#define MAX_DISTANCE 400
float duration, distance;


NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup(){ 
  pinMode(IN1, OUTPUT);                                   //Motor pinleri OUTPUT olarak ayarlanır.
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  Serial.begin(115200);                                         //Seri haberleşmeyi başlatıyoruz.
  Serial.println("AT");                                         //AT komutu ile modül kontrolünü yapıyoruz.
  while(!Serial.find("OK")){                                    //Modül hazır olana kadar bekliyoruz.
    Serial.println("AT");
  }
  delay(1000);  
  Serial.println("AT+RST");                                     //ESP8266'yı resetliyoruz.
  delay(1000);
  while(!Serial.find("OK"))                                  //Resetleme işleminin bitmesini bekliyoruz.
  delay(1000);
  Serial.println("AT+CWMODE=1");                                //Modülümüzü client olarak ayarlıyoruz.
  while(!Serial.find("OK"));                                    //Ayarlamanın tamamlanmasını bekliyoruz.
  Serial.println("AT+CWJAP=\""+agAdi+"\",\""+agSifresi+"\"");   //Wifi'a bağlanıyoruz.
  while(!Serial.find("OK"));                                    //Bağlanana kadar bekliyoruz.
  Serial.print("AT+CIFSR\r\n");                                 //IP adresini ve MAC adresini okuyoruz.
  Serial.print(espOkuma(1000));                                 //IP ve MAC adresini ekrana yazıyoruz.
  serialTemizle(2000);                                          //Seri haberleşmede kullanmadığımız verileri temizliyoruz.
  Serial.print("AT+CIPMUX=1\r\n");                              //Çoklu bağlantı ayarını yapıyoruz.
  serialTemizle(2000);
  Serial.print("AT+CIPSERVER=1,80\r\n");                        //Server oluşturuyoruz ve 80. porttan dinlemeye başlıyoruz.
  serialTemizle(2000);
}
void loop(){
  if(Serial.available()){                                       //Seri haberleşme verisi geldiğinde içindeki komutlar çalışır.
    if(Serial.find("+IPD,")){                                   //Bağlantı isteği geldiğinde içindeki komutlar çalışır.
      delay(200);
      int connectionId = Serial.read() - 48;                    //Bağlantı numarasını okuyoruz.
      String komut = espOkuma(1000);                           //Bağlantı komutlarını alıyoruz.    
      if (distance >= 1000 || distance <= 2){
        Serial.print(distance);
        Serial.println(" cm");
        delay(500);
        if(komut.indexOf("step=Feed") != -1){                    //İleri komutu geldiğinde içerisindeki komutlar çalışır.
        
        stepFeed(512);
        }
      }
      else{
        Serial.println("Out of range");
      }
      String sayfa = "<h1 style='margin-left:100px'>PetFeeder</h1><br>";                       
      sayfa+="<br><a href=\"?step=Feed\"><button onclick='increment()'><h1>Feed</h1></button></a>";
      sayfa+="<p id='count'>0</p>"; 
      sayfa+="<img src='https://www.electromaker.io/uploads/images/projects/thumbs/Raushan_Kumar_11010_1638108438_cover_image.jpeg'>";
      sayfa+="<script> function increment() { var count = document.getElementById('count'); count.innerHTML = parseInt(count.innerHTML) + 1; } </script>";
     komut = "AT+CIPSEND=";                                    //Sayfa verisinin uzunluğu komut değişkenine eşitlenir.
      komut += connectionId;
      komut += ",";
      komut +=sayfa.length();
      komut +="\r\n";
      Serial.print(komut);                                      //Komut gönderilir.
      delay(1000);
      Serial.print(sayfa);                                      //Sayfa verisi gönderilir.
      delay(1000);
      komut = "AT+CIPCLOSE=";                                   //Bağlantı sonlandırma komutu ayarlanır.    
      komut+=connectionId;
      komut+="\r\n";
      Serial.print(komut);                                      //Bağlantı sonlandırma komutu gönderilir.
    }
  }
}
String espOkuma(long int zamanAsimi){                           //ESP'den gelen dönütleri okur.
  long int baslangic = millis();  
  String gelen;
  while(millis() - baslangic < zamanAsimi){                   
    if(Serial.available()>0){
        char c = Serial.read(); 
        gelen += c;
    } 
  }
  gelen.replace("AT+","");
  return gelen;
}
void serialTemizle(long int zamanAsimi){                         //Seri haberleşmede kullanmadığımız byteları temizler.
  long int baslangic = millis();
  while(millis() - baslangic < zamanAsimi){
    if(Serial.available()>0){
        Serial.read(); 
    } 
  }
}
void stepFeed(int adimSayisi){                               //Step motora ileri hareketi yaptırır.
 for (int i = 0; i < adimSayisi; i++) {
    digitalWrite(IN4, HIGH);
    delay(sure);
    digitalWrite(IN4, LOW);
    digitalWrite(IN3, HIGH);
    delay(sure);
    digitalWrite(IN3, LOW);
    digitalWrite(IN2, HIGH);
    delay(sure);
    digitalWrite(IN2, LOW);
    digitalWrite(IN1, HIGH);
    delay(sure);
    digitalWrite(IN1, LOW);
  }
}