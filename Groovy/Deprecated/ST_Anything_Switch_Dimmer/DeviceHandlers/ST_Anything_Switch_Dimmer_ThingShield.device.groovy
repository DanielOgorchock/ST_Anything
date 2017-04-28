/**
 *  ST_AnyThing_Switch_Dimmer.groovy
 *
 *  Copyright 2016 Dan G Ogorchock 
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
 *    2016-04-30  Dan Ogorchock  Original Creation
 *
 */
 
metadata {
	definition (name: "ST_AnyThing_Switch_Dimmer_ThingShield", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Switch"
		capability "Switch Level"
		capability "Sensor"
		capability "Actuator"
        }

        simulator {
 
        }

    // Preferences
	preferences {
	}

	// Tile Definitions
	tiles {        
		standardTile("switch", "device.switch", width: 1, height: 1, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch.on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch.off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}
		controlTile("levelSliderControl", "device.level", "slider", height: 1, width: 3, inactiveLabel: false) {
			state "level", action:"switch level.setLevel"
		}
		valueTile("level", "device.level", inactiveLabel: false, decoration: "flat") {
			state "level", label: 'Dim ${currentValue}'
		}
        
        main(["switch"])
        details(["switch", "levelSliderControl", "level"])
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

def on() {
	log.debug "Executing 'switch on'"
	zigbee.smartShield(text: "switch on").format()
}

def off() {
	log.debug "Executing 'switch off'"
	zigbee.smartShield(text: "switch off").format()
}

def setLevel(value) {
    sendEvent(name: "level", value: value)
    log.debug "Executing 'switch ${value}'"
    zigbee.smartShield(text: "switch ${value}").format()
}