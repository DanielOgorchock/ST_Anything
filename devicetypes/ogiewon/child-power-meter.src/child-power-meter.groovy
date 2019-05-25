/**
 *  Child Power Meter
 *
 *  Copyright 2018 Daniel Ogorchock
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
 *    2018-03-03  Dan Ogorchock  Original Creation
 *    2018-06-02  Dan Ogorchock  Revised/Simplified for Hubitat Composite Driver Model
 *    2019-05-06  Richard Gross  Added Energy (KWH calcualtions) Basedon  Aeon Home Energy Meter gen-1 (US)	
 * 
 */
 
import groovy.time.TimeCategory 
 
metadata {
	definition (name: "Child Power Meter", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Power Meter"
		capability "Sensor"
		capability "Energy Meter"

		attribute "lastUpdated", "String"
       	attribute "currentKWH", "string" 		// Used to show current kWh since last reset
    	attribute "currentWATTS", "string"  	// Used to show current watts being used on the main tile
    	attribute "minWATTS", "string"   		// Used to store/display minimum watts used since last reset
    	attribute "maxWATTS", "string"   		// Used to store/display maximum watts used since last reset
    	attribute "resetMessage", "string"  	// Used for messages of what was reset (min, max, energy, or all values)
    	attribute "kwhCosts", "string"  		// Used to show energy costs since last reset
        
        command "resetkwh"
        command "resetmin"
    	command "resetmax"
	}

	simulator {

	}
    
    preferences {
/*RAG        	input "displayEvents", "boolean",
            	title: "Display all events in the Recently tab and the device's event log?", 
            	defaultValue: false,
                required: false,
  	         	displayDuringSetup: true 
*/            	input "kWhCost", "string",
            	title: "Enter your cost per kWh (or just use the default, or use 0 to not calculate):",
            	defaultValue: 0.16,
                required: false,                
            	displayDuringSetup: true
    }
    
	tiles(scale: 2) {
		multiAttributeTile(name: "power", type: "generic", width: 6, height: 4, canChangeIcon: true) {
			tileAttribute("device.power", key: "PRIMARY_CONTROL") {
				attributeState("default", label: '${currentValue}W', unit:"W", defaultState: true)
			}
 			tileAttribute("device.lastUpdated", key: "SECONDARY_CONTROL") {
    				attributeState("default", label:'    Last updated ${currentValue}',icon: "st.Health & Wellness.health9")
            }
		}        

        standardTile("iconTile", "iconTile", inactiveLabel: false, decoration: "flat", width: 1, height: 1) {
			state "default", icon:"https://raw.githubusercontent.com/constjs/jcdevhandlers/master/img/device-activity-tile@2x.png"
		}
        valueTile("statusText", "statusText", inactiveLabel: false, decoration: "flat", width: 5, height: 1) {
			state "default", label:'${currentValue}', backgroundColor:"#ffffff"
	    }
        standardTile("iconTile", "iconTile", inactiveLabel: false, decoration: "flat", width: 1, height: 1) {
			state "default", icon:"https://raw.githubusercontent.com/constjs/jcdevhandlers/master/img/device-activity-tile@2x.png"
		}
        valueTile("resetMessage", "device.resetMessage", width: 5, height: 1, inactiveLabel: false, decoration: "flat") {
            state("default", label: '${currentValue}', backgroundColor:"#ffffff")
        }
        valueTile("energy", "device.energy", width: 3, height: 1, inactiveLabel: false, decoration: "flat") {
            state("default", label: '${currentValue}kWh', backgroundColor:"#ffffff")     
        }
        valueTile("kwhCosts", "device.kwhCosts", width: 3, height: 1, inactiveLabel: false, decoration: "flat") {
            state("default", label: '${currentValue}', backgroundColor:"#ffffff")
        }
        standardTile("resetmin", "device.resetmin", width: 2, height: 2, inactiveLabel: false, decoration: "flat") {
            state "default", label:'Reset Min', action:"resetmin", icon:"st.secondary.refresh-icon"
        }
        standardTile("resetmax", "device.resetmax", width: 2, height: 2, inactiveLabel: false, decoration: "flat") {
            state "default", label:'Reset Max', action:"resetmax", icon:"st.secondary.refresh-icon"
        }        
        standardTile("resetkwh", "device.resetkwh", width: 2, height: 2, inactiveLabel: false, decoration: "flat") {
			state "default", label:'Reset Energy', action:"resetkwh", icon:"st.secondary.refresh-icon"
		}
	}

}

