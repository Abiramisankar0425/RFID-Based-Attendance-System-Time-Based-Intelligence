#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <RTClib.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
RTC_DS1307 rtc;

// LCD (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

// ---------------- UID ----------------
byte rameshUID[4] = {0xD3, 0x88, 0xD8, 0x2C};
byte roseUID[4]   = {0x01, 0x01, 0x5D, 0x61};
byte jackUID[4]   = {0x40, 0x7A, 0xA4, 0x61};

// ---------------- EEPROM ----------------
#define DAY_ADDR     0
#define MONTH_ADDR   1
#define YEAR_ADDR    2

#define RAMESH_ADDR  3
#define ROSE_ADDR    4
#define JACK_ADDR    5

bool rameshScanned;
bool roseScanned;
bool jackScanned;

// ---------------- SETUP ----------------
void setup() {

  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  rfid.PCD_AntennaOn();   // 🔥 important
  Wire.begin();

  lcd.begin(16, 2);
  lcd.print("RFID Attendance");
  delay(2000);
  lcd.clear();

  if (!rtc.begin()) {
    lcd.print("RTC ERROR");
    while (1);
  }

  // ⚠️ RUN ONLY ONCE, then comment this line
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  DateTime now = rtc.now();

  // -------- RESET DAILY --------
  if (EEPROM.read(DAY_ADDR) != now.day() ||
      EEPROM.read(MONTH_ADDR) != now.month() ||
      EEPROM.read(YEAR_ADDR) != (now.year() % 100)) {

    EEPROM.write(DAY_ADDR, now.day());
    EEPROM.write(MONTH_ADDR, now.month());
    EEPROM.write(YEAR_ADDR, now.year() % 100);

    EEPROM.write(RAMESH_ADDR, 0);
    EEPROM.write(ROSE_ADDR, 0);
    EEPROM.write(JACK_ADDR, 0);
  }

  rameshScanned = EEPROM.read(RAMESH_ADDR) != 0;
  roseScanned   = EEPROM.read(ROSE_ADDR) != 0;
  jackScanned   = EEPROM.read(JACK_ADDR) != 0;

  lcd.print("Scan Card...");
}

// ---------------- LOOP ----------------
void loop() {

  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  DateTime now = rtc.now();
  int hour = now.hour();
  int minute = now.minute();

  lcd.clear();

  // ---------------- RAMESH ----------------
  if (checkUID(rameshUID)) {

    lcd.print("Ramesh");
    lcd.setCursor(0, 1);

    if (rameshScanned) {
      lcd.print("Already Marked");
    } else {
      int statusCode = getStatusCode(hour, minute);
      displayStatus(statusCode, hour, minute);

      EEPROM.write(RAMESH_ADDR, statusCode);
      rameshScanned = true;
    }
  }

  // ---------------- ROSE ----------------
  else if (checkUID(roseUID)) {

    lcd.print("Rose");
    lcd.setCursor(0, 1);

    if (roseScanned) {
      lcd.print("Already Marked");
    } else {
      int statusCode = getStatusCode(hour, minute);
      displayStatus(statusCode, hour, minute);

      EEPROM.write(ROSE_ADDR, statusCode);
      roseScanned = true;
    }
  }

  // ---------------- JACK ----------------
  else if (checkUID(jackUID)) {

    lcd.print("Jack");
    lcd.setCursor(0, 1);

    if (jackScanned) {
      lcd.print("Already Marked");
    } else {
      int statusCode = getStatusCode(hour, minute);
      displayStatus(statusCode, hour, minute);

      EEPROM.write(JACK_ADDR, statusCode);
      jackScanned = true;
    }
  }

  // ---------------- UNKNOWN CARD → EXPORT ----------------
  else {
    lcd.print("Sending Data...");
    sendStoredData();
  }

  delay(3000);

  lcd.clear();
  lcd.print("Scan Card...");

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

// ---------------- STATUS CODE ----------------
int getStatusCode(int hour, int minute) {

  if (hour < 1 || (hour == 1 && minute <= 19)) {
    return 1; // PRESENT
  }
  else if (hour == 1 && minute <= 20) {
    return 2; // LATE
  }
  else {
    return 3; // ABSENT
  }
}

// ---------------- DISPLAY STATUS ----------------
void displayStatus(int code, int hour, int minute) {

  if (code == 1) lcd.print("PRESENT ");
  else if (code == 2) lcd.print("LATE ");
  else if (code == 3) lcd.print("ABSENT ");

  lcd.print(hour);
  lcd.print(":");
  if (minute < 10) lcd.print("0");
  lcd.print(minute);
}

// ---------------- UID CHECK ----------------
bool checkUID(byte *card) {
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != card[i])
      return false;
  }
  return true;
}

// ---------------- SEND STORED DATA ----------------
void sendStoredData() {

  Serial.println("START");

  int day = EEPROM.read(DAY_ADDR);
  int month = EEPROM.read(MONTH_ADDR);
  int year = EEPROM.read(YEAR_ADDR);

  printStudent("Ramesh", EEPROM.read(RAMESH_ADDR), day, month, year);
  printStudent("Rose",   EEPROM.read(ROSE_ADDR),   day, month, year);
  printStudent("Jack",   EEPROM.read(JACK_ADDR),   day, month, year);

  Serial.println("END");
}

// ---------------- PRINT FUNCTION ----------------
void printStudent(String name, int status, int day, int month, int year) {

  Serial.print(name);
  Serial.print(",");

  if (status == 1) Serial.print("PRESENT");
  else if (status == 2) Serial.print("LATE");
  else if (status == 3) Serial.print("ABSENT");
  else Serial.print("NO DATA");

  Serial.print(",");
  Serial.print(day);
  Serial.print("/");
  Serial.print(month);
  Serial.print("/20");
  Serial.println(year);
}
