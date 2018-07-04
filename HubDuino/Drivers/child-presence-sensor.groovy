/**
 *  Child Presence Sensor
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
 *    2018-02-24  Dan Ogorchock  Original Creation
 *    2018-06-02  Dan Ogorchock  Revised/Simplified for Hubitat Composite Driver Model
 * 
 */
metadata {
	definition (name: "Child Presence Sensor", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Sensor"
		capability "Presence Sensor"

		attribute "lastUpdated", "String"
        attribute "level", "Number"
	}

	simulator {

	}
    
	preferences {
		section("Prefs") {
			input "presenceTriggerValue", "number", title: "(Optional) Presence Trigger Value\nAt what value is presence triggered?", required: false, displayDuringSetup: false
            input "invertTriggerLogic", "bool", title: "(Optional) Invert Logic", description: "False = Present > Trigger Value\nTrue = Present < Trigger Value", default: false, required: false, displayDuringSetup: false
		}
	}
    
	tiles(scale: 2) {
		multiAttributeTile(name: "presence", type: "generic", width: 2, height: 2, canChangeBackground: true) {
			tileAttribute ("device.presence", key: "PRIMARY_CONTROL") {
            	attributeState "present", label: 'Present', icon:"st.tesla.tesla-car", backgroundColor:"#00A0DC"
				attributeState "not present", label: 'Away', icon:"st.doors.garage.garage-open", backgroundColor:"#ffffff"
            }
 			tileAttribute("device.level", key: "SECONDARY_CONTROL") {
    				attributeState("default", label:'    Level ${currentValue}')
            }
		}
        valueTile("lastUpdated", "device.lastUpdated", inactiveLabel: false, decoration: "flat", width: 6, height: 2) {
    			state "default", label:'Last Updated ${currentValue}', backgroundColor:"#ffffff"
		}
	}
}

def parse(String description) {
    log.debug "parse(${description}) called"
	def parts = description.split(" ")
    def name  = parts.length>0?parts[0].trim():null
    def value = parts.length>1?parts[1].trim():null
    if (name && value) {
        if (value.isNumber()) {
            sendEvent(name: "level", value: value)
            if (presenceTriggerValue) {
                log.debug "Presence received a numeric value. Perform comparison of value: ${Float.valueOf(value.trim())} versus presenceTriggerValue: ${presenceTriggerValue}"
                if (Float.valueOf(value.trim()) >= presenceTriggerValue) {
                    value = invertTriggerLogic?"not present":"present"
                } 
                else {
                    value = invertTriggerLogic?"present":"not present"
                }
            }
            else {
                log.error "Please configure the Presence Trigger Value in device settings!"
            }
        }
        else {
            log.debug "Presence received a string.  value = ${value}"
            if (value != "present") { value = "not present" }
        }
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