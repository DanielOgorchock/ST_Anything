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
 *    2019-01-30  Jeff Albers	 Set upper level value as 100 instead of 99
 *    2019-02-09  Dan Ogorchock  Updated to support enhanced EX_Servo class - special thanks to Jeff Albers!
 *    2019-07-01  Dan Ogorchock  Added importUrl
 *    2020-01-25  Dan Ogorchock  Ensure RATE from user preference section is always used in setLevel() command 
 *    2020-01-25  Dan Ogorchock  Remove custom lastUpdated attribute & general code cleanup
 *    2020-01-26  Dan Ogorchock  Add Reverse Direction and OVerride Rate user preferences - thanks @flotsam1! 
 * 
 */
metadata {
	definition (name: "Child Servo", namespace: "ogiewon", author: "Dan Ogorchock", importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-servo.groovy") {
            capability "Switch"
            capability "Switch Level"
            capability "Actuator"
            capability "Sensor"

            attribute "angle", "number"
            attribute "rate", "number"
	}

   	preferences {
            input ("onvalue", "number", title: "On Percentage", required: true, defaultValue: 100, description: "Percentage that should be used for On command.")
            input ("offvalue", "number", title: "Off Percentage", required: true, defaultValue: 0, description: "Percentage that should be used for Off command.")
            input ("rateValue", "number", title: "Default Rate (Duration)", required: true, defaultValue: 1000, description: "Time in milliseconds to transition from 0 to 180 degrees (0 = full speed)")
            input ("reverseDirection", "bool", title: "Reverse Servo Direction?", required: true, defaultValue: false, description: "Can be useful for backwards blinds")
            input ("overrideRate", "bool", title: "Override Rate?", required: true, defaultValue: false, description: "On = always use 'Default Rate'")
            input name: "logEnable", type: "bool", title: "Enable debug logging", defaultValue: true
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

def setLevel(level, rate = null) {
    if (overrideRate == true) rate = rateValue.toInteger()    
    if (rate == null) rate = rateValue.toInteger()

    if (reverseDirection == true) level = 100 - level
    
	if (logEnable) log.debug "setLevel >> level: ${level}, rate: ${rate}"
    
    level = Math.max(Math.min(level.toInteger(), 100), 0)
    rate = Math.max(Math.min(rate.toInteger(), 30000), 0)
    
    sendData("${level}:${rate}")
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
        def myValues = value.split(':')
        if (reverseDirection == true) {
            myValues[0] = 100 - myValues[0].toInteger()
            myValues[1] = 180 - myValues[1].toInteger()
        }
        sendEvent(name: "level",value: myValues[0].toInteger())
        sendEvent(name: "angle", value: myValues[1].toInteger())
        sendEvent(name: "rate", value: myValues[2].toInteger())
        if (myValues[0].toInteger() <= offvalue.toInteger()){
            sendEvent(name: "switch", value: "off")
        }
        else {
            sendEvent(name: "switch", value: "on")
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
