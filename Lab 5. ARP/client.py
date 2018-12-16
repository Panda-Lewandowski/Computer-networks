import socket
import sys

host = 'localhost'
port = 8800
addr = (host, port)

udp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
udp_socket.connect(addr)

data = input('Write to server: ')
if not data :
    udp_socket.close()
    sys.exit(1)

# encode - recodes the entered data in bytes,
# decode - back
data = str.encode(data)
udp_socket.send(data)
data = bytes.decode(data)

data = udp_socket.recv(1024)
data = data.decode("ascii")

print(data)

udp_socket.close()
