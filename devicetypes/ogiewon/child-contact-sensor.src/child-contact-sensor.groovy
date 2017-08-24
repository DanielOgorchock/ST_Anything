/**
 *  Child Contact Sensor
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
 *    	Date		Who		What
 *    	----		---		----
 *    	2017-04-10	Dan Ogorchock  	Original Creation
 *	2017-08-23  	Allan (vseven) 	Added a generateEvent routine that gets info from the parent device.  This routine runs each time the value is updated which can lead to other modifications of the device.
 *	2017-08-24  	Allan (vseven) 	Added a lastUpdated attribute that will display on the multitile.
 *
 * 
 */
metadata {
	definition (name: "Child Contact Sensor", namespace: "ogiewon", author: "Dan Ogorchock") {
		capability "Contact Sensor"
		capability "Sensor"        
        	attribute "lastUpdated", "String"
	}

	simulator {

	}

	tiles(scale: 2) {
		multiAttributeTile(name:"contact", type: "generic"){
			tileAttribute ("device.contact", key: "PRIMARY_CONTROL") {
				attributeState "open", label:'${name}', icon:"st.contact.contact.open", backgroundColor:"#e86d13"
				attributeState "closed", label:'${name}', icon:"st.contact.contact.closed", backgroundColor:"#00a0dc"
            }
			tileAttribute("device.lastUpdated", key: "SECONDARY_CONTROL") {
    			attributeState("default", label:'    Last updated ${currentValue}',icon: "st.Health & Wellness.health9")
            }
        }
	main "contact"
        details(["contact", "lastUpdated"])
	}
}

def generateEvent(String name, String value) {
	// log.debug("Passed values to routine generateEvent in device named $device: Name - $name  -  Value - $value")
	// Update device
	sendEvent(name: name,value: value)
    	// Update lastUpdated date and time
    	def nowDay = new Date().format("MMM dd", location.timeZone)
    	def nowTime = new Date().format("h:mm a", location.timeZone)
    	sendEvent(name: "lastUpdated", value: nowDay + " at " + nowTime)
}
