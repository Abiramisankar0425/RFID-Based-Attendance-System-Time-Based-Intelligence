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

## Repository Structure

```text
RFID-Based-Attendance-System/
│
├── Arduino_Code/
│   └── RFID_Attendance_System.ino
│
├── Python_Code/
│   └── attendance_logger.py
│
├── Simulation/
│   ├── wokwi-project.json
│   └── diagram.json
│
├── Circuit_Diagram/
│   └── circuit_diagram.png
│
├── Simulated_Output/
│   ├── startup_screen.png
│   ├── present_status.png
│   ├── late_status.png
│   ├── duplicate_scan.png
│   └── excel_output_simulation.png
│
├── Real_Output/
│   ├── hardware_setup.jpg
│   ├── attendance_present.jpg
│   ├── attendance_late.jpg
│   ├── attendance_duplicate.jpg
│   └── excel_output_real.png
│
└── README.md
```

---

## Arduino Source Code

Location:

```text
Arduino_Code/Arduino_Code.ino
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

Location:

```text
Python_Code/python_code.py
```

The Python application:

* Receives attendance data from Arduino
* Processes attendance records
* Generates attendance logs
* Stores attendance information in Excel format

---

## Simulation Files

Location:

```text
Simulation/
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
