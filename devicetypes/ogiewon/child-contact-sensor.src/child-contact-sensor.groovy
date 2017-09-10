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
 *		2017-08-23  Allan (vseven) 	Added a generateEvent routine that gets info from the parent device.  This routine runs each time the value is updated which can lead to other modifications of the device.
 *		2017-08-24  Allan (vseven) 	Added a lastUpdated attribute that will display on the multitile.
 *  	2017-08-24	Allan (vseven)	Added the ability to change the displayed labels for Open and Closed.
 * 		2017-08-29	Allan (vseven)	Added the ability to invert the displayed colors along with some additional labels.
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
    
  preferences {
    section("prefs") {
      input(name: "displayLabel", type: "ENUM", title: "Select the text to display for the contact.  First value is when Closed, second when Open.", multiple: false, 
      		required: true, options: ["Closed/Open", "On/Off", "True/False", "Yes/No", "Active/Inactive", "Running/Off", "Heating/Off", "Cooling/Off", "Armed/Disarmed"])
      input(name: "invertColors", type: "boolean", title: "Invert the background colors.  Useful for when you want to show Open with a blue background and Closed with orange")
	}
  }
    
	tiles(scale: 2) {
		multiAttributeTile(name:"contact", type: "generic"){
			tileAttribute ("device.contactDisplay", key: "PRIMARY_CONTROL") {
            	// Regular
                attributeState("open", label:"Open", icon:"st.contact.contact.open", backgroundColor: "#e86d13")
                attributeState("closed", label:"Closed", icon:"st.contact.contact.closed", backgroundColor: "#00a0dc")
                attributeState("off", label:"Off", icon:"st.contact.contact.open", backgroundColor: "#e86d13")
                attributeState("on", label:"On", icon:"st.contact.contact.closed", backgroundColor: "#00a0dc")
                attributeState("false", label:"False", icon:"st.contact.contact.open", backgroundColor: "#e86d13")
                attributeState("true", label:"True", icon:"st.contact.contact.closed", backgroundColor: "#00a0dc")
                attributeState("no", label:"No", icon:"st.contact.contact.open", backgroundColor: "#e86d13")
                attributeState("yes", label:"Yes", icon:"st.contact.contact.closed", backgroundColor: "#00a0dc")
                attributeState("inactive", label:"Inactive", icon:"st.contact.contact.open", backgroundColor: "#e86d13")
                attributeState("active", label:"Active", icon:"st.contact.contact.closed", backgroundColor: "#00a0dc")
                attributeState("running", label:"Running", icon:"st.contact.contact.closed", backgroundColor: "#00a0dc")
                attributeState("heating", label:"Heating", icon:"st.contact.contact.closed", backgroundColor: "#00a0dc")
                attributeState("cooling", label:"Cooling", icon:"st.contact.contact.closed", backgroundColor: "#00a0dc")
                attributeState("disarmed", label:"Disarmed", icon:"st.contact.contact.open", backgroundColor: "#e86d13")
                attributeState("armed", label:"Armed", icon:"st.contact.contact.closed", backgroundColor: "#00a0dc")
                // Inverted
                attributeState("openInverted", label:"Open", icon:"st.contact.contact.open", backgroundColor: "#00a0dc")
                attributeState("closedInverted", label:"Closed", icon:"st.contact.contact.closed", backgroundColor: "#e86d13")
                attributeState("offInverted", label:"Off", icon:"st.contact.contact.open", backgroundColor: "#00a0dc")
                attributeState("onInverted", label:"On", icon:"st.contact.contact.closed", backgroundColor: "#e86d13")
                attributeState("falseInverted", label:"False", icon:"st.contact.contact.open", backgroundColor: "#00a0dc")
                attributeState("trueInverted", label:"True", icon:"st.contact.contact.closed", backgroundColor: "#e86d13")
                attributeState("noInverted", label:"No", icon:"st.contact.contact.open", backgroundColor: "#00a0dc")
                attributeState("yesInverted", label:"Yes", icon:"st.contact.contact.closed", backgroundColor: "#e86d13")
                attributeState("inactiveInverted", label:"Inactive", icon:"st.contact.contact.open", backgroundColor: "#00a0dc")
                attributeState("activeInverted", label:"Active", icon:"st.contact.contact.closed", backgroundColor: "#e86d13")
                attributeState("runningInverted", label:"Running", icon:"st.contact.contact.closed", backgroundColor: "#e86d13")
                attributeState("heatingInverted", label:"Heating", icon:"st.contact.contact.closed", backgroundColor: "#e86d13")
                attributeState("coolingInverted", label:"Cooling", icon:"st.contact.contact.closed", backgroundColor: "#e86d13")
                attributeState("disarmedInverted", label:"Disarmed", icon:"st.contact.contact.open", backgroundColor: "#00a0dc")
                attributeState("armedInverted", label:"Armed", icon:"st.contact.contact.closed", backgroundColor: "#e86d13")
            }
            tileAttribute("device.lastUpdated", key: "SECONDARY_CONTROL") {
    			attributeState("default", label:'      Last updated ${currentValue}',icon: "st.Health & Wellness.health9")
            }
        }
		main "contact"
        details(["contact", "lastUpdated"])
	}
}

