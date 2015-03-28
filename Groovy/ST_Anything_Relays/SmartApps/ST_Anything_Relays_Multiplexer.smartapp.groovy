/**
 *  ST_Anything_Relays Multiplexer - ST_Anything_Relays_Multiplexer.smartapp.groovy
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
 *    2015-03-27  Dan Ogorchock  Original Creation
 *
 */
definition(
    name: "ST_Anything_Relays Multiplexer",
    namespace: "ogiewon",
    author: "Daniel Ogorchock",
    description: "Virtual Switches to Arduino Relays Multiplexer/Demultiplexer",
    category: "My Apps",
    iconUrl: "https://s3.amazonaws.com/smartapp-icons/Convenience/Cat-Convenience.png",
    iconX2Url: "https://s3.amazonaws.com/smartapp-icons/Convenience/Cat-Convenience@2x.png",
    iconX3Url: "https://s3.amazonaws.com/smartapp-icons/Convenience/Cat-Convenience@2x.png")


preferences {
	section("Select the Relays (Virtual Switch devices)") {
		input "virtual_switch_1", title: "Virtual Switch for Relay 1", "capability.switch", required: true
		input "virtual_switch_2", title: "Virtual Switch for Relay 2", "capability.switch", required: false
		input "virtual_switch_3", title: "Virtual Switch for Relay 3", "capability.switch", required: false
		input "virtual_switch_4", title: "Virtual Switch for Relay 4", "capability.switch", required: false
		input "virtual_switch_5", title: "Virtual Switch for Relay 5", "capability.switch", required: false
		input "virtual_switch_6", title: "Virtual Switch for Relay 6", "capability.switch", required: false
		input "virtual_switch_7", title: "Virtual Switch for Relay 7", "capability.switch", required: false
		input "virtual_switch_8", title: "Virtual Switch for Relay 8", "capability.switch", required: false
		input "virtual_switch_9", title: "Virtual Switch for Relay 9", "capability.switch", required: false
		input "virtual_switch_10", title: "Virtual Switch for Relay 10", "capability.switch", required: false
		input "virtual_switch_11", title: "Virtual Switch for Relay 11", "capability.switch", required: false
		input "virtual_switch_12", title: "Virtual Switch for Relay 12", "capability.switch", required: false
		input "virtual_switch_13", title: "Virtual Switch for Relay 13", "capability.switch", required: false
		input "virtual_switch_14", title: "Virtual Switch for Relay 14", "capability.switch", required: false
		input "virtual_switch_15", title: "Virtual Switch for Relay 15", "capability.switch", required: false
		input "virtual_switch_16", title: "Virtual Switch for Relay 16", "capability.switch", required: false
	}

	section("Select the Arduino ST_Anything_Relays device") {
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

        subscribe(arduino, "switch1.on", switch1on)
        subscribe(arduino, "switch1.off", switch1off)
        subscribe(virtual_switch_1, "switch.on", relay1on)
        subscribe(virtual_switch_1, "switch.off", relay1off)
	
    if (virtual_switch_2) {
        subscribe(arduino, "switch2.on", switch2on)
        subscribe(arduino, "switch2.off", switch2off)
        subscribe(virtual_switch_2, "switch.on", relay2on)
        subscribe(virtual_switch_2, "switch.off", relay2off)
    }

    if (virtual_switch_3) {
        subscribe(arduino, "switch3.on", switch3on)
        subscribe(arduino, "switch3.off", switch3off)
        subscribe(virtual_switch_3, "switch.on", relay3on)
        subscribe(virtual_switch_3, "switch.off", relay3off)
    }

    if (virtual_switch_4) {
        subscribe(arduino, "switch4.on", switch4on)
        subscribe(arduino, "switch4.off", switch4off)
        subscribe(virtual_switch_4, "switch.on", relay4on)
        subscribe(virtual_switch_4, "switch.off", relay4off)
    }

    if (virtual_switch_5) {
        subscribe(arduino, "switch5.on", switch5on)
        subscribe(arduino, "switch5.off", switch5off)
        subscribe(virtual_switch_5, "switch.on", relay5on)
        subscribe(virtual_switch_5, "switch.off", relay5off)
    }

    if (virtual_switch_6) {
        subscribe(arduino, "switch6.on", switch6on)
        subscribe(arduino, "switch6.off", switch6off)
        subscribe(virtual_switch_6, "switch.on", relay6on)
        subscribe(virtual_switch_6, "switch.off", relay6off)
    }

    if (virtual_switch_7) {
        subscribe(arduino, "switch7.on", switch7on)
        subscribe(arduino, "switch7.off", switch7off)
        subscribe(virtual_switch_7, "switch.on", relay7on)
        subscribe(virtual_switch_7, "switch.off", relay7off)
    }

    if (virtual_switch_8) {
        subscribe(arduino, "switch8.on", switch8on)
        subscribe(arduino, "switch8.off", switch8off)
        subscribe(virtual_switch_8, "switch.on", relay8on)
        subscribe(virtual_switch_8, "switch.off", relay8off)
    }

    if (virtual_switch_9) {
        subscribe(arduino, "switch9.on", switch9on)
        subscribe(arduino, "switch9.off", switch9off)
        subscribe(virtual_switch_9, "switch.on", relay9on)
        subscribe(virtual_switch_9, "switch.off", relay9off)
    }

    if (virtual_switch_10) {
        subscribe(arduino, "switch10.on", switch10on)
        subscribe(arduino, "switch10.off", switch10off)
        subscribe(virtual_switch_10, "switch.on", relay10on)
        subscribe(virtual_switch_10, "switch.off", relay10off)
    }

    if (virtual_switch_11) {
        subscribe(arduino, "switch11.on", switch11on)
        subscribe(arduino, "switch11.off", switch11off)
        subscribe(virtual_switch_11, "switch.on", relay11on)
        subscribe(virtual_switch_11, "switch.off", relay11off)
    }

    if (virtual_switch_12) {
        subscribe(arduino, "switch12.on", switch12on)
        subscribe(arduino, "switch12.off", switch2off)
        subscribe(virtual_switch_12, "switch.on", relay12on)
        subscribe(virtual_switch_12, "switch.off", relay12off)
    }

    if (virtual_switch_13) {
        subscribe(arduino, "switch13.on", switch13on)
        subscribe(arduino, "switch13.off", switch13off)
        subscribe(virtual_switch_13, "switch.on", relay13on)
        subscribe(virtual_switch_13, "switch.off", relay13off)
    }

    if (virtual_switch_14) {
        subscribe(arduino, "switch14.on", switch14on)
        subscribe(arduino, "switch14.off", switch14off)
        subscribe(virtual_switch_14, "switch.on", relay14on)
        subscribe(virtual_switch_14, "switch.off", relay14off)
    }

    if (virtual_switch_15) {
        subscribe(arduino, "switch15.on", switch15on)
        subscribe(arduino, "switch15.off", switch15off)
        subscribe(virtual_switch_15, "switch.on", relay15on)
        subscribe(virtual_switch_15, "switch.off", relay15off)
    }

    if (virtual_switch_16) {
        subscribe(arduino, "switch16.on", switch16on)
        subscribe(arduino, "switch16.off", switch16off)
        subscribe(virtual_switch_16, "switch.on", relay16on)
        subscribe(virtual_switch_16, "switch.off", relay16off)
    }

}

//--------------- Relay 1 handlers ---------------
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
def relay1on(evt)
{
    if (arduino.currentValue("switch1") != "on") {
        log.debug "relay1event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino Relay to match Virtual Switch"
        arduino.switch1on()
    }
}
def relay1off(evt)
{
    if (arduino.currentValue("switch1") != "off") {
        log.debug "relay1event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino Relay to match Virtual Switch"
        arduino.switch1off()	
    }
}

//--------------- Relay 2 handlers ---------------
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
def relay2on(evt)
{
    if (arduino.currentValue("switch2") != "on") {
        log.debug "relay2event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino Relay to match Virtual Switch"
        arduino.switch2on()
    }	
}
def relay2off(evt)
{
    if (arduino.currentValue("switch2") != "off") {
        log.debug "relay2event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino Relay to match Virtual Switch"
        arduino.switch2off()
    }
}

//--------------- Relay 3 handlers ---------------
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
def relay3on(evt)
{
    if (arduino.currentValue("switch3") != "on") {
        log.debug "relay3event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino Relay to match Virtual Switch"
        arduino.switch3on()
    }	
}
def relay3off(evt)
{
    if (arduino.currentValue("switch3") != "off") {
        log.debug "relay3event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino Relay to match Virtual Switch"
        arduino.switch3off()
    }	
}

//--------------- Relay 4 handlers ---------------
def switch4on(evt)
{
    if (virtual_switch_4.currentValue("switch") != "on") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping On Virtual Switch to match Arduino"
    	virtual_switch_4.on()
    }	
}
def switch4off(evt)
{
    if (virtual_switch_4.currentValue("switch") != "off") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping Off Virtual Switch to match Arduino"
    	virtual_switch_4.off()
    }	
}
def relay4on(evt)
{
    if (arduino.currentValue("switch4") != "on") {
        log.debug "relay4event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino Relay to match Virtual Switch"
        arduino.switch4on()
    }	
}
def relay4off(evt)
{
    if (arduino.currentValue("switch4") != "off") {
        log.debug "relay4event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino Relay to match Virtual Switch"
        arduino.switch4off()
    }	
}

