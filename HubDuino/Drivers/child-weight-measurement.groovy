/**
 *  Child Weight Measurement
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-weight-measurement.groovy
 *
 *  Copyright 2021 Malcolm Dobson (based on prior work of Dan Ogorchock)
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
 *    2021-01-19  Malcolm Dobson Original Creation
 *    2021-01-24  Malcolm Dobson Option of daily tare in Prefeneces
 * 
 */
metadata {
	definition (name: "Child Weight Measurement", namespace: "ogiewon", author: "Malcolm Dobson", importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/child-weight-measurment.groovy") {
		capability "Sensor"
		
        command "tare", [[name:"Zero the scales", description: "Reset the displayed weight on the scales to zero"]]
        command "calibrate", [[name:"Known weight*", type: "NUMBER", description: "Use a known weight to calibrate the scales. Ensure empty scales are zeroed prior to calibration", required: true, range: "0..200"]]

        attribute "weight", "Number"
        attribute "units", "String"
        attribute "calibrationLastRun", "String"
        attribute "tareLastRun", "String"
        
	}

	preferences {
		section("Prefs") {
			//input "calibrate", type: "number", title: "Calibration process", description: "Use a known weight to calibrate this device. Ensure empty scales are zeroed prior to calibration", range: "0..200", displayDuringSetup: false
			input name: "units", type: "enum", title: "Select units", description: "", defaultValue: "kg", required: true, multiple: false, options:["kg", "lbs"], displayDuringSetup: false
            input name: "logEnable", type: "bool", title: "Enable debug logging", defaultValue: true
			input name: "scheduleDailyTare", type: "bool", title: "Schedule daily tare", defaultValue: false, displayDuringSetup: false
			input name: "dailyTareTime", type: "time", title: "tare time (HH:mm)", defaultValue: "18:00", displayDuringSetup: false
        }
	}
}

def logsOff(){
    log.warn "debug logging disabled..."
    device.updateSetting("logEnable",[value:"false",type:"bool"])
}

def tare() {
    if (logEnable) log.debug "tare() called"
    sendData("tare")
    sendEvent(name: "tareLastRun", value: new Date().format('yyyy-MM-dd HH:mm:ss'))  
}

def calibrate(Number value) {
    if (logEnable) log.debug "calibrate() called with ${value}"
    if (value > 0.0)
    {
      if (units == "lbs") {
        if (logEnable) log.debug "converting known weight to kg"     
        value /= 2.2046;     
      }
        def str = "calibrate:" + value.toString()
        sendData(str)
        sendEvent(name: "calibrationLastRun", value: new Date().format('yyyy-MM-dd HH:mm:ss'))
        
    }    
    else {
        log.error "Negative calibration weight.  Cannot calibrate!"
    }

}

def sendData(String value) {
    def name = device.deviceNetworkId.split("-")[-1]
    parent.sendData("${name} ${value}")
}

def parse(String description) {
    if (logEnable) log.debug "parse(${description}) called"
	def parts = description.split(" ")
    def name  = parts.length>0?parts[0].trim():null
    def value = parts.length>1?parts[1].trim():null
	//def dispUnit = "kg"
    if (name && value) {
        value = Float.valueOf(value.trim());
        if (units == "lbs") {
            if (logEnable) log.debug "units = ${units}"
            value *= 2.20462; // convert kg to lbs       
            value = value.round(1); 
           // dispUnit = "lbs"
        }       
        // Update device
        sendEvent(name: name, value: value, unit: units)
        //sendEvent(name: name, value: value, unit: dispUnit)
        sendEvent(name: "units", value: units)
    }
    else {
    	log.error "Missing either name or value.  Cannot parse!"
    }
}

def installed() {
    state.Setup = "1. Run a tare command to zero the scales 2. Calibrate the scales using a known weight"
    sendEvent(name: "tareLastRun", value: "never")
    sendEvent(name: "calibrationLastRun", value: "never")
    updated()
}

def updated() {
    if (logEnable) {
        log.debug "update called"
        runIn(1800,logsOff)
    }
    //  update device
    sendData("updated")
    //

    // Add scheduled tare job at given time every day
    unschedule()
    if (scheduleDailyTare) { 
        log.debug "time is ${dailyTareTime}"
        try {
            date = timeToday(dailyTareTime)
            def h = date.getHours()
            def m = date.getMinutes()
            schedule("0 ${m} ${h} 1/1 * ? *", tare)      
        }
        catch (e) {log.error e}         
    }      
}


