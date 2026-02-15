# WiFiNINA Library

## WiFi Class

### `WiFi.begin()`

#### Description
Initializes the WiFiNINA library's network settings and provides the current status.

#### Syntax

```
WiFi.begin(ssid);
WiFi.begin(ssid, pass);
WiFi.begin(ssid, keyIndex, key);
```

#### Parameters

- ssid: the SSID (Service Set Identifier) is the name of the WiFi network you want to connect to.
- keyIndex: WEP encrypted networks can hold up to 4 different keys. This identifies which key you are going to use.
- key: a hexadecimal string used as a security code for WEP encrypted networks.
- pass: WPA encrypted networks use a password in the form of a string for security.

#### Returns

- WL_CONNECTED when connected to a network
- WL_IDLE_STATUS when not connected to a network, but powered on

#### Example

```
#include <WiFiNINA.h>

//SSID of your network
char ssid[] = "yourNetwork";
//password of your WPA Network
char pass[] = "secretPassword";

void setup()
{
 WiFi.begin(ssid, pass);
}

void loop () {}
```

### `WiFi.end()`

#### Description
Turns off the WiFi module. If WiFi.begin() was used to connect to an access point, the connection will be disconnected.
If WiFi.beginAP() was used before to create an access point, the WiFi.end() will stop listening it too.

#### Syntax

```
WiFi.end();

```

#### Parameters

- None

#### Returns

- Nothing

### `WiFi.beginAP()`

#### Description

Initializes the WiFiNINA library in Access Point (AP) mode. Other WiFi devices will be able to discover and connect to the created Access Point.

#### Syntax

```
WiFi.beginAP(ssid);
WiFi.beginAP(ssid, channel);
WiFi.beginAP(ssid, passphrase);
WiFi.beginAP(ssid, passphrase, channel);
```

#### Parameters

- ssid: the SSID (Service Set Identifier) of the created Access Point. Must be 8 or more characters.
- passphrase: optional, the WPA password of the created Access Point. Must be 8 or more characters.
- channel: optional, channel of created Access Point (1 - 14). Defaults to channel 1;

#### Returns

- WL_AP_LISTENING when creating access point succeeds
- WL_CONNECT_FAILED when creating access point fails

#### Example

```
/*
  WiFi Web Server LED Blink

  A simple web server that lets you blink an LED via the web.
  This sketch will create a new access point (with no password).
  It will then launch a new server and print out the IP address
  to the Serial Monitor. From there, you can open that address in a web browser
  to turn on and off the LED on pin 13.

  If the IP address of your board is yourAddress:
    http://yourAddress/H turns the LED on
    http://yourAddress/L turns it off

  created 25 Nov 2012
  by Tom Igoe
  adapted to WiFi AP by Adafruit
 */

#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key index number (needed only for WEP)

int led =  LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Access Point Web Server");

  pinMode(led, OUTPUT);      // set the LED pin mode

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // by default the local IP address will be 192.168.4.1
  // you can override it with the following:
  // WiFi.config(IPAddress(10, 0, 0, 1));

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  // wait 10 seconds for connection:
  delay(10000);

  // start the web server on port 80
  server.begin();

  // you're connected now, so print out the status
  printWiFiStatus();
}


void loop() {
  // compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      // a device has connected to the AP
      Serial.println("Device connected to AP");
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }
 
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      delayMicroseconds(10);                // This is required for the Arduino Nano RP2040 Connect - otherwise it will loop so fast that SPI will never be served.
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out to the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> turn the LED on<br>");
            client.print("Click <a href=\"/L\">here</a> turn the LED off<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(led, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(led, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);

}
```

### `WiFi.beginEnterprise()`

#### Description

Initializes the WiFiNINA library's network settings for a common WPA2 Enterprise network with username and password authentication (PEAP/MSCHAPv2).

Note: this feature requires NINA firmware version 1.3.0 or later. All string parameter supplied must have a combined length of under 4000 bytes.

#### Syntax

```
WiFi.beginEnterprise(ssid, username, password);
WiFi.beginEnterprise(ssid, username, password, identity);
WiFi.beginEnterprise(ssid, username, password, identity, ca);
```

#### Parameters

- ssid: the SSID (Service Set Identifier) is the name of the WiFi network you want to connect to.
- username: username part of WPA2 Enterprise (RADIUS) credentials
- password: password part of WPA2 Enterprise (RADIUS) credentials
- identity: WPA2 enterprise identity (optional)
- ca: root certificate (string) to validate against (optional)

#### Returns

- WL_CONNECTED when connected to a network
- WL_IDLE_STATUS when not connected to a network, but powered on

#### Example

```
/*
  This example connects to a WPA2 Enterprise WiFi network.
  Then it prints the MAC address of the WiFi module,
  the IP address obtained, and other network details.

  Based on ConnectWithWPA.ino by dlf (Metodo2 srl) and Tom Igoe
*/
#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;  // your WPA2 enterprise network SSID (name)
char user[] = SECRET_USER;  // your WPA2 enterprise username
char pass[] = SECRET_PASS;  // your WPA2 enterprise password
int status = WL_IDLE_STATUS;     // the WiFi radio's status

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA2 enterprise network:
    // - You can optionally provide additional identity and CA cert (string) parameters if your network requires them:
    //      WiFi.beginEnterprise(ssid, user, pass, identity, caCert)
    status = WiFi.beginEnterprise(ssid, user, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

}

void loop() {
  // check the network connection once every 10 seconds:
  delay(10000);
  printCurrentNet();
}

void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
```

### `WiFi.disconnect()`

#### Description

Disconnects the WiFi from the current network.

#### Syntax

```
WiFi.disconnect()
```

#### Parameters

- None

#### Returns

- Nothing

### `WiFi.config()`

#### Description

WiFi.config() allows you to configure a static IP address as well as change the DNS, gateway, and subnet addresses on the WiFi shield.

Unlike WiFi.begin() which automatically configures the WiFi shield to use DHCP, WiFi.config() allows you to manually set the network address of the shield.

Calling WiFi.config() before WiFi.begin() forces begin() to configure the WiFi shield with the network addresses specified in config().

You can call WiFi.config() after WiFi.begin(), but the shield will initialize with begin() in the default DHCP mode. Once the config() method is called, it will change the network address as requested.

#### Syntax

```
WiFi.config(ip);
WiFi.config(ip, dns);
WiFi.config(ip, dns, gateway);
WiFi.config(ip, dns, gateway, subnet);
```

#### Parameters

