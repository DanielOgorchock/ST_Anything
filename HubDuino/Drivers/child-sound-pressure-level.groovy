/**
 *  Child Sound Pressure Level
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-sound-pressure-level.groovy
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
 *
 * 
 */
metadata {
	definition (name: "Child Sound Pressure Level", namespace: "ogiewon", author: "Daniel Ogorchock", importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-sound-pressure-level.groovy") {
        capability "Sensor"
        capability "Sound Pressure Level"
        capability "Contact Sensor"
	}
        
    preferences {
        input "maxSPL", "number", title: "SPL Threshold", description: "SPL values above this threshold will CLOSE the contact sensor attribute.", range: "*..*", displayDuringSetup: true
        input name: "logEnable", type: "bool", title: "Enable debug logging", defaultValue: true
	}

}

def logsOff(){
    log.warn "debug logging disabled..."
    device.updateSetting("logEnable",[value:"false",type:"bool"])
}

def parse(String description) {
    if (logEnable) log.debug "parse(${description}) called"
	def parts = description.split(" ")
    def name  = parts.length>0?parts[0].trim():null
    def value = parts.length>1?parts[1].trim():null
    if (name && value) {
        // Update device
        sendEvent(name: name, value: value, unit: "dBA")
        float tmpValue = Float.parseFloat(value)
        if (maxSPL) {
            if (tmpValue >= Float.valueOf(maxSPL)) {
                sendEvent(name: "contact", value: "closed")
            } else {
                sendEvent(name: "contact", value: "open")
            }
        }
    }
    else {
    	log.error "Missing either name or value.  Cannot parse!"
    }
}

def installed() {
    updated()
}

def updated() {
    if (logEnable) runIn(1800,logsOff)
}
