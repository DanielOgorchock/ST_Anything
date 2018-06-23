/**
 *  Child Generic Sensor
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
 *    2017-10-20  Allan (vseven) Original Creation (based on Dan Ogorchock's child illuminance switch)
 *    2018-06-02  Dan Ogorchock  Revised/Simplified for Hubitat Composite Driver Model
 * 
 */
metadata {
	definition (name: "Child Generic Sensor", namespace: "ogiewon", author: "Allan (vseven) - based on code by Daniel Ogorchock") {
		capability "Sensor"

		attribute "lastUpdated", "String"
		attribute "genericValue", "String"
	}

	simulator {

	}

	tiles(scale: 2) {
		multiAttributeTile(name: "primaryTile", type: "generic", width: 6, height: 4, canChangeIcon: true) {
			tileAttribute("device.genericValue", key: "PRIMARY_CONTROL") {
				attributeState("genericValue", label: '${currentValue}')
			}
			tileAttribute("device.lastUpdated", key: "SECONDARY_CONTROL") {
    			attributeState("default", label:'    Last updated ${currentValue}',icon: "st.Health & Wellness.health9")
			}
		}

		// Since this is a generic DTH there is only the main tile to hold the value and the last updated tile
		// If you are using multiple values you should add more tiles and update the details line below
        main(["primaryTile"])
		details(["primaryTile", "lastUpdated"])
	}
}

def parse(String description) {
    log.debug "parse(${description}) called"
	def parts = description.split(" ")
    def name  = parts.length>0?parts[0].trim():null
    def value = parts.length>1?parts[1].trim():null
    if (name && value) {
        // Update device
        sendEvent(name: "genericValue", value: value)
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