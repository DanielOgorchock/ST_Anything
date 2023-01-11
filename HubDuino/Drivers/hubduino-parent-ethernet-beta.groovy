/**
 *  HubDuino_Parent_Ethernet.groovy
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/hubduino-parent-ethernet-beta.groovy
 *
 *  Copyright 2017 Dan G Ogorchock 
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 *  in compliance with the License. You may obtain a copy of the License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed
 *  on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License
 *  for the specific language governing permissions and limitations under the License.
 *
 *  Change History:
 *
 *    Date        Who            What
 *    ----        ---            ----
 *    2017-02-08  Dan Ogorchock  Original Creation
 *    2017-02-12  Dan Ogorchock  Modified to work with Ethernet based devices instead of ThingShield
 *    2017-02-24  Dan Ogorchock  Created the new "Multiples" device handler as a new example
 *    2017-04-16  Dan Ogorchock  Updated to use the new Composite Device Handler feature
 *    2017-06-10  Dan Ogorchock  Added Dimmer Switch support
 *    2017-07-09  Dan Ogorchock  Added number of defined buttons tile
 *    2017-08-24  Allan (vseven) Change the way values are pushed to child devices to allow a event to be executed allowing future customization
 *    2007-09-24  Allan (vseven) Added RGB LED light support with a setColorRGB routine
 *    2017-10-07  Dan Ogorchock  Cleaned up formatting for readability
 *    2017-09-24  Allan (vseven) Added RGBW LED strip support with a setColorRGBW routine
 *    2017-12-29  Dan Ogorchock  Added WiFi RSSI value per request from ST user @stevesell
 *    2018-02-15  Dan Ogorchock  Added @saif76's Ultrasonic Sensor
 *    2018-02-25  Dan Ogorchock  Added Child Presence Sensor
 *    2018-03-03  Dan Ogorchock  Added Child Power Meter
 *    2018-06-02  Dan Ogorchock  Revised/Simplified for Hubitat Composite Driver Model
 *    2018-06-24  Dan Ogorchock  Added Child Servo
 *    2018-07-01  Dan Ogorchock  Added Pressure Measurement
 *    2018-08-06  Dan Ogorchock  Added formatting of MAC address
 *    2018-09-22  Dan Ogorchock  Added preference for debug logging
 *    2019-02-05  Dan Ogorchock  Added Child Energy Meter
 *    2019-04-23  Dan Ogorchock  Fixed debug logging, added importURL
 *    2019-06-24  Dan Ogorchock  Added Delete All Child Devices Command (helpful during testing)
 *    2019-07-08  Dan Ogorchock  Added support for Sound Pressure Level device
 *    2019-09-01  Dan Ogorchock  Added Presence Capability to know if the HubDuino device is online or offline
 *    2019-09-04  Dan Ogorchock  Automatically detect maximum number of buttons and set numberOfButtons attribute accordingly
 *    2019-09-04  Dan Ogorchock  Eliminate the need for user to supply MAC address of the Arduino. Configure the Parent DNI to use Arduino IP Address instead.
 *    2019-10-30  Dan Ogorchock  Added Child Valve
 *    2020-02-08  Dan Ogorchock  Added refresh() call to initialize() command
 *    2020-06-09  Dan Ogorchock  Improved HubDuino board 'Presence' logic
 *    2020-06-25  Dan Ogorchock  Added Window Shade
 *    2020-09-19  Dan Ogorchock  Added "Releasable Button" Capability (requires new Arduino IS_Button.cpp and .h code)
 *    2022-02-08  Dan Ogorchock  Added support for new custom "weight measurement" child device
 *    2023-01-10  Dan Ogorchock  Use Built-In Hubitat "Generic Component ..." drivers wherever possible
 *	
 */

metadata {
	definition (name: "HubDuino Parent Ethernet Beta", namespace: "ogiewon", author: "Dan Ogorchock", importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/hubduino-parent-ethernet-beta.groovy") {
        capability "Refresh"
        capability "Pushable Button"
        capability "Holdable Button"
        capability "Releasable Button"
        capability "Signal Strength"
        capability "Presence Sensor"  //used to determine is the HubDuino microcontroller is still reporting data or not
        
        //command "sendData", ["string"]
        //command "componentOn"
        //command "componentOff"
        //command "componentRefresh"
        //command "deleteAllChildDevices"
	}

    // Preferences
	preferences {
		input "ip", "text", title: "Arduino IP Address", description: "IP Address in form 192.168.1.226", required: true, displayDuringSetup: true
		input "port", "text", title: "Arduino Port", description: "port in form of 8090", defaultValue: "8090",required: true, displayDuringSetup: true
        input "timeOut", "number", title: "Timeout in Seconds", description: "Max time w/o HubDuino update before setting presence to 'not present'", defaultValue: "900", range: "600..*",required: true, displayDuringSetup:true
        input name: "logEnable", type: "bool", title: "Enable debug logging", defaultValue: true
    }
}

