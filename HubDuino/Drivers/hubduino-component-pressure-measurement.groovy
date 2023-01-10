/**
 *  HubDuino Component  Pressure Measurement
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/hubduino-component-pressure-measurement.groovy
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
 *    2018-07-01  Dan Ogorchock  Original Creation
 *    2018-09-22  Dan Ogorchock  Added preference for debug logging
 *    2018-11-10  Dan Ogorchock  Corrected Pressure Measurement attribute name
 *    2019-03-10  Dan Ogorchock  Added user preference for unit conversion
 *    2019-07-01  Dan Ogorchock  Added importUrl
 *    2020-01-25  Dan Ogorchock  Remove custom lastUpdated attribute & general code cleanup
 *    2022-12-31  Dan Ogorchock  Revised to follow standard Hubitat Generic Component driver architecture
 *
 * 
 */
metadata {
	definition (name: "HubDuino Component Pressure Measurement", namespace: "ogiewon", author: "Daniel Ogorchock", component: true, importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/hubduino-component-pressure-measurement.groovy") {
		capability "PressureMeasurement"
		capability "Sensor"
        capability "Refresh"
	}

    preferences {
        input "tempUnitConversion", "enum", title: "Pressure Unit Conversion - select hPa to mmHg, hPa to inHg, or no conversion", description: "", defaultValue: "1", required: true, multiple: false, options:[["1":"none"], ["2":"hPa to mm Hg"], ["3":"hPa to in Hg"]], displayDuringSetup: false
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
        if (it.name in ["pressure"]) {
            if (txtEnable) log.info it.descriptionText
            def dispUnit = "hPa"
            float tmpValue = Float.parseFloat(it.value)
            if (tempUnitConversion == "2") {
                //if (logEnable) log.debug "tempUnitConversion = ${tempUnitConversion}"
                tmpValue = tmpValue * 0.75006  //convert from hPa to mm Hg
                dispUnit = "mmHg"
            }

            if (tempUnitConversion == "3") {
                //if (logEnable) log.debug "tempUnitConversion = ${tempUnitConversion}"
                tmpValue = tmpValue * 0.02953  //convert from hPa to in Hg
                dispUnit = "inHg"
            }

            // Update device
            tmpValue = tmpValue.round(2)
            sendEvent(name: it.name, value: it.value, unit: dispUnit)
        }
    }
}

void refresh() {
    parent?.componentRefresh(this.device)
}
