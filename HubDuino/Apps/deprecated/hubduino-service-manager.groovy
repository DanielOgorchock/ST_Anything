/**
 *  HubDuino Service Manager
 *
 *  Copyright 2018 Daniel Ogorchock
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
 *    2018-02-08  Dan Ogorchock  Original Creation
 *    2018-02-15  Dan Ogorchock  Added @saif76's Ultrasonic Sensor *
 *    2018-02-25  Dan Ogorchock  Added Child Presence Sensor
 *    2018-03-03  Dan Ogorchock  Added Child Power Meter
 *	
 */
 
definition(
    name: "HubDuino Service Manager",
    namespace: "ogiewon",
    author: "Daniel Ogorchock",
    description: "Connects HubDuino Bridge to virtual Child Devices",
    category: "My Apps",
    iconUrl: "https://s3.amazonaws.com/smartapp-icons/Convenience/Cat-Convenience.png",
    iconX2Url: "https://s3.amazonaws.com/smartapp-icons/Convenience/Cat-Convenience@2x.png",
    iconX3Url: "https://s3.amazonaws.com/smartapp-icons/Convenience/Cat-Convenience@2x.png")

preferences {
	section("Select the HubDuino Bridge Device") {
		input "HubDuino", "capability.sensor"
    }    
}

def installed() {
	log.debug "Installed with settings: ${settings}"
	subscribe()
    HubDuino.refresh()
}

def uninstalled() {
    childDevices.each { deleteChildDevice(it.deviceNetworkId) }
}

def updated() {
	log.debug "Updated with settings: ${settings}"
	unsubscribe()
	subscribe()
    HubDuino.refresh()
}

def subscribe() {
    subscribe(HubDuino, "all", childHandler)
}

def childHandler(evt) {
	def parts = evt.value.split(":")
	def name  = parts.length>0?parts[0].trim():null
	def value = parts.length>0?parts[1].trim():null
    log.debug "HubDuino Event(name: ${name}, value: ${value}, deviceID: ${evt.deviceId})"
    
	def nameparts = name.split("\\d+", 2)
	def namebase = nameparts.length>0?nameparts[0].trim():null
    def namenum = name.substring(namebase.length()).trim()   
    
    def isChild = containsDigit(name)
	def childDevice = null

		try {

            childDevices.each {
				try{
            		//log.debug "Looking for child with app.id = ${HubDuino.deviceNetworkId}-${name} against ${it.deviceNetworkId}"
                	if (it.deviceNetworkId == "${HubDuino.deviceNetworkId}-${name}") {
                	childDevice = it
                    //log.debug "Found a match!!!"
                	}
            	}
            	catch (e) {
            		log.debug e
            	}
        	}
            
            //If a child should exist, but doesn't yet, automatically add it!            
        	if (isChild && childDevice == null) {
        		log.debug "isChild = true, but no child found - Auto Add it!"
            	log.debug "    Need a ${namebase} with id = ${namenum}"
            
            	createChildDevice(namebase, namenum)
            	//find child again, since it should now exist!
            	childDevices.each {
					try{
            			//log.debug "Looking for child with deviceNetworkID = ${HubDuino.deviceNetworkId}-${name} against ${it.deviceNetworkId}"
                		if (it.deviceNetworkId == "${HubDuino.deviceNetworkId}-${name}") {
                			childDevice = it
                    		//log.debug "Found a match!!!"
                		}
            		}
            		catch (e) {
            			log.debug e
            		}
        		}
        	}
            
            if (childDevice != null) {
                //log.debug "childHandler() found child device ${childDevice.deviceNetworkId}"
                
                childDevice.generateEvent(namebase, value)
				log.debug "${childDevice.deviceNetworkId} - name: ${namebase}, value: ${value}"

            }
            else  //must not be a child, perform normal update
            {
                //log.debug "Error in childHandler() - not a child device!"
            }
		}
        catch (e) {
        	log.error "Error in childHandler() routine, error = ${e}"
        }   
}

def childOn(String dni) {
    def name = dni.split("-")[-1]
    log.debug "childOn($dni), name = ${name}"
    HubDuino.sendEthernet("${name} on")
}

