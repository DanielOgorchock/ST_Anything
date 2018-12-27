/**
 *  Child Alarm
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-alarm.groovy
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
 *    2017-08-23  Allan (vseven) Added a generateEvent routine that gets info from the parent device.  This routine runs each time the value is updated which can lead to other modifications of the device.
 *    2018-06-02  Dan Ogorchock  Revised/Simplified for Hubitat Composite Driver Model
 *    2018-09-22  Dan Ogorchock  Added preference for debug logging
 *
 * 
 */
metadata {
	definition (name: "Child Alarm", namespace: "ogiewon", author: "Dan Ogorchock") {
		capability "Actuator"
		capability "Alarm"
		capability "Switch"

		attribute "lastUpdated", "String"

		command "test"
	}

	simulator {

	}

    preferences {
        input name: "logEnable", type: "bool", title: "Enable debug logging", defaultValue: true
	}

	tiles(scale: 2) {
        multiAttributeTile(name:"alarm", type: "generic", width: 6, height: 4){
            tileAttribute ("device.alarm", key: "PRIMARY_CONTROL") {
                attributeState "off", label:'off', action:'alarm.both', icon:"st.alarm.alarm.alarm", backgroundColor:"#ffffff"
                attributeState "both", label:'alarm!', action:'alarm.off', icon:"st.alarm.alarm.alarm", backgroundColor:"#e86d13"
                attributeState "strobe", label:'strobe!', action:'alarm.off', icon:"st.alarm.alarm.alarm", backgroundColor:"#e86d13"
                attributeState "siren", label:'siren!', action:'alarm.off', icon:"st.alarm.alarm.alarm", backgroundColor:"#e86d13"
    		}
 			tileAttribute("device.lastUpdated", key: "SECONDARY_CONTROL") {
    				attributeState("default", label:'    Last updated ${currentValue}',icon: "st.Health & Wellness.health9")
            }
	}
        standardTile("siren", "device.alarm", inactiveLabel: false, decoration: "flat", width: 2, height: 2) {
//            state "default", label:'', action:"alarm.siren", icon:"st.secondary.siren"
			state "off", label:'', action:"alarm.siren", icon:"st.secondary.siren", backgroundColor:"#ffffff"
			state "strobe", label:'', action:"alarm.siren", icon:"st.secondary.siren", backgroundColor:"#ffffff"
			state "siren", label:'', action:'alarm.off', icon:"st.secondary.siren", backgroundColor:"#e86d13"
			state "both", label:'', action:'alarm.siren', icon:"st.secondary.siren", backgroundColor:"#e86d13"
        }
        standardTile("strobe", "device.alarm", inactiveLabel: false, decoration: "flat", width: 2, height: 2) {
//            state "default", label:'', action:"alarm.strobe", icon:"st.secondary.strobe"
			state "off", label:'', action:"alarm.strobe", icon:"st.secondary.strobe", backgroundColor:"#ffffff"
			state "siren", label:'', action:"alarm.strobe", icon:"st.secondary.strobe", backgroundColor:"#ffffff"
			state "strobe", label:'', action:'alarm.off', icon:"st.secondary.strobe", backgroundColor:"#e86d13"
			state "both", label:'', action:'alarm.strobe', icon:"st.secondary.strobe", backgroundColor:"#e86d13"
		}
        standardTile("off", "device.alarm", inactiveLabel: false, decoration: "flat", width: 2, height: 2) {
            state "default", label:'', action:"alarm.off", icon:"st.secondary.off"
        }
        standardTile("test", "device.alarm", inactiveLabel: false, decoration: "flat", width: 2, height: 2) {
            state "default", label:'', action:"test", icon:"st.secondary.test"
        }

        main "alarm"
        details(["alarm", "siren", "strobe", "off", "test"])
	}
}

def on() {
    sendData("both")
}

def off() {
    sendData("off")
}

def strobe() {
    sendData("strobe")
}

def siren() {
    sendData("siren")
}

def both() {
    sendData("both")
}

def test() {
	both()
    runIn(3, off)
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
        sendEvent(name: name, value: value)
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