def updated() {
    state.displayDisabled = ("true" == displayEvents)
    log.debug "updated (kWhCost: ${kWhCost}, wattsLimit: ${wattsLimit}, reportType: ${reportType}, wattsChanged: ${wattsChanged}, wattsPercent: ${wattsPercent}, secondsWatts: ${secondsWatts}, secondsKwh: ${secondsKwh}, decimalPositions: ${decimalPositions})"
}
def parse(String description) {
    log.debug "parse(${description}) called"
   	def parts = description.split(" ")
    def name  = parts.length>0?parts[0].trim():null
    def value = parts.length>1?parts[1].trim():null
    def KW = value as Double
    def timeString = new Date().format("MM-dd-yy h:mm a", location.timeZone)
    if (name && value) {
        // Update device
        sendEvent(name: name, value: value)
        // Update lastUpdated date and time  
        def nowDay = new Date().format("MMM dd", location.timeZone)
        def nowTime = new Date().format("h:mm a", location.timeZone)
        sendEvent(name: "lastUpdated", value: nowDay + " at " + nowTime, displayed: false)
        
        if (state.KWH  != null) {
            state.KWH = ((( now() - state.lastTime )  / 3600000000) * KW)  + state.KWH
            state.kw = KW
            log.debug "KWH $state.KWH $value  kWhCost kwhCost $state.kw"
        }
        else if (state.KHW == null) {
            state.KWH =0
        }
        state.lastTime  = now()
        sendEvent(name: "energy", value: state.KWH , unit: "kWh", displayed: false)
        
        def costDecimal = (state.KWH  * ( kWhCost as BigDecimal))
        sendEvent(name: "kwhCosts", value: "Cost " + (String.format("\$%5.2f",costDecimal)), unit: "", displayed: false)
        
        log.debug ("$state.kw $state.powerHigh")
        if (state.kw > state.powerHigh) {
            state.powerHigh = state.kw
	        sendEvent(name: "maxWATTS", value: String.format("%3.0f",state.powerHigh) +"w"+ " on " + timeString , unit: "", displayed: false)
	    }
        
	    if (state.kw < state.powerLow) {
            state.powerLow = state.kw
            sendEvent(name: "minWATTS", value: String.format("%3.0f",state.powerLow) +"w"+ " on " + timeString , unit: "", displayed: false)
	    }
       
    }
    else {
    	log.debug "Missing either name or value.  Cannot parse!"
    }
 
	def statusTextmsg = "Min was ${device.currentState('minWATTS')?.value}.\nMax was ${device.currentState('maxWATTS')?.value}.\n$state.kwhReset"
    sendEvent("name": "statusText", "value": statusTextmsg)
}

def resetkwh() {
    log.debug "${device.name} reset kWh/Cost values"
	   state.KWH = 0
       state.lastTime  = now()
    def timeString = new Date().format("MM-dd-yy h:mm a", location.timeZone)
    state.kwhReset = "kWh/Cost Reset on "+timeString
    log.debug "$state.kwhReset"
    sendEvent(name: "resetMessage", value: "Energy Data (kWh/Cost) Reset On:\n"+timeString, unit: "")       
    sendEvent(name: "currentkWh", value: "", unit: "")
    sendEvent(name: "kwhCosts", value: "Cost\n--", unit: "")
}

def resetmin() {
    log.debug "${device.name} reset minimum watts value"
    state.powerLow = 99999
	def timeString = new Date().format("MM-dd-yy h:mm a", location.timeZone)
    sendEvent(name: "resetMessage", value: "Watts Data Minimum Value Reset On:\n"+timeString, unit: "")
    sendEvent(name: "minWATTS", value: "", unit: "")    
}

def resetmax() {
    log.debug "${device.name} reset maximum watts value"
    state.powerHigh = 0
	def timeString = new Date().format("MM-dd-yy h:mm a", location.timeZone)
    sendEvent(name: "resetMessage", value: "Watts Data Maximum Value Reset On:\n"+timeString, unit: "")    
    sendEvent(name: "maxWATTS", value: "", unit: "")    
}

def installed() {
}