#include <EEPROM.h>

#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPLUCBeUXme"
#define BLYNK_DEVICE_NAME "monitoring moisture soil"
#define BLYNK_AUTH_TOKEN "MEdcoPRZSx9FRix45FjYOQRiWhXxWUhL"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

//include library DS18b20
#include <OneWire.h>
#include <DallasTemperature.h>

//include library rtc ds3231
#include <DS3231.h>

#define pinSoil 34
#define pinPump 2
#define ONE_WIRE_BUS 32

//init sensor DS3231
DS3231 rtc;
RTCDateTime dt;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensorSuhu(&oneWire);

char auth[] = BLYNK_AUTH_TOKEN;

//wifi dan password
char ssid[] = "Redmi";
char pass[] = "qwertyuiop1234";

int soilMoistureValue = 0;
int soilMoisturePercent = 0;
int isManual = 0;
int setTime[8] = { -1, -1, -1, -1, -1, -1, -1, -1};
int currentTime = -1;
int currentMinute = -1;
int currentSecond = -1;
int currentDay = -1;
int timeOn = 2000;
int lastHour7;
int lastHour8;
int lastHour9;
int lastHour10;
int lastHour15;
int lastHour16;
int lastHour17;
int lastHour18;
int i;

void setup() {
  EEPROM.begin(12);
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);


  //start sensor rtc DS3231
  rtc.begin();
  rtc.setDateTime(2022, 9, 17, 6, 59, 40);

  pinMode(pinPump, OUTPUT);
  sensorSuhu.begin();

for (i=1; i<30;i++){
  digitalWrite(i, HIGH);
  delay(100);
}
  

  initButton();


}


void loop() {

  Blynk.run();
  double suhu = ambilSuhu();
  soilMoistureValue = analogRead(pinSoil);
  soilMoisturePercent = map(soilMoistureValue, 3620, 1680, 0, 100);

  dt = rtc.getDateTime();

  for (i = 0; i < 8; i++) {
    setTime[i] = EEPROM.read(i);
  }
  delay(1000);

  currentTime = dt.hour;
  currentMinute = dt.minute;
  currentSecond = dt.second;

  Serial.println(currentTime);
  Serial.println(currentMinute);
  Serial.println(currentSecond);

  //  Serial.println(soilMoistureValue);
  //  Serial.println(soilMoisturePercent);
  if (soilMoisturePercent > 100)
  {
    //    Serial.println("100 %");
  }
  else if (soilMoisturePercent < 0)
  {
    //    Serial.println("0 %");
  }
  else if (soilMoisturePercent >= 0 && soilMoisturePercent <= 100)
  {
    //    Serial.print(soilMoisturePercent);
    //    Serial.println("%");
  }

  Serial.print("currentTimeLuar: ");
  Serial.println(currentTime);

  for (i = 0; i < 8; i++) {
    Serial.print("array: ");
    Serial.print(setTime[i] - 1);
    Serial.print(" | ");
    Serial.println(currentTime);

    if ((setTime[i] - 1) == (currentTime) && currentMinute == 59 && currentSecond >= 58) {
      String text = "Hidup Otomatis Jam " + (String)(currentTime + 1);
      Serial.println(text);
      Blynk.virtualWrite(V100, text);
      digitalWrite(pinPump, HIGH);
      delay(timeOn);
      digitalWrite(pinPump, LOW);
    }
    delay(10);
  }

  Blynk.virtualWrite(V0, soilMoisturePercent);
  Blynk.virtualWrite(V2, suhu);
  EEPROM.commit();
  initButton();
  delay(1000);
}

float ambilSuhu()
{
  sensorSuhu.requestTemperatures();
  float suhu = sensorSuhu.getTempCByIndex(0);
  return suhu;
}


//pagi
BLYNK_WRITE(V7) {
  if (param.asInt() == 1) {
    EEPROM.write(0, 7);
    Blynk.virtualWrite(V100, "Terkirim 7");
    delay(80);
  } else {
    EEPROM.write(0, -1);
  }
}

BLYNK_WRITE(V8) {
  if (param.asInt() == 1) {
    EEPROM.write(1, 8);
    Blynk.virtualWrite(V100, "Terkirim 8");
    delay(80);
  } else {
    EEPROM.write(1, -1);
  }
}

