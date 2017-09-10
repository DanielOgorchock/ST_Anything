/**
 *  Child Illuminance Sensor
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
 * 
 */
metadata {
	definition (name: "Child IlluminanceRGB Sensor", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Illuminance Measurement"
		capability "Sensor"

		attribute "lastUpdated", "String"

		command "generateEvent", ["string", "string"]
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
    
def generateEvent(String name, String value) {
	//log.debug("Passed values to routine generateEvent in device named $device: Name - $name  -  Value - $value")
    // The value is a string containing all the information seperated by colons.   
    // For a Adafruit TCS34725 the order is  Color Temp, Lux, Red, Green, Blue, then Clear.   Modify as needed.
    def myValues = value.split(':')
	// Update our device
	sendEvent(name: "illuminance",value: myValues[0])
    sendEvent(name: "colorTemperature", value: myValues[1])
    sendEvent(name: "redValue", value: myValues[2])
    sendEvent(name: "greenValue", value: myValues[3])
    sendEvent(name: "blueValue", value: myValues[4])
    sendEvent(name: "clearValue", value: myValues[5])   
}