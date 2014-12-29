#ifndef ST_EVERYTHING_H
#define ST_EVERYTHING_H

#include "Arduino.h"
#include "Constants.h"
#include "Sensor.h"
#include "Executor.h"

#include "SmartThings.h"
//#include "SoftwareSerial.h"

namespace st
{

	class Everything
	{
		private:
			static Sensor* m_Sensors[Constants::MAX_SENSOR_COUNT];
			static unsigned int m_nSensorCount;
			
			static Executor* m_Executors[Constants::MAX_EXECUTOR_COUNT];
			static unsigned int m_nExecutorCount;
			
			//SmartThings Object
			#ifndef DISABLE_SMARTTHINGS
				static SmartThings SmartThing;
			#endif
			
			static SmartThingsNetworkState_t stNetworkState;
		
			static void updateNetworkState();
		
			static void updateSensors(); //simply calls update on all the sensors
			
			static unsigned long lastmillis;
			
			#ifdef ENABLE_SERIAL
				static void readSerial();
			#endif
		
		public:
			
			static void init();
			static void initDevices();
			static void run();
			
			static void sendSmartString(const String &str);
			static Device* getDeviceByName(const String &str);
			
			
			static bool addSensor(Sensor *sensor);
			static bool addExecutor(Executor *executor);
		
			static bool debug;
			
			static String Return_String;
			
			friend SmartThingsCallout_t receiveSmartString; //act on data received from SmartThings
			
	};
}



#endif