/**
 *  HubDuino Component Sound Pressure Level
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/hubduino-component-sound-pressure-level.groovy
 *
 *  Copyright 2019 Daniel Ogorchock
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
 *    2019-07-08  Dan Ogorchock  Original Creation
 *    2020-01-25  Dan Ogorchock  Remove custom lastUpdated attribute & general code cleanup
 *    2022-12-10  Dan Ogorchock  Revised to follow standard Hubitat Generic Component driver architecture
 *
 * 
 */
metadata {
	definition (name: "HubDuino Component Sound Pressure Level", namespace: "ogiewon", author: "Daniel Ogorchock", importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/hubduino-component-sound-pressure-level.groovy") {
        capability "Sensor"
        capability "Sound Pressure Level"
        capability "Contact Sensor"
	}
        
    preferences {
        input "maxSPL", "number", title: "SPL Threshold", description: "SPL values above this threshold will CLOSE the contact sensor attribute.", range: "*..*", displayDuringSetup: true
        input name: "logEnable", type: "bool", title: "Enable debug logging", defaultValue: true
        input name: "txtEnable", type: "bool", title: "Enable descriptionText logging", defaultValue: true
	}

}

void updated() {
    log.info "Updated..."
    log.warn "description logging is: ${txtEnable == true}"
    if (logEnable) runIn(1800,logsOff)
}

void installed() {
    log.info "Installed..."
    device.updateSetting("txtEnable",[type:"bool",value:true])
    refresh()
    updated()
}

void logsOff(){
    log.warn "debug logging disabled..."
    device.updateSetting("logEnable",[value:"false",type:"bool"])
}

void parse(String description) { log.warn "parse(String description) not implemented" }

void parse(List<Map> description) {
    if (logEnable) log.debug "parse received ${description}"
    description.each {
        if (it.name in ["soundPressureLevel"]) {
            if (txtEnable) log.info it.descriptionText
            sendEvent(it)
            
            float tmpValue = Float.parseFloat(it.value)
            if (maxSPL) {
                if (tmpValue >= Float.valueOf(maxSPL)) {
                    sendEvent(name: "contact", value: "closed")
                } else {
                    sendEvent(name: "contact", value: "open")
                }
            }
        }
    }
}

void refresh() {
    parent?.componentRefresh(this.device)
}
/**
 *  HubDuino Component Sound Pressure Level
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/hubduino-component-sound-pressure-level.groovy
 *
 *  Copyright 2019 Daniel Ogorchock
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
 *    2019-07-08  Dan Ogorchock  Original Creation
 *    2020-01-25  Dan Ogorchock  Remove custom lastUpdated attribute & general code cleanup
 *    2022-12-10  Dan Ogorchock  Revised to follow standard Hubitat Generic Component driver architecture
 *
 * 
 */
metadata {
	definition (name: "HubDuino Component Sound Pressure Level", namespace: "ogiewon", author: "Daniel Ogorchock", importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/hubduino-component-sound-pressure-level.groovy") {
        capability "Sensor"
        capability "Sound Pressure Level"
        capability "Contact Sensor"
	}
        
    preferences {
        input "maxSPL", "number", title: "SPL Threshold", description: "SPL values above this threshold will CLOSE the contact sensor attribute.", range: "*..*", displayDuringSetup: true
        input name: "logEnable", type: "bool", title: "Enable debug logging", defaultValue: true
        input name: "txtEnable", type: "bool", title: "Enable descriptionText logging", defaultValue: true
	}

}

void updated() {
    log.info "Updated..."
    log.warn "description logging is: ${txtEnable == true}"
    if (logEnable) runIn(1800,logsOff)
}

void installed() {
    log.info "Installed..."
    device.updateSetting("txtEnable",[type:"bool",value:true])
    refresh()
    updated()
}

void logsOff(){
    log.warn "debug logging disabled..."
    device.updateSetting("logEnable",[value:"false",type:"bool"])
}

void parse(String description) { log.warn "parse(String description) not implemented" }

void parse(List<Map> description) {
    if (logEnable) log.debug "parse received ${description}"
    description.each {
        if (it.name in ["soundPressureLevel"]) {
            if (txtEnable) log.info it.descriptionText
            sendEvent(it)
            
            float tmpValue = Float.parseFloat(it.value)
            if (maxSPL) {
                if (tmpValue >= Float.valueOf(maxSPL)) {
                    sendEvent(name: "contact", value: "closed")
                } else {
                    sendEvent(name: "contact", value: "open")
                }
            }
        }
    }
}

void refresh() {
    parent?.componentRefresh(this.device)
}