def childOff(String dni) {
    def name = dni.split("-")[-1]
    log.debug "childOff($dni), name = ${name}"
    HubDuino.sendEthernet("${name} off")
}

def childAlarmOn(String dni) {
    def name = dni.split("-")[-1]
    log.debug "childAlarmOn($dni), name = ${name}"
    HubDuino.sendEthernet("${name} both")
}

def childAlarmSiren(String dni) {
    def name = dni.split("-")[-1]
    log.debug "childAlarmOn($dni), name = ${name}"
    HubDuino.sendEthernet("${name} siren")
}

def childAlarmStrobe(String dni) {
    def name = dni.split("-")[-1]
    log.debug "childAlarmOn($dni), name = ${name}"
    HubDuino.sendEthernet("${name} strobe")
}

def childAlarmBoth(String dni) {
    def name = dni.split("-")[-1]
    log.debug "childAlarmOn($dni), name = ${name}"
    HubDuino.sendEthernet("${name} both")
}

def childAlarmOff(String dni) {
    def name = dni.split("-")[-1]
    log.debug "childAlarmOff($dni), name = ${name}"
    HubDuino.sendEthernet("${name} off")
}

def childAlarmTest(String dni) {
    def name = dni.split("-")[-1]
    log.debug "childAlarmTest($dni), name = ${name}"
    HubDuino.sendEthernet("${name} both")
	runIn(3, childAlarmTestOff, [data: [devicenetworkid: dni]])
}

def childAlarmTestOff(data) {
	childAlarmOff(data.devicenetworkid)
}

def childDoorOpen(String dni) {
    def name = dni.split("-")[-1]
    log.debug "childDoorOpen($dni), name = ${name}"
    HubDuino.sendEthernet("${name} on")
}

def childDoorClose(String dni) {
    def name = dni.split("-")[-1]
    log.debug "childDoorClose($dni), name = ${name}"
    HubDuino.sendEthernet("${name} on")
}

def childSetLevel(String dni, value) {
    def name = dni.split("-")[-1]
    log.debug "childSetLevel($dni), name = ${name}, level = ${value}"
    HubDuino.sendEthernet("${name} ${value}")
}

def childSetColorRGB(String dni, value) {
    def name = dni.split("-")[-1]
    log.debug "childSetColorRGB($dni), name = ${name}, colorRGB = ${value}"
    HubDuino.sendEthernet("${name} ${value}")
}

def childSetColorRGBW(String dni, value) {
    def name = dni.split("-")[-1]
    log.debug "childSetColorRGBW($dni), name = ${name}, colorRGBW = ${value}"
    HubDuino.sendEthernet("${name} ${value}")
}

def childRelayOn(String dni) {
    def name = dni.split("-")[-1]
    log.debug "childRelayOn($dni), name = ${name}"
    HubDuino.sendEthernet("${name} on")
}

def childRelayOff(String dni) {
    def name = dni.split("-")[-1]
    log.debug "childRelayOff($dni), name = ${name}"
    HubDuino.sendEthernet("${name} off")
}

private void createChildDevice(String deviceName, String deviceNumber) {
    
		log.trace "createChildDevice:  Creating Child Device '${HubDuino.deviceNetworkId}-${deviceName}${deviceNumber}'"
        
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
			default: 
                		log.error "No Child Device Handler case for ${deviceName}"
      		}
            if (deviceHandlerName != "") {
    			def child = addChildDevice("ogiewon", deviceHandlerName, "${HubDuino.deviceNetworkId}-${deviceName}${deviceNumber}", null, [name: "${app.label}-${deviceName}${deviceNumber}", label: "${app.label} ${deviceName}${deviceNumber}", completedSetup: true])
        	}   
    	} catch (e) {
        	log.error "Child device creation failed with error = ${e}"
        	log.debug "Child device creation failed. Please make sure that the '${deviceHandlerName}' is installed and published."
	    	
    	}
}

private boolean containsDigit(String s) {
    boolean containsDigit = false;

    if (s != null && !s.isEmpty()) {
//		log.debug "containsDigit .matches = ${s.matches(".*\\d+.*")}"
		containsDigit = s.matches(".*\\d+.*")
    }
    return containsDigit
}

def initialize() {
	// TODO: subscribe to attributes, devices, locations, etc.
}
