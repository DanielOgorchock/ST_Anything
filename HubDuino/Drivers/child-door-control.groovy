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

	simulator {

	}

	tiles(scale: 2) {
		multiAttributeTile(name:"door", type: "generic"){
			tileAttribute ("device.door", key: "PRIMARY_CONTROL") {
               		attributeState "open", label: 'Open', action: "doorControl.close", icon: "st.doors.garage.garage-open", backgroundColor: "#e86d13", nextState: "open"
                	attributeState "closed", label: 'Closed', action: "doorControl.open", icon: "st.doors.garage.garage-closed", backgroundColor: "#00a0dc", nextState: "closed"
                	attributeState "opening", label: 'Opening', action: "doorControl.close", icon: "st.doors.garage.garage-opening", backgroundColor: "#e86d13", nextState: "closing"
                	attributeState "closing", label: 'Closing', action: "doorControl.open", icon: "st.doors.garage.garage-closing", backgroundColor: "#00a0dc", nextState: "opening"
            	}
 			tileAttribute("device.lastUpdated", key: "SECONDARY_CONTROL") {
    				attributeState("default", label:'    Last updated ${currentValue}',icon: "st.Health & Wellness.health9")
            }
        }		
        
		standardTile("contact", "device.contact", width: 2, height: 2) {
			state("open", label:'${name}', icon:"st.contact.contact.open", backgroundColor:"#e86d13")
			state("closed", label:'${name}', icon:"st.contact.contact.closed", backgroundColor:"#00a0dc")
		}
        
 		main (["door", "contact"])
		details (["door", "contact"])
    }
}

def logsOff(){
    log.warn "debug logging disabled..."
    device.updateSetting("logEnable",[value:"false",type:"bool"])
}

// handle commands
def open() {
	if(device.currentValue("door") == "closed") sendData("on")
}

def close() {
	if(device.currentValue("door") == "open") sendData("on")
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
        // Also update the "Contact Sensor" device as this is useful for SmartApps that do not support the "Door Control" capability
        if((value == "open") || (value == "closed")) {
            sendEvent(name: "contact", value: value)
        }
        // Update lastUpdated date and time
        def nowDay = new Date().format("MMM dd", location.timeZone)
        def nowTime = new Date().format("h:mm a", location.timeZone)
        sendEvent(name: "lastUpdated", value: nowDay + " at " + nowTime, displayed: false)
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
