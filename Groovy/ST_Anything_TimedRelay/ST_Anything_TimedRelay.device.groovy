/**
 *  ST_Anything_TimedRelay Device Type - ST_Anything_TimedRelay.device.groovy
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
 */
metadata {
	definition (name: "ST_Anything_TimedRelay", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Momentary"
		capability "Switch"
	}

	simulator {
		// TODO: define status and reply messages here
	}

	tiles {
        standardTile("switch", "device.switch", width: 1, height: 1, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch.on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch.off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}

        main("switch")
        details("switch")

	}
}

// parse events into attributes
def parse(String description) {
	log.debug "Parsing '${description}'"
    def msg = zigbee.parse(description)?.text
    log.debug "Parse got '${msg}'"

    def parts = msg.split(" ")
    def name  = parts.length>0?parts[0].trim():null
    def value = parts.length>1?parts[1].trim():null

    name = value != "ping" ? name : null

    def result = createEvent(name: name, value: value)

    log.debug result

    return result
}


// handle commands
def push() {
	log.debug "Executing 'push'"
	zigbee.smartShield(text: "switch on").format()
}

def on() {
	log.debug "Executing 'switch on'"
	zigbee.smartShield(text: "switch on").format()
}

def off() {
	log.debug "Executing 'switch off'"
	zigbee.smartShield(text: "switch off").format()
}
