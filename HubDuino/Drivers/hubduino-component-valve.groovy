/**
 *  HubDuino Component Valve
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/hubduino-component-valve.groovy
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
 *    2019-10-30  Dan Ogorchock  Original Creation
 *    2019-10-31  Dan Ogorchock  Cleaned up
 *    2020-01-25  Dan Ogorchock  Remove custom lastUpdated attribute & general code cleanup
 *    2023-01-09  Dan Ogorchock  Revised to follow standard Hubitat Generic Component driver architecture
 *
 * 
 */
metadata {
	definition (name: "HubDuino Component Valve", namespace: "ogiewon", author: "Dan Ogorchock", component: true, importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/hubduino-component-valve.groovy") {
		capability "Valve"
		capability "Actuator"
		capability "Sensor"
        capability "Refresh"
    }

    preferences {
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
        if (it.name in ["valve"]) {
            if (txtEnable) log.info it.descriptionText
            sendEvent(name: it.name, value: it.value)      
        }
    }
}

void refresh() {
    parent?.componentRefresh(this.device)
}

void close() {
    parent?.componentClose(this.device)
}

void open() {
    parent?.componentOpen(this.device)
}