- ip: the IP address of the device (array of 4 bytes)
- dns: the address for a DNS server.
- gateway: the IP address of the network gateway (array of 4 bytes). - optional: defaults to the device IP address with the last octet set to 1
- subnet: the subnet mask of the network (array of 4 bytes). optional: defaults to 255.255.255.0

#### Returns

- Nothing

#### Example

```
This example shows how to set the static IP address, 192.168.0.177, of the LAN network to the WiFi:

#include <SPI.h>
#include <WiFiNINA.h>

// the IP address for the shield:
IPAddress ip(192, 168, 0, 177);    

char ssid[] = "yourNetwork";    // your network SSID (name)
char pass[] = "secretPassword"; // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;

void setup()
{  
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  WiFi.config(ip);

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // print your WiFi's IP address:
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop () {}
```

### `WiFi.setDNS()`

#### Description

WiFi.setDNS() allows you to configure the DNS (Domain Name System) server.

#### Syntax

```
WiFi.setDNS(dns_server1)
WiFi.setDNS(dns_server1, dns_server2)

```

#### Parameters

- dns_server1: the IP address of the primary DNS server
- dns_server2: the IP address of the secondary DNS server

#### Returns

- Nothing

#### Example

```
This example shows how to set the Google DNS (8.8.8.8). You can set it as an object IPAddress.

#include <SPI.h>
#include <WiFiNINA.h>

// the IP address for the shield:
IPAddress dns(8, 8, 8, 8);  //Google DNS  

char ssid[] = "yourNetwork";    // your network SSID (name)
char pass[] = "secretPassword"; // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;

void setup()
{  
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // print your WiFi's IP address:
  WiFi.setDNS(dns);
  Serial.print("DNS configured.");
}

void loop () {
}
 
```

### `WiFi.setHostname()`

#### Description
Sets the hostname of the module, the hostname is sent in WiFi.begin(...) when an IP address is requested from a DHCP server.

#### Syntax

```
WiFi.setHostname(hostname)

```

#### Parameters

- hostname - new hostname to use

#### Returns

- Nothing

#### Example

```
…
  WiFi.setHostname("MyArduino");

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
…
```

### `WiFi.setTimeout()`

#### Description
Sets the connection timeout value in milliseconds for WiFi.begin(...).

#### Syntax

```
WiFi.setTimeout(timeout)
```

#### Parameters

- timeout - the connection timeout value in milliseconds

#### Returns

- Nothing

#### Example

```
…
 WiFi.setTimeout(120 * 1000);

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

…
```

### `WiFi.SSID()`

#### Description
Gets the SSID of the current network

#### Syntax

```
WiFi.SSID();
WiFi.SSID(wifiAccessPoint)
```

#### Parameters

- wifiAccessPoint: specifies from which network to get the information

#### Returns

- A string containing the SSID the WiFi is currently connected to.
- A string containing name of network requested.

#### Example

```
#include <SPI.h>
#include <WiFiNINA.h>

//SSID of your network
char ssid[] = "yourNetwork";
int status = WL_IDLE_STATUS;     // the WiFi radio's status

void setup()
{
  // initialize serial:
  Serial.begin(9600);

  // scan for existing networks:
  Serial.println("Scanning available networks...");
  scanNetworks();

  // attempt to connect using WEP encryption:
  Serial.println("Attempting to connect to open network...");
  status = WiFi.begin(ssid);

  Serial.print("SSID: ");
  Serial.println(ssid);

}

void loop () {}

void scanNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  byte numSsid = WiFi.scanNetworks();

  // print the list of networks seen:
  Serial.print("SSID List:");
  Serial.println(numSsid);
  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet<numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") Network: ");
    Serial.println(WiFi.SSID(thisNet));
  }
}
 
```

### `WiFi.BSSID()`

#### Description
Gets the MAC address of the router you are connected to or the MAC address of a network that was scanned.

#### Syntax

```
WiFi.BSSID(bssid)
WiFi.BSSID(wifiAccessPoint, bssid)

```

#### Parameters

- bssid - 6 byte array
- wifiAccessPoint - specifies from which network to get the information (optional), only needed after a scan

#### Returns

A byte array containing the MAC address of the router the WiFi shield is currently connected to or the MAC address of a network that was scanned. The first array index contains the last byte of the MAC address.

#### Example

```
#include <WiFiNINA.h>

//SSID of your network
char ssid[] = "yourNetwork";
//password of your WPA Network
char pass[] = "secretPassword";

void setup()
{
 WiFi.begin(ssid, pass);

  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  // if you are connected, print out info about the connection:
  else {
  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);    
  Serial.print("BSSID: ");
  Serial.print(bssid[5],HEX);
  Serial.print(":");
  Serial.print(bssid[4],HEX);
  Serial.print(":");
  Serial.print(bssid[3],HEX);
  Serial.print(":");
  Serial.print(bssid[2],HEX);
  Serial.print(":");
  Serial.print(bssid[1],HEX);
  Serial.print(":");
  Serial.println(bssid[0],HEX);
  }
}

void loop () {}
```

### `WiFi.RSSI()`

#### Description
Gets the signal strength of the connection to the router

#### Syntax

```
WiFi.RSSI();
WiFi.RSSI(wifiAccessPoint);
```

#### Parameters

- wifiAccessPoint: specifies from which network to get the information

#### Returns

- long : the current RSSI/Received Signal Strength in dBm

#### Example

```
#include <SPI.h>
#include <WiFiNINA.h>

//SSID of your network
char ssid[] = "yourNetwork";
//password of your WPA Network
char pass[] = "secretPassword";

void setup()
{
 WiFi.begin(ssid, pass);

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  // if you are connected, print out info about the connection:
  else {
   // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("RSSI:");
  Serial.println(rssi);
  }
}

void loop () {}
```

### `WiFi.channel()`

#### Description
Gets the WiFi channel of a network that was scanned.

#### Syntax

```
WiFi.channel(wifiAccessPoint)

```

#### Parameters

- wifiAccessPoint - specifies from which network to get the information

#### Returns

- WiFi channel of scanned network

#### Example

```
…
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1)
  {
    Serial.println("Couldn't get a WiFi connection");
    while (true);
  }

  // print the list of networks seen:
  Serial.print("number of available networks: ");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet + 1);
    Serial.print(") ");
    Serial.print("Signal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tChannel: ");
    Serial.print(WiFi.channel(thisNet));
    byte bssid[6];
    Serial.print("\t\tBSSID: ");
    printMacAddress(WiFi.BSSID(thisNet, bssid));
    Serial.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
    Serial.print("\t\tSSID: ");
    Serial.println(WiFi.SSID(thisNet));
    Serial.flush();
  }
  Serial.println();

…
```

