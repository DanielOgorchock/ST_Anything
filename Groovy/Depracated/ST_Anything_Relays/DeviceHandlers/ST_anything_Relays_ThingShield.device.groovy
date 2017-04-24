/**
 *  ST_Anything_Relays - ST_Anything_Relays.device.groovy
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
 *
 */

metadata {
	definition (name: "ST_Anything_Relays_ThingShield", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Actuator"
		capability "Switch"

		attribute "switch1", "string"
		attribute "switch2", "string"
		attribute "switch3", "string"
		attribute "switch4", "string"
		attribute "switch5", "string"
		attribute "switch6", "string"
		attribute "switch7", "string"
		attribute "switch8", "string"
		attribute "switch9", "string"
		attribute "switch10", "string"
		attribute "switch11", "string"
		attribute "switch12", "string"
		attribute "switch13", "string"
		attribute "switch14", "string"
		attribute "switch15", "string"
		attribute "switch16", "string"
        
		command "switch1on"
		command "switch1off"
		command "switch2on"
		command "switch2off"
		command "switch3on"
		command "switch3off"
		command "switch4on"
		command "switch4off"
		command "switch5on"
		command "switch5off"
		command "switch6on"
		command "switch6off"
		command "switch7on"
		command "switch7off"
		command "switch8on"
		command "switch8off"
		command "switch9on"
		command "switch9off"
		command "switch10on"
		command "switch10off"
		command "switch11on"
		command "switch11off"
		command "switch12on"
		command "switch12off"
		command "switch13on"
		command "switch13off"
		command "switch14on"
		command "switch14off"
		command "switch15on"
		command "switch15off"
		command "switch16on"
		command "switch16off"

}

    simulator {
    }
	

	// tile definitions
	tiles {

        standardTile("switch1", "device.switch1", width: 1, height: 1, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch1on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch1off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}

        standardTile("switch2", "device.switch2", width: 1, height: 1, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch2on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch2off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}
        
        standardTile("switch3", "device.switch3", width: 1, height: 1, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch3on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch3off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}
        
        standardTile("switch4", "device.switch4", width: 1, height: 1, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch4on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch4off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}

        standardTile("switch5", "device.switch5", width: 1, height: 1, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch5on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch5off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}

        standardTile("switch6", "device.switch6", width: 1, height: 1, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch6on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch6off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}

        standardTile("switch7", "device.switch7", width: 1, height: 1, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch7on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch7off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}

        standardTile("switch8", "device.switch8", width: 1, height: 1, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch8on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch8off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}

        standardTile("switch9", "device.switch9", width: 1, height: 1, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch9on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch9off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}

        standardTile("switch10", "device.switch10", width: 1, height: 1, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch10on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch10off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}

        standardTile("switch11", "device.switch11", width: 1, height: 1, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch11on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch11off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}

        standardTile("switch12", "device.switch12", width: 1, height: 1, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch12on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch12off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}

        standardTile("switch13", "device.switch13", width: 1, height: 1, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch13on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch13off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}

        standardTile("switch14", "device.switch14", width: 1, height: 1, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch14on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch14off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}

        standardTile("switch15", "device.switch15", width: 1, height: 1, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch15on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch15off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}

        standardTile("switch16", "device.switch16", width: 1, height: 1, canChangeIcon: true) {
			state "off", label: '${name}', action: "switch16on", icon: "st.switches.switch.off", backgroundColor: "#ffffff"
			state "on", label: '${name}', action: "switch16off", icon: "st.switches.switch.on", backgroundColor: "#79b821"
		}

        main (["switch1"])
        details (["switch1","switch2","switch3","switch4","switch5","switch6","switch7","switch8","switch9","switch10","switch11","switch12","switch13","switch14","switch15","switch16"])
	}
}

//Map parse(String description) {
def parse(String description) {
    def msg = zigbee.parse(description)?.text
    log.debug "Parse got '${msg}'"

    def parts = msg.split(" ")
    def name  = parts.length>0?parts[0].trim():null
    def value = parts.length>1?parts[1].trim():null

    name = value != "ping" ? name : null

    def result = createEvent(name: name, value: value, isStateChange: true)

    log.debug result

    return result
}

def switch1on() {
	log.debug "Executing 'switch1on' = 'switch1 on'"
    zigbee.smartShield(text: "switch1 on").format()
}

def switch1off() {
	log.debug "Executing 'switch1off' = 'switch1 off'"
    zigbee.smartShield(text: "switch1 off").format()
}

