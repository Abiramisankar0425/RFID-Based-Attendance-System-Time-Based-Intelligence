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
