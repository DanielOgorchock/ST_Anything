/**
 *  HubDuino Component DoorControl
 *
 *  https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/hubduino-component-door-control.groovy
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
 *    Date        Who            What
 *    ----        ---            ----
 *    2015-04-14  Dan Ogorchock  Original Creation
 *    2017-08-23  Allan (vseven) Added a generateEvent routine that gets info from the parent device.  This routine runs each time the value is updated which can lead to other modifications of the device.
 *    2018-06-02  Dan Ogorchock  Revised/Simplified for Hubitat Composite Driver Model
 *    2018-09-22  Dan Ogorchock  Added preference for debug logging
 *    2019-07-01  Dan Ogorchock  Added importUrl
 *    2019-07-28  Dan Ogorchock  Minor tweak to support option for a switch output instead of just a momentary output - requires updated Arduino Door Control Code!!!
 *    2020-01-25  Dan Ogorchock  Remove custom lastUpdated attribute & general code cleanup
 *    2022-12-31  Dan Ogorchock  Borrowed extensively from @tomw from the HADB Project at https://github.com/ymerj/HE-HA-control to follow Hubitat Generic Component driver architecture
 *
 *
 */
metadata
{
    definition(name: "HubDuino Component Door Control", namespace: "ogiewon", author: "Dan Ogorchock", component: true, importUrl: "https://raw.githubusercontent.com/DanielOgorchock/ST_Anything/master/HubDuino/Drivers/hubduino-component-door-control.groovy")
    {
        capability "Door Control"
        capability "Garage Door Control"
        capability "Contact Sensor"
		capability "Actuator"
		capability "Sensor"
		capability "Momentary"
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
        if (it.name in ["door", "doorControl"]) {
            if (txtEnable) log.info it.descriptionText
            sendEvent(name: "door", value: it.value)
            
            //Also implement the "Contact Sensor" device as this is useful for Apps that do not support the "Door Control" capability
            if((it.value == "open") || (it.value == "closed")) { sendEvent(name: "contact", value: it.value) } 
        }
    }
}

void refresh() {
    parent?.componentRefresh(this.device)
}

void close() {
    //Special Handling to be compatible with ST_Anything Arduino code
    //parent?.componentClose(this.device)
    parent?.componentOff(this.device)
}

void open() {
    //Special Handling to be compatible with ST_Anything Arduino code
    //parent?.componentOpen(this.device)
    parent?.componentOn(this.device)
}

void push() {
	parent?.componentOn(this.device)
}
