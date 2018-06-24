/**
 *  Child IlluminanceRGB Sensor
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
 * 
 */
metadata {
	definition (name: "Child IlluminanceRGB Sensor", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Illuminance Measurement"
		capability "Sensor"

		attribute "lastUpdated", "String"
	}

	simulator {

	}

	tiles(scale: 2) {
		multiAttributeTile(name: "illuminance", type: "generic", width: 6, height: 4, canChangeIcon: true) {
			tileAttribute("device.illuminance", key: "PRIMARY_CONTROL") {
				attributeState("illuminance", label: '${currentValue} ${unit}', unit:"lux", 
                	backgroundColors: [
						[value: 9, color: "#767676"],
						[value: 315, color: "#ffa81e"],
						[value: 1000, color: "#fbd41b"]
					])
			}
			tileAttribute("device.colorTemperature", key: "SECONDARY_CONTROL") {
				attributeState("colorTemperature", label: "Color Temp: " + '${currentValue} ${unit}', unit:"K")
			}   
 
		}
        standardTile("redValue", "device.redValue", inactiveLabel: false, width: 2, height: 2) {
    		state "default", label:  '${currentValue}' + "\nRed"
		}
		standardTile("greenValue", "device.greenValue", inactiveLabel: false, width: 2, height: 2) {
    		state "default", label: '${currentValue}' + "\nGreen"
		}
		standardTile("blueValue", "device.blueValue", inactiveLabel: false, width: 2, height: 2) {
    		state "default", label: '${currentValue}' + "\nBlue"
		}
		standardTile("clearValue", "device.clearValue", inactiveLabel: false, width: 2, height: 2) {
    		state "default", label: '${currentValue}' + "\nClear"
		}
		main(["illuminance"])
        details(["illuminance", "colorTemperature", "redValue", "greenValue", "blueValue", "clearValue"])
	}
}
    
def parse(String description) {
    log.debug "parse(${description}) called"
	def parts = description.split(" ")
    def name  = parts.length>0?parts[0].trim():null
    def value = parts.length>1?parts[1].trim():null
    if (name && value) {
        // Update device
        // The value is a string containing all the information seperated by colons.   
        // For a Adafruit TCS34725 the order is  Color Temp, Lux, Red, Green, Blue, then Clear.   Modify as needed.
        def myValues = value.split(':')
        sendEvent(name: "illuminance",value: myValues[0])
        sendEvent(name: "colorTemperature", value: myValues[1])
        sendEvent(name: "redValue", value: myValues[2])
        sendEvent(name: "greenValue", value: myValues[3])
        sendEvent(name: "blueValue", value: myValues[4])
        sendEvent(name: "clearValue", value: myValues[5])
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