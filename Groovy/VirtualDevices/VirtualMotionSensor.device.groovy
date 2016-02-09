/**
 *  Virtual Motion Sensor Device Handler - VirtualMotionSensor.device.groovy
 *
 *  Copyright 2016 Daniel Ogorchock
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
 *    2016-02-09  Dan Ogorchock  Original Creation (inspired by ST Community user "nerfnerfnerf")
 *
 *

 */
metadata {
	definition (name: "Virtual Motion Sensor", namespace: "ogiewon", author: "Daniel Ogorchock") {
        capability "Sensor"
        capability "Motion Sensor"
        
        command "active"
        command "inactive"
	}

	simulator {
		// TODO: define status and reply messages here
	}

	tiles {
		standardTile("motion", "device.motion", width: 2, height: 2) { 
 			state("active", label:'motion', icon:"st.motion.motion.active", backgroundColor:"#53a7c0") 
 			state("inactive", label:'no motion', icon:"st.motion.motion.inactive", backgroundColor:"#ffffff") 
 		} 
        
	main(["motion"])
	details(["motion"])
	}
}

// parse events into attributes
def parse(String description) {
	log.debug "Parsing '${description}'"
	// TODO: handle 'contact' attribute

}

def active() {
    log.debug "Virtual Motion Active"
    sendEvent(name: "motion", value: "active", isStateChange: true)
}

def inactive() {
    log.debug "Virtual Motion Inactive"
    sendEvent(name: "motion", value: "inactive", isStateChange: true)
}