def logsOff(){
    log.warn "debug logging disabled..."
    device.updateSetting("logEnable",[value:"false",type:"bool"])
}

// parse events into attributes
def parse(String description) {
	if (logEnable) log.debug "description= '${description}'"
    def msg = parseLanMessage(description)
	def headerString = msg.header
    def mac = msg.mac  //needed for backwards compatability
    
    if (!headerString) {
        //log.debug "headerstring was null for some reason :("
    }

    def bodyString = msg.body

    if (bodyString) {
        if (logEnable) log.debug "msg= $bodyString"
    	def parts = bodyString.split(" ")
    	def name  = parts.length>0?parts[0].trim():null
    	def value = parts.length>1?parts[1].trim():null
        
		def nameparts = name.split("\\d+", 2)
		def namebase = nameparts.length>0?nameparts[0].trim():null
        def namenum = name.substring(namebase.length()).trim()
		
        def results = []
        
        if (device.currentValue("presence") != "present") {
            sendEvent(name: "presence", value: "present", isStateChange: true, descriptionText: "New update received from HubDuino device")
        }
        
        //Keep track of when the last update came in from the Arduino board
        state.parseLastRanAt = now()
                
		if (name.startsWith("button")) {
            if (logEnable) log.debug "In parse:  name = ${name}, value = ${value}, btnNum = " + namenum
            if (state.numButtons < namenum.toInteger()) {
                state.numButtons = namenum.toInteger()
                sendEvent(name: "numberOfButtons", value: state.numButtons)
            }
            if ((value == "pushed") || (value == "held") || (value == "released")) {
        	    results << createEvent(name: value, value: namenum, isStateChange: true)
			    if (logEnable) log.debug results
			    return results
            } 
            else 
            {
                return
            }
        }

		if (name.startsWith("rssi")) {
			if (logEnable) log.debug "In parse: RSSI name = ${name}, value = ${value}"
           	results = createEvent(name: name, value: value, displayed: false)
            if (logEnable) log.debug results
			return results
        }


        def isChild = containsDigit(name)
   		//if (logEnable) log.debug "Name = ${name}, isChild = ${isChild}, namebase = ${namebase}, namenum = ${namenum}"      
        //if (logEnable) log.debug "parse() childDevices.size() =  ${childDevices.size()}"

		def childDevice = null

		try {

            childDevices.each {
				try{
                	if ((it.deviceNetworkId == "${device.id}-${name}") || (it.deviceNetworkId == "${device.deviceNetworkId}-${name}") || (it.deviceNetworkId == "${mac}-${name}")) {
                	    childDevice = it
                        if (logEnable) log.debug "Found a match!!!"
                	}
            	}
            	catch (e) {
                    log.error e
            	}
        	}
            
            //If a child should exist, but doesn't yet, automatically add it!            
        	if (isChild && childDevice == null) {
        		if (logEnable) log.debug "isChild = true, but no child found - Auto Add it!"
            	if (logEnable) log.debug "    Need a ${namebase} with id = ${namenum}"
            
            	createChildDevice(namebase, namenum)
            	//find child again, since it should now exist!
            	childDevices.each {
					try{
                		if ((it.deviceNetworkId == "${device.id}-${name}") || (it.deviceNetworkId == "${device.deviceNetworkId}-${name}") || (it.deviceNetworkId == "${mac}-${name}")) {
                			childDevice = it
                    		if (logEnable) log.debug "Found a match!!!"
                		}
            		}
            		catch (e) {
            			log.error e
            		}
        		}
        	}
            
            if (childDevice != null) {
				if (logEnable) log.debug "Calling Parse with ${childDevice.deviceNetworkId} - name: ${namebase}, value: ${value}"
                //childDevice.parse("${namebase} ${value}")
                if (namebase == "relaySwitch") namebase = "switch"  //handle old ST_Anything RelaySwitch device class as normal switch
                if (namebase == "presence") value = (value == "notpresent") ? "not present" : value
                childDevice.parse([[name: namebase, value: value, descriptionText:"${name} is now ${value}"]])

            }
            else  //must not be a child, perform normal update
            {
                log.error "No child found for name = ${name}, value = ${value}. Please make sure all HubDuino Component drivers are installed."  
//                results = createEvent(name: name, value: value)
//                if (logEnable) log.debug results
//                return results
            }
		}
        catch (e) {
        	log.error "Error in parse() routine, error = ${e}"
        }
	}
}

