/**
 *  Child Temperature Sensor
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-temperature-sensor.groovy
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
 *    2017-08-23  Allan (vseven) Added a generateEvent routine that gets info from the parent device.  This routine runs each time the value is updated which can lead to other modifications of the device.
 *    2017-11-04  Dan Ogorchock  Added preference for Temperature Unit Conversion: Fahrenheit to Celsius, Celsius to Fahrenheit, or none 
 *    2018-02-16  Dan Ogorchock  Fixed preferences to work with Hubitat.
 *    2018-06-02  Dan Ogorchock  Revised/Simplified for Hubitat Composite Driver Model
 *    2018-09-22  Dan Ogorchock  Added preference for debug logging
 *    2019-02-10  Dan Ogorchock  Added temperature units for display on the Hubitat Dashboard
 *    2019-03-06  Dan Ogorchock  Improved rounding
 *    2019-07-01  Dan Ogorchock  Added importUrl
 *    2019-10-30  Dan Ogorchock  Fixed type conversion error found by @kuzenkohome
 *    2020-01-25  Dan Ogorchock  Remove custom lastUpdated attribute & general code cleanup
 * 
 */
metadata {
	definition (name: "Child Temperature Sensor", namespace: "ogiewon", author: "Daniel Ogorchock", importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-temperature-sensor.groovy") {
		capability "Temperature Measurement"
		capability "Sensor"
	}

	preferences {
		section("Prefs") {
			input "tempOffset", "number", title: "Temperature Offset", description: "Adjust temperature by this many degrees", range: "*..*", displayDuringSetup: false
			input "tempUnitConversion", "enum", title: "Temperature Unit Conversion - select F to C, C to F, or no conversion", description: "", defaultValue: "1", required: true, multiple: false, options:[["1":"none"], ["2":"Fahrenheit to Celsius"], ["3":"Celsius to Fahrenheit"]], displayDuringSetup: false
            input name: "logEnable", type: "bool", title: "Enable debug logging", defaultValue: true
        }
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
	def dispUnit = "°F"
    if (name && value) {
    	// Offset the temperature based on preference
        float tmpValue = Float.parseFloat(value)    

        if (tempUnitConversion == "2") {
            //if (logEnable) log.debug "tempUnitConversion = ${tempUnitConversion}"
            tmpValue = fahrenheitToCelsius(tmpValue)  //convert from Fahrenheit to Celsius
            dispUnit = "°C"
        }

        if (tempUnitConversion == "3") {
            //if (logEnable) log.debug "tempUnitConversion = ${tempUnitConversion}"
            tmpValue = celsiusToFahrenheit(tmpValue)  //convert from Celsius to Fahrenheit
            dispUnit = "°F"
        }
        
        if (tempOffset) {
            tmpValue = tmpValue + tempOffset.toFloat()
        }

        // Update device
        tmpValue = tmpValue.round(1)
        sendEvent(name: name, value: tmpValue, unit: dispUnit)
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
