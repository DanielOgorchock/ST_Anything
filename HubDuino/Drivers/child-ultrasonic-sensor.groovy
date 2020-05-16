/**
 *  Child Ultrasonic Sensor
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-ultrasonic-sensor.groovy
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
 *    2018-06-02  Dan Ogorchock  Revised/Simplified for Hubitat Composite Driver Model
 *    2018-09-22  Dan Ogorchock  Added preference for debug logging
 *    2019-07-01  Dan Ogorchock  Added importUrl
 *    2020-01-25  Dan Ogorchock  Remove custom lastUpdated attribute & general code cleanup
 *    2020-05-16  Dan Ogrochock  Improved Capabilities/Attributes to work with other Apps
 *   
 *
 * 
 */
metadata {
	definition (name: "Child Ultrasonic Sensor", namespace: "ogiewon", author: "Daniel Ogorchock", importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-ultrasonic-sensor.groovy") {
        capability "Sensor"
        capability "Voltage Measurement"
        
        attribute "ultrasonic", "Number"
	attribute "liters", "Number"
    }

    preferences {
        input name: "height", type: "number", title: "Height", description: "Enter height of tank in cm", required: true
        input name: "diameter", type: "number", title: "Diameter", description: "Enter diameter of tank", required: true
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
        sendEvent(name: "voltage", value: value)
        double sensorValue = value as float
        def volume = 3.14159 * (diameter/2) * (diameter/2) * height
        double capacityLiters = volume / 1000 * 2
        capacityLiters = capacityLiters.round(2)
        sendEvent(name: "liters", value: capacityLiters)
        double capacityValue = 100 - (sensorValue/height * 100 )
        if(capacityValue != 100)
        {
            capacityValue = capacityValue.round(2)
            sendEvent(name: name, value: capacityValue)
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
