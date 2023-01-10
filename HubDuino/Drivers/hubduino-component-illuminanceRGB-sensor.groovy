/**
 *  HubDuino Component IlluminanceRGB Sensor
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/hubduino-component-illuminanceRGB-sensor.groovy
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
 *    2017-04-10  Dan Ogorchock  Original Creation
 *    2017-09-07  Allan (vseven) Added a generateEvent routine that gets info from the parent device.  This routine runs each time the value is updated which can lead to other modifications of the device.
 *    2017-09-07  Allan (vseven) Added color temperature and RGB value tiles
 *    2018-06-02  Dan Ogorchock  Revised/Simplified for Hubitat Composite Driver Model
 *    2018-09-22  Dan Ogorchock  Added preference for debug logging
 *    2019-07-01  Dan Ogorchock  Added importUrl
 *    2020-01-25  Dan Ogorchock  Remove custom lastUpdated attribute & general code cleanup
 *    2022-11-04  ss41812        Add custom attribute definitions
 *    2022-12-12  Dan Ogorchock  Revised to follow standard Hubitat Generic Component driver architecture
 *
 * 
 */
metadata {
    definition(name: "HubDuino Component IlluminanceRGB Sensor", namespace: "ogiewon", author: "Dan Ogorchock", component: true, importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/hubduino-component-illuminanceRGB-sensor.groovy") {
        capability "Illuminance Measurement"
        capability "Refresh"
		capability "Sensor"

		attribute "redValue", "Number"
		attribute "greenValue", "Number"
		attribute "blueValue", "Number"
		attribute "colorTemperature", "Number"
		attribute "clearValue", "Number"
    }
    preferences {
        input name: "logEnable", type: "bool", title: "Enable debug logging", defaultValue: true
        input name: "txtEnable", type: "bool", title: "Enable descriptionText logging", defaultValue: true
    }
}

void updated() {
    log.info "Updated..."
    log.warn "description logging is: ${txtEnable == true}"
    if (logEnable) runIn(1800,logsOff)
}

void installed() {
    log.info "Installed..."
    device.updateSetting("txtEnable",[type:"bool",value:true])
    refresh()
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
        if (it.name in ["illuminancergb"]) {
            if (txtEnable) log.info it.descriptionText
            //sendEvent(it)  //This driver requires custom data processing, so commenting out the typical sendEvent() call
            // The value is a string containing all the information seperated by colons.   
            // For a Adafruit TCS34725 the order is  Color Temp, Lux, Red, Green, Blue, then Clear.   Modify as needed.
            def myValues = it.value.split(':')
            sendEvent(name: "illuminance",value: myValues[0])
            sendEvent(name: "colorTemperature", value: myValues[1])
            sendEvent(name: "redValue", value: myValues[2])
            sendEvent(name: "greenValue", value: myValues[3])
            sendEvent(name: "blueValue", value: myValues[4])
            sendEvent(name: "clearValue", value: myValues[5])
        }
    }
}

void refresh() {
    parent?.componentRefresh(this.device)
}
