# RFID-Based-Attendance-System-Time-Based-Intelligence
A smart attendance management system that uses RFID cards to automatically record attendance with timestamps. The system analyzes attendance data to track punctuality, working hours, and attendance trends, providing efficient and accurate monitoring for schools, colleges, and workplaces.
# RFID-Based Attendance System with Time-Based Intelligence

## Overview

The RFID-Based Attendance System with Time-Based Intelligence is an embedded systems project designed to automate attendance tracking using RFID technology and real-time data processing.

The system identifies users through RFID cards, records attendance with accurate timestamps provided by an RTC module, and classifies attendance status as **Present**, **Late**, or **Absent** based on predefined time rules. Attendance records are transferred using Python and stored in an Excel spreadsheet for efficient management and analysis.

This project demonstrates the integration of hardware and software components to create a reliable and intelligent attendance management solution.

---

## Features

* Contactless attendance marking using RFID cards/tags
* Real-time attendance logging with date and time
* Time-based attendance classification

  * Present
  * Late
  * Absent
* Duplicate attendance prevention
* LCD-based user feedback
* EEPROM-based attendance storage
* Serial communication with Python
* Automatic attendance report generation in Excel
* Simulation support using Wokwi

---

## System Working

The block diagram represents the working of an RFID-based attendance system.

When an RFID card is brought near the MFRC522 RFID reader, the reader detects the unique ID (UID) of the card and sends the information to the Arduino Uno through SPI communication.

The Arduino Uno acts as the central controller and verifies the scanned UID against the registered records stored in the system.

Simultaneously, the RTC module provides accurate date and time information through I²C communication. Based on the current time, the Arduino determines the attendance status of the user.

The attendance status and timestamp are displayed on the 16×2 LCD display. The attendance record is then transmitted through serial communication to a Python application, which stores the data in an Excel spreadsheet.

This process ensures accurate, real-time, and automated attendance recording.

---

## Hardware Components

| Component                  | Purpose                       |
| -------------------------- | ----------------------------- |
| Arduino Uno                | Main controller               |
| MFRC522 RFID Reader        | Reads RFID cards/tags         |
| RFID Cards/Tags            | User identification           |
| RTC Module (DS1307/DS3231) | Real-time date and time       |
| 16×2 LCD Display           | User interface                |
| EEPROM                     | Attendance data storage       |
| Jumper Wires               | Hardware connections          |
| USB Cable                  | Programming and communication |

---

## Software & Tools

* Arduino IDE
* Python
* Wokwi Simulator
* Microsoft Excel

---


## Arduino Source Code

```
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
```
The Arduino program is responsible for:

* RFID card detection
* UID verification
* Attendance classification
* RTC communication
* EEPROM storage
* LCD display updates
* Serial communication with Python

---

## Python Source Code
```
import serial
from openpyxl import Workbook

ser = serial.Serial('COM3', 9600)

wb = Workbook()
ws = wb.active

ws.append(["Name", "Status", "Date"])

print("Waiting for data...")

while True:
    data = ser.readline().decode().strip()
    print("Received:", data)

    if data == "START":
        continue

    elif data == "END":
        wb.save("attendance.xlsx")
        print("Saved to Excel")
        break

    else:
        parts = data.split(",")

        if len(parts) == 3:
            name = parts[0]
            status = parts[1]
            date = parts[2]

            ws.append([name, status, date])
```
The Python application:

* Receives attendance data from Arduino
* Processes attendance records
* Generates attendance logs
* Stores attendance information in Excel format

---

## Simulation Files

wokwi link

```
https://wokwi.com/projects/458891527160648705
```

The Wokwi simulation includes:

* Arduino Uno
* MFRC522 RFID Reader
* RTC Module
* LCD Display
* RFID Card Simulation

---

## Circuit Diagram

Location:

```text
Circuit_Diagram/circuit_diagram.png
```

The circuit diagram illustrates the connections between:

* Arduino Uno
* MFRC522 RFID Reader
* RTC Module
* 16×2 LCD Display
* Power Supply

---

## Attendance Classification Logic

| Condition                  | Status  |
| -------------------------- | ------- |
| Within attendance time     | Present |
| After allowed time limit   | Late    |
| Attendance window exceeded | Absent  |

---

## Duplicate Attendance Prevention

To prevent multiple attendance entries, the system stores attendance information in EEPROM.

When a user scans an RFID card more than once on the same day, the LCD displays:

```text
Already Marked
```

and the attendance record is not duplicated.

---

## Simulated Output

Location:

```text
Simulated_Output/
```

### Startup Screen

```text
RFID Attendance
Scan Card...
```

### Present Status

```text
Ramesh
PRESENT 09:05
```

### Late Status

```text
Rose
LATE 09:21
```

### Duplicate Scan Detection

```text
Jack
Already Marked
```

### Excel Output

Attendance records generated during simulation.

---

## Real Hardware Output

Location:

```text
Real_Output/
```

Include:

* Hardware setup photograph
* RFID card scanning process
* LCD attendance display
* Excel attendance records generated from real hardware testing

---

## Demonstration

### Wokwi Simulation

The complete project was simulated and verified using Wokwi before hardware implementation.

### Hardware Testing

Successfully tested using:

* Arduino Uno
* MFRC522 RFID Reader
* RTC Module
* 16×2 LCD Display
* RFID Tags
* Python-based Excel Logging

---

## Applications

* Educational Institutions
* Colleges and Universities
* Offices and Workplaces
* Laboratories
* Training Centers
* Employee Attendance Management Systems

---

## Future Enhancements

* Cloud-based attendance storage
* IoT integration for remote monitoring
* Web dashboard for attendance analytics
* Email/SMS notifications
* Fingerprint authentication
* Face recognition integration

---

## Author

Developed as an Embedded Systems and IoT project demonstrating RFID technology, real-time attendance tracking, EEPROM data management, serial communication, and intelligent attendance classification using Arduino and Python.
