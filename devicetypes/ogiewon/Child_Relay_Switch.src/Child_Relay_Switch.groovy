/**
 *  Child Relay Switch
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
 *    2017-04-16  Dan Ogorchock  Original Creation
 *
 * 
 */
metadata {
	definition (name: "Child Relay Switch", namespace: "ogiewon", author: "Dan Ogorchock") {
		capability "Switch"
		capability "Relay Switch"
		capability "Actuator"
		capability "Sensor"
	}

	tiles(scale: 2) {
		multiAttributeTile(name:"relaySwitch", type: "lighting", width: 3, height: 4, canChangeIcon: true){
			tileAttribute ("device.relaySwitch", key: "PRIMARY_CONTROL") {
				attributeState "off", label: '${name}', action: "switch.on", icon: "st.switches.switch.off", backgroundColor: "#ffffff", nextState:"turningOn"
				attributeState "on", label: '${name}', action: "switch.off", icon: "st.switches.switch.on", backgroundColor: "#00A0DC", nextState:"turningOff"
				attributeState "turningOn", label:'${name}', action:"switch.off", icon:"st.switches.switch.on", backgroundColor:"#00A0DC", nextState:"turningOff"
				attributeState "turningOff", label:'${name}', action:"switch.on", icon:"st.switches.switch.off", backgroundColor:"#ffffff", nextState:"turningOn"
			}
		}
	}
}

void on() {
	parent.childRelayOn(device.deviceNetworkId)
}

void off() {
	parent.childRelayOff(device.deviceNetworkId)
}