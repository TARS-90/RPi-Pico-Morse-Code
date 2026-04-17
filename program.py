import serial

s = serial.Serial('/dev/serial0', baudrate=115200, timeout=.5)

buffor = input("Seq: ")
for word in buffor.split():
    word += '\0'
    s.write(word.encode())

s.close()
