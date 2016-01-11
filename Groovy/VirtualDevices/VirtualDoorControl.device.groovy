/**
 *  Virtual Door Control Device Type - VirtualDoorControl.device.groovy
 *
 *  Copyright 2015 Daniel Ogorchock
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
 *    2015-01-10  Dan Ogorchock  Original Creation
 *    2015-06-06  Dan Ogorchock  Added "Momentary" capability with "push()" command
 *
 *
 */
metadata {
	definition (name: "Virtual Door Control", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Door Control"
		capability "Contact Sensor"
		capability "Actuator"
		capability "Sensor"
		capability "Momentary"
        
		command "opening"
		command "closing"
		command "actuate"
	}

	simulator {
		// TODO: define status and reply messages here
	}

	tiles {
		standardTile("doorControl", "device.doorControl", width: 2, height: 2, canChangeIcon: true) {
			state "closed", label: 'Closed', action: "actuate", icon: "st.doors.garage.garage-closed", backgroundColor: "#79b821", nextState: "closed"
        	state "open", label: 'Open', action: "actuate", icon: "st.doors.garage.garage-open", backgroundColor: "#ffa81e", nextState: "open"
        	state "opening", label: 'Opening', action: "actuate", icon: "st.doors.garage.garage-opening", backgroundColor: "89C2E8", nextState: "opening"
        	state "closing", label: 'Closing', action: "actuate", icon: "st.doors.garage.garage-closing", backgroundColor: "89C2E8", nextState: "closing"
 		}
		standardTile("contact", "device.contact", width: 1, height: 1) {
			state("open", label:'${name}', icon:"st.contact.contact.open", backgroundColor:"#ffa81e")
			state("closed", label:'${name}', icon:"st.contact.contact.closed", backgroundColor:"#79b821")
		}
 	main (["doorControl", "contact"])
	details (["doorControl", "contact"])
    }
}

// parse events into attributes
def parse(String description) {
	log.debug "Parsing '${description}'"
	// TODO: handle 'door' attribute

}

// handle commands
def open() {
    sendEvent(name: "contact", value: "open", isStateChange: true)
	sendEvent(name: "doorControl", value: "open", isStateChange: true)
}

def opening() {
	sendEvent(name: "doorControl", value: "opening", isStateChange: true)
}

def closing() {
	sendEvent(name: "doorControl", value: "closing", isStateChange: true)
}

def close() {
    sendEvent(name: "contact", value: "closed", isStateChange: true)
	sendEvent(name: "doorControl", value: "closed", isStateChange: true)
}

def actuate() {
	log.debug "Sending button press event"
	sendEvent(name: "buttonPress", value: "true", isStateChange: true, unit: "")
}

def push() {
	actuate()
}