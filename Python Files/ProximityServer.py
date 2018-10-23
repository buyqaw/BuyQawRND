#!/usr/bin/env python3
# -*- coding: utf-8 -*-

__author__ = "Bauyrzhan Ospan"
__copyright__ = "Copyright 2018, KazPostBot"
__version__ = "1.0.1"
__maintainer__ = "Bauyrzhan Ospan"
__email__ = "bospan@cleverest.tech"
__status__ = "Development"

from flask import Flask, render_template, request, Markup, jsonify
import time
import datetime, socket
from gevent import pywsgi
from geventwebsocket.handler import WebSocketHandler

app = Flask(__name__)  # Creating new flask app
# manager_main = -100
worker_main = -100
# manager_extra = -100
worker_extra = -100
moving = 0
manager_past = 3
worker_past = 3

# Login page, no authorisation with password
@app.route("/main/<data>")
def main(data):
    # global manager_main
    global worker_main


    data = data.split(";")[:-1]

    adr = []
    sig = []

    for el in data:
        adr.append(str(el).split("M:")[1].split("S:")[0])
        sig.append(int(str(el).split("M:")[1].split("S:")[1]))

    for el in range(len(adr)):
        # if adr[el] == "a4:c1:7a:57:1c:c1":
        #     manager_main = sig[el]
        if adr[el] == "12:3b:6a:1b:56:77":
            worker_main = sig[el]

    return "OK"


@app.route("/extra/<data>")
def extra(data):
    # global manager_extra
    global worker_extra

    data = data.split(";")[:-1]

    adr = []
    sig = []

    for el in data:
        adr.append(str(el).split("M:")[1].split("S:")[0])
        sig.append(int(str(el).split("M:")[1].split("S:")[1]))


    for el in range(len(adr)):
        # if adr[el] == "a4:c1:7a:57:1c:c1":
        #     manager_extra = sig[el]
        if adr[el] == "12:3b:6a:1b:56:77":
            worker_extra = sig[el]

    return "OK"


@app.route("/report/")
def dash():
    global manager_main
    global worker_main
    global manager_extra
    global worker_extra
    global moving
    global in_main
    global in_extra
    global manager_past
    global worker_past


    in_main2 = ""
    in_extra2 = ""
    in_main_num2 = 0

    #
    # if manager_main > manager_extra:
    #     manager = "Arman Kaisarov [manager]"
    #     manager_now = 0
    #     if manager_now != manager_past:
    #         moving = moving + 1
    #         manager_past = manager_now
    #         in_main2 = in_main2 + str(manager) + "<br>"
    #         in_main_num2 = in_main_num2 + 1
    #     else:
    #         manager_past = manager_now
    #         in_main2 = in_main2 + str(manager) + "<br>"
    #         in_main_num2 = in_main_num2 + 1
    # else:
    #     manager = "Arman Kaisarov [manager]"
    #     manager_now = 1
    #     if manager_now != manager_past:
    #         moving = moving + 1
    #         manager_past = manager_now
    #         in_extra2 = in_extra2 + str(manager) + "<br>"
    #     else:
    #         manager_past = manager_now
    #         in_extra2 = in_extra2 + str(manager) + "<br>"

    worker = "Aizat Armanov [worker]"
    if worker_main > worker_extra:

        worker_now = 0
        if worker_now != worker_past:
            moving = moving + 1
            worker_past = worker_now
            in_main2 = in_main2 + str(worker) + "<br>"
            in_main_num2 = in_main_num2 + 1
        else:
            worker_past = worker_now
            in_main2 = in_main2 + str(worker) + "<br>"
            in_main_num2 = in_main_num2 + 1
    elif worker_main < worker_extra:
        worker = "Aizat Armanov [worker]"
        worker_now = 1
        if worker_now != worker_past:
            moving = moving + 1
            worker_past = worker_now
            in_extra2 = in_extra2 + str(worker) + "<br>"
        else:
            worker_past = worker_now
            in_extra2 = in_extra2 + str(worker) + "<br>"
    else:
        if worker_past == 0:
            in_main2 = in_main2 + str(worker) + "<br>"
            in_main_num2 = in_main_num2 + 1
        else:
            in_extra2 = in_extra2 + str(worker) + "<br>"

    # manager_main = -100
    # manager_extra = -100
    worker_main = -100
    worker_extra = -100

    in_main2 = Markup(in_main2)
    in_extra2 = Markup(in_extra2)
    move = moving
    return render_template(
        "main.html", **locals())


# Main flask app
if __name__ == "__main__":
    #app.run(host="0.0.0.0", port=7777, threaded == True)
    server = pywsgi.WSGIServer(('0.0.0.0', 7777), app, handler_class=WebSocketHandler)
    server.serve_forever()
