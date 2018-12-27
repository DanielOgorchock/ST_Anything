/**
 *   HubDuino_Parent_ThingShield.groovy
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
 *    2017-04-25  Dan Ogorchock  Updated to use the new Composite Device Handler feature
 *    2017-06-10  Dan Ogorchock  Added Dimmer Switch support
 *    2017-07-09  Dan Ogorchock  Added number of defined buttons tile
 *    2017-08-24  Allan (vseven) Change the way values are pushed to child devices to allow a event to be executed allowing future customization
 *    2018-02-15  Dan Ogorchock  Added @saif76's Ultrasonic Sensor *
 *    2018-02-25  Dan Ogorchock  Added Child Presence Sensor
 *    2018-03-03  Dan Ogorchock  Added Child Power Meter
 *    2018-06-02  Dan Ogorchock  Revised/Simplified for Hubitat Composite Driver Model
 *    2018-06-24  Dan Ogorchock  Added Child Servo
 *    2018-07-01  Dan Ogorchock  Added Pressure Measurement
 *    2018-09-22  Dan Ogorchock  Added preference for debug logging
 *	
 */
 
metadata {
	definition (name: "HubDuino Parent Thingshield", namespace: "ogiewon", author: "Dan Ogorchock") {
        capability "Configuration"
        capability "Refresh"
        capability "Pushable Button"
        capability "Holdable Button"
        //capability "DoubleTapableButton"
        
        command "sendData", ["string"]
	}

    simulator {
    }

    // Preferences
	preferences {
		input "numButtons", "number", title: "Number of Buttons", description: "Number of Buttons, 0 to n", required: true, displayDuringSetup: true
        input name: "logEnable", type: "bool", title: "Enable debug logging", defaultValue: true
	}
}

def logsOff(){
    log.warn "debug logging disabled..."
    device.updateSetting("logEnable",[value:"false",type:"bool"])
}

// parse events into attributes
def parse(String description) {
    if (logEnable) log.debug "Parsing: ${msg}"
    def msg = parseThingShield(description)
	def parts = []
    def name = ""
    def value = ""

	try {
    	if (logEnable) log.debug "Parsing: ${msg}"
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
        
		if (name.startsWith("button")) {
			//if (logEnable) log.debug "In parse:  name = ${name}, value = ${value}, btnName = ${name}, btnNum = ${namemun}"
        	results << createEvent(name: value, value: namenum, isStateChange: true)
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
            		//if (logEnable) log.debug "1-Looking for child with deviceNetworkID = ${device.deviceNetworkId}-${name} against ${it.deviceNetworkId}"
                	if (it.deviceNetworkId == "${device.deviceNetworkId}-${name}") {
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
            			//if (logEnable) log.debug "2-Looking for child with deviceNetworkID = ${device.deviceNetworkId}-${name} against ${it.deviceNetworkId}"
                		if (it.deviceNetworkId == "${device.deviceNetworkId}-${name}") {
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
                //childDevice.generateEvent(namebase, value)
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

def fromHexString(String hex) {
    StringBuilder str = new StringBuilder();
    for (int i = 0; i < hex.length(); i+=2) {
        str.append((char) Integer.parseInt(hex.substring(i, i + 2), 16));
    }
    return str.toString();
}

def parseThingShield(String description) {
    def resultMap = zigbee.parseDescriptionAsMap(description)
    if (resultMap.attrId) {
    	return fromHexString(resultMap.attrId.substring(0,2)) + fromHexString(resultMap.encoding) + fromHexString(resultMap.value)
    } 
    else {
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
def configure() {
	if (logEnable) log.debug "Executing 'configure()'"
    refresh()
	sendEvent(name: "numberOfButtons", value: numButtons)
}

def refresh() {
	if (logEnable) log.debug "Executing 'refresh()'"
	sendThingShield("refresh")
}

def installed() {
	log.info "Executing 'installed()'"
}

def initialize() {
	log.info "Executing 'initialize()'"
    sendEvent(name: "numberOfButtons", value: numButtons)
}

def updated() {
	if (!state.updatedLastRanAt || now() >= state.updatedLastRanAt + 5000) {
		state.updatedLastRanAt = now()
		log.info "Executing 'updated()'"
    	runIn(2, "refresh")
        //refresh()
		sendEvent(name: "numberOfButtons", value: numButtons)
	}
	else {
//		log.trace "updated(): Ran within last 5 seconds so aborting."
	}
    
    if (logEnable) runIn(1800,logsOff)
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
         		case "servo": 
                		deviceHandlerName = "Child Servo" 
                	break
         		case "pressure": 
                		deviceHandlerName = "Child Pressure Measurement" 
                	break
			default: 
                		log.error "No Child Device Handler case for ${deviceName}"
      		}
            if (deviceHandlerName != "") {
         		addChildDevice(deviceHandlerName, "${device.deviceNetworkId}-${deviceName}${deviceNumber}",
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