//--------------- Relay 5 handlers ---------------
def switch5on(evt)
{
    if (virtual_switch_5.currentValue("switch") != "on") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping On Virtual Switch to match Arduino"
    	virtual_switch_5.on()
    }	
}
def switch5off(evt)
{
    if (virtual_switch_5.currentValue("switch") != "off") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping Off Virtual Switch to match Arduino"
    	virtual_switch_5.off()
    }	
}
def relay5on(evt)
{
    if (arduino.currentValue("switch5") != "on") {
        log.debug "relay5event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino Relay to match Virtual Switch"
        arduino.switch5on()
    }	
}
def relay5off(evt)
{
    if (arduino.currentValue("switch5") != "off") {
        log.debug "relay5event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino Relay to match Virtual Switch"
        arduino.switch5off()
    }
}

//--------------- Relay 6 handlers ---------------
def switch6on(evt)
{
    if (virtual_switch_6.currentValue("switch") != "on") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping On Virtual Switch to match Arduino"
    	virtual_switch_6.on()
    }	
}
def switch6off(evt)
{
    if (virtual_switch_6.currentValue("switch") != "off") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping Off Virtual Switch to match Arduino"
    	virtual_switch_6.off()
    }	
}
def relay6on(evt)
{
    if (arduino.currentValue("switch6") != "on") {
        log.debug "relay6event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino Relay to match Virtual Switch"
        arduino.switch6on()
    }	
}
def relay6off(evt)
{
    if (arduino.currentValue("switch6") != "off") {
        log.debug "relay6event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino Relay to match Virtual Switch"
        arduino.switch6off()
    }
}

