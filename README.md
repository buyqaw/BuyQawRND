# bayqaw
Project "Bayqaw"

Installation  
You need to install ESP32 library from git: https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/mac.md
  
Then you need to install usb to UART driver  
https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers


## BLE-Mesh network
BLE-Mesh network is the special form of connection type where all elements of the network are nodes of the connection. 
Normally in the specification of the BLE-Mesh network is written that the devices that works in the mesh network have to be dual connection devices. 
By the other words, they got to have two BLE modules. In our case, we need to decrease cost of the project to the global minimum. 
As a result, we are introducing new protocol called "BPro" that was developed to satisfy limitations and purposes of the Technical Task 
of this research and development project.
### Protocol "BPro"
#### Overall
BPro protocol has several differences from standard BLE-Mesh network protocol:
* It uses only one BLE module to communicate.
* It is slower than standard protocol.
* It can handle more devices than standard protocol.  

These characteristics of the BLE-Mesh network are archived by next architectural differences:
* BPro protocol uses each device as BLE-server and BLE-client to IO data.
* BPro protocol uses special identification algorithm based on unique service UUID.
* Because of asynchronous nature and strict UUID based identification it can handle huge number of devices (theoretically 16^8 devices).

#### Identification
The identification number of the device is stored in the service UUID of each module. 
Normally BLE device has service UUID as next:
```
4fafc201-1fb5-459e-8fcc-c5c9c331914b
```
These groups of bytes are used to encapsulate data as in our case. But we use it to identify priority of the devices.
1. Highest priority: 1XXXXXXX - Device directly connected to the Wi-Fi.
2. Lowest priority: XXXXXXX1 - Device connected to the 7th device counting from the device with highest priority.

For example, there is pseudocode of the priority generation:
``` Python
is_wifi_connected = try_to_connect(ESSID, passwd)
if is_wifi_connected == TRUE:
  UUID = "1".join(["%s" % randint(0, 9) for num in range(0, 7)])
else:
  highes_priority_neighbour = scan_for_other_devices(find_the_highest_priority_of_neighbours)
  UUID = create_UUID_with_less_priority(highes_priority_neighbour)
return UUID
```

By simple words:
1. If device is connected to Wi-Fi -> it has highes priority
2. If device is not connected -> it searches neighbour with highes priority
3. Then it creates UUID with priority 1 byte less than the priority of the highest priority neighbour
4. It means that the priority level is the distance of device to the Wi-Fi hotspot

#### Algorithm
The next list is the steps of the connection protocol of the BLE-Mesh network of the "Bayqaw" project. 
1. Device is turned on.
2. It starts to scan for wi-fi -> if Wi-Fi is reachable go to point 5.
3. It scans neighbours with highes priority.
4. Device connects to neighbour with highest priority (parent neighbour) and creates new UUID.
5. Device does job (reads sensor values or whatever) and takes information from daughter devices - 2 minutes.
6. Device turns to client mode and starts to scan parent neighbour.
7. Device sends all data to parent neighbour, which is in server mode OR to tcp server if there is Wi-Fi.
8. Device goes to 5.
