/**
 *  Child Humidity Sensor
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
 *	  2017-08-23  Allan (vseven) Added a generateEvent routine that gets info from the parent device.  This routine runs each time the value is updated which can lead to other modifications of the device.
 *	  2017-08-24  Allan (vseven) Added a lastUpdated attribute that will display on the multitile.
 *    2017-09-09  Allan (vseven) Added preference to offset the humidity.
 *    2018-06-02  Dan Ogorchock  Revised/Simplified for Hubitat Composite Driver Model
 *
 * 
 */
metadata {
	definition (name: "Child Humidity Sensor", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Relative Humidity Measurement"
		capability "Sensor"
        
        attribute "lastUpdated", "String"
	}

	simulator {

	}
    
	preferences {
		section("Prefs") {
//			input title: "Humidity Offset", description: "This feature allows you to correct any humidity variations by selecting an offset. Ex: If your sensor consistently reports a humidity that's 6% higher then a similiar calibrated sensor, you'd enter \"-6\".", displayDuringSetup: false, type: "paragraph", element: "paragraph"
			input "humidityOffset", "number", title: "Humidity Offset in Percent", description: "Adjust humidity by this percentage", range: "*..*", displayDuringSetup: false
		}
	}

	tiles(scale: 2) {
		multiAttributeTile(name: "humidity", type: "generic", width: 6, height: 4, canChangeIcon: true) {
			tileAttribute("device.humidity", key: "PRIMARY_CONTROL") {
                attributeState("humidity", label:'${currentValue}%', unit:"%", defaultState: true,
                    backgroundColors:[
                        [value: 0, color: "#635C0C"],
                        [value: 16, color: "#EBEB21"],
                        [value: 22, color: "#C7DE6A"],
                        [value: 42, color: "#9AD290"],
                        [value: 64, color: "#44B621"],
                        [value: 80, color: "#3D79D9"],
                        [value: 96, color: "#0A50C2"]
                    ])
                }
                
             tileAttribute("device.lastUpdated", key: "SECONDARY_CONTROL") {
    			attributeState("default", label:'    Last updated ${currentValue}',icon: "st.Health & Wellness.health9")
             }
		}
		main(["humidity"])
        details(["humidity", "lastUpdated"])
	}
}

def parse(String description) {
    log.debug "parse(${description}) called"
	def parts = description.split(" ")
    def name  = parts.length>0?parts[0].trim():null
    def value = parts.length>1?parts[1].trim():null
    if (name && value) {
        // Offset the humidity based on preference
        def offsetValue = Math.round((Float.parseFloat(value))*100.0)/100.0d
        if (humidityOffset) {
            offsetValue = offsetValue + humidityOffset
        }
        // Update device
        sendEvent(name: name, value: offsetValue)
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