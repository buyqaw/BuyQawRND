import serial
import glob
import sys

class Person:

	def __init__(self, ID, position = "employee",
				spot = "unknown", type="BLEcard"):
		self.UUID = ID
		self.type = type
		self.spot = spot
		self.position = position

	def check(self, UUID):
		if str(UUID).lower() == str(self.UUID).lower():
			return True
		else:
			return False

	def locate(self, Spot):
		for el in Spot.UUIDs:
			if el == self.UUID:
				if Spot.RSSI(el) > -80:
					self.spot = Spot.name


class Spot:

	def __init__(self, ID, name = "Noname", UUIDs = [], RSSIs = []):
		self.ID = ID
		self.name = name
		self.UUIDs = UUIDs
		self.RSSIs = RSSIs

	def RSSI(self, UUID):
		for i in range(len(self.UUIDs)):
			if str(self.UUIDs[i]).lower() == str(UUID).lower():
				return self.RSSIs[i]
		return None


class Interface:

	def __init__(self, baud = 115200):
		if sys.platform.startswith('win'):
			ports = ['COM%s' % (i + 1) for i in range(256)]
		elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
			# this excludes your current terminal "/dev/tty"
			ports = glob.glob('/dev/ttyACM*')
			print(ports)
		elif sys.platform.startswith('darwin'):
			ports = glob.glob('/dev/tty.SLAB_USBtoUART*')
		else:
			raise EnvironmentError('Unsupported platform')
		self.serial = serial.Serial(ports[0], baud)
		IDs = []
		while True:
			if(self.serial.in_waiting > 0):
				line = str(self.serial.readline().decode("utf-8")).replace("\r", "").replace("\n", "")
				if line[0] == "!":
					IDs.append(line.split("!ID: ")[1].split("- ")[0])
					print(IDs)
					break


	def update(self):
		while True:
			if(self.serial.in_waiting > 0):
				line = str(self.serial.readline().decode("utf-8")).replace("\r", "").replace("\n", "")
				if line[0] == "!":
					print("sexy")




def main():
	HMI = Interface()
	# HMI.update()


main()
