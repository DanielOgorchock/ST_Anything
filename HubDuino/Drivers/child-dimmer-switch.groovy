/**
 *  Child Dimmer Switch
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-dimmer-switch.groovy
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
 *    2017-06-10  Dan Ogorchock  Original Creation
 *    2017-08-23  Allan (vseven) Added a generateEvent routine that gets info from the parent device.  This routine runs each time the value is updated which can lead to other modifications of the device.
 *    2018-05-19  Dan Ogorchock  Added support to receive the dimmer level from the microcontroller
 *    2018-06-02  Dan Ogorchock  Revised/Simplified for Hubitat Composite Driver Model
 *    2018-09-22  Dan Ogorchock  Added preference for debug logging
 *    2019-07-01  Dan Ogorchock  Added importUrl
 *    2020-01-25  Dan Ogorchock  Remove custom lastUpdated attribute & general code cleanup
 *
 * 
 */
metadata {
	definition (name: "Child Dimmer Switch", namespace: "ogiewon", author: "Dan Ogorchock", importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-dimmer-switch.groovy") {
		capability "Switch Level"
        capability "Switch"
		capability "Relay Switch"
		capability "Actuator"
		capability "Sensor"
	}
    
    preferences {
        input name: "logEnable", type: "bool", title: "Enable debug logging", defaultValue: true
	}
}

def logsOff(){
    log.warn "debug logging disabled..."
    device.updateSetting("logEnable",[value:"false",type:"bool"])
}

def on() {
	sendData("on")
}

def off() {
	sendData("off")
}

def setLevel(value,duration=null) {
	if (logEnable) log.debug "setLevel >> value: $value"
	def valueaux = value as Integer
	def level = Math.max(Math.min(valueaux, 99), 0)
    sendData("${level}")
//	if (level > 0) {
//		sendEvent(name: "switch", value: "on")
//	} else {
//		sendEvent(name: "switch", value: "off")
//	}
	sendEvent(name: "level", value: level, unit: "%")
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
    if (name && value) {    // Update device
        if ((value == "on") || (value == "off")) {
            sendEvent(name: "switch", value: value)
        }
        else
        {
            sendEvent(name: "level", value: value)
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
