#!/usr/bin/env python3
from threading import Lock
from flask import Flask, render_template, session, request
from flask_socketio import SocketIO, emit, join_room, leave_room, \
	close_room, rooms, disconnect
import random
import sys
import glob
import time
import serial


async_mode = None

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app, async_mode=async_mode)
thread = None
thread_lock = Lock()
interest = "12:3b:6a:1b:56:77"
places = {'2383295673': "k", '981643341': "h", '2385238209': "o"}
IDs = ['2383295673', '981643341', '2385238209']


def action(text, oldtext):
	head = '<tr><td><i class="fa fa-user w3-text-blue w3-large"></i></td><td>'
	text = text
	body = '</td><td><i>'
	time1 = time.strftime('%d %b %Y %H:%M:%S', time.localtime())
	tail = "</i></td></tr>"
	all = head + text + body + time1 + tail
	texts = oldtext.split("</tr>")
	if len(texts)>20:
		for i in range(len(texts) - 1):
			all += str(texts[i] + "</tr>")
	else:
		all += oldtext

	return all


def init():
	if sys.platform.startswith('win'):
		ports = ['COM%s' % (i + 1) for i in range(256)]
	elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
		# this excludes your current terminal "/dev/tty"
		ports = glob.glob('/dev/ttyUSB*')
		print(ports)
	elif sys.platform.startswith('darwin'):
		ports = glob.glob('/dev/tty.SLAB_USBtoUART*')
	else:
		raise EnvironmentError('Unsupported platform')
	ser = serial.Serial(ports[0], 115200)
	return ser

ser = init()

# То что крутиться на заднем фоне
def background_thread():
	print("Background thread is going")
    headold = '<td><i class="fa fa-share-alt w3-text-green w3-large"></i></td><td>Mesh network reconfigured.</td><td><i>'
	tailold = '</i></td>'
	time4 = time.strftime('%d %b %Y %H:%M:%S', time.localtime())
	oldtext = headold + time4 + tailold
	count = 0
	RSSI = {}
	oldplace = "h"
	responce = {'k': "0", 'h': "0", 'o': "0", "s": "0", "t": ""}
	ser.flush()
	while True:
		try:
			socketio.sleep(1)
			count += 1
			if count == 10:
				RSSI = {}
			if(ser.in_waiting > 0):
				lineraw = ser.readline()
				print(lineraw)
				line = str(lineraw.decode("utf-8")).replace("\r", "").replace("\n", "")
				print(line)
				b = False
				for el in IDs:
					if line.split("!")[0] == el:
						print(line.split("!")[0])
						b = True
						break
				if b == False:
					print("Error in ESP32")
				else:
					macs = line.split("!")[1].split(";")
					print("Macs: " + str(macs))
					try:
						a = 0
						for mac in macs:
							print("Check it")
							print(str(mac.split("=")[0]))
							print(interest)
							if str(mac.split("=")[0]) == str(interest):
								RSSI[line.split("!")[0]] = int(mac.split("=")[1])
								a = 1
						if a == 0:
							RSSI[line.split("!")[0]] = -200
					except:
						print("Prob here")
			print(RSSI)
			try:
				place = max(RSSI, key=RSSI.get)
				print(place)
				if RSSI[place] > -200 and place!=oldplace:
					oldplace = place
					for el in IDs:
						responce[places[el]] = "0"
						responce["s"] = "0"
					responce[places[place]] = "1"
					text = "Worker entered: " + str(place)
					oldtext = action(text, oldtext)
					responce["t"] = str(oldtext)
					#
					socketio.emit('my_response',
								  responce,
								  namespace='/test')
				if count >= 100000 and place==oldplace or RSSI[place] == -200:
					for el in IDs:
						responce[places[el]] = "0"
					responce["s"] = "1"
					oldplace = ""
					count = 0
					#
					socketio.emit('my_response',
								  responce,
								  namespace='/test')
			except:
				for el in IDs:
					responce[places[el]] = "0"
				responce["s"] = "1"
				oldplace = ""
				count = 0
				#
				socketio.emit('my_response',
							  responce,
							  namespace='/test')
		except:
			ser.flush()
			print("SHIT!")



@socketio.on('connect', namespace='/test')
def test_connect():
	global thread
	with thread_lock:
		if thread is None:
			thread = socketio.start_background_task(target=background_thread)


@app.route('/')
def index():
	return render_template('home.html', async_mode=socketio.async_mode)



if __name__ == '__main__':
	socketio.run(app, debug=True, host='0.0.0.0', port=7777)
