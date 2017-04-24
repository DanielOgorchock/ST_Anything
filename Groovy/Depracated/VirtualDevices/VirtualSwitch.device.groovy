/**
 *  Virtual Switch Device Type - VirtualSwitch.device.groovy
 *
 *  Copyright 2014 Daniel Ogorchock
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
 *    2015-03-27  Dan Ogorchock  Original Creation
 *
 *
 */
 
metadata {
	definition (name: "Virtual Switch", namespace: "ogiewon", author: "Daniel Ogorchock") {
        capability "Actuator"
        capability "Switch"
        capability "Relay Switch"
	}

	simulator {
		// TODO: define status and reply messages here
	}

	tiles {
    	standardTile("switch", "device.switch", width: 2, height: 2, canChangeIcon: true) {
        	state "on", label: '${name}', action: "switch.off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
        	state "off", label: '${name}', action: "switch.on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"

		}
        
	main(["switch"])
	details(["switch"])
	}
}

// parse events into attributes
def parse(String description) {
	log.debug "Parsing '${description}'"
	// TODO: handle 'contact' attribute

}


// handle commands
def on() {
	log.debug "Virtual Switch - On"
    sendEvent (name: "switch", value: "on", isStateChange: true)
}

def off() {
	log.debug "Virtual Switch - Off"
    sendEvent (name: "switch", value: "off", isStateChange: true)
}