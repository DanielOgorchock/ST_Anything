/**
 *  ST_Anything_Temperatures.device.groovy
 *
 *  Copyright 2014 Dan Ogorchock 
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
 *    2015-03-24  Dan Ogorchock  Original Creation
 *
 */
 
metadata {
	definition (name: "ST_AnyThing_Temperatures_ThingShield", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Configuration"
		capability "Temperature Measurement"
		capability "Relative Humidity Measurement"
		capability "Sensor"
		capability "Polling"
        
		attribute "t_Oven", "string"
		attribute "t_Broiler", "string"
		attribute "t_Fridge", "string"
		attribute "t_Freezer", "string"
		attribute "t_Othercrisp", "string"
		attribute "t_Moistcrisp", "string"
		attribute "h_Fridge", "string"
		attribute "h_Freezer", "string"
		attribute "h_Othercrisp", "string"
		attribute "h_Moistcrisp", "string"
	}

    simulator {
 
    }


    // Preferences
	preferences {
    	input "temphumidSampleRate", "number", title: "Temperature/Humidity Sensors Sampling Interval (seconds)", description: "Sampling Interval (seconds)", defaultValue: 30, required: true, displayDuringSetup: true
    	input "thermocoupleSampleRate", "number", title: "Thermocouple Sensors Sampling Interval (seconds)", description: "Sampling Interval (seconds)", defaultValue: 30, required: true, displayDuringSetup: true
	}


	// Tile Definitions
	tiles {

    	valueTile("t_Oven", "device.t_Oven", width: 1, height: 1, inactiveLabel: false) {
        	state("temperature", label: 'Oven ${currentValue}°F', unit:"F", 
            	backgroundColors: [
                	[value: 31, color: "#153591"],
                	[value: 44, color: "#1e9cbb"],
                	[value: 59, color: "#90d2a7"],
                	[value: 74, color: "#44b621"],
                	[value: 84, color: "#f1d801"],
                	[value: 95, color: "#d04e00"],
                	[value: 96, color: "#bc2323"]
            	]
        	)
    	}
        
    	valueTile("t_Broiler", "device.t_Broiler", width: 1, height: 1, inactiveLabel: false) {
        	state("temperature", label: 'Broiler ${currentValue}°F', unit:"F", 
            	backgroundColors: [
                	[value: 31, color: "#153591"],
                	[value: 44, color: "#1e9cbb"],
                	[value: 59, color: "#90d2a7"],
                	[value: 74, color: "#44b621"],
                	[value: 84, color: "#f1d801"],
                	[value: 95, color: "#d04e00"],
                	[value: 96, color: "#bc2323"]
            	]
        	)
    	}
        
    	valueTile("t_Fridge", "device.t_Fridge", width: 1, height: 1, inactiveLabel: false) {
        	state("temperature", label: 'Fridge\n${currentValue}°F', unit:"F", 
            	backgroundColors: [
            	    [value: 31, color: "#153591"],
                	[value: 44, color: "#1e9cbb"],
                	[value: 59, color: "#90d2a7"],
                	[value: 74, color: "#44b621"],
                	[value: 84, color: "#f1d801"],
                	[value: 95, color: "#d04e00"],
                	[value: 96, color: "#bc2323"]
            	]
        	)
    	}
        
    	valueTile("t_Freezer", "device.t_Freezer", width: 1, height: 1, inactiveLabel: false) {
        	state("temperature", label: 'Freezer\n${currentValue}°F', unit:"F", 
            	backgroundColors: [
                	[value: 31, color: "#153591"],
                	[value: 44, color: "#1e9cbb"],
                	[value: 59, color: "#90d2a7"],
                	[value: 74, color: "#44b621"],
                	[value: 84, color: "#f1d801"],
                	[value: 95, color: "#d04e00"],
                	[value: 96, color: "#bc2323"]
            	]
        	)
    	}
        
    	valueTile("t_Othercrisp", "device.t_Othercrisp", width: 1, height: 1, inactiveLabel: false) {
        	state("temperature", label: 'Other Crisper\n${currentValue}°F', unit:"F", 
            	backgroundColors: [
                	[value: 31, color: "#153591"],
                	[value: 44, color: "#1e9cbb"],
                	[value: 59, color: "#90d2a7"],
                	[value: 74, color: "#44b621"],
                	[value: 84, color: "#f1d801"],
                	[value: 95, color: "#d04e00"],
                	[value: 96, color: "#bc2323"]
            	]
        	)
    	}
        
    	valueTile("t_Moistcrisp", "device.t_Moistcrisp", width: 1, height: 1, inactiveLabel: false) {
        	state("temperature", label: 'Moist Crisper\n${currentValue}°F', unit:"F", 
            	backgroundColors: [
                	[value: 31, color: "#153591"],
                	[value: 44, color: "#1e9cbb"],
                	[value: 59, color: "#90d2a7"],
                	[value: 74, color: "#44b621"],
                	[value: 84, color: "#f1d801"],
                	[value: 95, color: "#d04e00"],
                	[value: 96, color: "#bc2323"]
            	]
        	)
    	}

    	valueTile("h_Fridge", "device.h_Fridge", width: 1, height: 1, inactiveLabel: false) {
        	state "humidity", label:'Fridge\n${currentValue}% humidity', unit:""
    	}

    	valueTile("h_Freezer", "device.h_Freezer", width: 1, height: 1, inactiveLabel: false) {
        	state "humidity", label:'Freezer\n${currentValue}% humidity', unit:""
    	}

    	valueTile("h_Moistcrisp", "device.h_Moistcrisp", width: 1, height: 1, inactiveLabel: false) {
        	state "humidity", label:'Moist\nCrisper ${currentValue}% humidity', unit:""
    	}

    	valueTile("h_Othercrisp", "device.h_Othercrisp", width: 1, height: 1, inactiveLabel: false) {
        	state "humidity", label:'Other\nCrisper ${currentValue}% humidity', unit:""
    	}

    	standardTile("configure", "device.configure", inactiveLabel: false, decoration: "flat") {
        	state "configure", label:'', action:"configuration.configure", icon:"st.secondary.configure"
    	}

        main (["t_Oven", "t_Broiler", "t_Fridge", "t_Freezer", "t_Moistcrisp", "t_Othercrisp", "h_Fridge", "h_Freezer", "h_Moistcrisp", "h_Othercrisp"])
        details(["t_Oven", "t_Broiler", "t_Fridge", "t_Freezer", "t_Moistcrisp", "t_Othercrisp", "h_Fridge", "h_Freezer", "h_Moistcrisp", "h_Othercrisp", "configure"])
	}
}

