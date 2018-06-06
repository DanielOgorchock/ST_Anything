/**
 *  HubDuino ThingShield Bridge - Driver
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
 *    2018-02-09  Dan Ogorchock  Original Creation
 *    2018-03-03  Dan Ogorchock  Added custom command declaration
 *
 */
metadata {
	definition (name: "HubDuino ThingShield Bridge", namespace: "ogiewon", author: "Dan Ogorchock") {
//        capability "Bridge"
        capability "Sensor"
        capability "Refresh"
        capability "Pushable Button"
        capability "Holdable Button"
        capability "DoubleTapableButton" }

        command "sendEthernet"
    
    // Preferences
	preferences {
		input "numButtons", "number", title: "Number of Buttons", description: "Number of Buttons, 0 to n", required: true, displayDuringSetup: true
	}

}

def parse(String description) {
    //log.debug "Parse got '${description}'"
    def msg = parseThingShield(description)
    log.debug "parseThingShield() returned '${msg}'"
    
    if (!msg.startsWith("ping") && !msg.startsWith("catchall")) {

        def parts = msg.split(" ")
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
            results << createEvent(name: "all", value: "${name}:${value}") 
        }
        log.debug "results = " + results
		return results
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

def sendThingShield(String message) {
    def encodedString = DataType.pack(message, DataType.STRING_CHAR).substring(2)
	return "he raw 0x${device.deviceNetworkId} 1 0x${device.endpointId} 0x0 {00000a0a" + encodedString + "}"
}

//Slight hack for ThingShield support...this function allows the same service manager for both ThingShield and LAN devices 
def sendEthernet(String message) {
    sendThingShield(message)
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
   

def refresh() {
	log.debug "Executing 'refresh()'"
	sendThingShield("refresh")
}

def installed() {
	log.debug "Executing 'installed()'"
    if (numButtons) { sendEvent(name: "numberOfButtons", value: numButtons) }
}

def initialize() {
	log.debug "Executing 'initialize()'"
    sendEvent(name: "numberOfButtons", value: numButtons)
}

def updated() {
	if (!state.updatedLastRanAt || now() >= state.updatedLastRanAt + 5000) {
		state.updatedLastRanAt = now()
		log.debug "Executing 'updated()'"
		sendEvent(name: "numberOfButtons", value: numButtons)
	}
	else {
		log.trace "updated(): Ran within last 5 seconds so aborting."
	}
}

