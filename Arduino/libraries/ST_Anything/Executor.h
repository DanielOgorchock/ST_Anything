//******************************************************************************************
//  File: Executor.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  st::Executor is a generic class which inherits from st::Device.  This is the
//			  parent class for the st::EX_Switch class.
//			  In general, this file should not need to be modified.   
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//
//
//******************************************************************************************

#ifndef ST_EXECUTOR_H
#define ST_EXECUTOR_H

#include "Device.h"

namespace st
{
	class Executor: public Device
	{
		private:
			//Inherits private members from parent st::Device class
		
		public:
			//constructor
			Executor(const __FlashStringHelper *name);
			
			//destructor
			virtual ~Executor();
		
			//initialization routine
			virtual void init();	
		
			//debug flag to determine if debug print statements are executed (set value in your sketch)
			static bool debug;
	
	};


}



#endif