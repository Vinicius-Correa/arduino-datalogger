// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
#include "DHT.h"
#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>

//criar um objeto File para manipular um arquivo
File myFile;
const int chipSelect = A3;

RTC_DS3231 rtc;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  
DHT dht;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {
  lcd.begin(16, 2);
  
  Serial.begin(9600);

  delay(3000); // wait for console opening

  //Inicializa o cartao SD
  Serial.print("Inicializando o SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Nao inicializado!");
    return;
  }
  Serial.println(" Ok!");

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  dht.setup(2); // data pin 2
}

void loop () {
    //leitura no sensor de temperatura e umidade
    delay(dht.getMinimumSamplingPeriod());
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();
    
    //faz aquisicao da data e hora
    DateTime now = rtc.now();

    //imrpime dados via serial
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.print(" - T: ");
    Serial.print(temperature, 1);
    Serial.print(" - H: ");
    Serial.print(humidity, 1);
    Serial.println();

    //imprime dados no display
    lcd.setCursor(0,0);
    lcd.print(now.day(), DEC); 
    lcd.print('/'); 
    lcd.print(now.month(), DEC);
    lcd.print('/');
    lcd.print(now.year(), DEC); 
    lcd.setCursor(0,1);
    lcd.print(now.hour(), DEC); 
    lcd.print(':'); 
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.print(now.second(), DEC); 
    lcd.setCursor(10,0);
    lcd.print("T:");
    lcd.print(temperature);
    lcd.setCursor(10,1);
    lcd.print("H:");
    lcd.print(humidity);

    //escreve no cartao SD
    myFile = SD.open("log.txt", FILE_WRITE);
    if (myFile) {
        Serial.print("Escrevendo dados no cartao SD. ");
        myFile.print(now.year(), DEC);
        myFile.print('/');
        myFile.print(now.month(), DEC);
        myFile.print('/');
        myFile.print(now.day(), DEC);
        myFile.print(" ");
        myFile.print(now.hour(), DEC);
        myFile.print(':');
        myFile.print(now.minute(), DEC);
        myFile.print(':');
        myFile.print(now.second(), DEC);
        myFile.print(" - T: ");
        myFile.print(temperature, 1);
        myFile.print(" - H: ");
        myFile.print(humidity, 1);
        myFile.println();
        // Terminou de escrever, fecha-se o arquivo:
        myFile.close();
        Serial.println(" Escreveu com sucesso!");
    }
    else {
        Serial.println(" Nao foi possivel abrir o arquivo");
    }
    delay(3000);
}