private getHostAddress() {
    def ip = settings.ip
    def port = settings.port

	if (logEnable) log.debug "Using ip: ${ip} and port: ${port} for device: ${device.id}"
    return ip + ":" + port
}

def sendData(message) {
    log.trace "sendData called with message = " + message
    sendEthernet(message) 
}

def sendEthernet(message) {
    if (message.contains(" ")) {
        def parts = message.split(" ")
        def name  = parts.length>0?parts[0].trim():null
        def value = parts.length>0?parts[1].trim():null
        message = name + "%20" + value
    }
    if (logEnable) log.debug "Executing 'sendEthernet' ${message}, to ${settings.ip}:${settings.port}"
  
    if (settings.ip != null && settings.port != null) {
    	sendHubCommand(new hubitat.device.HubAction(
    		method: "POST",
    		path: "/${message}?",
    		headers: [ HOST: "${getHostAddress()}" ]
		))
    }
    else {
    	log.warn "Parent HubDuino Ethernet Device: Please verify IP address and Port are configured."    
    }
}

def refresh() {
	if (logEnable) log.debug "Executing 'refresh()'"
	sendEthernet("refresh")
}

def installed() {
	log.info "Executing 'installed()'"
    state.numButtons = 0
    sendEvent(name: "numberOfButtons", value: state.numButtons)
}

def uninstalled() {
    log.info "Executing 'uninstalled()'"
    unschedule()
    deleteAllChildDevices()
}

def initialize() {
	log.info "Executing 'initialize()'"

    //Schedule Presence Check Routine
    runEvery5Minutes("checkHubDuinoPresence")
    
    //Have the Arduino send an updated value for every device attached.
    refresh()
}

def updated() {
    log.info "Executing 'updated()'"
    log.info "Hub IP Address = ${device.hub.getDataValue("localIP")}, Hub Port = ${device.hub.getDataValue("localSrvPortTCP")}"
    log.info "Arduino IP Address = ${ip}, Hub Port = ${port}"
    
    def iphex = convertIPtoHex(ip)
    log.info "Setting DNI = ${iphex}"
    device.setDeviceNetworkId("${iphex}")
    
    unschedule()
    
    if (logEnable) {
        log.info "Enabling Debug Logging for 30 minutes" 
        runIn(1800,logsOff)
    } else {
        unschedule(logsoff)
    }
    
    //Schedule Presence Check Routine
    runEvery5Minutes("checkHubDuinoPresence")
    
	//Have the Arduino send an updated value for every device attached.  This will auto-created child devices!
    log.info "Sending REFRESH command to Arduino, which will create any missing child devices."
    refresh()
}


