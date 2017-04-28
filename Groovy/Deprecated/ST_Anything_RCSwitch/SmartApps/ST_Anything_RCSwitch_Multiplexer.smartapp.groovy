/**
 *  ST_Anything_RCSwitch Multiplexer - ST_Anything_RCSwitch _Multiplexer.smartapp.groovy
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
 *    2015-06-18  Dan Ogorchock  Original Creation
 *
 */
definition(
    name: "ST_Anything_RCSwitch Multiplexer",
    namespace: "ogiewon",
    author: "Daniel Ogorchock",
    description: "Virtual Switches to Arduino RCSwitches Multiplexer/Demultiplexer",
    category: "My Apps",
    iconUrl: "https://s3.amazonaws.com/smartapp-icons/Convenience/Cat-Convenience.png",
    iconX2Url: "https://s3.amazonaws.com/smartapp-icons/Convenience/Cat-Convenience@2x.png",
    iconX3Url: "https://s3.amazonaws.com/smartapp-icons/Convenience/Cat-Convenience@2x.png")


preferences {
	section("Select the Switches (Virtual Switch devices)") {
		input "virtual_switch_1", title: "Virtual Switch for RCSwitch 1", "capability.switch", required: true
		input "virtual_switch_2", title: "Virtual Switch for RCSwitch 2", "capability.switch", required: false
		input "virtual_switch_3", title: "Virtual Switch for RCSwitch 3", "capability.switch", required: false
	}

	section("Select the Arduino ST_Anything_RCSwitch device") {
		input "arduino", "capability.switch", required: true
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

        subscribe(arduino, "rcswitch1.on", switch1on)
        subscribe(arduino, "rcswitch1.off", switch1off)
        subscribe(virtual_switch_1, "switch.on", rcswitch1on)
        subscribe(virtual_switch_1, "switch.off", rcswitch1off)
	
    if (virtual_switch_2) {
        subscribe(arduino, "rcswitch2.on", switch2on)
        subscribe(arduino, "rcswitch2.off", switch2off)
        subscribe(virtual_switch_2, "switch.on", rcswitch2on)
        subscribe(virtual_switch_2, "switch.off", rcswitch2off)
    }

    if (virtual_switch_3) {
        subscribe(arduino, "rcswitch3.on", switch3on)
        subscribe(arduino, "rcswitch3.off", switch3off)
        subscribe(virtual_switch_3, "switch.on", rcswitch3on)
        subscribe(virtual_switch_3, "switch.off", rcswitch3off)
    }

}

//--------------- RCSwitch 1 handlers ---------------
def switch1on(evt)
{
    if (virtual_switch_1.currentValue("switch") != "on") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
    	log.debug "Flipping On Virtual Switch to match Arduino"
	virtual_switch_1.on()
    }	
}
def switch1off(evt)
{
    if (virtual_switch_1.currentValue("switch") != "off") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping Off Virtual Switch to match Arduino"
    	virtual_switch_1.off()
    }	
}
def rcswitch1on(evt)
{
    if (arduino.currentValue("switch1") != "on") {
        log.debug "rcswitch1event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino RCSwitch to match Virtual Switch"
        arduino.rcswitch1on()
    }
}
def rcswitch1off(evt)
{
    if (arduino.currentValue("switch1") != "off") {
        log.debug "rcswitch1event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino RCSwitch to match Virtual Switch"
        arduino.rcswitch1off()	
    }
}

//--------------- RCSwitch 2 handlers ---------------
def switch2on(evt)
{
    if (virtual_switch_2.currentValue("switch") != "on") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping On Virtual Switch to match Arduino"
    	virtual_switch_2.on()
    }	
}
def switch2off(evt)
{
    if (virtual_switch_2.currentValue("switch") != "off") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping Off Virtual Switch to match Arduino"
    	virtual_switch_2.off()
    }	
}
def rcswitch2on(evt)
{
    if (arduino.currentValue("switch2") != "on") {
        log.debug "rcswitch2event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino RCSwitch to match Virtual Switch"
        arduino.rcswitch2on()
    }	
}
def rcswitch2off(evt)
{
    if (arduino.currentValue("switch2") != "off") {
        log.debug "rcswitch2event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino RCSwitch to match Virtual Switch"
        arduino.rcswitch2off()
    }
}

//--------------- RCSwitch 3 handlers ---------------
def switch3on(evt)
{
    if (virtual_switch_3.currentValue("switch") != "on") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping On Virtual Switch to match Arduino"
    	virtual_switch_3.on()
    }	
}
def switch3off(evt)
{
    if (virtual_switch_3.currentValue("switch") != "off") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping Off Virtual Switch to match Arduino"
    	virtual_switch_3.off()
    }	
}
def rcswitch3on(evt)
{
    if (arduino.currentValue("switch3") != "on") {
        log.debug "rcswitch3event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino RCSwitch to match Virtual Switch"
        arduino.rcswitch3on()
    }	
}
def rcswitch3off(evt)
{
    if (arduino.currentValue("switch3") != "off") {
        log.debug "rcswitch3event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino RCSwitch to match Virtual Switch"
        arduino.rcswitch3off()
    }	
}