### `WiFi.encryptionType()`

#### Description
Gets the encryption type of the current network

#### Syntax

```
WiFi.encryptionType();
WiFi.encryptionType(wifiAccessPoint);
```

#### Parameters

- wifiAccessPoint: specifies which network to get information from

#### Returns

byte : value represents the type of encryption

- TKIP (WPA) = 2
- WEP = 5
- CCMP (WPA) = 4
- NONE = 7
- AUTO = 8

#### Example

```
#include <SPI.h>
#include <WiFiNINA.h>

//SSID of your network
char ssid[] = "yourNetwork";
//password of your WPA Network
char pass[] = "secretPassword";

void setup()
{
 WiFi.begin(ssid, pass);

  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  // if you are connected, print out info about the connection:
  else {
   // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption,HEX);
  }
}

void loop () {}
```

### `WiFi.scanNetworks()`

#### Description
Scans for available WiFi networks and returns the discovered number

#### Syntax

```
WiFi.scanNetworks()

```

#### Parameters

- None

#### Returns

- byte : number of discovered networks

#### Example

```
/*
 This example prints the board's MAC address, and
 scans for available WiFi networks using the NINA module.
 Every ten seconds, it scans again. It doesn't actually
 connect to any network, so no encryption scheme is specified.

 Circuit:
 * Board with NINA module (Arduino MKR WiFi 1010, MKR VIDOR 4000 and Uno WiFi Rev.2)

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 21 Junn 2012
 by Tom Igoe and Jaymes Dec
 */


#include <SPI.h>
#include <WiFiNINA.h>

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  printMacAddress(mac);
}

void loop() {
  // scan for existing networks:
  Serial.println("Scanning available networks...");
  listNetworks();
  delay(10000);
}

void listNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("Couldn't get a WiFi connection");
    while (true);
  }

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
  }
}

void printEncryptionType(int thisType) {
  // read the encryption type and print out the name:
  switch (thisType) {
    case ENC_TYPE_WEP:
      Serial.println("WEP");
      break;
    case ENC_TYPE_TKIP:
      Serial.println("WPA");
      break;
    case ENC_TYPE_CCMP:
      Serial.println("WPA2");
      break;
    case ENC_TYPE_NONE:
      Serial.println("None");
      break;
    case ENC_TYPE_AUTO:
      Serial.println("Auto");
      break;
    case ENC_TYPE_UNKNOWN:
    default:
      Serial.println("Unknown");
      break;
  }
}


void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
```

### `WiFi.ping()`

#### Description

- Ping a remote device on the network.

#### Syntax

```
WiFi.ping(ip);
WiFi.ping(ip, ttl);
WiFi.ping(host);
WiFi.ping(host, ttl);
```

#### Parameters

- ip: the IP address to ping (array of 4 bytes)
- host: the host to ping (string)
- ttl: Time of live (optional, defaults to 128). Maximum number of routers the request can be forwarded to.

#### Returns

- WL_PING_SUCCESS when the ping was successful
- WL_PING_DEST_UNREACHABLE when the destination (IP or host is unreachable)
- WL_PING_TIMEOUT when the ping times out
- WL_PING_UNKNOWN_HOST when the host cannot be resolved via DNS
- WL_PING_ERROR when an error occurs

#### Example

```
/*
  This example connects to an encrypted WiFi network (WPA/WPA2).
  Then it prints the MAC address of the board,
  the IP address obtained, and other network details.
  Then it continuously pings given host specified by IP Address or name.

  Circuit:
  * Board with NINA module (Arduino MKR WiFi 1010, MKR VIDOR 4000 and Uno WiFi Rev.2)

  created 13 July 2010
  by dlf (Metodo2 srl)
  modified 09 June 2016
  by Petar Georgiev
*/
#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the WiFi radio's status

// Specify IP address or hostname
String hostName = "www.google.com";
int pingResult;

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 5 seconds for connection:
    delay(5000);
  }

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");
  printCurrentNet();
  printWiFiData();
}

void loop() {
  Serial.print("Pinging ");
  Serial.print(hostName);
  Serial.print(": ");

  pingResult = WiFi.ping(hostName);

  if (pingResult >= 0) {
    Serial.print("SUCCESS! RTT = ");
    Serial.print(pingResult);
    Serial.println(" ms");
  } else {
    Serial.print("FAILED! Error code: ");
    Serial.println(pingResult);
  }

  delay(5000);
}

void printWiFiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP address : ");
  Serial.println(ip);

  Serial.print("Subnet mask: ");
  Serial.println((IPAddress)WiFi.subnetMask());

  Serial.print("Gateway IP : ");
  Serial.println((IPAddress)WiFi.gatewayIP());

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI): ");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type: ");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
```

### `WiFi.status()`

#### Description
Return the connection status.

#### Syntax

```
WiFi.status()

```

#### Parameters

- None

#### Returns

- WL_CONNECTED: assigned when connected to a WiFi network;
- WL_AP_CONNECTED : assigned when a device is connected in Access Point mode;
- WL_AP_LISTENING : assigned when the listening for connections in Access Point mode;
- WL_NO_SHIELD: assigned when no WiFi shield is present;
- WL_NO_MODULE: assigned when the communication with an integrated WiFi module fails;
- WL_IDLE_STATUS: it is a temporary status assigned when WiFi.begin() is called and remains active until the number of attempts expires (resulting in WL_CONNECT_FAILED) or a connection is established (resulting in WL_CONNECTED);
- WL_NO_SSID_AVAIL: assigned when no SSID are available;
- WL_SCAN_COMPLETED: assigned when the scan networks is completed;
- WL_CONNECT_FAILED: assigned when the connection fails for all the attempts;
- WL_CONNECTION_LOST: assigned when the connection is lost;
- WL_DISCONNECTED: assigned when disconnected from a network;

#### Example

```

#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "yourNetwork";                     // your network SSID (name)
char key[] = "D0D0DEADF00DABBADEAFBEADED";       // your network key
int keyIndex = 0;                                // your network key Index number
int status = WL_IDLE_STATUS;                     // the WiFi radio's status

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WEP network, SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, keyIndex, key);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // once you are connected :
  Serial.print("You're connected to the network");
}

void loop() {
  // check the network status connection once every 10 seconds:
  delay(10000);
 Serial.println(WiFi.status());
}

 
```

