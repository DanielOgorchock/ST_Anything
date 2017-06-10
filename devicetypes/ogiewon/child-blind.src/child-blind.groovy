/**
 *  Child Switch
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
	definition (name: "Child Blind", namespace: "ogiewon", author: "Dan Ogorchock") {
		capability "Window Shade"
        capability "Switch"
		capability "Relay Switch"
		capability "Actuator"
		capability "Sensor"
        command "my"
	}

	tiles(scale: 2) {
    
        // standard tile with actions
     		multiAttributeTile(name:"blind", type: "generic", width: 3, height: 4, canChangeIcon: true){
			tileAttribute ("blind", key: "PRIMARY_CONTROL") {
				attributeState "closed", label: '${name}', action: "windowShade.open", icon: "st.switches.switch.off", backgroundColor: "#ffffff", nextState:"opening"
				attributeState "open", label: '${name}', action: "windowShade.close", icon: "st.switches.switch.on", backgroundColor: "#00A0DC", nextState:"closing"
				attributeState "opening", label:'${name}', action:"windowShade.close", icon:"st.switches.switch.on", backgroundColor:"#00A0DC", nextState:"closing"
				attributeState "closing", label:'${name}', action:"windowShade.open", icon:"st.switches.switch.off", backgroundColor:"#ffffff", nextState:"opening"
			}
		}
        
       	standardTile("open", "generic", inactiveLabel: false, decoration: "flat", width: 2, height: 2) {
			state "open", label:'open', action:"windowShade.open"
		}
       	standardTile("close", "generic", inactiveLabel: false, decoration: "flat", width: 2, height: 2) {
			state "closed", label:'close', action:"windowShade.close"
		}
       	standardTile("my", "generic", inactiveLabel: false, decoration: "flat", width: 2, height: 2) {
			state "default", label:'my', action:"my"
		}
        main "blind"
		details(["blind", "open", "close", "my"])
	}
}

	preferences {
		input "SecondsToOpen", "int",
			title: "Time it takes in seconds to open the blinds.",
			description: "Enter the number of seconds it takes to open the shades.",
			defaultValue: "10" as Integer,
			required: false,
			displayDuringSetup: true
            
		input "SecondsToClose", "integer",
			title: "Time it takes in seconds to close the blinds.",
			description: "Enter the number of seconds it takes to close the shades.",
			defaultValue: "10" as Integer,
			required: false,
			displayDuringSetup: true
            }

void open() {
	parent.childOpen(device.deviceNetworkId)
    //sendEvent(name: "windowShade", value: "open")
    //sendEvent(name: "switch", value: "on")
}

void close() {
	parent.childClose(device.deviceNetworkId)
    //sendEvent(name: "windowShade", value: "closed")
    //sendEvent(name: "switch", value: "off")
}

void my() {
	parent.childCustom(device.deviceNetworkId, "stop")
}

void on(){
	open()
}

void off(){
	close()
}