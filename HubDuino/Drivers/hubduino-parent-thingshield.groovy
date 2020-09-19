/**
 *  HubDuino_Parent_ThingShield.groovy
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/hubduino-parent-thingshield.groovy
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
 *    2017-02-24  Dan Ogorchock  Created the new "Multiples" device handler as a new example
 *    2017-04-16  Dan Ogorchock  Updated to use the new Composite Device Handler feature
 *    2017-06-10  Dan Ogorchock  Added Dimmer Switch support
 *    2017-07-09  Dan Ogorchock  Added number of defined buttons tile
 *    2017-08-24  Allan (vseven) Change the way values are pushed to child devices to allow a event to be executed allowing future customization
 *    2007-09-24  Allan (vseven) Added RGB LED light support with a setColorRGB routine
 *    2017-10-07  Dan Ogorchock  Cleaned up formatting for readability
 *    2017-09-24  Allan (vseven) Added RGBW LED strip support with a setColorRGBW routine
 *    2017-12-29  Dan Ogorchock  Added WiFi RSSI value per request from ST user @stevesell
 *    2018-02-25  Dan Ogorchock  Added Child Presence Sensor
 *    2018-03-03  Dan Ogorchock  Added Child Power Meter
 *    2018-06-02  Dan Ogorchock  Revised/Simplified for Hubitat Composite Driver Model
 *    2018-06-24  Dan Ogorchock  Added Child Servo
 *    2018-07-01  Dan Ogorchock  Added Pressure Measurement
 *    2018-09-22  Dan Ogorchock  Added preference for debug logging
 *    2019-02-05  Dan Ogorchock  Added Child Energy Meter
 *    2019-04-23  Dan Ogorchock  Fixed debug logging, added importURL, and added Fingerprint
 *    2019-04-24  Dan Ogorchock  Improved parseThingShield() routine to support Hubitat firmware changes
 *    2019-06-24  Dan Ogorchock  Added Delete All Child Devices Command (helpful during testing)
 *    2019-07-08  Dan Ogorchock  Added support for Sound Pressure Level device
 *    2019-09-01  Dan Ogorchock  Added Presence Capability to know if the HubDuino device is online or offline
 *    2019-09-04  Dan Ogorchock  Automatically detect maximum number of buttons and set numberOfButtons attribute accordingly
 *    2019-10-30  Dan Ogorchock  Added Child Valve
 *    2020-02-08  Dan Ogorchock  Added refresh() call to initialize() command
 *    2020-06-09  Dan Ogorchock  Improved HubDuino board 'Presence' logic
 *    2020-06-25  Dan Ogorchock  Added Window Shade
 *    2020-09-19  Dan Ogorchock  Added "Releasable Button" Capability (requires new Arduino IS_Button.cpp and .h code)
 *	
 */
 
