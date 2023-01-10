/**
 *  Hubduiono Component Ultrasonic Sensor
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/hubduino-component-ultrasonic-sensor.groovy
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
 *    2022-12-10  Dan Ogorchock  Revised to follow standard Hubitat Generic Component driver architecture
 *
 * 
 */
metadata {
    definition(name: "HubDuino Component Ultrasonic Sensor", namespace: "ogiewon", author: "Dan Ogorchock", component: true, importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/hubduino-component-ultrasonic-sensor.groovy") {
        capability "Sensor"
        capability "Refresh"
        
        attribute "ultrasonic", "number"
        attribute "liters", "number"
        attribute "capacityLiters", "number"
        attribute "percentFull", "number"
    }
    preferences {
        input name: "height", type: "number", title: "Height", description: "Enter height of tank in cm", required: true
        input name: "diameter", type: "number", title: "Diameter", description: "Enter diameter of tank in cm", required: true
        input name: "logEnable", type: "bool", title: "Enable debug logging", defaultValue: true
        input name: "txtEnable", type: "bool", title: "Enable descriptionText logging", defaultValue: true
    }
}

void updated() {
    log.info "Updated..."
    log.warn "description logging is: ${txtEnable == true}"
    if (logEnable) runIn(1800,logsOff)
    
    float radius = diameter.toFloat()/2
    float volume = 3.14159 * radius * radius * height.toFloat()
    float capacityLiters = volume / 1000
    capacityLiters = capacityLiters.round(1)
    sendEvent(name: "capacityLiters", value: capacityLiters)
    refresh()
}

void installed() {
    log.info "Installed..."
    device.updateSetting("txtEnable",[type:"bool",value:true])
    updated()
}

void logsOff(){
    log.warn "debug logging disabled..."
    device.updateSetting("logEnable",[value:"false",type:"bool"])
}

void parse(String description) { log.warn "parse(String description) not implemented" }

void parse(List<Map> description) {
    if (logEnable) log.debug "parse received ${description}"
    description.each {
        if (it.name in ["ultrasonic"]) {
            if (txtEnable) log.info it.descriptionText
            sendEvent(it)

			//handle custom attributes for use as tank level sensor
            double sensorValue = Float.parseFloat(it.value)
            if (sensorValue > height.toFloat()) sensorValue = height.toFloat()
                
            float percentFull = 100.0 - (sensorValue/height.toFloat() * 100.0 )
            sendEvent(name: "percentFull", value: percentFull.round(1))

            double liters = percentFull * device.currentValue("capacityLiters") / 100
            sendEvent(name: "liters", value: liters.round(1))
        }
    }
}

void refresh() {
    parent?.componentRefresh(this.device)
}
