/**
 *  Child Power Meter
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
 *    2018-03-03  Dan Ogorchock  Original Creation
 * 
 */
metadata {
	definition (name: "Child Power Meter", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Power Meter"
		capability "Sensor"

		attribute "lastUpdated", "String"

		command "generateEvent", ["string", "string"]
	}

	simulator {

	}
    
	preferences {
	}
    
	tiles(scale: 2) {
		multiAttributeTile(name: "power", type: "generic", width: 6, height: 4, canChangeIcon: true) {
			tileAttribute("device.power", key: "PRIMARY_CONTROL") {
				attributeState("default", label: '${currentValue}', unit:"W", defaultState: true)
			}
 			tileAttribute("device.lastUpdated", key: "SECONDARY_CONTROL") {
    				attributeState("default", label:'    Last updated ${currentValue}',icon: "st.Health & Wellness.health9")
            }
		}
	}
}

def generateEvent(String name, String value) {
	log.debug("Passed values to routine generateEvent in device named $device: Name - $name  -  Value - $value")
    
    // Update device
	sendEvent(name: name, value: value)
    // Update lastUpdated date and time
    def nowDay = new Date().format("MMM dd", location.timeZone)
    def nowTime = new Date().format("h:mm a", location.timeZone)
    sendEvent(name: "lastUpdated", value: nowDay + " at " + nowTime, displayed: false)
}


def installed() {
}