private void createChildDevice(String deviceName, String deviceNumber) {
    
		log.info "createChildDevice:  Creating Child Device '${device.displayName} (${deviceName}${deviceNumber})'"
        
        def deviceHandlerName = ""
    
		try {
        	
            def deviceNameSpace = "hubitat"
        	switch (deviceName) {
         		case "contact": 
                		deviceHandlerName = "Generic Component Contact Sensor" 
                	break
         		case "switch": 
                		deviceHandlerName = "Generic Component Switch" 
                	break
         		case "dimmerSwitch":
                		deviceHandlerName = "Generic Component Dimmer" 
                	break
//         		case "rgbSwitch": 
//                        deviceNameSpace = "ogiewon"
//                		deviceHandlerName = "Child RGB Switch" 
//                	break
//         		case "generic": 
//                        deviceNameSpace = "ogiewon"
//                		deviceHandlerName = "Child Generic Sensor" 
//                	break
//         		case "rgbwSwitch": 
//                        deviceNameSpace = "ogiewon"
//                		deviceHandlerName = "Child RGBW Switch" 
//                	break
         		case "relaySwitch": 
                		deviceHandlerName = "Generic Component Switch" 
                	break
         		case "temperature": 
                		deviceHandlerName = "Generic Component Temperature Sensor" 
                	break
         		case "humidity": 
                		deviceHandlerName = "Generic Component Humidity Sensor" 
                	break
         		case "motion": 
                		deviceHandlerName = "Generic Component Motion Sensor" 
                	break
         		case "water": 
                		deviceHandlerName = "Generic Component Water Sensor" 
                	break
         		case "illuminance": 
                        deviceNameSpace = "ogiewon"
                		deviceHandlerName = "HubDuino Component Illuminance Sensor"
                	break
         		case "illuminancergb": 
                        deviceNameSpace = "ogiewon"
                		deviceHandlerName = "HubDuino Component IlluminanceRGB Sensor"
                	break
         		case "voltage": 
                		deviceHandlerName = "Generic Component Voltage Sensor" 
                	break
         		case "smoke": 
                		deviceHandlerName = "Generic Component Smoke Detector" 
                	break    
         		case "carbonMonoxide": 
                		deviceHandlerName = "Generic Component Carbon Monoxide Detector"
                	break    
         		case "alarm": 
                        deviceNameSpace = "ogiewon"
                		deviceHandlerName = "HubDuino Component Alarm" 
                	break    
         		case "doorControl": 
                        deviceNameSpace = "ogiewon"
                		deviceHandlerName = "HubDuino Component Door Control"
                	break
         		case "ultrasonic": 
                        deviceNameSpace = "ogiewon"
                		deviceHandlerName = "HubDuino Component Ultrasonic Sensor"
                	break
         		case "presence": 
                		deviceHandlerName = "Generic Component Presence Sensor"
                	break
         		case "power": 
                		deviceHandlerName = "Generic Component Power Meter"
                	break
          		case "energy": 
                		deviceHandlerName = "Generic Component Energy Meter"
                	break
        		case "servo": 
                        deviceNameSpace = "ogiewon"
                		deviceHandlerName = "HubDuino Component Servo"
                	break
         		case "pressure": 
                        deviceNameSpace = "ogiewon"
                		deviceHandlerName = "HubDuino Component Pressure Measurement"
                	break
//         		case "soundPressureLevel": 
//                        deviceNameSpace = "ogiewon"
//                		deviceHandlerName = "Child Sound Pressure Level" 
//                	break        
         		case "valve": 
                        deviceNameSpace = "ogiewon"
                		deviceHandlerName = "HubDuino Component Valve" 
                	break        
         		case "windowShade": 
                		deviceHandlerName = "Generic Component Window Shade"
                	break        
//         		case "weight": 
//                        deviceNameSpace = "ogiewon"
//                		deviceHandlerName = "Child Weight Measurement" 
//                	break        
			default: 
                	log.error "No Child Device Handler case for ${deviceName}"
      		}
            if (deviceHandlerName != "") {
         		addChildDevice(deviceNameSpace, deviceHandlerName, "${device.id}-${deviceName}${deviceNumber}",
         			[label: "${device.displayName} (${deviceName}${deviceNumber})", 
                	 isComponent: false, 
                     name: "${deviceName}${deviceNumber}"])
        	}   
    	} catch (e) {
        	log.error "Child device creation failed with error = ${e}"
        	log.error "Child device creation failed. Please make sure that the '${deviceHandlerName}' driver is installed."
    	}
}

private boolean containsDigit(String s) {
    boolean containsDigit = false;

    if (s != null && !s.isEmpty()) {
		//if (logEnable) log.debug "containsDigit .matches = ${s.matches(".*\\d+.*")}"
		containsDigit = s.matches(".*\\d+.*")
    }
    return containsDigit
}

def deleteAllChildDevices() {
    log.info "Uninstalling all Child Devices"
    getChildDevices().each {
          deleteChildDevice(it.deviceNetworkId)
       }
}

def checkHubDuinoPresence() {
    def tmr = 900
    
    if (timeOut != null) {
        if (timeOut >= 600) {
            tmr = timeOut.toInteger()
        } else {
            tmr = 600
        }
    }
   
    if (now() >= state.parseLastRanAt + (tmr * 1000)) {
        //If the timeout exceeds the threshold, mark this Parent Device as 'not present' to allow action to be taken
        if (device.currentValue("presence") != "not present") {
            sendEvent(name: "presence", value: "not present", isStateChange: true, descriptionText: "No update received from HubDuino device in past ${timeOut} seconds")
        }
    }
}

private String convertIPtoHex(ipAddress) { 
    String hex = ipAddress.tokenize( '.' ).collect {  String.format( '%02x', it.toInteger() ) }.join()
    return hex.toUpperCase()

}