def switch2on() {
	log.debug "Executing 'switch2on' = 'switch2 on'"
    zigbee.smartShield(text: "switch2 on").format()
}

def switch2off() {
	log.debug "Executing 'switch2off' = 'switch2 off'"
    zigbee.smartShield(text: "switch2 off").format()
}

def switch3on() {
	log.debug "Executing 'switch3on' = 'switch3 on'"
    zigbee.smartShield(text: "switch3 on").format()
}

def switch3off() {
	log.debug "Executing 'switch3off' = 'switch3 off'"
    zigbee.smartShield(text: "switch3 off").format()
}

def switch4on() {
	log.debug "Executing 'switch4on' = 'switch4 on'"
    zigbee.smartShield(text: "switch4 on").format()
}

def switch4off() {
	log.debug "Executing 'switch4off' = 'switch4 off'"
    zigbee.smartShield(text: "switch4 off").format()
}

def switch5on() {
	log.debug "Executing 'switch5on' = 'switch5 on'"
    zigbee.smartShield(text: "switch5 on").format()
}

def switch5off() {
	log.debug "Executing 'switch5off' = 'switch5 off'"
    zigbee.smartShield(text: "switch5 off").format()
}

def switch6on() {
	log.debug "Executing 'switch6on' = 'switch6 on'"
    zigbee.smartShield(text: "switch6 on").format()
}

def switch6off() {
	log.debug "Executing 'switch6off' = 'switch6 off'"
    zigbee.smartShield(text: "switch6 off").format()
}

def switch7on() {
	log.debug "Executing 'switch7on' = 'switch7 on'"
    zigbee.smartShield(text: "switch7 on").format()
}

def switch7off() {
	log.debug "Executing 'switch7off' = 'switch7 off'"
    zigbee.smartShield(text: "switch7 off").format()
}

def switch8on() {
	log.debug "Executing 'switch8on' = 'switch8 on'"
    zigbee.smartShield(text: "switch8 on").format()
}

def switch8off() {
	log.debug "Executing 'switch8off' = 'switch8 off'"
    zigbee.smartShield(text: "switch8 off").format()
}

def switch9on() {
	log.debug "Executing 'switch9on' = 'switch9 on'"
    zigbee.smartShield(text: "switch9 on").format()
}

def switch9off() {
	log.debug "Executing 'switch9off' = 'switch9 off'"
    zigbee.smartShield(text: "switch9 off").format()
}

def switch10on() {
	log.debug "Executing 'switch10on' = 'switch10 on'"
    zigbee.smartShield(text: "switch10 on").format()
}

def switch10off() {
	log.debug "Executing 'switch10off' = 'switch10 off'"
    zigbee.smartShield(text: "switch10 off").format()
}

def switch11on() {
	log.debug "Executing 'switch11on' = 'switch11 on'"
    zigbee.smartShield(text: "switch11 on").format()
}

def switch11off() {
	log.debug "Executing 'switch11off' = 'switch11 off'"
    zigbee.smartShield(text: "switch11 off").format()
}

def switch12on() {
	log.debug "Executing 'switch12on' = 'switch12 on'"
    zigbee.smartShield(text: "switch12 on").format()
}

def switch12off() {
	log.debug "Executing 'switch12off' = 'switch12 off'"
    zigbee.smartShield(text: "switch12 off").format()
}

def switch13on() {
	log.debug "Executing 'switch13on' = 'switch13 on'"
    zigbee.smartShield(text: "switch13 on").format()
}

def switch13off() {
	log.debug "Executing 'switch13off' = 'switch13 off'"
    zigbee.smartShield(text: "switch13 off").format()
}

def switch14on() {
	log.debug "Executing 'switch14on' = 'switch14 on'"
    zigbee.smartShield(text: "switch14 on").format()
}

def switch14off() {
	log.debug "Executing 'switch14off' = 'switch14 off'"
    zigbee.smartShield(text: "switch14 off").format()
}

def switch15on() {
	log.debug "Executing 'switch15on' = 'switch15 on'"
    zigbee.smartShield(text: "switch15 on").format()
}

def switch15off() {
	log.debug "Executing 'switch15off' = 'switch15 off'"
    zigbee.smartShield(text: "switch15 off").format()
}

def switch16on() {
	log.debug "Executing 'switch16on' = 'switch16 on'"
    zigbee.smartShield(text: "switch16 on").format()
}

def switch16off() {
	log.debug "Executing 'switch16off' = 'switch16 off'"
    zigbee.smartShield(text: "switch16 off").format()
}
