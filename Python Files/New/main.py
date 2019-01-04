#!/usr/bin/env python3
from threading import Lock
from flask import Flask, render_template, session, request
from flask_socketio import SocketIO, emit, join_room, leave_room, \
	close_room, rooms, disconnect
import random
import sys
import glob
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
ints = {"12:3b:6a:1b:56:77": {"name": "Worker 0", 'place': None}}
ints["a4:c1:7a:57:1c:c1"] = {"name": "Worker 1", 'place': None}


@socketio.on('connect', namespace='/test')
def test_connect():
	global thread
	with thread_lock:
		if thread is None:
			thread = socketio.start_background_task(target=background_thread)


@app.route('/extra/<line>')
def datain(line):
	macs = line.split("!")[1].split(";")[:-1]
	place = places[line.split("!")[0]]
	print("Macs: " + str(macs))
	try:
		for mac in macs:
			mad = str(mac.split("=")[0])
			rrr = int(mac.split("=")[1])
			print("Checking for mac " + str(mad) + " with " + str(rrr))
			if rrr > -85:
				if mad in ints:
					ints[mad]['place'] = place
				else:
					ints[mad] = {"name": ("Worker " + str(len(ints))), 'place': place}
				print("Worker is " + str(ints[mad]))
	except Exception as e:
		print(e)
		print("Problem here")
	responce = {'k': "", 'h': "", 'o': "", "s": ""}
	for key in ints:
		responce["s"] += str(ints[key]["name"]) + "; "
		for key_r in responce:
			if key_r == ints[key]['place']:
				responce[key_r] += str(ints[key]['name']) + ";"
	socketio.emit('my_response',
				  responce,
				  namespace='/test')

@app.route('/')
def index():
	return render_template('index.html', async_mode=socketio.async_mode)



if __name__ == '__main__':
	socketio.run(app, debug=False, host='0.0.0.0', port=7777)
