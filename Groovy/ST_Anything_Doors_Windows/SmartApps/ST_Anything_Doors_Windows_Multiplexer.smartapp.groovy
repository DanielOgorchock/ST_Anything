/**
 *  ST_Anything Doors & Windows Multiplexer - ST_Anything_Doors_Windows_Multiplexer.smartapp.groovy
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
 *    2015-10-31  Dan Ogorchock  Original Creation
 *    2017-02-18  Dan Ogorchock	 Fixed some typos
 *
 */
 
definition(
    name: "ST_Anything Doors & Windows Multiplexer",
    namespace: "ogiewon",
    author: "Daniel Ogorchock",
    description: "Connects single Arduino with multiple ContactSensor devices to their virtual device counterparts.",
    category: "My Apps",
    iconUrl: "https://s3.amazonaws.com/smartapp-icons/Convenience/Cat-Convenience.png",
    iconX2Url: "https://s3.amazonaws.com/smartapp-icons/Convenience/Cat-Convenience@2x.png",
    iconX3Url: "https://s3.amazonaws.com/smartapp-icons/Convenience/Cat-Convenience@2x.png")

preferences {

	section("Select the House Doors (Virtual Contact Sensor devices)") {
		input "frontdoor", title: "Virtual Contact Sensor for Front Door", "capability.contactSensor"
		input "bedroomdoor", title: "Virtual Contact Sensor for Bedroom Door", "capability.contactSensor"
		input "kitchendoor", title: "Virtual Contact Sensor for Kitchen Door", "capability.contactSensor"
		input "garagedoor", title: "Virtual Contact Sensor for Garage Door", "capability.contactSensor"
	}

	section("Select the Windows (Virtual Contact Sensor devices)") {
		input "kitchenwindow1", title: "Virtual Contact Sensor for Kitchen Window 1", "capability.contactSensor"
		input "kitchenwindow2", title: "Virtual Contact Sensor for Kitchen Window 2", "capability.contactSensor"
		input "kitchenwindow3", title: "Virtual Contact Sensor for Kitchen Window 3", "capability.contactSensor"
		input "masterwindow1", title: "Virtual Contact Sensor for Master Window 1", "capability.contactSensor"
		input "masterwindow2", title: "Virtual Contact Sensor for Master Window 2", "capability.contactSensor"
		input "officewindow1", title: "Virtual Contact Sensor for Office Window 1", "capability.contactSensor"
		input "officewindow2", title: "Virtual Contact Sensor for Office Window 2", "capability.contactSensor"
		input "guestwindow1", title: "Virtual Contact Sensor for Guest Window 1", "capability.contactSensor"
		input "guestwindow2", title: "Virtual Contact Sensor for Guest Window 2", "capability.contactSensor"
	}

	section("Select the Arduino ST_Anything_Doors_Windows device") {
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

   
    subscribe(arduino, "frontDoor.open", frontDoorOpen)
    subscribe(arduino, "frontDoor.closed", frontDoorClosed)
    
    subscribe(arduino, "bedroomDoor.open", bedroomDoorOpen)
    subscribe(arduino, "bedroomDoor.closed", bedroomDoorClosed)

    subscribe(arduino, "kitchenDoor.open", kitchenDoorOpen)
    subscribe(arduino, "kitchenDoor.closed", kitchenDoorClosed)
    
    subscribe(arduino, "garageDoor.open", garageDoorOpen)
    subscribe(arduino, "garageDoor.closed", garageDoorClosed)

    subscribe(arduino, "kitchenWindow1.open", kitchenWindow1Open)
    subscribe(arduino, "kitchenWindow1.closed", kitchenWindow1Closed)

    subscribe(arduino, "kitchenWindow2.open", kitchenWindow2Open)
    subscribe(arduino, "kitchenWindow2.closed", kitchenWindow2Closed)

    subscribe(arduino, "kitchenWindow3.open", kitchenWindow3Open)
    subscribe(arduino, "kitchenWindow3.closed", kitchenWindow3Closed)

    subscribe(arduino, "masterWindow1.open", masterWindow1Open)
    subscribe(arduino, "masterWindow1.closed", masterWindow1Closed)

    subscribe(arduino, "masterWindow2.open", masterWindow2Open)
    subscribe(arduino, "masterWindow2.closed", masterWindow2Closed)

    subscribe(arduino, "officeWindow1.open", officeWindow1Open)
    subscribe(arduino, "officeWindow1.closed", officeWindow1Closed)

    subscribe(arduino, "officeWindow2.open", officeWindow2Open)
    subscribe(arduino, "officeWindow2.closed", officeWindow2Closed)

    subscribe(arduino, "guestWindow1.open", guestWindow1Open)
    subscribe(arduino, "guestWindow1.closed", guestWindow1Closed)

    subscribe(arduino, "guestWindow2.open", guestWindow2Open)
    subscribe(arduino, "guestWindow2.closed", guestWindow2Closed)

}

// --- Front Door --- 
def frontDoorOpen(evt)
{
    if (frontdoor.currentValue("contact") != "open") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    	frontdoor.openme()
    }
}

