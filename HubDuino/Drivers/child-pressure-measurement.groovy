/**
 *  Child Pressure Measurement
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-pressure-measurement.groovy
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
 *
 * 
 */
metadata {
	definition (name: "Child Pressure Measurement", namespace: "ogiewon", author: "Daniel Ogorchock", importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-pressure-measurement.groovy") {
		capability "PressureMeasurement"
		capability "Sensor"
	}

    preferences {
        input "tempUnitConversion", "enum", title: "Pressure Unit Conversion - select hPa to mmHg, hPa to inHg, or no conversion", description: "", defaultValue: "1", required: true, multiple: false, options:[["1":"none"], ["2":"hPa to mm Hg"], ["3":"hPa to in Hg"]], displayDuringSetup: false
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
    def dispUnit = "hPa"
    if (name && value) {
        float tmpValue = Float.parseFloat(value)
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
        sendEvent(name: "pressure", value: tmpValue, unit: dispUnit)
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
