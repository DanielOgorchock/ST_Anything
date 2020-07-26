/**
 *  Child Voltage Sensor
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-voltage-sensor.groovy
 *
 *  Copyright 2017 Daniel Ogorchock
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
 *    2017-04-19  Dan Ogorchock  Original Creation
 *    2017-08-23  Allan (vseven) Added a generateEvent routine that gets info from the parent device.  This routine runs each time the value is updated which can lead to other modifications of the device.
 *    2018-06-02  Dan Ogorchock  Revised/Simplified for Hubitat Composite Driver Model
 *    2018-09-22  Dan Ogorchock  Added preference for debug logging
 *    2019-07-01  Dan Ogorchock  Added importUrl
 *    2020-01-25  Dan Ogorchock  Remove custom lastUpdated attribute & general code cleanup
 *    2020-07-26  Dan Ogorchock  User can now select the number of decimal places stored.
 *
 * 
 */
metadata {
	definition (name: "Child Voltage Sensor", namespace: "ogiewon", author: "Daniel Ogorchock", importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-voltage-sensor.groovy") {
		capability "Voltage Measurement"
		capability "Sensor"
	}
        
    preferences {
        input name: "logEnable", type: "bool", title: "Enable debug logging", defaultValue: true
		input name: "numDecimalPlaces", type: "enum", title: "Number of decimal places", description: "", defaultValue: "1", required: true, multiple: false, options:[["0":"0"], ["1":"1"], ["2":"2"], ["3":"3"]], displayDuringSetup: false
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
        if (numDecimalPlaces == null) {
            device.updateSetting("numDecimalPlaces", [value: "1", type: "enum"])
        }
        
        if (numDecimalPlaces == "0") {
            int tmpValue = Float.parseFloat(value)
            sendEvent(name: name, value: tmpValue, unit: "V")
        }
        else {
            float tmpValue = Float.parseFloat(value)
            tmpValue = tmpValue.round(numDecimalPlaces.toInteger())
            sendEvent(name: name, value: tmpValue, unit: "V")
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