### `WiFi.macAddress()`

#### Description
Gets the MAC Address of your WiFi NINA module

#### Syntax

```
WiFi.macAddress(mac)

```

#### Parameters

- mac: a 6 byte array to hold the MAC address

#### Returns

- byte array : 6 bytes representing the MAC address of your module

#### Example

```
#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "yourNetwork";     // the name of your network
int status = WL_IDLE_STATUS;     // the WiFi radio's status

byte mac[6];                     // the MAC address of your WiFi Module


void setup()
{
 Serial.begin(9600);

 status = WiFi.begin(ssid);

 if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  // if you are connected, print your MAC address:
  else {
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
  }
}

void loop () {}
```

### `WiFi.firmwareVersion()`

#### Description
Returns the firmware version running on the module as a string.

#### Syntax

```
WiFi.firmwareVersion()

```

#### Parameters

- None

#### Returns

- The firmware version running on the module as a string

#### Example

```
…
String fv = WiFi.firmwareVersion();
if (fv < "1.0.0") {
  Serial.println("Please upgrade the firmware");
}
…
```

### `WiFi.lowPowerMode()`

#### Description

Enable low power mode. This is an automatically managed mode where the WiFi NINA Module reduces its power drain bringing the overall power consumption to 30 mA. Any incoming data is received and the device sends out regularly the beacon signal each 100 ms to keep the AP connection alive.

#### Syntax

```
WiFi.lowPowerMode()
```

#### Returns

- None

### `WiFi.noLowPowerMode()`

#### Description

Disables the power saving modes enabled with lowPowerMode(). This is the default status of Power Mode.

#### Syntax

```
WiFi.noLowPowerMode()
```

#### Returns

- None

### `WiFi.reasonCode()`

#### Description

Return The deauthentication reason code.

#### Syntax

```
WiFi.reasonCode()

```

#### Parameters

- None

#### Returns

- The deauthentication reason code

#### Example

```
…

  while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);
        if (status != WL_CONNECTED) {

                Serial.print("Reason code: ");
                Serial.println(WiFi.reasonCode());

        }
        // wait 10 seconds for connection:
        delay(10000);
  }

…
```

### `WiFi.hostByName()`

#### Description
Resolve the given hostname to an IP address

#### Syntax

```
WiFi.hostByName(hostname, result)

```

#### Parameters

- hostname: Name to be resolved
- result: IPAddress structure to store the returned IP address

#### Returns

- 1 if hostname was successfully converted to an IP address, else the error code

#### Example

```
…

  while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);
        delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  IPAddress result;
  int err = WiFi.hostByName(server, result) ;
  if(err == 1){
        Serial.print("IP address: ");
        Serial.println(result);
  } else {
        Serial.print("Error code: ");
        Serial.println(err);
  }

…
```

### `WiFi.localIP()`

#### Description

Gets the WiFi's IP address

#### Syntax

```
WiFi.localIP()

```

#### Parameters
- None

#### Returns
- the IP address of the board

#### Example

```

#include <WiFiNINA.h>

char ssid[] = "yourNetwork";      //SSID of your network

int status = WL_IDLE_STATUS;     // the WiFi radio's status

IPAddress ip;                    // the IP address of your board

void setup()
{
 // initialize serial:
 Serial.begin(9600);

 WiFi.begin(ssid);

  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  // if you are connected, print out info about the connection:
  else {
 //print the local IP address
  ip = WiFi.localIP();
  Serial.println(ip);

  }
}

void loop () {}
```

### `WiFi.subnetMask()`

#### Description
Gets the WiFi's subnet mask

#### Syntax

```
WiFi.subnet()

```

#### Parameters

- None

#### Returns

- the subnet mask of the board

#### Example

```
#include <WiFiNINA.h>
int status = WL_IDLE_STATUS;     // the WiFi radio's status

//SSID of your network
char ssid[] = "yourNetwork";
//password of your WPA Network
char pass[] = "secretPassword";

IPAddress ip;
IPAddress subnet;
IPAddress gateway;

void setup()
{
  WiFi.begin(ssid, pass);

  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  // if you are connected, print out info about the connection:
  else {

    // print your subnet mask:
    subnet = WiFi.subnetMask();
    Serial.print("NETMASK: ");
    Serial.println();

  }
}

void loop () {
}
 
```

### `WiFi.gatewayIP()`

#### Description
Gets the WiFi's gateway IP address.

#### Syntax

```
WiFi.gatewayIP()

```

#### Parameters

- None

#### Returns

- An array containing the board's gateway IP address

#### Example

```
#include <SPI.h>
#include <WiFiNINA.h>

int status = WL_IDLE_STATUS;     // the WiFi radio's status

//SSID of your network
char ssid[] = "yourNetwork";
//password of your WPA Network
char pass[] = "secretPassword";

IPAddress gateway;

void setup()
{
  Serial.begin(9600);

 WiFi.begin(ssid, pass);

  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  // if you are connected, print out info about the connection:
  else {

  // print your gateway address:
  gateway = WiFi.gatewayIP();
  Serial.print("GATEWAY: ");
  Serial.println(gateway);

  }
}

void loop () {}
```

### `WiFi.dnsIP()`

#### Description

Returns the DNS server IP address for the device.


#### Syntax

```
WiFi.dnsIP()
WiFi.dnsIP(n)

```

#### Parameters
optional parameter n for the number of the DNS server to get the second DNS serverv

#### Returns
- the DNS server IP address for the device (IPAddress).

#### Example

```
#include <WiFiNINA.h>

#include "arduino_secrets.h"
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

IPAddress emptyIP;

void setup() {

  Serial.begin(115200);
  while (!Serial) {}

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  int status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }

  Serial.print("DHCP assigned DNS server: ");
  IPAddress dns1 = WiFi.dnsIP();
  if (dns1 == emptyIP) {
    Serial.println("not set");
  } else {
    dns1.printTo(Serial);
    Serial.println();
    IPAddress dns2 = WiFi.dnsIP(1);
    if (dns2 != emptyIP) {
      Serial.print("DNS server2: ");
      dns2.printTo(Serial);
      Serial.println();
    }
  }

}

void loop() {
}
```
### `WiFi.getTime()`

#### Description
Get the time in seconds since January 1st, 1970. The time is retrieved from the WiFi module which periodically fetches the NTP time from an NTP server.

#### Syntax

```
WiFi.getTime();
```