//--------------- Relay 7 handlers ---------------
def switch7on(evt)
{
    if (virtual_switch_7.currentValue("switch") != "on") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping On Virtual Switch to match Arduino"
    	virtual_switch_7.on()
    }	
}
def switch7off(evt)
{
    if (virtual_switch_7.currentValue("switch") != "off") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping Off Virtual Switch to match Arduino"
    	virtual_switch_7.off()
    }	
}
def relay7on(evt)
{
    if (arduino.currentValue("switch7") != "on") {
        log.debug "relay7event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino Relay to match Virtual Switch"
        arduino.switch7on()
    }	
}
def relay7off(evt)
{
    if (arduino.currentValue("switch7") != "off") {
        log.debug "relay7event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino Relay to match Virtual Switch"
        arduino.switch7off()
    }	
}

//--------------- Relay 8 handlers ---------------
def switch8on(evt)
{
    if (virtual_switch_8.currentValue("switch") != "on") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping On Virtual Switch to match Arduino"
    	virtual_switch_8.on()
    }	
}
def switch8off(evt)
{
    if (virtual_switch_8.currentValue("switch") != "off") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping Off Virtual Switch to match Arduino"
    	virtual_switch_8.off()
    }	
}
def relay8on(evt)
{
    if (arduino.currentValue("switch8") != "on") {
        log.debug "relay8event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino Relay to match Virtual Switch"
        arduino.switch8on()
    }	
}
def relay8off(evt)
{
    if (arduino.currentValue("switch8") != "off") {
        log.debug "relay8event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino Relay to match Virtual Switch"
        arduino.switch8off()
    }
}

