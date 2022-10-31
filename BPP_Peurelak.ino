/*
  Created: Ahlillah, 23 Oktober 2022.
*/

//penyimpanan di esp32
#include <EEPROM.h>

//#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLB56ONdF2"
#define BLYNK_DEVICE_NAME "IoT Distanbun Aceh"
#define BLYNK_AUTH_TOKEN "OktPUInM8kjJmxzcCJo33Hrb7xUr0Kwo"

char auth[] = BLYNK_AUTH_TOKEN;

//wifi dan password
char ssid[] = "PUSMEKTAN-USK";
char pass[] = "darussalam";

//pin pompa
int pinPump = 4;

//pin temperature ds18b20
int pinTemp = 32;

//pin soil
int pinHumidity = 33;

//pin ph from nano
int pinPh = 34;

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

//library komunikasi UART esp32 to nano
HardwareSerial pH(0);

//library rtc 3231 (sensor jam)
#include <DS3231.h>
DS3231 rtc;
RTCDateTime dt;

//library DS18b20 (sensor suhu)
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS pinTemp //sensor ds18b20 di pin 32
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensorSuhu(&oneWire);

//variabel manual
int isManual;

//variabel semi otomatis
int isSemiOtomatis;

//variabel otomatis
int isOtomatis;

//input kelembapan
int inputKelembapan;

//input time
int inputTime;

//variabel kelembapan
int soilMoistureValue;
int soilMoisturePercent;

//variabel suhu
double suhu;

//variabel pH
double dataPh;

//variabel waktu
int setTime[8] = { -1, -1, -1, -1, -1, -1, -1, -1};
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
  EEPROM.begin(120);
  Serial.begin(115200);
  pH.begin(115200, SERIAL_8N1, 34, 35);
  Blynk.begin(auth, ssid, pass);

  //start sensor rtc DS3231
  rtc.begin();
  rtc.setDateTime(2022, 9, 17, 6, 59, 40);

  pinMode(pinPump, OUTPUT);
  digitalWrite(pinPump, HIGH);
  sensorSuhu.begin();

  initButton();

}


void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Blynk.begin(auth, ssid, pass);
    delay(50);
  }

  Blynk.run();

  //sensor suhu
  suhu = getTemperature();

  //sensor pH
  dataPh = getPh();
  Serial.println(dataPh);

  //sensor kelembapan
  soilMoistureValue = analogRead(pinHumidity);
  soilMoisturePercent = map(soilMoistureValue, 3620, 1680, 0, 100);

  for (i = 0; i < 8; i++) {
    setTime[i] = EEPROM.read(i);
  }
  delay(100);

  EEPROM.commit();
  setFromHumidity();
  setFromTime();
  initButton();
  delay(100);
}

//pagi
BLYNK_WRITE(V7) {
  if (param.asInt() == 1) {
    EEPROM.write(0, 7);
    delay(80);
  } else {
    EEPROM.write(0, -1);
  }
}

BLYNK_WRITE(V8) {
  if (param.asInt() == 1) {
    EEPROM.write(1, 8);
    delay(80);
  } else {
    EEPROM.write(1, -1);
  }
}

BLYNK_WRITE(V9) {
  if (param.asInt() == 1) {
    EEPROM.write(2, 9);
    delay(80);
  } else {
    EEPROM.write(2, -1);
  }
}

BLYNK_WRITE(V10) {
  if (param.asInt() == 1) {
    EEPROM.write(3, 10);
    delay(80);
  } else {
    EEPROM.write(3, -1);
  }
}

//sore
BLYNK_WRITE(V15) {
  if (param.asInt() == 1) {
    EEPROM.write(4, 15);
    delay(80);
  } else {
    EEPROM.write(4, -1);
  }
}

BLYNK_WRITE(V16) {
  if (param.asInt() == 1) {
    EEPROM.write(5, 16);
    delay(80);
  } else {
    EEPROM.write(5, -1);
  }
}

BLYNK_WRITE(V17) {
  if (param.asInt() == 1) {
    EEPROM.write(6, 17);
    delay(80);
  } else {
    EEPROM.write(6, -1);
  }
}

BLYNK_WRITE(V18) {
  if (param.asInt() == 1) {
    EEPROM.write(7, 18);
    delay(80);
  } else {
    EEPROM.write(7, -1);
  }
}

//pin on off pompa
BLYNK_WRITE(V30) {
  digitalWrite(pinPump, HIGH);
  isManual = param.asInt();
  if (isManual == 1) {
    isOtomatis = 0;
    isSemiOtomatis = 0;
    EEPROM.write(31, isSemiOtomatis);
    EEPROM.write(32, isOtomatis);
    digitalWrite(pinPump, LOW);
  } else {
    digitalWrite(pinPump, HIGH);
  }
  EEPROM.write(30, isManual);
}

