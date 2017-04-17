/**
 *  Child Illuminance Sensor
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
 *    2017-04-10  Dan Ogorchock  Original Creation
 *
 * 
 */
metadata {
	definition (name: "Child Illuminance Sensor", namespace: "ogiewon", author: "Daniel Ogorchock") {
		capability "Illuminance Measurement"
		capability "Sensor"
	}

	tiles(scale: 2) {
		multiAttributeTile(name: "illuminance", type: "generic", width: 6, height: 4, canChangeIcon: true) {
			tileAttribute("device.illuminance", key: "PRIMARY_CONTROL") {
				attributeState("illuminance", label: '${currentValue} ${unit}', unit:"lux", defaultState: true, 
						backgroundColors: [
							[value: 9, color: "#767676"],
							[value: 315, color: "#ffa81e"],
							[value: 1000, color: "#fbd41b"]
						])
			}
		}
	}
}