//--------------- Relay 9 handlers ---------------
def switch9on(evt)
{
    if (virtual_switch_9.currentValue("switch") != "on") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping On Virtual Switch to match Arduino"
    	virtual_switch_9.on()
    }	
}
def switch9off(evt)
{
    if (virtual_switch_9.currentValue("switch") != "off") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping Off Virtual Switch to match Arduino"
    	virtual_switch_9.off()
    }	
}
def relay9on(evt)
{
    if (arduino.currentValue("switch9") != "on") {
        log.debug "relay9event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino Relay to match Virtual Switch"
        arduino.switch9on()
    }	
}
def relay9off(evt)
{
    if (arduino.currentValue("switch9") != "off") {
        log.debug "relay9event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino Relay to match Virtual Switch"
        arduino.switch9off()
    }	
}

//--------------- Relay 10 handlers ---------------
def switch10on(evt)
{
    if (virtual_switch_10.currentValue("switch") != "on") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping On Virtual Switch to match Arduino"
    	virtual_switch_10.on()
    }	
}
def switch10off(evt)
{
    if (virtual_switch_10.currentValue("switch") != "off") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping Off Virtual Switch to match Arduino"
    	virtual_switch_10.off()
    }	
}
def relay10on(evt)
{
    if (arduino.currentValue("switch10") != "on") {
        log.debug "relay10event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino Relay to match Virtual Switch"
        arduino.switch10on()
    }	
}
def relay10off(evt)
{
    if (arduino.currentValue("switch10") != "off") {
        log.debug "relay10event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino Relay to match Virtual Switch"
        arduino.switch10off()
    }
}

//--------------- Relay 11 handlers ---------------
def switch11on(evt)
{
    if (virtual_switch_11.currentValue("switch") != "on") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping On Virtual Switch to match Arduino"
    	virtual_switch_11.on()
    }	
}
def switch11off(evt)
{
    if (virtual_switch_11.currentValue("switch") != "off") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping Off Virtual Switch to match Arduino"
    	virtual_switch_11.off()
    }	
}
def relay11on(evt)
{
    if (arduino.currentValue("switch11") != "on") {
        log.debug "relay11event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino Relay to match Virtual Switch"
        arduino.switch11on()
    }	
}
def relay11off(evt)
{
    if (arduino.currentValue("switch11") != "off") {
        log.debug "relay11event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino Relay to match Virtual Switch"
        arduino.switch11off()
    }	
}

