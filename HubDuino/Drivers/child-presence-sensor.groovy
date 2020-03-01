/**
 *  Child Presence Sensor
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-presence-sensor.groovy
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
 *    2018-02-24  Dan Ogorchock  Original Creation
 *    2018-06-02  Dan Ogorchock  Revised/Simplified for Hubitat Composite Driver Model
 *    2018-09-22  Dan Ogorchock  Added preference for debug logging
 *    2019-07-01  Dan Ogorchock  Added importUrl
 *    2019-12-17  Dan Ogorchock  Suppress debug logging based on user setting
 *    2020-01-25  Dan Ogorchock  Remove custom lastUpdated attribute & general code cleanup
 *    2020-03-01  Dan Ogorchock  Incorporated optional feature from @ritchierich to prevent presence value 'flapping'
 * 
 */
metadata {
	definition (name: "Child Presence Sensor", namespace: "ogiewon", author: "Daniel Ogorchock", importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-presence-sensor.groovy") {
		capability "Sensor"
		capability "Presence Sensor"

        attribute "level", "Number"
	}
   
	preferences {
        input "waitSeconds", "number", title: "Number of seconds to wait to verify presence", required: false
        input "presenceTriggerValue", "number", title: "(Optional) Presence Trigger Value\nAt what value is presence triggered?", required: false, displayDuringSetup: false
        input "invertTriggerLogic", "bool", title: "(Optional) Invert Logic", description: "False = Present > Trigger Value\nTrue = Present < Trigger Value", default: false, required: false, displayDuringSetup: false
        input name: "logEnable", type: "bool", title: "Enable debug logging", defaultValue: true
	}
}

def logsOff(){
    log.warn "debug logging disabled..."
    device.updateSetting("logEnable",[value:"false",type:"bool"])
}

def parse(String description) {
    if (logEnable) log.debug "parse(${description}) called"
	def parts = description.split(" ")
    def name  = parts.length>0?parts[0].trim():null
    def value = parts.length>1?parts[1].trim():null
    if (name && value) {
        if (value.isNumber()) {
            sendEvent(name: "level", value: value)
            if (presenceTriggerValue) {
                if (logEnable) log.debug "Presence received a numeric value. Perform comparison of value: ${Float.valueOf(value.trim())} versus presenceTriggerValue: ${presenceTriggerValue}"
                if (Float.valueOf(value.trim()) >= presenceTriggerValue) {
                    value = invertTriggerLogic?"not present":"present"
                } 
                else {
                    value = invertTriggerLogic?"present":"not present"
                }
            }
            else {
                log.error "Please configure the Presence Trigger Value in device settings!"
            }
        }
        else {
            if (logEnable) log.debug "Presence received a string.  value = ${value}"
            if (value != "present") { value = "not present" }
        }
        // Update device
        if (waitSeconds) {
            state.lastValue = value
            if (device.currentValue("presence") != value && state.isScheduled == false) {
                state.isScheduled = true
                runIn(waitSeconds, "processDelay")
                if (logEnable) log.debug "Scheduled delay.  state.lastValue = ${state.lastValue}, scheduled: ${state.isScheduled}"
            } else if (state.isScheduled == true) {
                unschedule("processDelay")
                state.isScheduled = false
                if (logEnable) log.debug "UnScheduled delay.  state.lastValue = ${state.lastValue}, scheduled: ${state.isScheduled}"
            }
        } else {
            sendEvent(name: name, value: value)
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
    state.lastUpdated = ""
    state.isScheduled = false
    if (waitSeconds) {
        state.lastValue = ""
    }
}

private processDelay() {
    if (logEnable) log.debug "processDelay currentValue = ${device.currentValue("presence")}, state.lastValue: ${state.lastValue}"
    state.isScheduled = false
    if (device.currentValue("presence") != state.lastValue) {
        sendEvent(name: "presence", value: state.lastValue)
    }
}