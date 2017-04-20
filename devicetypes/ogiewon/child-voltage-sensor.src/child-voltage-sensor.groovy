/**
 *  Child Voltage Sensor
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
 *    2017-04-19  Dan Ogorchock  Original Creation
 *
 * 
 */
metadata {
	definition (name: "Child Voltage Sensor", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Voltage Measurement"
		capability "Sensor"
	}
        
	tiles(scale: 2) {
		multiAttributeTile(name: "voltage", type: "generic", width: 6, height: 4, canChangeIcon: true) {
			tileAttribute("device.voltage", key: "PRIMARY_CONTROL") {
				attributeState("voltage", label: '${currentValue} ${unit}', unit: "mV", defaultState: true)
			}
		}
	}
}