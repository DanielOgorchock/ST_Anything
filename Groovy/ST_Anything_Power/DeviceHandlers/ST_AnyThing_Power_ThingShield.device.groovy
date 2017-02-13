/**
 *  ST_AnyThing_Power.device.groovy
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
 *    2015-04-01  Dan Ogorchock  Original Creation
 *
 */
 
metadata {
	definition (name: "ST_Anything_Power_ThingShield", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Configuration"
		capability "Power Meter"
		capability "Sensor"
		capability "Polling"
	}

	simulator {
	}

    // Preferences
	preferences {
		input "powerSampleRate", "number", title: "Power Meter Sampling Interval (seconds)", description: "Sampling Interval (seconds)", defaultValue: 30, required: true, displayDuringSetup: true
    }

	// Tile Definitions
	tiles {     
        valueTile("power", "device.power", width: 2, height: 2, decoration: "flat") {
			state "default", label:'${currentValue} W', unit:""
		}

		standardTile("configure", "device.configure", inactiveLabel: false, decoration: "flat") {
			state "configure", label:'', action:"configuration.configure", icon:"st.secondary.configure"
		}

        main(["power"])
        details(["power","configure"])
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


def poll() {
	//temporarily implement poll() to issue a configure() command to send the polling interval settings to the arduino
	configure()
}


def configure() {
	log.debug "Executing 'configure'"
    log.debug "power" + powerSampleRate
    [
		zigbee.smartShield(text: "power " + powerSampleRate).format()
    ]
}