private String convertPortToHex(port) {
	String hexport = port.toString().format( '%04x', port.toInteger() )
    return hexport.toUpperCase()
}

//child device methods
def componentRefresh(cd) {
    log.info "received refresh request from ${cd.displayName}"
    runIn(3, 'refresh') //use runIn() to prevent slamming the Micontroller device with refresh call as each child is created. Only last the one will run.
}

def componentOn(cd){
    if (logEnable) log.debug "received ON request from DN = ${cd.displayName}, DNI = ${cd.deviceNetworkId}"
    def name = cd.deviceNetworkId.split("-")[-1]
    sendEthernet("${name} on")
}

def componentOff(cd){
    if (logEnable) log.debug "received OFF request from DN = ${cd.displayName}, DNI = ${cd.deviceNetworkId}"
    def name = cd.deviceNetworkId.split("-")[-1]
    sendEthernet("${name} off")
}

def componentSetLevel(cd, level){
    if (logEnable) log.debug "received setLevel request from DN = ${cd.displayName}, DNI = ${cd.deviceNetworkId}, level = ${level}"
    def name = cd.deviceNetworkId.split("-")[-1]
    sendEthernet("${name} ${level}")
}

void componentSetLevel(cd, level, transitionTime) {
    if (logEnable) log.debug "received setLevel request from DN = ${cd.displayName}, DNI = ${cd.deviceNetworkId}, level = ${level}, transition time = ${transitionTime}"
    def name = cd.deviceNetworkId.split("-")[-1]
    sendEthernet("${name} ${level}:${transitionTime}")
}

def componentStartLevelChange(cd, direction){
    if (logEnable) log.debug "received startLevelChange request from DN = ${cd.displayName}, DNI = ${cd.deviceNetworkId}, direction = ${direction}"
    def name = cd.deviceNetworkId.split("-")[-1]
    sendEthernet("${name} startLevelChange${direction}")
}

def componentStopLevelChange(cd){
    if (logEnable) log.debug "received stopLevelChange request from DN = ${cd.displayName}, DNI = ${cd.deviceNetworkId}"
    def name = cd.deviceNetworkId.split("-")[-1]
    sendEthernet("${name} stopLevelChange")
}

def componentStrobe(cd){
    if (logEnable) log.debug "received strobe request from DN = ${cd.displayName}, DNI = ${cd.deviceNetworkId}"
    def name = cd.deviceNetworkId.split("-")[-1]
    sendEthernet("${name} strobe")
}

def componentSiren(cd){
    if (logEnable) log.debug "received siren request from DN = ${cd.displayName}, DNI = ${cd.deviceNetworkId}"
    def name = cd.deviceNetworkId.split("-")[-1]
    sendEthernet("${name} siren")
}

def componentBoth(cd){
    if (logEnable) log.debug "received both request from DN = ${cd.displayName}, DNI = ${cd.deviceNetworkId}"
    def name = cd.deviceNetworkId.split("-")[-1]
    sendEthernet("${name} both")
}

def componentClose(cd){
    if (logEnable) log.debug "received close request from DN = ${cd.displayName}, DNI = ${cd.deviceNetworkId}"
    def name = cd.deviceNetworkId.split("-")[-1]
    sendEthernet("${name} close")
}

def componentOpen(cd){
    if (logEnable) log.debug "received open request from DN = ${cd.displayName}, DNI = ${cd.deviceNetworkId}"
    def name = cd.deviceNetworkId.split("-")[-1]
    sendEthernet("${name} open")
}

def componentSetPosition(cd, position){
    if (logEnable) log.debug "received setPosition request from DN = ${cd.displayName}, DNI = ${cd.deviceNetworkId}, position = ${position}"
    def name = cd.deviceNetworkId.split("-")[-1]
    sendEthernet("${name} ${position}")
}
    
def componentStartPositionChange(cd, position){
    if (logEnable) log.debug "received startPositionChange request from DN = ${cd.displayName}, DNI = ${cd.deviceNetworkId}, position = ${position}"
    def name = cd.deviceNetworkId.split("-")[-1]
    sendEthernet("${name} start${position}")
}    
    
def componentStopPositionChange(cd){
    if (logEnable) log.debug "received stopPositionChange request from DN = ${cd.displayName}, DNI = ${cd.deviceNetworkId}"
    def name = cd.deviceNetworkId.split("-")[-1]
    sendEthernet("${name} stop")
}