#### Parameters

- None

#### Returns

- Returns the time in seconds since January 1st, 1970 on success. 0 on failure.

## Client Class

### `Client()`

#### Description

Client is the base class for all WiFi client based calls. It is not called directly, but invoked whenever you use a function that relies on it.

### `WiFiClient()`

#### Description

Creates a client that can connect to to a specified internet IP address and port as defined in client.connect().

#### Syntax

```
WiFiClient client;
```

#### Parameters

- client : the named client to refer to

#### Returns

- None

#### Example

```
#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "myNetwork";          //  your network SSID (name)
char pass[] = "myPassword";   // your network password

int status = WL_IDLE_STATUS;
IPAddress server(74,125,115,105);  // Google

// Initialize the client library
WiFiClient client;

void setup() {
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    // don't do anything else:
    while(true);
  }
  else {
    Serial.println("Connected to WiFi");
    Serial.println("\nStarting connection...");
    // if you get a connection, report back via serial:
    if (client.connect(server, 80)) {
      Serial.println("connected");
      // Make a HTTP request:
      client.println("GET /search?q=arduino HTTP/1.0");
      client.println();
    }
  }
}

void loop() {

}
```

### `WiFiSSLClient`

#### Description
This class allows to create a client that always connects in SSL to the specified IP address and port, even if client.connect() is used instead of client.connectSSL(). This is useful If you have a library that accepts only plain Client, but you want to force it to use SSL, keeping the same method names of the non SSL client.

#### Syntax

```
WiFiNINASSLClient client;

```

#### Parameters

- client : the named client to refer to

#### Return

- None

#### Example

```
/*
This example creates a client object that connects and transfers
data using always SSL.

It is compatible with the methods normally related to plain
connections, like client.connect(host, port).

Written by Arturo Guadalupi
last revision November 2015

*/

#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "www.google.com";    // name address for Google (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiSSLClient client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWiFiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 443)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /search?q=arduino HTTP/1.1");
    client.println("Host: www.google.com");
    client.println("Connection: close");
    client.println();
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forevermore:
    while (true);
  }
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
```

### `client.connected()`

#### Description
Whether or not the client is connected. Note that a client is considered connected if the connection has been closed but there is still unread data.

#### Syntax

```
client.connected()

```

#### Parameters

- None

#### Returns

- Returns true if the client is connected, false if not.

#### Example

```
#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "myNetwork";          //  your network SSID (name)
char pass[] = "myPassword";   // your network password

int status = WL_IDLE_STATUS;
IPAddress server(74,125,115,105);  // Google

// Initialize the client library
WiFiClient client;

void setup() {
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    // don't do anything else:
    while(true);
  }
  else {
    Serial.println("Connected to WiFi");
    Serial.println("\nStarting connection...");
    // if you get a connection, report back via serial:
    if (client.connect(server, 80)) {
      Serial.println("connected");
      // Make a HTTP request:
      client.println("GET /search?q=arduino HTTP/1.0");
      client.println();
    }
  }
}

void loop() {
   if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    for(;;)
      ;
  }
}
```

### `client.connect()`

#### Description
Connect to the IP address and port specified in the constructor. The return value indicates success or failure. connect() also supports DNS lookups when using a domain name (e.g., google.com).

#### Syntax

```
client.connect(ip, port)
client.connect(URL, port)

```

#### Parameters

- ip: the IP address that the client will connect to (array of 4 bytes)
- URL: the domain name the client will connect to (string e.g., "arduino.cc")
- port: the port that the client will connect to (int)

#### Returns

- Returns true if the connection succeeds, false if not.

#### Example

```
#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "myNetwork";          //  your network SSID (name)
char pass[] = "myPassword";   // your network password

int status = WL_IDLE_STATUS;
char servername[]="google.com";  // remote server we will connect to

WiFiClient client;

void setup() {
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    // don't do anything else:
    while(true);
  }
  else {
    Serial.println("Connected to WiFi");
    Serial.println("\nStarting connection...");
    // if you get a connection, report back via serial:
    if (client.connect(servername, 80)) {
      Serial.println("connected");
      // Make a HTTP request:
      client.println("GET /search?q=arduino HTTP/1.0");
      client.println();
    }
  }
}

void loop() {

}
```

### `client.connectSSL()`

#### Description

Connect to the IP address and port specified in the constructor using the SSL protocol. The method connectSSL is required when the server provides only HTTPS connections. Before using this method, it is required to load the SSL certificate used by the server into the Arduino WiFi module . The boards come already loaded with certificates and it should be ready to use. To change or upload new SSL certificates you should follow the procedures that will be made available. connectSSL() also supports DNS lookups when using a domain name (e.g., google.com).

#### Syntax

```
client.connectSSL(ip, port)
client.connectSSL(URL, port)

```

#### Parameters

- ip: the IP address that the client will connect to (array of 4 bytes)
- URL: the domain name the client will connect to (string e.g., "arduino.cc")
- port: the port that the client will connect to (int)

#### Returns
Returns true if the connection succeeds, false if not.

#### Example

```
…

/*
  Web client

 This sketch connects to a website through an SSL connection
 using a WiFi board.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.

 Circuit:
 * WiFiNINA supported board

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 */


#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "yourNetwork"; //  your network SSID (name)
char pass[] = "secretPassword";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
char server[] = "arduino.tips";    // name address for Arduino (using DNS)

// Initialize the WiFi client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to WiFi");

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connectSSL(server, 443)) {
    Serial.println("Connected to server");
    // Make a HTTP request:
    client.println("GET /asciilogo.txt HTTP/1.1");
    client.println("Host: arduino.tips");
    client.println("Connection: close");
    client.println();
    Serial.println("Request sent");
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forevermore:
    while (true);
  }
}


…
```

### `client.status()`

#### Description
Return Connection status.

#### Syntax

```
client.status()

```

#### Parameters

- None

#### Returns

- The client connection status

#### Example

```
…

void setup() {

  Serial.begin(9600);
  while (!Serial) {
        ;  }
  if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
                while (true);
  }

   while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
                status = WiFi.begin(ssid, pass);
        delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  IPAddress result;
  int err = WiFi.hostByName(server, result) ;
  if (err == 1) {
        Serial.print("IP address: ");
        Serial.println(result);
  } else {
        Serial.print("Error code: ");
        Serial.println(err);
  }


  if (client.connect(result, 80)) {
        Serial.println("connected to server");
                client.println("GET /search?q=arduino HTTP/1.1");
        client.println("Host: www.google.com");
        client.println("Connection: close");
        client.println();
  }
  Serial.print("status: ");
  Serial.println(client.status());
}

…
```

