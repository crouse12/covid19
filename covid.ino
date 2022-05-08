#include <WiFi.h>
#include <WiFiClientSecure.h>  
WiFiClientSecure client;
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
unsigned long timeout;
char* host;
String webreqresponse="";
int httpPort=443;
String url="";

int infected=0;
int recovered=0;
int deaths=0;


void setup() {
    Serial.begin(9600);
    WiFi.begin("TP-LINK_34B518", "batman88");
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
    u8g2.begin();
    u8g2.enableUTF8Print();
    u8g2.setFont(u8g2_font_unifont_t_chinese1);
    u8g2.firstPage();
    do {
      u8g2.setCursor(0,15);
      u8g2.print("顯示COVID19");
    }while (u8g2.nextPage());
 

}

void loop() {
 u8g2.firstPage();
 httpPort = 443;
 host="www.worldometers.info";
 Serial.println(host);
    if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      return;
    }
    url = "/coronavirus/country/US";
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    String s1 = "";
    String s2 = "";
    int read_value_step=0;
    while(client.available() && read_value_step!=3){
      webreqresponse = client.readStringUntil('\n');
      if (read_value_step == 0) {
      int place = webreqresponse.indexOf("<h1>Coronavirus Cases:</h1>");
      if (place != -1 && read_value_step==0) {
         read_value_step=1;
         webreqresponse = client.readStringUntil('\n');
         webreqresponse = client.readStringUntil('\n');
         webreqresponse.replace("<span style=\"color:#aaa\">","");
         webreqresponse.replace(" </span>","");
         webreqresponse.replace(",","");
         infected = webreqresponse.toInt();
      }
      }
      if (read_value_step == 1) {
         int place = webreqresponse.indexOf("Deaths:");
         if ((place != -1) && (place < 350)) {
          read_value_step = 2; 
          webreqresponse = client.readStringUntil('\n');
          webreqresponse = client.readStringUntil('\n');
          webreqresponse.replace("<span>","");
          webreqresponse.replace("</span>","");
          webreqresponse.replace(",","");
          deaths = webreqresponse.toInt();
          Serial.println("222");
         } 
      }
        if (read_value_step == 2) {
         int place = webreqresponse.indexOf("Recovered:");
         if ((place != -1) && (place < 350)) { 
          read_value_step == 3;
          webreqresponse = client.readStringUntil('\n');
          webreqresponse = client.readStringUntil('\n');
          webreqresponse.replace("<span>","");
          webreqresponse.replace("</span>","");
          webreqresponse.replace(",","");
          recovered = webreqresponse.toInt();
         } 
      }
    }
    client.stop();
 do {
      u8g2.setCursor(0,15);
      u8g2.print("顯示美國COVID-19");
      u8g2.setCursor(0,30);
      u8g2.print("感染");
      u8g2.setCursor(50,30);
      u8g2.print(String(infected));
      u8g2.setCursor(0,45);
      u8g2.print("恢復");
      u8g2.setCursor(50,45);
      u8g2.print(String(recovered));
      u8g2.setCursor(0,60);
      u8g2.print("死亡");
      u8g2.setCursor(50,60);
      u8g2.print(String(deaths));
 }while (u8g2.nextPage());
 delay(3000);
}
