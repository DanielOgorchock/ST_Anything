/**
 *  Virtual Contact Sensor Device Type - VirtualContactSensor.device.groovy
 *
 *  Copyright 2014 Daniel Ogorchock
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
 *    2015-01-10  Dan Ogorchock  Original Creation
 *
 *

 */
metadata {
	definition (name: "Virtual Contact Sensor", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Contact Sensor"
        
		command "openme"
		command "closeme"
	}

	simulator {
		// TODO: define status and reply messages here
	}

	tiles {
		standardTile("contact", "device.contact", width: 2, height: 2) {
			state("open", label:'${name}', icon:"st.contact.contact.open", backgroundColor:"#ffa81e")
			state("closed", label:'${name}', icon:"st.contact.contact.closed", backgroundColor:"#79b821")
		}
        
	main(["contact"])
	details(["contact"])
	}
}

// parse events into attributes
def parse(String description) {
	log.debug "Parsing '${description}'"
	// TODO: handle 'contact' attribute

}

def openme() {
    log.debug "Virtual Contact Open"
    sendEvent(name: "contact", value: "open", isStateChange: true)
}

def closeme() {
    log.debug "Virtual Contact Closed"
    sendEvent(name: "contact", value: "closed", isStateChange: true)
}