### `client.write()`

#### Description
Write data to all the clients connected to a server.

#### Syntax

```
client.write(data)
client.write(buffer, size);

```

#### Parameters

- data: the outgoing byte
- buffer: the outgoing message
- size: the size of the buffer

#### Returns

- The number of bytes written. It is not necessary to read this.

### `client.print()`

#### Description

Print data to the server that a client is connected to. Prints numbers as a sequence of digits, each an ASCII character (e.g. the number 123 is sent as the three characters '1', '2', '3').

#### Syntax

```
client.print(data)
client.print(data, BASE)

```

#### Parameters
- data: the data to print (char, byte, int, long, or string)
- BASE (optional): the base in which to print numbers:, DEC for decimal (base 10), OCT for octal (base 8), HEX for hexadecimal (base 16).

#### Returns

- byte : returns the number of bytes written, though reading that number is optional

### `client.println()`

#### Description

Print data, followed by a carriage return and newline, to the server a client is connected to. Prints numbers as a sequence of digits, each an ASCII character (e.g. the number 123 is sent as the three characters '1', '2', '3').

#### Syntax

```
client.println()
client.println(data)
client.print(data, BASE)

```

#### Parameters

- data (optional): the data to print (char, byte, int, long, or string)
- BASE (optional): the base in which to print numbers: DEC for decimal (base 10), OCT for octal (base 8), HEX for hexadecimal (base 16).

#### Returns

- byte: return the number of bytes written, though reading that number is optional

### `client.available()`

#### Description
Returns the number of bytes available for reading (that is, the amount of data that has been written to the client by the server it is connected to).

available() inherits from the Stream utility class.

#### Syntax

```
client.available()

```

#### Parameters

- None

#### Returns

- The number of bytes available.

#### Example

```
#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "myNetwork";          //  your network SSID (name)
char pass[] = "myPassword";   // your network password

int status = WL_IDLE_STATUS;
char servername[]="google.com";  // Google

WiFiClient client;

void setup() {
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    // don't do anything else:
    while(true);
  }
  else {
    Serial.println("Connected to WiFi");
    Serial.println("\nStarting connection...");
    // if you get a connection, report back via serial:
    if (client.connect(servername, 80)) {
      Serial.println("connected");
      // Make a HTTP request:
      client.println("GET /search?q=arduino HTTP/1.0");
      client.println();
    }
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for(;;)
      ;
  }
}
```

### `client.peek()`

#### Description

Read a byte from the file without advancing to the next one. That is, successive calls to peek() will return the same value, as will the next call to read().

This function inherited from the Stream class. See the Stream class main page for more information.

#### Syntax

```
client.peek()
```

#### Parameters

- None

#### Returns

- b: the next byte or character
- -1: if none is available

#### Example

```
…

#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h"
char ssid[] = SECRET_SSID;      // your network SSID (name)
char pass[] = SECRET_PASS;      // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;               // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
char server[] = "www.google.com";       // name address for Google (using DNS)


WiFiClient client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
        ;   }

  if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        while (true);
  }

  while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);
        delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  IPAddress result;
  int err = WiFi.hostByName(server, result) ;
  if (err == 1) {
        Serial.print("IP address: ");
        Serial.println(result);
  } else {
        Serial.print("Error code: ");
        Serial.println(err);
  }

   if (client.connect(result, 80)) {
        Serial.println("connected to server");
        client.println("GET /search?q=arduino HTTP/1.1");
        client.println("Host: www.google.com");
        client.println("Connection: close");
        client.println();
  }
}

void loop() {
    int i = 0;
  while (client.available()) {
        char c = client.peek();
        Serial.print("peek: ");
        Serial.println(c);

        Serial.print("calling a second time peek, the char is the same: ");
        c = client.peek();
        Serial.println(c);

        Serial.print("calling the read retry the char and erase from the buffer: ");
        c = client.read();
        Serial.println(c);
        if (i == 2) {
        while (1);
        }
        i++;
  }
}

…
```

### `client.read()`

#### Description

Reads data from the client. If no arguments are given, it will return the next character in the buffer.

#### Syntax

```
client.read()
client.read(buffer, size);
```

#### Parameters

- buffer: buffer to hold incoming packets (char*)
- len: maximum size of the buffer (int)

#### Returns
- b: the next character in the buffer (char)
- size: the size of the data
- -1: if no data is available

### `client.flush()`

#### Description

Clears the buffer once all outgoing characters have been sent.

