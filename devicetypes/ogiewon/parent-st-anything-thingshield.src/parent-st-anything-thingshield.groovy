/**
 *  Parent_ST_Anything_Thingshield.groovy
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
 *    2018-02-25  Dan Ogorchock  Added Child Presence Sensor
 *    2018-06-05  Dan Ogorchock  Simplified Parent & Child Device Handlers
 *    2018-06-24  Dan Ogorchock  Added Child Servo
 *    2018-07-01  Dan Ogorchock  Added Pressure Measurement
 *	
 */
 
metadata {
	definition (name: "Parent_ST_Anything_Thingshield", namespace: "ogiewon", author: "Dan Ogorchock") {
		capability "Configuration"
        capability "Refresh"
        capability "Button"
        capability "Holdable Button"
        
        command "sendData", ["string"]
    }

    simulator {
    }

    // Preferences
	preferences {
		input "numButtons", "number", title: "Number of Buttons", description: "Number of Buttons to be implemented", defaultValue: 0, required: true, displayDuringSetup: true
	}

	// Tile Definitions
	tiles (scale: 2){
		standardTile("refresh", "device.refresh", inactiveLabel: false, decoration: "flat", width: 2, height: 2) {
			state "default", label:'Refresh', action: "refresh.refresh", icon: "st.secondary.refresh-icon"
		}
        
		standardTile("configure", "device.configure", inactiveLabel: false, decoration: "flat", width: 2, height: 2) {
			state "configure", label:'Configure', action:"configuration.configure", icon:"st.secondary.tools"
		}

        valueTile("numberOfButtons", "device.numberOfButtons", inactiveLabel: false, width: 2, height: 2) {
			state "numberOfButtons", label:'${currentValue} buttons', unit:""
		}
 
		childDeviceTiles("all")
	}
}

// parse events into attributes
def parse(String description) {
	//log.debug "Parsing '${description}'"
    def msg = zigbee.parse(description)?.text
	def parts = []
    def name = ""
    def value = ""

	try {
    	log.debug "Parsing: ${msg}"
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
		log.debug "Parsing name: ${name}"
		def nameparts = name.split("\\d+", 2)
		def namebase = nameparts.length>0?nameparts[0].trim():null
        def namenum = name.substring(namebase.length()).trim()
		
        def results = []
        
		if (name.startsWith("button")) {
			//log.debug "In parse:  name = ${name}, value = ${value}, btnName = ${name}, btnNum = ${namemun}"
        	results = createEvent([name: namebase, value: value, data: [buttonNumber: namenum], descriptionText: "${namebase} ${namenum} was ${value} ", isStateChange: true, displayed: true])
			log.debug results
			return results
        }

        def isChild = containsDigit(name)
   		//log.debug "Name = ${name}, isChild = ${isChild}, namebase = ${namebase}, namenum = ${namenum}"      
        //log.debug "parse() childDevices.size() =  ${childDevices.size()}"

		def childDevice = null

		try {

            childDevices.each {
				try{
            		//log.debug "1-Looking for child with deviceNetworkID = ${device.deviceNetworkId}-${name} against ${it.deviceNetworkId}"
                	if (it.deviceNetworkId == "${device.deviceNetworkId}-${name}") {
                	childDevice = it
                    //log.debug "Found a match!!!"
                	}
            	}
            	catch (e) {
            	//log.debug e
            	}
        	}
            
            //If a child should exist, but doesn't yet, automatically add it!            
        	if (isChild && childDevice == null) {
        		//log.debug "isChild = true, but no child found - Auto Add it!"
            	//log.debug "    Need a ${namebase} with id = ${namenum}"
            
            	createChildDevice(namebase, namenum)
            	//find child again, since it should now exist!
            	childDevices.each {
					try{
            			//log.debug "2-Looking for child with deviceNetworkID = ${device.deviceNetworkId}-${name} against ${it.deviceNetworkId}"
                		if (it.deviceNetworkId == "${device.deviceNetworkId}-${name}") {
                			childDevice = it
                    		log.debug "Found a match!!!"
                		}
            		}
            		catch (e) {
            			//log.debug e
            		}
        		}
        	}
            
            if (childDevice != null) {
                //log.debug "parse() found child device ${childDevice.deviceNetworkId}"
                childDevice.parse("${namebase} ${value}")
				log.debug "${childDevice.deviceNetworkId} - name: ${namebase}, value: ${value}"
            }
            else  //must not be a child, perform normal update
            {
                results = createEvent(name: name, value: value)
                log.debug results
                return results
            }
		}
        catch (e) {
        	log.error "Error in parse() routine, error = ${e}"
        }

	}
}

def sendData(message) {
    sendThingShield(message) 
}

def sendThingShield(message) {
	log.debug "Executing 'sendThingShield' ${message}"
    def cmd = zigbee.smartShield(text: "${message}").format()
    sendHubCommand(new physicalgraph.device.HubAction(cmd))
}

// handle commands
def configure() {
	log.debug "Executing 'configure()'"
    refresh()
	sendEvent(name: "numberOfButtons", value: numButtons)
}

def refresh() {
	log.debug "Executing 'refresh()'"
    sendEvent(name: "numberOfButtons", value: numButtons)
	sendThingShield("refresh")
}

def installed() {
	log.debug "Executing 'installed()'"
}

def initialize() {
	log.debug "Executing 'initialize()'"
    sendEvent(name: "numberOfButtons", value: numButtons)
}

def updated() {
	if (!state.updatedLastRanAt || now() >= state.updatedLastRanAt + 5000) {
		state.updatedLastRanAt = now()
		log.debug "Executing 'updated()'"
    	runIn(2, "refresh")
        //refresh()
		sendEvent(name: "numberOfButtons", value: numButtons)
	}
	else {
//		log.trace "updated(): Ran within last 5 seconds so aborting."
	}
}


private void createChildDevice(String deviceName, String deviceNumber) {

	log.trace "createChildDevice:  Creating Child Device '${device.displayName} (${deviceName}${deviceNumber})'"

    try 
    {
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
            addChildDevice(deviceHandlerName, "${device.deviceNetworkId}-${deviceName}${deviceNumber}", null,
                           [completedSetup: true, label: "${device.displayName} (${deviceName}${deviceNumber})", 
                            isComponent: false, componentName: "${deviceName}${deviceNumber}", componentLabel: "${deviceName} ${deviceNumber}"])
        }   
    } catch (e) {
        log.error "Child device creation failed with error = ${e}"
        state.alertMessage = "Child device creation failed. Please make sure that the '${deviceHandlerName}' is installed and published."
        runIn(2, "sendAlert")
    }
}

private sendAlert() {
   sendEvent(
      descriptionText: state.alertMessage,
	  eventType: "ALERT",
	  name: "childDeviceCreation",
	  value: "failed",
	  displayed: true,
   )
}

private boolean containsDigit(String s) {
    boolean containsDigit = false;

    if (s != null && !s.isEmpty()) {
//		log.debug "containsDigit .matches = ${s.matches(".*\\d+.*")}"
		containsDigit = s.matches(".*\\d+.*")
    }
    return containsDigit
}