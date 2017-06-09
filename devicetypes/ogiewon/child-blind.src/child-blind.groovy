/**
 *  Child Blind
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
 *    2017-06-09  joshs85		  Original Creation
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
	}
}

void open() {
	parent.childOpen(device.deviceNetworkId)
}

void close() {
	parent.childClose(device.deviceNetworkId)
}

void on(){
	open()
}

void off(){
	close()
}