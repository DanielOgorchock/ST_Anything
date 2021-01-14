/**
 *  Child Ultrasonic Sensor
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
 *    2021-01-13  Andy Alsup     Revised for new ST mobile app
 *
 * 
 */
metadata {
	definition (name: "Child Ultrasonic Sensor", namespace: "ogiewon", author: "Daniel Ogorchock", mnmn: "SmartThingsCommunity", vid: "c9b3ec68-b299-3513-96b2-497d1f7b8e8d") {
		capability "Sensor"
        capability "Refresh"
        capability "Water Sensor"
        capability "afterwatch06989.ultrasonic"
        capability "afterwatch06989.lastupdated"
    }

    preferences {
        input(
            name: "wet_limit", 
            title: "Wet limit", 
            type: "number", 
            description: "Enter limit to report wet (in cm)",
            required: true 
        )
        input(
            name: "wet_condition", 
            title: "Wet condition", 
            type: "enum", 
            options: ["Below limit", "Above limit"],
            description: "Select condition to report wet", 
            required: true
        )
    }
}

def parse(String description) {
    log.debug "parse(${description}) called"
	def parts = description.split(" ")
    def name  = parts.length>0?parts[0].trim():null
    def value = parts.length>1?parts[1].trim():null

    if (name && value) {
        // Update `ultrasonic`
        float distance = value as float
        sendEvent(name: "ultrasonic", value: distance, unit: "cm")

        // Update `water`
        if (wet_limit && wet_condition) {
            def water = "dry"

            if ("Below limit".equalsIgnoreCase(wet_condition)) {
                if (distance < wet_limit) {
                    water = "wet"
                }
            } else {
                if (distance > wet_limit) {
                    water = "wet"
                }
            }
            log.debug "water: " + water
            sendEvent(name: "water", value: water)
        }

        // Update `lastUpdated`
        def nowDateTime = new Date().format("yyyyMMdd HH:mm:ss z", location.timeZone)
        sendEvent(name: "lastUpdated", value: nowDateTime)
    }
    else {
    	log.debug "Missing either name or value.  Cannot parse!"
    }
}

def installed() {

}
