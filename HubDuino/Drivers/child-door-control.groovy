/**
 *  Child DoorControl
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-door-control.groovy
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
 *    2015-04-14  Dan Ogorchock  Original Creation
 *    2017-08-23  Allan (vseven) Added a generateEvent routine that gets info from the parent device.  This routine runs each time the value is updated which can lead to other modifications of the device.
 *    2018-06-02  Dan Ogorchock  Revised/Simplified for Hubitat Composite Driver Model
 *    2018-09-22  Dan Ogorchock  Added preference for debug logging
 *    2019-07-01  Dan Ogorchock  Added importUrl
 *    2019-07-28  Dan Ogorchock  Minor tweak to support option for a switch output instead of just a momentary output - requires updated Arduino Door Control Code!!!
 *    2020-01-25  Dan Ogorchock  Remove custom lastUpdated attribute & general code cleanup
 *
 *
 */
metadata {
	definition (name: "Child Door Control", namespace: "ogiewon", author: "Daniel Ogorchock", importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-door-control.groovy") {
		capability "Door Control"
		capability "Garage Door Control"
		capability "Contact Sensor"
		capability "Actuator"
		capability "Sensor"
		capability "Momentary"

		attribute "lastUpdated", "String"
	}

    preferences {
        input name: "logEnable", type: "bool", title: "Enable debug logging", defaultValue: true
	}
}

def logsOff(){
    log.warn "debug logging disabled..."
    device.updateSetting("logEnable",[value:"false",type:"bool"])
}

// handle commands
def open() {
	sendData("on")
}

def close() {
	sendData("off")
}

def push() {
	sendData("on")
}

def sendData(String value) {
    def name = device.deviceNetworkId.split("-")[-1]
    parent.sendData("${name} ${value}")  
}

def parse(String description) {
    if (logEnable) log.debug "parse(${description}) called"
	def parts = description.split(" ")
    def name  = parts.length>0?parts[0].trim():null
    def value = parts.length>1?parts[1].trim():null
    if (name && value) {
        // Update device
        sendEvent(name: "door", value: value)
        // Also update the "Contact Sensor" device as this is useful for Apps that do not support the "Door Control" capability
        if((value == "open") || (value == "closed")) {
            sendEvent(name: "contact", value: value)
        }
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
