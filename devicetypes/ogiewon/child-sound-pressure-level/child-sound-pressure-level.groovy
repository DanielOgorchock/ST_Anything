/**
 *  Child Sound Pressure Level
 *
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
 *    2019-07-08  Dan Ogorchock  Original Creation
 *    2020-01-25  Dan Ogorchock  Remove custom lastUpdated attribute & general code cleanup
 *    2020-05-16  Dan Ogorchock  Ported from Hubitat to SmartThings
 *
 * 
 */
metadata {
	definition (name: "Child Sound Pressure Level", namespace: "ogiewon", author: "Daniel Ogorchock") {
        capability "Sensor"
        capability "Sound Pressure Level"
        capability "Contact Sensor"
		
		attribute "lastUpdated", "String"
	}
        
    preferences {
        input "maxSPL", "number", title: "SPL Threshold", description: "SPL values above this threshold will CLOSE the contact sensor attribute.", range: "*..*", displayDuringSetup: true
	}

	tiles(scale: 2) {
		multiAttributeTile(name: "soundPressureLevel", type: "generic", width: 6, height: 4, canChangeIcon: true) {
			tileAttribute("device.soundPressureLevel", key: "PRIMARY_CONTROL") {
				attributeState("soundPressureLevel", label: '${currentValue} ${unit}', unit: "dB", defaultState: true)
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
        sendEvent(name: name, value: value, unit: "dB")
        float tmpValue = Float.parseFloat(value)
        if (maxSPL) {
            if (tmpValue >= Float.valueOf(maxSPL)) {
                sendEvent(name: "contact", value: "closed")
            } else {
                sendEvent(name: "contact", value: "open")
            }
        }
        // Update lastUpdated date and time
        def nowDay = new Date().format("MMM dd", location.timeZone)
        def nowTime = new Date().format("h:mm a", location.timeZone)
        sendEvent(name: "lastUpdated", value: nowDay + " at " + nowTime, displayed: false)
    }
    else {
    	log.error "Missing either name or value.  Cannot parse!"
    }
}

def installed() {
}
