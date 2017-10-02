/**
 *  Child RGBLED Switch
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
 *    2017-10-01  Allan (vseven) Original Creation (based on Dan Ogorchock's child dimmer switch)
 * 
 */

// for the UI
metadata {
	definition (name: "Child RGB Switch", namespace: "vseven", author: "Alan (vseven) - based on code by Dan Ogorchock") {
	capability "Switch"
	capability "Color Control"

	command "generateEvent", ["string", "string"]
	}

	simulator {
		// TODO: define status and reply messages here
	}

	tiles (scale: 2){
		multiAttributeTile(name:"switch", type: "lighting", width: 6, height: 4, canChangeIcon: true){
			tileAttribute ("device.switch", key: "PRIMARY_CONTROL") {
				attributeState "on", label:'${name}', action:"switch.off", icon:"st.lights.philips.hue-single", backgroundColor:"#00A0DC", nextState:"turningOff"
				attributeState "off", label:'${name}', action:"switch.on", icon:"st.lights.philips.hue-single", backgroundColor:"#ffffff", nextState:"turningOn"
				attributeState "turningOn", label:'${name}', action:"switch.off", icon:"st.lights.philips.hue-single", backgroundColor:"#00A0DC", nextState:"turningOff"
				attributeState "turningOff", label:'${name}', action:"switch.on", icon:"st.lights.philips.hue-single", backgroundColor:"#ffffff", nextState:"turningOn"
			}
			tileAttribute ("device.color", key: "COLOR_CONTROL") {
				attributeState "color", action:"color control.setColor"
			}
		}
 		valueTile("lastUpdated", "device.lastUpdated", inactiveLabel: false, decoration: "flat", width: 6, height: 2) {
    			state "default", label:'Last Updated ${currentValue}', backgroundColor:"#ffffff"
		}

		main(["switch"])
		details(["switch", "color", "lastUpdated"])
	}
}


void on() {
	parent.childOn(device.deviceNetworkId)
	// Send the last hex value to turn back on
	parent.childSetColor(device.deviceNetworkId, "000000")
}

void off() {
	parent.childOff(device.deviceNetworkId)
	// Send a all 0 hex value to turn off the LED
	parent.childSetColor(device.deviceNetworkId, "000000")
}

def setColor(value) {
    log.debug("Color value in hex: $value.hex")
    // If the color is being changed we should also turn on
    parent.childOn(device.deviceNetworkId)
    parent.childSetColor(device.deviceNetworkId, value.hex)
}

def generateEvent(String name, String value) {
  //log.debug("Passed values to routine generateEvent in device named $device: Name - $name  -  Value - $value")
  // The name coming in from ST_Anything will be "dimmerSwitch", but we want to the ST standard "switch" attribute for compatibility with normal SmartApps
  sendEvent(name: "switch", value: value)
  // Update lastUpdated date and time
  def nowDay = new Date().format("MMM dd", location.timeZone)
  def nowTime = new Date().format("h:mm a", location.timeZone)
  sendEvent(name: "lastUpdated", value: nowDay + " at " + nowTime, displayed: false)
}