def updated() {
	//log.debug("Updated called.  Make sure to update labels.")
    updateLabels(device.currentValue("contact"))
}

def generateEvent(String name, String value) {
	//log.debug("Passed values to routine generateEvent in device named $device: Name - $name  -  Value - $value")
	// Update device
	sendEvent(name: name,value: value)
    // Update lastUpdated date and time
    def nowDay = new Date().format("MMM dd", location.timeZone)
    def nowTime = new Date().format("h:mm a", location.timeZone)
    sendEvent(name: "lastUpdated", value: nowDay + " at " + nowTime)
    // Update the display
    updateLabels(value)
}

def updateLabels (String value) {
	// log.debug("updateLabels called.  Passed value is $value.  openDisplayLabel is $openDisplayLabel.  closedDisplayLabel is $closedDisplayLabel.")
	// Update tile with custom labels
    def myPassedValue
    
    if (displayLabel.equals("On/Off")) {
     	if (value.equals("closed")) {
    		myPassedValue = "on"    
		} else {
			myPassedValue = "off"
		}  
    } else if (displayLabel.equals("True/False")) {
     	if (value.equals("closed")) {
    		myPassedValue = "true"
		} else {
			myPassedValue = "false"
		}  
    } else if (displayLabel.equals("Yes/No")) {
     	if (value.equals("closed")) {
    		myPassedValue = "yes"
		} else {
			myPassedValue = "no"
		}  
    } else if (displayLabel.equals("Active/Inactive")) {
     	if (value.equals("closed")) {
    		myPassedValue = "active"
		} else {
			myPassedValue = "inactive"
		}  
    } else if (displayLabel.equals("Running/Off")) {
     	if (value.equals("closed")) {
    		myPassedValue = "running"
		} else {
			myPassedValue = "off"
		}  
    } else if (displayLabel.equals("Heating/Off")) {
     	if (value.equals("closed")) {
    		myPassedValue = "heating"
		} else {
			myPassedValue = "off"
		}  
    } else if (displayLabel.equals("Cooling/Off")) {
     	if (value.equals("closed")) {
    		myPassedValue = "cooling"
		} else {
			myPassedValue = "off"
		}  
    } else if (displayLabel.equals("Armed/Disarmed")) {
     	if (value.equals("closed")) {
    		myPassedValue = "armed"
		} else {
			myPassedValue = "disarmed"
		}  
    } else {
     	if (value.equals("closed")) {
    		myPassedValue = "closed"
		} else {
			myPassedValue = "open"
		}  
    }

    if (invertColors.equals("true")) {
    	myPassedValue = myPassedValue + "Inverted"
    }
    //log.debug("sendEvent value is " + myPassedValue)
    sendEvent(name: "contactDisplay", value: myPassedValue, isStateChange: true)
}