//--------------- Relay 12 handlers ---------------
def switch12on(evt)
{
    if (virtual_switch_12.currentValue("switch") != "on") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping On Virtual Switch to match Arduino"
    	virtual_switch_12.on()
    }	
}
def switch12off(evt)
{
    if (virtual_switch_12.currentValue("switch") != "off") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping Off Virtual Switch to match Arduino"
    	virtual_switch_12.off()
    }	
}
def relay12on(evt)
{
    if (arduino.currentValue("switch12") != "on") {
        log.debug "relay12event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino Relay to match Virtual Switch"
        arduino.switch12on()
    }	
}
def relay12off(evt)
{
    if (arduino.currentValue("switch12") != "off") {
        log.debug "relay12event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino Relay to match Virtual Switch"
        arduino.switch12off()
    }	
}

//--------------- Relay 13 handlers ---------------
def switch13on(evt)
{
    if (virtual_switch_13.currentValue("switch") != "on") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping On Virtual Switch to match Arduino"
    	virtual_switch_13.on()
    }	
}
def switch13off(evt)
{
    if (virtual_switch_13.currentValue("switch") != "off") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping Off Virtual Switch to match Arduino"
    	virtual_switch_13.off()
    }	
}
def relay13on(evt)
{
    if (arduino.currentValue("switch13") != "on") {
        log.debug "relay13event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino Relay to match Virtual Switch"
        arduino.switch13on()
    }	
}
def relay13off(evt)
{
    if (arduino.currentValue("switch13") != "off") {
        log.debug "relay13event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino Relay to match Virtual Switch"
        arduino.switch13off()
    }	
}

//--------------- Relay 14 handlers ---------------
def switch14on(evt)
{
    if (virtual_switch_14.currentValue("switch") != "on") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping On Virtual Switch to match Arduino"
    	virtual_switch_14.on()
    }	
}
def switch14off(evt)
{
    if (virtual_switch_14.currentValue("switch") != "off") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping Off Virtual Switch to match Arduino"
    	virtual_switch_14.off()
    }	
}
def relay14on(evt)
{
    if (arduino.currentValue("switch14") != "on") {
        log.debug "relay14event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino Relay to match Virtual Switch"
        arduino.switch14on()
    }	
}
def relay14off(evt)
{
    if (arduino.currentValue("switch14") != "off") {
        log.debug "relay14event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino Relay to match Virtual Switch"
        arduino.switch14off()
    }	
}

//--------------- Relay 15 handlers ---------------
def switch15on(evt)
{
    if (virtual_switch_15.currentValue("switch") != "on") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping On Virtual Switch to match Arduino"
    	virtual_switch_15.on()
    }	
}
def switch15off(evt)
{
    if (virtual_switch_15.currentValue("switch") != "off") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping Off Virtual Switch to match Arduino"
    	virtual_switch_15.off()
    }	
}
def relay15on(evt)
{
    if (arduino.currentValue("switch15") != "on") {
        log.debug "relay15event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino Relay to match Virtual Switch"
        arduino.switch15on()
    }	
}
def relay15off(evt)
{
    if (arduino.currentValue("switch15") != "off") {
        log.debug "relay15event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino Relay to match Virtual Switch"
        arduino.switch15off()
    }	
}

//--------------- Relay 16 handlers ---------------
def switch16on(evt)
{
    if (virtual_switch_16.currentValue("switch") != "on") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping On Virtual Switch to match Arduino"
    	virtual_switch_16.on()
    }	
}
def switch16off(evt)
{
    if (virtual_switch_16.currentValue("switch") != "off") {
    	log.debug "arduinoevent($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Flipping Off Virtual Switch to match Arduino"
    	virtual_switch_16.off()
    }	
}
def relay16on(evt)
{
    if (arduino.currentValue("switch16") != "on") {
        log.debug "relay16event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning On Arduino Relay to match Virtual Switch"
        arduino.switch16on()
    }	
}
def relay16off(evt)
{
    if (arduino.currentValue("switch16") != "off") {
        log.debug "relay16event($evt.name: $evt.value: $evt.deviceId)"
        log.debug "Turning Off Arduino Relay to match Virtual Switch"
        arduino.switch16off()
    }
}
