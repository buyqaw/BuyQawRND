import serial
import glob
import sys

class Person:

	def __init__(self, ID, position = "employee",
				spot = "unknown", type="BLEcard", level=-80):
		self.MAC = ID
		self.type = type
		self.spot = spot
		self.position = position
		self.level = level

	def check(self, MAC):
		if str(MAC).lower() == str(self.MAC).lower():
			return True
		else:
			return False

	def locate(self, Spot):
		for el in Spot.MACs:
			if el == self.MAC:
				if Spot.RSSI(el) > self.level:
					self.spot = Spot.name


class Spot:

	def __init__(self, ID, name = "Noname", MACs = [], RSSIs = []):
		self.ID = ID
		self.name = name
		self.MACs = MACs
		self.RSSIs = RSSIs

	def RSSI(self, MAC):
		for i in range(len(self.MACs)):
			if str(self.MACs[i]).lower() == str(MAC).lower():
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
				IDs.append(line.split("!")[0])
				if(len(IDs) == 1):
					break
		self.IDs = IDs


	def update(self, spots):
		while True:
			if(self.serial.in_waiting > 0):
				lines = str(self.serial.readlines().decode("utf-8")).replace("\r", "").replace("\n", "")
				for line in lines:
					for el in spots:
						macNow = line.split("!")[0]
						if macNow ==




def main():
	HMI = Interface()
	mac = "12:3b:6a:1b:56:77"
	for el in HMI.IDs:
		if el == "2383295673":
			kitchen = Spot(el, "Кухня", [mac])
	# HMI.update()


main()
