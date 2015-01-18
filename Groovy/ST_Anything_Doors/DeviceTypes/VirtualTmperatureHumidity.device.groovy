/**
 *  Virtual TemperatureHumidity
 *
 *  Copyright 2015 Daniel Ogorchock
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
 *    2015-01-18  Dan Ogorchock  Original Creation
 *
 * 
 */
metadata {
	definition (name: "Virtual TemperatureHumidity", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Relative Humidity Measurement"
		capability "Temperature Measurement"
		capability "Sensor"

		command "updateTemperature"
		command "updateHumidity"
	}

	simulator {
		// TODO: define status and reply messages here
	}

	tiles {
        valueTile("temperature", "device.temperature", width: 2, height: 2) {
			state("temperature", label:'${currentValue}°', 
				backgroundColors:[
					[value: 31, color: "#153591"],
					[value: 44, color: "#1e9cbb"],
					[value: 59, color: "#90d2a7"],
					[value: 74, color: "#44b621"],
					[value: 84, color: "#f1d801"],
					[value: 95, color: "#d04e00"],
					[value: 96, color: "#bc2323"]
				]
			)
		}
        
        valueTile("humidity", "device.humidity", inactiveLabel: false) {
			state "humidity", label:'${currentValue}% humidity', unit:""
		}

        main(["temperature","humidity"])
        details(["temperature","humidity"])

	}
}

// parse events into attributes
def parse(String description) {
	log.debug "Parsing '${description}'"
	// TODO: handle 'humidity' attribute
	// TODO: handle 'temperature' attribute

}


def updateTemperature(newval) {
    	log.debug "New Temperature = $newval"
    	sendEvent(name: "temperature", value: newval, isStateChange: true)
}

def updateHumidity(newval) {
    	log.debug "New Humidity = $newval"
    	sendEvent(name: "humidity", value: newval, isStateChange: true)
}