/**
 *  ST_Anything Example Multiplexer - ST_Anything_Example_Multiplexer.smartapp.groovy
 *
 *  Copyright 2016 Daniel Ogorchock
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
 *    2016-10-25  Dan Ogorchock  Original Creation
 *
 */
 
definition(
    name: "ST_Anything Example Multiplexer",
    namespace: "ogiewon",
    author: "Daniel Ogorchock",
    description: "Connects single Arduino with multiple similar devices to their virtual device counterparts.",
    category: "My Apps",
    iconUrl: "https://s3.amazonaws.com/smartapp-icons/Convenience/Cat-Convenience.png",
    iconX2Url: "https://s3.amazonaws.com/smartapp-icons/Convenience/Cat-Convenience@2x.png",
    iconX3Url: "https://s3.amazonaws.com/smartapp-icons/Convenience/Cat-Convenience@2x.png")

preferences {

	section("Select the Virtual Contact Sensor devices") {
		input "contact1", title: "Virtual Contact Sensor 1", "capability.contactSensor"
		input "contact2", title: "Virtual Contact Sensor 2", "capability.contactSensor"
	}

	section("Select the Virtual Motion Sensor devices") {
		input "motion1", title: "Virtual Motion Sensor 1", "capability.motionSensor"
		input "motion2", title: "Virtual Motion Sensor 2", "capability.motionSensor"
	}

	section("Select the Arduino device") {
		input "arduino", "capability.contactSensor"
    }    
}

def installed() {
	log.debug "Installed with settings: ${settings}"
	subscribe()
}

def updated() {
	log.debug "Updated with settings: ${settings}"
	unsubscribe()
	subscribe()
}

def subscribe() {

    subscribe(arduino, "contact1.open", contact1Open)
    subscribe(arduino, "contact1.closed", frontDoorClosed)
    
    subscribe(arduino, "contact2.open", contact2Open)
    subscribe(arduino, "contact2.closed", contact3Closed)

    subscribe(arduino, "motion1.active", motion1Active)
    subscribe(arduino, "motion1.inactive", motion1Inactive)
    
    subscribe(arduino, "motion2.active", motion2Active)
    subscribe(arduino, "motion2.inactive", motion2Inactive)

}


// --- contact1 ------------------------------------------------------ 
def contact1Open(evt)
{
    if (contact1.currentValue("contact") != "open") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    	contact1.openme()
    }
}

def contact1Closed(evt)
{
    if (contact1.currentValue("contact") != "closed") {
	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    	contact1.closeme()
    }
}

// --- contact2 ------------------------------------------------------ 
def contact2Open(evt)
{
    if (contact2.currentValue("contact") != "open") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    	contact2.openme()
    }
}

def contact2Closed(evt)
{
    if (contact2.currentValue("contact") != "closed") {
	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    	contact2.closeme()
    }
}

// --- motion1 ------------------------------------------------------ 
def motion1Active(evt)
{
    if (motion1.currentValue("motion") != "active") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    	motion1.active()
    }
}

def motion1Inactive(evt)
{
    if (motion1.currentValue("motion") != "inactive") {
	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    	motion1.inactive()
    }
}

// --- motion2 ------------------------------------------------------ 
def motion2Active(evt)
{
    if (motion2.currentValue("motion") != "active") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    	motion2.active()
    }
}

def motion2Inactive(evt)
{
    if (motion2.currentValue("motion") != "inactive") {
	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    	motion2.inactive()
    }
}


def initialize() {
	// TODO: subscribe to attributes, devices, locations, etc.
}