/**
 *  Child Servo
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-servo.groovy
 *
 *  Copyright 2018 Daniel Ogorchock
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
 *    2018-06-24  Dan Ogorchock  Original Creation
 *    2018-09-22  Dan Ogorchock  Added preference for debug logging
 *    2019-01-04  Dan Ogorchock  Added Switch Capability and preset values for ON and OFF (thanks @ryan780)
 * 
 */
metadata {
	definition (name: "Child Servo", namespace: "ogiewon", author: "Dan Ogorchock") {
            capability "Switch"
            capability "Switch Level"
            capability "Actuator"
            capability "Sensor"

            attribute "lastUpdated", "String"
            attribute "angle", "number"
	}

	simulator {

	}

    preferences {
        input ("onvalue", "number", title: "On Percentage", required: false, defaultValue: 50, description: "Percentage that should be used for On command.")
        input ("offvalue", "number", title: "Off Percentage", required: false, defaultValue: 0, description: "Percentage that should be used for Off command.")
        input name: "logEnable", type: "bool", title: "Enable debug logging", defaultValue: true
	}

	tiles(scale: 2) {
		controlTile("levelSliderControl", "device.level", "slider", height: 2, width: 2, inactiveLabel: false) {
			state "level", action:"switch level.setLevel"
		}
 		valueTile("level", "device.level", inactiveLabel: false, decoration: "flat", width: 2, height: 2) {
			state "level", label:'${currentValue}%', unit:"%", backgroundColor:"#ffffff"
		}
 		valueTile("angle", "device.angle", inactiveLabel: false, decoration: "flat", width: 2, height: 2) {
			state "angle", label:'${currentValue}°', unit:"degrees", backgroundColor:"#ffffff"
		}
 		valueTile("lastUpdated", "device.lastUpdated", inactiveLabel: false, decoration: "flat", width: 6, height: 2) {
    		state "default", label:'Last Updated ${currentValue}', backgroundColor:"#ffffff"
        }
       
		main(["angle"])
		details(["levelSliderControl", "level", "angle", "lastUpdated"])       
	}
}

def on() {
	setLevel(onvalue)
}

def off() {
	setLevel(offvalue)
}

def logsOff(){
    log.warn "debug logging disabled..."
    device.updateSetting("logEnable",[value:"false",type:"bool"])
}

def setLevel(value,duration=null) {
    if (logEnable) log.debug "setLevel >> value: $value"
    def valueaux = value as Integer
    def level = Math.max(Math.min(valueaux, 99), 0)
    sendData("${level}")
    //sendEvent(name: "level", value: level, unit: "%")
    if (level == offvalue){
        sendEvent(name: "switch", value: "off", isStateChange: true)
    }
    else {
	    sendEvent(name: "switch", value: "on", isStateChange: true)
    }
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
        sendEvent(name: "level", value: value)
        def angleValue = value.toFloat() * 1.81818
        angleValue = angleValue.round(2)
        sendEvent(name: "angle", value: angleValue)
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