BLYNK_WRITE(V9) {
  if (param.asInt() == 1) {
    EEPROM.write(2, 9);
    Blynk.virtualWrite(V100, "Terkirim 9");
    delay(80);
  } else {
    EEPROM.write(2, -1);
  }
}

BLYNK_WRITE(V10) {
  if (param.asInt() == 1) {
    EEPROM.write(3, 10);
    Blynk.virtualWrite(V100, "Terkirim 10");
    delay(80);
  } else {
    EEPROM.write(3, -1);
  }
}

//sore
BLYNK_WRITE(V15) {
  if (param.asInt() == 1) {
    EEPROM.write(4, 15);
    Blynk.virtualWrite(V100, "Terkirim 3");
    delay(80);
  } else {
    EEPROM.write(4, -1);
  }
}

BLYNK_WRITE(V16) {
  if (param.asInt() == 1) {
    EEPROM.write(5, 16);
    Blynk.virtualWrite(V100, "Terkirim 4");
    delay(80);
  } else {
    EEPROM.write(5, -1);
  }
}

BLYNK_WRITE(V17) {
  if (param.asInt() == 1) {
    EEPROM.write(6, 17);
    Blynk.virtualWrite(V100, "Terkirim 5");
    delay(80);
  } else {
    EEPROM.write(6, -1);
  }
}

BLYNK_WRITE(V18) {
  if (param.asInt() == 1) {
    EEPROM.write(7, 18);
    Blynk.virtualWrite(V100, "Terkirim 6");
    delay(80);
  } else {
    EEPROM.write(7, -1);
  }
}


BLYNK_WRITE(V1) {
  isManual = param.asInt();
  if (isManual == 1) {
    Serial.println("Pompa Hidup Manual");
    digitalWrite(pinPump, param.asInt());
    Blynk.virtualWrite(V100, "Pompa Hidup Manual");
    Blynk.virtualWrite(V7, 0);
    Blynk.virtualWrite(V8, 0);
    Blynk.virtualWrite(V9, 0);
    Blynk.virtualWrite(V10, 0);

    Blynk.virtualWrite(V15, 0);
    Blynk.virtualWrite(V16, 0);
    Blynk.virtualWrite(V17, 0);
    Blynk.virtualWrite(V18, 0);

    for (i = 0; i < 8; i++) {
      setTime[i] = -1;
      EEPROM.write(i, setTime[i]);
    }
//    delay(timeOn);
//    digitalWrite(pinPump, LOW);
  }else{
    digitalWrite(pinPump, LOW);
    Blynk.virtualWrite(V100, "Pompa Mati Manual");
    delay(100);
  }
}

void initButton() {
  if (EEPROM.read(0) == 255) {
    lastHour7 = -1;
  } else {
    lastHour7 = EEPROM.read(0);
  }

  if (EEPROM.read(1) == 255) {
    lastHour8 = -1;
  } else {
    lastHour8 = EEPROM.read(1);
  }

  if (EEPROM.read(2) == 255) {
    lastHour9 = -1;
  } else {
    lastHour9 = EEPROM.read(2);
  }

  if (EEPROM.read(3) == 255) {
    lastHour10 = -1;
  } else {
    lastHour10 = EEPROM.read(3);
  }

  if (EEPROM.read(4) == 255) {
    lastHour15 = -1;
  } else {
    lastHour15 = EEPROM.read(4);
  }

  if (EEPROM.read(5) == 255) {
    lastHour16 = -1;
  } else {
    lastHour16 = EEPROM.read(5);
  }

  if (EEPROM.read(6) == 255) {
    lastHour17 = -1;
  } else {
    lastHour17 = EEPROM.read(6);
  }

  if (EEPROM.read(7) == 255) {
    lastHour18 = -1;
  } else {
    lastHour18 = EEPROM.read(7);
  }
  Blynk.virtualWrite(V7, lastHour7);
  Blynk.virtualWrite(V8, lastHour8);
  Blynk.virtualWrite(V9, lastHour9);
  Blynk.virtualWrite(V10, lastHour10);

  Blynk.virtualWrite(V15, lastHour15);
  Blynk.virtualWrite(V16, lastHour16);
  Blynk.virtualWrite(V17, lastHour17);
  Blynk.virtualWrite(V18, lastHour18);

  delay(100);
}
