/**
 *  ST_Anything Doors Multiplexer - ST_Anything_Doors_Multiplexer.smartapp.groovy
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
 *    2015-01-10  Dan Ogorchock  Original Creation
 *
 *
 */
 
definition(
    name: "ST_Anything Doors Multiplexer",
    namespace: "ogiewon",
    author: "Daniel Ogorchock",
    description: "Connects single Arduino with multiple DoorControl and ContactSensor devices to their virtual device counterparts.",
    category: "My Apps",
    iconUrl: "https://s3.amazonaws.com/smartapp-icons/Convenience/Cat-Convenience.png",
    iconX2Url: "https://s3.amazonaws.com/smartapp-icons/Convenience/Cat-Convenience@2x.png",
    iconX3Url: "https://s3.amazonaws.com/smartapp-icons/Convenience/Cat-Convenience@2x.png")

preferences {
	section("Select the Garage Doors (Virtual Door Control devices)") {
		input "leftdoor", title: "Left Garage Door", "capability.doorControl"
		input "rightdoor", title: "Right Garage Door", "capability.doorControl"
	}

	section("Select the House Doors (Virtual Contact Sensor devices)") {
		input "frontdoor", title: "Virtual Contact Sensor for Front Door", "capability.contactSensor"
		input "backdoor", title: "Virtual Contact Sensor for Back Door", "capability.contactSensor"
		input "kitchendoor", title: "Virtual Contact Sensor for Kitchen Door", "capability.contactSensor"
		input "garagesidedoor", title: "Virtual Contact Sensor for Garage Side Door", "capability.contactSensor"
	}

	section("Select the Arduino ST_Anything_Doors device") {
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

    subscribe(arduino, "leftDoor.open", leftDoorOpen)
    subscribe(arduino, "leftDoor.opening", leftDoorOpening)
    subscribe(arduino, "leftDoor.closed", leftDoorClosed)
    subscribe(arduino, "leftDoor.closing", leftDoorClosing)
    subscribe(leftdoor, "buttonPress.true", leftDoorPushButton)
    
    subscribe(arduino, "rightDoor.open", rightDoorOpen)
    subscribe(arduino, "rightDoor.opening", rightDoorOpening)
    subscribe(arduino, "rightDoor.closed", rightDoorClosed)
    subscribe(arduino, "rightDoor.closing", rightDoorClosing)
    subscribe(rightdoor, "buttonPress.true", rightDoorPushButton)
    
    subscribe(arduino, "frontDoor.open", frontDoorOpen)
    subscribe(arduino, "frontDoor.closed", frontDoorClosed)
    
    subscribe(arduino, "backDoor.open", backDoorOpen)
    subscribe(arduino, "backDoor.closed", backDoorClosed)

    subscribe(arduino, "kitchenDoor.open", kitchenDoorOpen)
    subscribe(arduino, "kitchenDoor.closed", kitchenDoorClosed)
    
    subscribe(arduino, "garagesideDoor.open", garagesideDoorOpen)
    subscribe(arduino, "garagesideDoor.closed", garagesideDoorClosed)
}

// --- Left Garage Door --- 
def leftDoorOpen(evt)
{
    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    leftdoor.open()
}

def leftDoorOpening(evt)
{
    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    leftdoor.opening()
}    
  
def leftDoorClosing(evt)
{
    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    leftdoor.closing()
}

def leftDoorClosed(evt)
{
    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    leftdoor.close()
}

def leftDoorPushButton(evt)
{
    log.debug "virtualGarageDoor($evt.name: $evt.value: $evt.deviceId)"
    arduino.pushLeft()
}

// --- Right Garage Door --- 
def rightDoorOpen(evt)
{
    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    rightdoor.open()
}

def rightDoorOpening(evt)
{
    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    rightdoor.opening()
}    
  
def rightDoorClosing(evt)
{
    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    rightdoor.closing()
}

def rightDoorClosed(evt)
{
    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    rightdoor.close()
}

def rightDoorPushButton(evt)
{
    log.debug "virtualGarageDoor($evt.name: $evt.value: $evt.deviceId)"
    arduino.pushRight()
}

// --- Front Door --- 
def frontDoorOpen(evt)
{
    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    frontdoor.openme()
}

def frontDoorClosed(evt)
{
    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    frontdoor.closeme()
}

// --- back Door --- 
def backDoorOpen(evt)
{
    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    backdoor.openme()
}

def backDoorClosed(evt)
{
    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    backdoor.closeme()
}

// --- Kitchen Door --- 
def kitchenDoorOpen(evt)
{
    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    kitchendoor.openme()
}

def kitchenDoorClosed(evt)
{
    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    kitchendoor.closeme()
}


// --- Garage Side Door --- 
def garagesideDoorOpen(evt)
{
    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    garagesidedoor.openme()
}

def garagesideDoorClosed(evt)
{
    log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    garagesidedoor.closeme()
}


def initialize() {
	// TODO: subscribe to attributes, devices, locations, etc.
}
