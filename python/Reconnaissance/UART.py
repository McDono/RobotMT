
#Exemple


import serial

#Configuration du port série :


S = serial.Serial("/dev/ttyS0",9600)
data='VtsM 50 50'
S.write(data.encode('ascii'))
receive=S.readline(len(data1))
print(receive.decode("utf-8"))
sleep(1)
data='Stop'
S.write(data.encode('ascii'))
receive=S.readline(len(data))
print(receive.decode("utf-8"))


S.close()