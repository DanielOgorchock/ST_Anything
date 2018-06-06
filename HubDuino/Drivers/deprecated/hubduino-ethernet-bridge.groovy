/**
 *  HubDuino Ethernet Bridge - Driver
 *
 *  Copyright 2018 Dan G Ogorchock 
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
 *    2018-02-07  Dan Ogorchock  Original Creation
 *    2018-03-03  Dan Ogorchock  Added custom command declaration
 *    2018-04-12  Dan Ogorchock  Fixed URL space encoding issue caused by Hubitat firmware upgrade (v707?)
 *    2018-05-19  Dan Ogorchock  Fixed refresh() command that was broken by previous space encoding fix  
 *
 */
metadata {
	definition (name: "HubDuino Ethernet Bridge", namespace: "ogiewon", author: "Dan Ogorchock") {
//        capability "Bridge"
        capability "Sensor"
        capability "Configuration"
        capability "Refresh"
        capability "Pushable Button"
        capability "Holdable Button"
        capability "DoubleTapableButton"
        capability "Signal Strength" }

        command "sendEthernet"
    
    // Preferences
	preferences {
		input "ip", "text", title: "Arduino IP Address", description: "IP Address in form 192.168.1.226", required: true, displayDuringSetup: true
		input "port", "text", title: "Arduino Port", description: "port in form of 8090", required: true, displayDuringSetup: true
		input "mac", "text", title: "Arduino MAC Addr", description: "MAC Address in form of 02A1B2C3D4E5", required: true, displayDuringSetup: true
		input "numButtons", "number", title: "Number of Buttons", description: "Number of Buttons, 0 to n", required: true, displayDuringSetup: true
	}

}

// parse events into attributes
def parse(String description) {
	//log.debug "Parsing '${description}'"
	def msg = parseLanMessage(description)
	def headerString = msg.header

	if (!headerString) {
		//log.debug "headerString was null for some reason :("
    }
    else {
		def bodyString = msg.body

		if (bodyString) {
        	//log.debug "bodyString: $bodyString"
    		def parts = bodyString.split(" ")
    		def name  = parts.length>0?parts[0].trim():null
    		def value = parts.length>0?parts[1].trim():null
            
            def nameparts = name.split("\\d+", 2)
			def namebase = nameparts.length>0?nameparts[0].trim():null
        	def namenum = name.substring(namebase.length()).trim()
            
            def results = []
            
            if (name.startsWith("button")) {
				//log.debug "In parse:  name = ${name}, value = ${value}, btnNum = " + namenum
        		results << createEvent(name: value, value: namenum)             
        	}
			else {
			    results << createEvent(name: name, value: value)
                if (!name.startsWith("rssi")) { 
                    results << createEvent(name: "all", value: "${name}:${value}") 
                }
            }
            log.debug "results = " + results
			return results
		}
        else {
        	//log.debug "bodyString was null for some reason :("
        }
    }
}

private getHostAddress() {
    def ip = settings.ip
    def port = settings.port

	log.debug "Using ip: ${ip} and port: ${port} for device: ${device.id}"
    return ip + ":" + port
}

def sendEthernet(message) {
    if (message.contains(" ")) {
        def parts = message.split(" ")
        def name  = parts.length>0?parts[0].trim():null
        def value = parts.length>0?parts[1].trim():null
        message = name + "%20" + value
    }
	log.debug "Executing 'sendEthernet' ${message}"
	if (settings.ip != null && settings.port != null) {
    	new hubitat.device.HubAction(
    		method: "POST",
    		path: "/${message}?",
    		headers: [ HOST: "${getHostAddress()}" ]
		)
    }
    else {
    	log.debug "HubDuino Ethernet Bridge Device: Please verify IP address and Port are configured."    
    }
}

// handle commands
def push(button) {
    log.debug "push(${button})"
    sendEvent(name: "pushed", value: button)
}

def hold(button) {
    log.debug "hold(${button})"    
    sendEvent(name: "hold", value: button)
}

def doubleTap(button) {
    log.debug "doubleTap(${button})"    
    sendEvent(name: "doubleTapped", value: button)
}
   

def configure() {
	log.debug "Executing 'configure()'"
    updateDeviceNetworkID()
	sendEvent(name: "numberOfButtons", value: numButtons)
}

def refresh() {
	log.debug "Executing 'refresh()'"
	sendEthernet("refresh")
}


def installed() {
	log.debug "Executing 'installed()'"
    if ( device.deviceNetworkId =~ /^[A-Z0-9]{12}$/)
    {
        if (numButtons) { sendEvent(name: "numberOfButtons", value: numButtons) }
    }
    else
    {
        log.error "HubDuino Ethernet Bridge Device has not been fully configured."
    }
}

def initialize() {
	log.debug "Executing 'initialize()'"
    sendEvent(name: "numberOfButtons", value: numButtons)
}

def updated() {
	if (!state.updatedLastRanAt || now() >= state.updatedLastRanAt + 5000) {
		state.updatedLastRanAt = now()
		log.debug "Executing 'updated()'"
    	updateDeviceNetworkID()
		sendEvent(name: "numberOfButtons", value: numButtons)
        log.debug "Hub IP Address = ${device.hub.getDataValue("localIP")}"
        log.debug "Hub Port = ${device.hub.getDataValue("localSrvPortTCP")}"
	}
	else {
		log.trace "updated(): Ran within last 5 seconds so aborting."
	}
}


def updateDeviceNetworkID() {
	log.debug "Executing 'updateDeviceNetworkID'"
    if(device.deviceNetworkId!=mac) {
    	log.debug "setting deviceNetworkID = ${mac}"
        device.setDeviceNetworkId("${mac}")
	}
    //Need deviceNetworkID updated BEFORE we can create Child Devices
	//Have the Arduino send an updated value for every device attached.  This will auto-created child devices!
	refresh()
}

