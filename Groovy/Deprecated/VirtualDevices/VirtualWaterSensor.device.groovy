/**
 *  Virtual Water Sensor Device Type - VirtualWaterSensor.device.groovy
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
 *    2015-12-29  Dan Ogorchock  Original Creation
 *
 *
 */
 
metadata {
	definition (name: "Virtual Water Sensor", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Water Sensor"
        
		command "dry"
		command "wet"
	}

	simulator {
		// TODO: define status and reply messages here
	}

	tiles {
		standardTile("water", "device.water", width: 1, height: 1) {
			state "dry", icon:"st.alarm.water.dry", backgroundColor:"#ffffff"
			state "wet", icon:"st.alarm.water.wet", backgroundColor:"#53a7c0"
		}
        
	main(["water"])
	details(["water"])
	}
}

// parse events into attributes
def parse(String description) {
	log.debug "Parsing '${description}'"
	// TODO: handle 'contact' attribute

}

def dry() {
    	log.debug "Virtual Water Dry"
    	sendEvent(name: "water", value: "dry", isStateChange: true)
}

def wet() {
    	log.debug "Virtual Water Wet"
    	sendEvent(name: "water", value: "wet", isStateChange: true)
}

