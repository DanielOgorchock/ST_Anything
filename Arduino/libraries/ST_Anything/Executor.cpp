//******************************************************************************************
//  File: Executor.cpp
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
//    2019-02-09  Dan Ogorchock  Added update() function
//
//
//******************************************************************************************


#include "Executor.h"

#include "Constants.h"
#include "Everything.h"

namespace
st
{
//private



//public
	//constructor
	Executor::Executor(const __FlashStringHelper *name):
		Device(name)
	{
	
	}
	
	//destructor
	Executor::~Executor()
	{
		
	}

	void Executor::init()
	{
		
	}
	
	void Executor::update()
	{

	}

	//debug flag to determine if debug print statements are executed (set value in your sketch)
	bool Executor::debug=false; 
	
}