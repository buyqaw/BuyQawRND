from flask import Flask, abort, request
import json

app = Flask(__name__)


@app.route('/', methods=['POST'])
def foo():
    print(request.get_data())
    if(request.get_data() == b''):
        requests = ['AAS54']
    else:
        requests = ['None']
    print("Our answer is: " + requests[0])
    return requests[0]


if __name__ == '__main__':
    app.run(host='192.168.1.109', port=80, debug=True)
