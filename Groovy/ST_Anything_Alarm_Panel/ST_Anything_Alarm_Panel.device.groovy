/**
 *  ST_Anything_Alarm_Panel Device Type - ST_Anything_Alarm_Panel.device.groovy
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
 *    2015-12-06  Dan Ogorchock  Original Creation
 *
 *
 */

metadata {
	definition (name: "ST_Anything_Alarm_Panel", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Actuator"
		capability "Alarm"
		capability "Contact Sensor"
		capability "Motion Sensor"
		capability "Sensor"
		capability "Smoke Detector"


		attribute "motion1", "string"
		attribute "motion2", "string"
		attribute "contact1", "string"
		attribute "contact2", "string"
		attribute "contact3", "string"
		attribute "contact4", "string"
 		attribute "contact5", "string"
		attribute "contact6", "string"       
		attribute "contact7", "string"       
		attribute "contact8", "string"       
		attribute "contact9", "string"       
		attribute "contact10", "string"       
		attribute "smoke1", "string"       
		attribute "smoke2", "string"       
		attribute "smoke3", "string"
        
        command "test"
	}

    simulator {

    }
	
    // Preferences

	// tile definitions
	tiles {
         standardTile("motion1", "device.motion1", width: 1, height: 1) {
			state("active", label:'motion', icon:"st.motion.motion.active", backgroundColor:"#53a7c0")
			state("inactive", label:'no motion', icon:"st.motion.motion.inactive", backgroundColor:"#ffffff")
		}
         standardTile("motion2", "device.motion2", width: 1, height: 1) {
			state("active", label:'motion', icon:"st.motion.motion.active", backgroundColor:"#53a7c0")
			state("inactive", label:'no motion', icon:"st.motion.motion.inactive", backgroundColor:"#ffffff")
		}

 
       standardTile("contact1", "device.contact1", width: 1, height: 1, canChangeIcon: true, canChangeBackground: true) {
			state("open", label:'CS1 ${name}', icon:"st.contact.contact.open", backgroundColor:"#ffa81e")
			state("closed", label:'CS1 ${name}', icon:"st.contact.contact.closed", backgroundColor:"#79b821")
 		}
        standardTile("contact2", "device.contact2", width: 1, height: 1, canChangeIcon: true, canChangeBackground: true) {
			state("open", label:'CS2 ${name}', icon:"st.contact.contact.open", backgroundColor:"#ffa81e")
			state("closed", label:'CS2 ${name}', icon:"st.contact.contact.closed", backgroundColor:"#79b821")
 		}
        standardTile("contact3", "device.contact3", width: 1, height: 1, canChangeIcon: true, canChangeBackground: true) {
			state("open", label:'CS3 ${name}', icon:"st.contact.contact.open", backgroundColor:"#ffa81e")
			state("closed", label:'CS3 ${name}', icon:"st.contact.contact.closed", backgroundColor:"#79b821")
 		}
        standardTile("contact4", "device.contact4", width: 1, height: 1, canChangeIcon: true, canChangeBackground: true) {
			state("open", label:'CS4 ${name}', icon:"st.contact.contact.open", backgroundColor:"#ffa81e")
			state("closed", label:'CS4 ${name}', icon:"st.contact.contact.closed", backgroundColor:"#79b821")
 		}
        standardTile("contact5", "device.contact5", width: 1, height: 1, canChangeIcon: true, canChangeBackground: true) {
			state("open", label:'CS5 ${name}', icon:"st.contact.contact.open", backgroundColor:"#ffa81e")
			state("closed", label:'CS5 ${name}', icon:"st.contact.contact.closed", backgroundColor:"#79b821")
 		}
        standardTile("contact6", "device.contact6", width: 1, height: 1, canChangeIcon: true, canChangeBackground: true) {
			state("open", label:'CS6 ${name}', icon:"st.contact.contact.open", backgroundColor:"#ffa81e")
			state("closed", label:'CS6 ${name}', icon:"st.contact.contact.closed", backgroundColor:"#79b821")
 		}
        standardTile("contact7", "device.contact7", width: 1, height: 1, canChangeIcon: true, canChangeBackground: true) {
			state("open", label:'CS7 ${name}', icon:"st.contact.contact.open", backgroundColor:"#ffa81e")
			state("closed", label:'CS7 ${name}', icon:"st.contact.contact.closed", backgroundColor:"#79b821")
 		}
        standardTile("contact8", "device.contact8", width: 1, height: 1, canChangeIcon: true, canChangeBackground: true) {
			state("open", label:'CS8 ${name}', icon:"st.contact.contact.open", backgroundColor:"#ffa81e")
			state("closed", label:'CS8 ${name}', icon:"st.contact.contact.closed", backgroundColor:"#79b821")
 		}
        standardTile("contact9", "device.contact9", width: 1, height: 1, canChangeIcon: true, canChangeBackground: true) {
			state("open", label:'CS9 ${name}', icon:"st.contact.contact.open", backgroundColor:"#ffa81e")
			state("closed", label:'CS9 ${name}', icon:"st.contact.contact.closed", backgroundColor:"#79b821")
 		}
         standardTile("contact10", "device.contact10", width: 1, height: 1, canChangeIcon: true, canChangeBackground: true) {
			state("open", label:'CS10 ${name}', icon:"st.contact.contact.open", backgroundColor:"#ffa81e")
			state("closed", label:'CS10 ${name}', icon:"st.contact.contact.closed", backgroundColor:"#79b821")
 		}

        standardTile("smoke1", "device.smoke1", width: 1, height: 1, canChangeIcon: true, canChangeBackground: true) {
			state("clear", label:"SM1 Clear", icon:"st.alarm.smoke.clear", backgroundColor:"#ffffff", action:"smoke")
			state("detected", label:"SM2 Smoke!", icon:"st.alarm.smoke.smoke", backgroundColor:"#e86d13", action:"clear")
		}
		standardTile("smoke2", "device.smoke2", width: 1, height: 1, canChangeIcon: true, canChangeBackground: true) {
			state("clear", label:"SM2 Clear", icon:"st.alarm.smoke.clear", backgroundColor:"#ffffff", action:"smoke")
			state("detected", label:"SM2 Smoke!", icon:"st.alarm.smoke.smoke", backgroundColor:"#e86d13", action:"clear")
		}
		standardTile("smoke3", "device.smoke3", width: 1, height: 1, canChangeIcon: true, canChangeBackground: true) {
			state("clear", label:"SM3 Clear", icon:"st.alarm.smoke.clear", backgroundColor:"#ffffff", action:"smoke")
			state("detected", label:"SM3 Smoke!", icon:"st.alarm.smoke.smoke", backgroundColor:"#e86d13", action:"clear")
		}
  
 		standardTile("alarm", "device.alarm", width: 1, height: 1) {
			state "off", label:'off', action:'alarm.siren', icon:"st.alarm.alarm.alarm", backgroundColor:"#ffffff"
            state "strobe", label:'', action:'alarm.off', icon:"st.secondary.strobe", backgroundColor:"#cccccc"
            state "siren", label:'siren!', action:'alarm.off', icon:"st.alarm.beep.beep", backgroundColor:"#e86d13"
			state "both", label:'alarm!', action:'alarm.off', icon:"st.alarm.alarm.alarm", backgroundColor:"#e86d13"
		}
		standardTile("test", "device.alarm", inactiveLabel: false, decoration: "flat") {
			state "default", label:'', action:"test", icon:"st.secondary.test"
		}
		standardTile("off", "device.alarm", , width: 1, height: 1) {
			state "default", label:'Alarm', action:"alarm.off", icon:"st.secondary.off"
		}

        main (["alarm"])
        details(["alarm","test","off",,"smoke1","smoke2","smoke3","motion1","motion2","contact1","contact2","contact3","contact4","contact5","contact6","contact7","contact8","contact9","contact10"])
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
	
    //if (name == "temperature") 
    //{
    //	value = fahrenheitToCelsius(value.toDouble())
    //}
    
    def result = createEvent(name: name, value: value, isStateChange: true)

    log.debug result

    return result
}

def off() {
	log.debug "Executing 'alarm off'"
	zigbee.smartShield(text: "alarm off").format()
}

def strobe() {
	log.debug "Executing 'alarm strobe'"
	zigbee.smartShield(text: "alarm strobe").format()
}

def siren() {
	log.debug "Executing 'alarm siren'"
	zigbee.smartShield(text: "alarm siren").format()
}

def both() {
	log.debug "Executing 'alarm both'"
	zigbee.smartShield(text: "alarm both").format()
}

def test() {
	log.debug "Executing 'alarm test'"
	[
		zigbee.smartShield(text: "alarm siren").format(),
		"delay 3000",
		zigbee.smartShield(text: "alarm off").format()
	]
}