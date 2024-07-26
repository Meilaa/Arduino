import serial
import csv
import math

EEPROM_CAPACITY = 1024 
DATA_SIZE = 12  


ser = serial.Serial('COM8', 9600)  

# Open a CSV file for writing
with open('data.csv', 'w', newline='') as csvfile:
    csv_writer = csv.writer(csvfile)
    csv_writer.writerow(['Timestamp', 'Humidity', 'Temperature'])  

    eeprom_entries = 0  

    while True:
        try:
            data = ser.readline().decode().strip()  # Read data from Arduino
            values = data.split(',')
            if len(values) == 3:
                timestamp, humidity, temperature = map(float, values)
                # Handle "NaN" humidity or temperature values
                if math.isnan(humidity):
                    humidity = None
                if math.isnan(temperature):
                    temperature = None
                csv_writer.writerow([timestamp, humidity, temperature])  # Write data to CSV

                eeprom_entries += 1

                if eeprom_entries * DATA_SIZE >= EEPROM_CAPACITY:
                    break
            else:
                print(f"Received unexpected data: {data}")
        except KeyboardInterrupt:
            break

ser.close()
print("Data saved to data.csv")