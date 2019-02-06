/**
 *  Child Energy Meter
 *
 *  Copyright 2019 Daniel Ogorchock
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
 *    2019-02-05  Dan Ogorchock  Original Creation
 * 
 */
metadata {
	definition (name: "Child Energy Meter", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Energy Meter"
		capability "Sensor"

		attribute "lastUpdated", "String"
	}

	simulator {

	}
    
	preferences {
	}
    
	tiles(scale: 2) {
		multiAttributeTile(name: "energy", type: "generic", width: 6, height: 4, canChangeIcon: true) {
			tileAttribute("device.energy", key: "PRIMARY_CONTROL") {
				attributeState("default", label: '${currentValue}', unit:"kWh", defaultState: true)
			}
 			tileAttribute("device.lastUpdated", key: "SECONDARY_CONTROL") {
    				attributeState("default", label:'    Last updated ${currentValue}',icon: "st.Health & Wellness.health9")
            }
		}
	}
}

def parse(String description) {
    log.debug "parse(${description}) called"
    def parts = description.split(" ")
    def name  = parts.length>0?parts[0].trim():null
    def value = parts.length>1?parts[1].trim():null
    if (name && value) {
        // Update device
        sendEvent(name: name, value: value)
        // Update lastUpdated date and time
        def nowDay = new Date().format("MMM dd", location.timeZone)
        def nowTime = new Date().format("h:mm a", location.timeZone)
        sendEvent(name: "lastUpdated", value: nowDay + " at " + nowTime, displayed: false)
    }
    else {
    	log.debug "Missing either name or value.  Cannot parse!"
    }
}

def installed() {
}