def frontDoorClosed(evt)
{
    if (frontdoor.currentValue("contact") != "closed") {
		log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    	frontdoor.closeme()
    }
}

// --- bedroom Door --- 
def bedroomDoorOpen(evt)
{
    if (bedroomdoor.currentValue("contact") != "open") {
		log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    	bedroomdoor.openme()
    }
}

def bedroomDoorClosed(evt)
{
    if (bedroomdoor.currentValue("contact") != "closed") {
		log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    	bedroomdoor.closeme()
	}
}

// --- Kitchen Door --- 
def kitchenDoorOpen(evt)
{
    if (kitchendoor.currentValue("contact") != "open") {
		log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    	kitchendoor.openme()
	}
}

def kitchenDoorClosed(evt)
{
    if (kitchendoor.currentValue("contact") != "closed") {
		log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    	kitchendoor.closeme()
	}
}


// --- Garage Door --- 
def garageDoorOpen(evt)
{
    if (garagedoor.currentValue("contact") != "open") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
 	   garagedoor.openme()
	}
}

def garageDoorClosed(evt)
{
    if (garagedoor.currentValue("contact") != "closed") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
	    garagedoor.closeme()
	}
}

// --- Kitchen Window 1 --- 
def kitchenWindow1Open(evt)
{
    if (kitchenwindow1.currentValue("contact") != "open") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
 	   kitchenwindow1.openme()
	}
}

def kitchenWindow1Closed(evt)
{
    if (kitchenwindow1.currentValue("contact") != "closed") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
	    kitchenwindow1.closeme()
	}
}

// --- Kitchen Window 2 --- 
def kitchenWindow2Open(evt)
{
    if (kitchenwindow2.currentValue("contact") != "open") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
 	   kitchenwindow2.openme()
	}
}

def kitchenWindow2Closed(evt)
{
    if (kitchenwindow2.currentValue("contact") != "closed") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
	    kitchenwindow2.closeme()
	}
}

// --- Kitchen Window 3 --- 
def kitchenWindow3Open(evt)
{
    if (kitchenwindow3.currentValue("contact") != "open") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
 	   kitchenwindow3.openme()
	}
}

def kitchenWindow3Closed(evt)
{
    if (kitchenwindow3.currentValue("contact") != "closed") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
	    kitchenwindow3.closeme()
	}
}

// --- Master Window 1 --- 
def masterWindow1Open(evt)
{
    if (masterwindow1.currentValue("contact") != "open") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
 	   masterwindow1.openme()
	}
}

def masterWindow1Closed(evt)
{
    if (masterwindow1.currentValue("contact") != "closed") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
	    masterwindow1.closeme()
	}
}

// --- Master Window 2 --- 
def masterWindow2Open(evt)
{
    if (masterwindow2.currentValue("contact") != "open") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
 	   masterwindow2.openme()
	}
}

def masterWindow2Closed(evt)
{
    if (masterwindow2.currentValue("contact") != "closed") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
	    masterwindow2.closeme()
	}
}

// --- Office Window 1 --- 
def officeWindow1Open(evt)
{
    if (officewindow1.currentValue("contact") != "open") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
 	   officewindow1.openme()
	}
}

def officeWindow1Closed(evt)
{
    if (officewindow1.currentValue("contact") != "closed") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
	    officewindow1.closeme()
	}
}

// --- Office Window 2 --- 
def officeWindow2Open(evt)
{
    if (officewindow2.currentValue("contact") != "open") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
 	   officewindow2.openme()
	}
}

def officeWindow2Closed(evt)
{
    if (officewindow2.currentValue("contact") != "closed") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
	    officewindow2.closeme()
	}
}

// --- Guest Window 1 --- 
def guestWindow1Open(evt)
{
    if (guestwindow1.currentValue("contact") != "open") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
 	   guestwindow1.openme()
	}
}

def guestWindow1Closed(evt)
{
    if (guestwindow1.currentValue("contact") != "closed") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
	    guestwindow1.closeme()
	}
}

// --- Guest Window 2 --- 
def guestWindow2Open(evt)
{
    if (guestwindow2.currentValue("contact") != "open") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
 	   guestwindow2.openme()
	}
}

def guestWindow2Closed(evt)
{
    if (guestwindow2.currentValue("contact") != "closed") {
	    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
	    guestwindow2.closeme()
	}
}


def initialize() {
	// TODO: subscribe to attributes, devices, locations, etc.
}