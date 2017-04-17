/**
 *  Child Alarm
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
 *
 * 
 */
metadata {
 definition (name: "Child Alarm", namespace: "ogiewon", author: "Dan Ogorchock") {
	capability "Actuator"
	capability "Alarm"
	capability "Switch"

	command "test"
 }

	tiles(scale: 2) {
        multiAttributeTile(name:"alarm", type: "generic", width: 6, height: 4){
            tileAttribute ("device.alarm", key: "PRIMARY_CONTROL") {
                attributeState "off", label:'off', action:'alarm.siren', icon:"st.alarm.alarm.alarm", backgroundColor:"#ffffff"
                attributeState "both", label:'alarm!', action:'alarm.off', icon:"st.alarm.alarm.alarm", backgroundColor:"#e86d13"
                attributeState "strobe", label:'strobe!', action:'alarm.off', icon:"st.secondary.strobe", backgroundColor:"#cccccc"
                attributeState "siren", label:'siren!', action:'alarm.off', icon:"st.alarm.beep.beep", backgroundColor:"#e86d13"
    		}
		}
        standardTile("test", "device.alarm", inactiveLabel: false, decoration: "flat", width: 2, height: 2) {
            state "default", label:'', action:"test", icon:"st.secondary.test"
        }
        standardTile("off", "device.alarm", inactiveLabel: false, decoration: "flat", width: 2, height: 2) {
            state "default", label:'', action:"alarm.off", icon:"st.secondary.off"
        }

        main "alarm"
        details(["alarm", "test", "off"])
	}
}

void on() {
	parent.childAlarmOn(device.deviceNetworkId)
}

void off() {
	parent.childAlarmOff(device.deviceNetworkId)
}

def strobe() {
	on()
}

def siren() {
	on()
}

def both() {
	on()
}

def test() {
	parent.childAlarmTest(device.deviceNetworkId)
}