metadata {
	definition (name: "HubDuino Parent Thingshield", namespace: "ogiewon", author: "Dan Ogorchock", importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/hubduino-parent-thingshield.groovy") {
        capability "Refresh"
        capability "Pushable Button"
        capability "Holdable Button"
        capability "Releasable Button"
        capability "Presence Sensor"  //used to determine is the HubDuino microcontroller is still reporting data or not
       
        command "sendData", ["string"]
        //command "deleteAllChildDevices"
		
		fingerprint profileId: "0104", deviceId: "0138", inClusters: "0000"
	}

    // Preferences
	preferences {
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
	def msg = parseThingShield(description)
	def parts = []
    def name = ""
    def value = ""

	try {
    	if (logEnable) log.debug "msg=  ${msg}"
    	parts = msg.split(" ")
    	name  = parts.length>0?parts[0].trim():null
    	value = parts.length>1?parts[1].trim():null
    	name = value != "ping" ? name : null
 	}
    catch (e) {
        //log.error "Error in parse() routine, error = ${e}"
        return
    }
    
	if (name != "ping") {  
		if (logEnable) log.debug "Parsing name: ${name}"
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

        def isChild = containsDigit(name)
   		//if (logEnable) log.debug "Name = ${name}, isChild = ${isChild}, namebase = ${namebase}, namenum = ${namenum}"      
        //if (logEnable) log.debug "parse() childDevices.size() =  ${childDevices.size()}"

		def childDevice = null

		try {

            childDevices.each {
				try{
                	if ((it.deviceNetworkId == "${device.id}-${name}") || (it.deviceNetworkId == "${device.deviceNetworkId}-${name}")) {
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
                		if ((it.deviceNetworkId == "${device.id}-${name}") || (it.deviceNetworkId == "${device.deviceNetworkId}-${name}")) {
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
                childDevice.parse("${namebase} ${value}")
				if (logEnable) log.debug "${childDevice.deviceNetworkId} - name: ${namebase}, value: ${value}"
            }
            else  //must not be a child, perform normal update
            {
                results = createEvent(name: name, value: value)
                if (logEnable) log.debug results
                return results
            }
		}
        catch (e) {
        	log.error "Error in parse() routine, error = ${e}"
        }

	}
}

def parseThingShield(String description) {
    if(description?.startsWith('catchall')) {
        def data = zigbee.parseDescriptionAsMap(description).data
        StringBuilder str = new StringBuilder()
        // start at 1, ignore first item, its always 0A
        for (int i = 1; i < data.size(); i++) {
            str.append((char) Integer.parseInt(data[i], 16))
        }
        return str.toString()
    } else if (description?.startsWith('read attr - ')) {
        // parse read attr message manually
        def descArr = description.substring("read attr - ".length()).split(',')
        def retMap = [:]
        descArr.each { listItem ->
            def keyValue = listItem.split(':')
            retMap.put(keyValue[0].trim(), keyValue[1].trim())
        }
        String data = retMap['raw'].substring(14)

        StringBuilder str = new StringBuilder()
        for (int i = 0; i < data.length(); i+=2) {
            str.append((char) Integer.parseInt(data.substring(i, i + 2), 16))
        }
        return str.toString()
    } else {
        return description
    }
}

def sendData(String message) {
    if (logEnable) log.debug "sendData(${message}) called"
    sendThingShield(message)
}

def sendThingShield(String message) {
    def encodedString = DataType.pack(message, DataType.STRING_CHAR).substring(2)
	return "he raw 0x${device.deviceNetworkId} 1 0x${device.endpointId} 0x0 {00000a0a" + encodedString + "}"
}

// handle commands
def refresh() {
	if (logEnable) log.debug "Executing 'refresh()'"
	sendThingShield("refresh")
}

def installed() {
	log.info "Executing 'installed()'"
    state.numButtons = 0
    sendEvent(name: "numberOfButtons", value: state.numButtons)
}

def uninstalled() {
    log.info "Executing 'uninstalled()'"
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
        
		try {
        	def deviceHandlerName = ""
        	switch (deviceName) {
         		case "contact": 
                		deviceHandlerName = "Child Contact Sensor" 
                	break
         		case "switch": 
                		deviceHandlerName = "Child Switch" 
                	break
         		case "dimmerSwitch": 
                		deviceHandlerName = "Child Dimmer Switch" 
                	break
         		case "rgbSwitch": 
                		deviceHandlerName = "Child RGB Switch" 
                	break
         		case "generic": 
                		deviceHandlerName = "Child Generic Sensor" 
                	break
         		case "rgbwSwitch": 
                		deviceHandlerName = "Child RGBW Switch" 
                	break
         		case "relaySwitch": 
                		deviceHandlerName = "Child Relay Switch" 
                	break
         		case "temperature": 
                		deviceHandlerName = "Child Temperature Sensor" 
                	break
         		case "humidity": 
                		deviceHandlerName = "Child Humidity Sensor" 
                	break
         		case "motion": 
                		deviceHandlerName = "Child Motion Sensor" 
                	break
         		case "water": 
                		deviceHandlerName = "Child Water Sensor" 
                	break
         		case "illuminance": 
                		deviceHandlerName = "Child Illuminance Sensor" 
                	break
         		case "illuminancergb": 
                		deviceHandlerName = "Child IlluminanceRGB Sensor" 
                	break
         		case "voltage": 
                		deviceHandlerName = "Child Voltage Sensor" 
                	break
         		case "smoke": 
                		deviceHandlerName = "Child Smoke Detector" 
                	break    
         		case "carbonMonoxide": 
                		deviceHandlerName = "Child Carbon Monoxide Detector" 
                	break    
         		case "alarm": 
                		deviceHandlerName = "Child Alarm" 
                	break    
         		case "doorControl": 
                		deviceHandlerName = "Child Door Control" 
                	break
         		case "ultrasonic": 
                		deviceHandlerName = "Child Ultrasonic Sensor" 
                	break
         		case "presence": 
                		deviceHandlerName = "Child Presence Sensor" 
                	break
         		case "power": 
                		deviceHandlerName = "Child Power Meter" 
                	break
         		case "energy": 
                		deviceHandlerName = "Child Energy Meter" 
                	break
         		case "servo": 
                		deviceHandlerName = "Child Servo" 
                	break
         		case "pressure": 
                		deviceHandlerName = "Child Pressure Measurement" 
                	break
         		case "soundPressureLevel": 
                		deviceHandlerName = "Child Sound Pressure Level" 
                	break        
         		case "valve": 
                		deviceHandlerName = "Child Valve" 
                	break        
         		case "windowShade": 
                		deviceHandlerName = "Child Window Shade" 
                	break        
			default: 
                		log.error "No Child Device Handler case for ${deviceName}"
      		}
            if (deviceHandlerName != "") {
         		addChildDevice(deviceHandlerName, "${device.id}-${deviceName}${deviceNumber}",
         			[label: "${device.displayName} (${deviceName}${deviceNumber})", 
                	 isComponent: false, 
                     name: "${deviceName}${deviceNumber}"])
        	}   
    	} catch (e) {
        	log.error "Child device creation failed with error = ${e}"
        	log.error "Child device creation failed. Please make sure that the '${deviceHandlerName}' is installed and published."
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