flush() inherits from the [Stream](https://www.arduino.cc/reference/en/language/functions/communication/stream/) utility class.

#### Syntax

```
client.flush()
```

#### Parameters
- None

#### Returns
- None

### `client.stop()`

#### Description

Disconnect from the server.

#### Syntax

```
client.stop()
```

#### Parameters

- None

#### Returns

- None

### `client.remoteIP()`

#### Description
Gets the IP address of the remote connection.

#### Syntax

```
client.remoteIP()

```

#### Parameters

- None

#### Returns

- The IP address of the host the client is connected to

### `client.remotePort()`

#### Description
Gets the port number of the remote connection.

#### Syntax

```
client.remotePort()

```

#### Parameters

- None

#### Returns
The port of the remote host that the client is connected to

## Server Class

### `Server()`

#### Description

Server is the base class for all WiFi server based calls. It is not called directly, but invoked whenever you use a function that relies on it.

### `WiFiServer()`

#### Description

Creates a server that listens for incoming connections on the specified port.

#### Syntax

```
WiFiServer server(port);
```

#### Parameters
- port: the port to listen on (int)

#### Returns
- None

#### Example

```
#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "myNetwork";          //  your network SSID (name)
char pass[] = "myPassword";   // your network password
int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  // initialize serial:
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  else {
    server.begin();
    Serial.print("Connected to WiFi. My address:");
    IPAddress myAddress = WiFi.localIP();
    Serial.println(myAddress);

  }
}


void loop() {

}
 
```

### `server.begin()`

#### Description
Tells the server to begin listening for incoming connections.

#### Syntax

```
server.begin()

```

#### Parameters

- None

#### Returns

- None

#### Example

```
#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "lamaison";          //  your network SSID (name)
char pass[] = "tenantaccess247";   // your network password
int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  // initialize serial:
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  else {
    server.begin();
    Serial.print("Connected to WiFi. My address:");
    IPAddress myAddress = WiFi.localIP();
    Serial.println(myAddress);

  }
}


void loop() {

}
 
```

### `if(server)`

#### Description
Indicates whether the server is listening for new clients. You can use this to detect whether server.begin() was successful.


#### Syntax

```
if (server)
if (!server)

```

#### Parameters
none

#### Returns
- whether the server is listening for new clients (bool).

#### Example

```
#include <WiFiNINA.h>

char ssid[] = "Network";          //  your network SSID (name)
char pass[] = "myPassword";   // your network password

WiFiServer server(23);

void setup() {

  Serial.begin(115200);
  while (!Serial) {}

  int status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }

  server.begin();
  if (!server) {
    Serial.println("Server failed to start.");
    while(true);
  }
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String s = client.readStringUntil('\n');
    server.println(s);
  }
}
```

### `server.status()`

#### Description
Return Connection status.

#### Syntax

```
server.status()
```

#### Parameters

- None

#### Returns

- The server connection status

#### Example

```
…

Serial.print("connection status: ");
Serial.println(client.status());

…
```

### `server.write()`

#### Description
Write data to all the clients connected to a server.

#### Syntax

```
server.write(data)
server.write(buffer, size);

```

#### Parameters

- data: the outgoing byte
- buffer: the outgoing message
- size: the size of the buffer

#### Returns

- The number of bytes written. It is not necessary to read this.

#### Example

```
#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "yourNetwork";
char pass[] = "yourPassword";
int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  // initialize serial:
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  else {
    server.begin();
  }
}

void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client == true) {
       // read bytes from the incoming client and write them back
    // to any clients connected to the server:
    server.write(client.read());
  }
}
```

### `server.print()`

#### Description

Print data to all the clients connected to a server. Prints numbers as a sequence of digits, each an ASCII character (e.g. the number 123 is sent as the three characters '1', '2', '3').

#### Syntax

```
server.print(data)
server.print(data, BASE)

```

#### Parameters
- data: the data to print (char, byte, int, long, or string)
- BASE (optional): the base in which to print numbers: BIN for binary (base 2), DEC for decimal (base 10), OCT for octal (base 8), HEX for hexadecimal (base 16).

#### Returns
- byte
- print() will return the number of bytes written, though reading that number is optional

### `server.println()`

#### Description
Prints data, followed by a newline, to all the clients connected to a server. Prints numbers as a sequence of digits, each an ASCII character (e.g. the number 123 is sent as the three characters '1', '2', '3').

#### Syntax

```
server.println()
server.println(data)
server.println(data, BASE)

```

#### Parameters

- data (optional): the data to print (char, byte, int, long, or string)
- BASE (optional): the base in which to print numbers: DEC for decimal (base 10), OCT for octal (base 8), HEX for hexadecimal (base 16).

#### Returns
- byte
- println() will return the number of bytes written, though reading that number is optional


### `server.available()`

#### Description
Gets a client that is connected to the server and has data available for reading. The connection persists when the returned client object goes out of scope; you can close it by calling client.stop().

available() inherits from the Stream utility class.

#### Syntax

```
server.available()
```

#### Parameters

- None

#### Returns

- a Client object; if no Client has data available for reading, this object will evaluate to false in an if-statement

#### Example

```
#include <SPI.h>
#include <NINA.h>

char ssid[] = "Network";          //  your network SSID (name)
char pass[] = "myPassword";   // your network password
int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  // initialize serial:
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }
  else {
    server.begin();
    Serial.print("Connected to WiFi. My address:");
    IPAddress myAddress = WiFi.localIP();
    Serial.println(myAddress);

  }
}

void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {

    if (client.connected()) {
      Serial.println("Connected to client");
    }

    // close the connection:
    client.stop();
  }
}
 
```

### `server.accept()`

#### Description

The traditional server.available() function would only tell you of a new client after it sent data, which makes some protocols like FTP impossible to properly implement.

The intention is programs will use either available() or accept(), but not both. With available(), the client connection continues to be managed by WiFiServer. You don’t need to keep a client object, since calling available() will give you whatever client has sent data. Simple servers can be written with very little code using available().

With accept(), WiFiServer gives you the client only once, regardless of whether it has sent any data. You must keep track of the connected clients. This requires more code, but you gain more control.


#### Syntax

```
server.accept()

```

#### Parameters
none

#### Returns
- a Client object. If no client has data available for reading, this object will evaluate to false in an if-statement. (WiFiClient).

#### Example

```
#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "Network";          //  your network SSID (name)
char pass[] = "myPassword";   // your network password

int status = WL_IDLE_STATUS;

// telnet defaults to port 23
WiFiServer server(23);

WiFiClient clients[8];

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to WiFi network:
  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while(true);
  }

  // start the server:
  server.begin();

  Serial.print("Chat server address:");
  Serial.println(WiFi.localIP());
}

void loop() {
  // check for any new client connecting, and say hello (before any incoming data)
  WiFiClient newClient = server.accept();
  if (newClient) {
    for (byte i=0; i < 8; i++) {
      if (!clients[i]) {
        Serial.print("We have a new client #");
        Serial.println(i);
        newClient.print("Hello, client number: ");
        newClient.println(i);
        // Once we "accept", the client is no longer tracked by WiFiServer
        // so we must store it into our list of clients
        clients[i] = newClient;
        break;
      }
    }
  }

  // check for incoming data from all clients
  for (byte i=0; i < 8; i++) {
    if (clients[i] && clients[i].available() > 0) {
      // read bytes from a client
      byte buffer[80];
      int count = clients[i].read(buffer, 80);
      // write the bytes to all other connected clients
      for (byte j=0; j < 8; j++) {
        if (j != i && clients[j].connected()) {
          clients[j].write(buffer, count);
        }
      }
    }
  }

  // stop any clients which disconnect
  for (byte i=0; i < 8; i++) {
    if (clients[i] && !clients[i].connected()) {
      Serial.print("disconnect client #");
      Serial.println(i);
      clients[i].stop();
    }
  }

}
```

### `server.peek()`

#### Description

Read a byte from the file without advancing to the next one. That is, successive calls to peek() will return the same value, as will the next call to read().

This function inherited from the Stream class. See the Stream class main page for more information.

#### Syntax

```
server.peek()

```

#### Parameters

- None

#### Returns
- b: the next byte or character
- -1: if none is available
#### Example

```
…

#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = SECRET_SSID;      // your network SSID (name)
char pass[] = SECRET_PASS;      // your network password

int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {

  Serial.begin(9600);
  while (!Serial) {

  }


  if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        while (true);
  }

  while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);

        status = WiFi.begin(ssid, pass);


        delay(10000);
  }
  server.begin();
}


void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
        Serial.println("new client");
        // an http request ends with a blank line
        boolean currentLineIsBlank = true;
        int i = 0;
        while (client.connected()) {
        if (client.available()) {
        char c = client.peek();
        Serial.print("peek: ");
        Serial.println(c);

        Serial.print("calling a second time peek, the char is the same: ");
        c = client.peek();
        Serial.println(c);

        Serial.print("calling the read retry the char and erase from the buffer: ");
        c = client.read();
        Serial.println(c);
        if (i == 2) {
        while (1);
        }
        i++;

        if (c == '\n') {
        // you're starting a new line
        currentLineIsBlank = true;
        } else if (c != '\r') {
        // you've gotten a character on the current line
        currentLineIsBlank = false;
        }
        }
        }
        // give the web browser time to receive the data
        delay(1);

        // close the connection:
        client.stop();
        Serial.println("client disconnected");
  }
}

…
```

## UDP Class

### `WiFiUDP`

#### Description
Creates a named instance of the WiFi UDP class that can send and receive UDP messages.

#### Syntax

```
WiFiUDP
```

#### Parameters
none

### `WiFiUDP.begin()`

#### Description
Initializes the WiFi UDP library and network settings. Starts WiFiUDP socket, listening at local port PORT.

#### Syntax

```
WiFiUDP.begin(port)
```

#### Parameters

- port: the local port to listen on (int)

#### Returns

- 1: if successful
- 0: if there are no sockets available to use

### `WiFiUDP.beginMulticast()`

#### Description

Initializes the WiFi UDP library and network settings. Starts WiFiUDP socket, listening at local port PORT and on specified multicast IP address.

#### Syntax

```
WiFiUDP.beginMulticast(IPAddress, port);
```

#### Parameters

- IPAddress: sdd
- port: the local port to listen on (int)

#### Returns

- 1: if successful
- 0: if there are no sockets available to use

#### Example

```
…
WiFiUDP udp;
void setup() {
  Serial.begin(9600);
  while (!Serial) {
        ;
  }

  if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");

        while (true);
  }

  while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);

        // wait 10 seconds for connection:
        delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");

  udp.beginMulticast(IPAddress(226, 1, 1, 1), 4096);
  Serial.println("\n connected UDP multicast");

…
```

### `WiFiUDP.beginPacket()`

#### Description
Starts a connection to write UDP data to the remote connection

#### Syntax

```
WiFiUDP.beginPacket(hostName, port);
WiFiUDP.beginPacket(hostIp, port);

```

#### Parameters

- hostName: the address of the remote host. It accepts a character string or an IPAddress
- hostIp: the IP address of the remote connection (4 bytes)
- port: the port of the remote connection (int)

#### Returns

- 1: if successful
- 0: if there was a problem with the supplied IP address or port


### `WiFiUDP.write()`

#### Description
Writes UDP data to the remote connection. Must be wrapped between beginPacket() and endPacket(). beginPacket() initializes the packet of data, it is not sent until endPacket() is called.

#### Syntax

```
WiFiUDP.write(byte);
WiFiUDP.write(buffer, size);

```

#### Parameters

- byte: the outgoing byte
- buffer: the outgoing message
- size: the size of the buffer

#### Returns

- single byte into the packet
- bytes size from buffer into the packet

### `WiFiUDP.endPacket()`

#### Description

Called after writing UDP data to the remote connection. It finishes off the packet and send it.

#### Syntax

```
WiFiUDP.endPacket()
```

#### Parameters

- None

#### Returns

- 1: if the packet was sent successfully
- 0: if there was an error

### `WiFiUDP.available()`

#### Description
Get the number of bytes (characters) available for reading from the buffer. This is data that's already arrived.

This function can only be successfully called after WiFiUDP.parsePacket().

available() inherits from the Stream utility class.

#### Syntax

```
WiFiUDP.available()

```

#### Parameters

- None

#### Returns

- the number of bytes available in the current packet
- 0: if WiFiUDP.parsePacket() hasn't been called yet

### `WiFiUDP.parsePacket()`

#### Description

It starts processing the next available incoming packet, checks for the presence of a UDP packet, and reports the size. parsePacket() must be called before reading the buffer with UDP.read().

#### Syntax

```
WiFiUDP..parsePacket()
```

#### Parameters
- None

#### Returns
- the size of the packet in bytes
- 0: if no packets are available

### `WiFiUDP.peek()`

#### Description
Read a byte from the file without advancing to the next one. That is, successive calls to peek() will return the same value, as will the next call to read().

This function inherited from the Stream class. See the Stream class main page for more information.

#### Syntax

```
WiFiUDP.peek()

```

#### Parameters

- None

#### Returns
- b: the next byte or character
- -1: if none is available

### `WiFiUDP.read()`

#### Description
Reads UDP data from the specified buffer. If no arguments are given, it will return the next character in the buffer.

This function can only be successfully called after WiFiUDP.parsePacket().

#### Syntax

```
WiFiUDP.read();
WiFiUDP.read(buffer, len);
```

#### Parameters
- buffer: buffer to hold incoming packets (char*)
len: maximum size of the buffer (int)

#### Returns
- b: the characters in the buffer (char)
- size: the size of the buffer
- -1: if no buffer is available

### `WiFiUDP.flush()`

#### Description
Discard any bytes that have been written to the client but not yet read.

`flush()` inherits from the Stream utility class.

#### Syntax

```
WiFiUDP.flush()
```

#### Parameters
none

#### Returns
- None

### `WiFiUDP.stop()`

#### Description
Disconnect from the server. Release any resource being used during the UDP session.

#### Syntax

```
WiFiUDP.stop()
```

#### Parameters
- None

#### Returns
- None

### `WiFiUDP.remoteIP()`

#### Description
Gets the IP address of the remote connection.

This function must be called after `WiFiUDP.parsePacket()`.

#### Syntax

```
WiFiUDP.remoteIP();
```

#### Parameters

- None

#### Returns

- 4 bytes : the IP address of the host who sent the current incoming packet


### `WiFiUDP.remotePort()`

#### Description
Gets the port of the remote UDP connection.

This function must be called after UDP.parsePacket().

#### Syntax

```
UDP.remotePort()
```

#### Parameters

- None

#### Returns

- The port of the host who sent the current incoming packet

