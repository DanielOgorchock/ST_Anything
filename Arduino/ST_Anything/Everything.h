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
			static void sendStrings(); //sends everything in Return_String
			
			static unsigned long lastmillis;
			
			//stuff for refreshing executors
			static unsigned long exLastMillis;
			static void refreshExecutors();
			
			#ifdef ENABLE_SERIAL
				static void readSerial();
			#endif
		
			static String Return_String; //essentially acts as a buffer
		
		public:
			
			static void init();
			static void initDevices();
			static void run();
			
			static bool sendSmartString(String &str); //sendSmartString() may edit the string reference passed to it
			
			static Device* getDeviceByName(const String &str);
			
			
			static bool addSensor(Sensor *sensor);
			static bool addExecutor(Executor *executor);
		
			static bool debug;
			
			friend SmartThingsCallout_t receiveSmartString; //act on data received from SmartThings
			
	};
}



#endif