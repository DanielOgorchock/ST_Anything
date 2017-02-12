//*******************************************************************************
//	SmartThings Arduino Library Base Class
//
//	License
//	(C) Copyright 2017 Dan Ogorchock
//
//	History
//	2017-02-04  Dan Ogorchock  Created
//*******************************************************************************
#include <SmartThings.h>

namespace st
{
	//*******************************************************************************
	// SmartThings Constructor  
	//*******************************************************************************
	SmartThings::SmartThings(SmartThingsCallout_t *callout, String shieldType, bool enableDebug) :
		_calloutFunction(callout),
		_shieldType(shieldType),
		_isDebugEnabled(enableDebug)
	{

	}

	//*****************************************************************************
	//SmartThings::~SmartThings()
	//*****************************************************************************
	SmartThings::~SmartThings()
	{

	}

}