//pin semi otomatis
BLYNK_WRITE(V31) {
  isSemiOtomatis = param.asInt();
  digitalWrite(pinPump, HIGH);
  if (isSemiOtomatis == 1) {
    isOtomatis = 0;
    isManual = 0;
    EEPROM.write(30, isManual);
    EEPROM.write(32, isOtomatis);
  }
  EEPROM.write(31, isSemiOtomatis);
}

//pin otomatis
BLYNK_WRITE(V32) {
  isOtomatis = param.asInt();
  digitalWrite(pinPump, HIGH);
  if (isOtomatis == 1) {
    isSemiOtomatis = 0;
    isManual = 0;
    EEPROM.write(30, isManual);
    EEPROM.write(31, isSemiOtomatis);
  }
  EEPROM.write(32, isOtomatis);
}

//inputKelembapan
BLYNK_WRITE(V101) {
  inputKelembapan = param.asInt();
  EEPROM.write(101, inputKelembapan);
  Serial.print("kelembapan: ");
  Serial.println(EEPROM.read(101));
}

//inputTime
BLYNK_WRITE(V102) {
  inputTime = param.asInt();
  EEPROM.write(102, inputTime);
  Serial.print("Waktu: ");
  Serial.println(EEPROM.read(102));
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

  //otomatisasi
  if (EEPROM.read(30) == 1) {
    isManual = 1;
  } else {
    isManual = 0;
  }

  if (EEPROM.read(31) == 1) {
    isSemiOtomatis = 1;
  } else {
    isSemiOtomatis = 0;
  }

  if (EEPROM.read(32) == 1) {
    isOtomatis = 1;
  } else {
    isOtomatis = 0;
  }

  inputKelembapan = EEPROM.read(101);
  inputTime = EEPROM.read(102);

  Blynk.virtualWrite(V1, suhu);
  Blynk.virtualWrite(V2, soilMoisturePercent);
  if (dataPh > 0.0) Blynk.virtualWrite(V3, dataPh);

  Blynk.virtualWrite(V7, lastHour7);
  Blynk.virtualWrite(V8, lastHour8);
  Blynk.virtualWrite(V9, lastHour9);
  Blynk.virtualWrite(V10, lastHour10);

  Blynk.virtualWrite(V15, lastHour15);
  Blynk.virtualWrite(V16, lastHour16);
  Blynk.virtualWrite(V17, lastHour17);
  Blynk.virtualWrite(V18, lastHour18);

  Blynk.virtualWrite(V30, isManual);
  Blynk.virtualWrite(V31, isSemiOtomatis);
  Blynk.virtualWrite(V32, isOtomatis);

  Blynk.virtualWrite(V101, inputKelembapan);
  Blynk.virtualWrite(V102, inputTime);

  delay(100);
}

void setFromHumidity() {
  
  if (isOtomatis == 1) {
    digitalWrite(pinPump, HIGH);
    if (soilMoisturePercent < inputKelembapan) {
      isManual = 0;
      digitalWrite(pinPump, LOW);
//      delay(10000);
    } else {
      isManual = 1;
      digitalWrite(pinPump, HIGH);
    }
    EEPROM.write(30, isManual);
  }
}

void setFromTime() {

  //get time
  dt = rtc.getDateTime();

  if (isSemiOtomatis == 1) {
    digitalWrite(pinPump, HIGH);
    for (i = 0; i < 8; i++) {
      if ((setTime[i] - 1) == (dt.hour) && dt.minute == 59 && dt.second >= 56) {
        isManual = 1;
        EEPROM.write(30, isManual);
        digitalWrite(pinPump, LOW);
        timeOn = EEPROM.read(102);
        Serial.println(timeOn);
        int mTimeOn = timeOn * 60000;
        delay(mTimeOn);
        isManual = 0;
        EEPROM.write(30, isManual);
        digitalWrite(pinPump, HIGH);
      } else {
        digitalWrite(pinPump, HIGH);
      }
      delay(10);
    }
  }
}

float getTemperature()
{
  sensorSuhu.requestTemperatures();
  float suhu = sensorSuhu.getTempCByIndex(0);
  return suhu;
}

double getPh()
{
  String s = "";
  while (pH.available()) {
    s += char(pH.read());
  }
  delay(10);
  
    double buff_data;
    char buf[s.length()];
    s.toCharArray(buf, s.length() + 1);
    buff_data = atof(buf);

    return buff_data;
}