// parse events into attributes
def parse(String description) {
	log.debug "Parsing '${description}'"
    def msg = zigbee.parse(description)?.text
    log.debug "Parse got '${msg}'"

    def parts = msg.split(" ")
    def name  = parts.length>0?parts[0].trim():null
    def value = parts.length>1?parts[1].trim():null

    name = value != "ping" ? name : null

    def result = createEvent(name: name, value: value)

    log.debug result

    return result
}

// handle commands

def poll() {
	//temporarily implement poll() to issue a configure() command to send the polling interval settings to the arduino
    //use the Pollster SmartApp to execute this polling routine about once per hour in case Arduino gets power cycled
	configure()
}


def configure() {
	log.debug "Executing 'configure'"
	log.debug "temphumid SampleRate = " + temphumidSampleRate
    log.debug "thermocouple SampleRate = " + thermocoupleSampleRate
	[
        zigbee.smartShield(text: "th_Freezer " + temphumidSampleRate).format(),
        "delay 1000",
        zigbee.smartShield(text: "th_Fridge " + temphumidSampleRate).format(),
        "delay 1000",
        zigbee.smartShield(text: "th_Moistcrisp " + temphumidSampleRate).format(),
        "delay 1000",
        zigbee.smartShield(text: "th_Othercrisp " + temphumidSampleRate).format(),
        "delay 1000",
        zigbee.smartShield(text: "t_Broiler " + thermocoupleSampleRate).format(),
        "delay 1000",
        zigbee.smartShield(text: "t_Oven " + thermocoupleSampleRate).format()
	]
}