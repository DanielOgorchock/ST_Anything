//*******************************************************************************
//	SmartThings Arduino Library Base Class
//
//	License
//	(C) Copyright 2017 Dan Ogorchock
//
//	History
//	2017-02-04  Dan Ogorchock  Created
//*******************************************************************************
#ifndef __SMARTTHINGS_H__ 
#define __SMARTTHINGS_H__

#include <Arduino.h>

//*******************************************************************************
// Callout Function Definition for Messages Received from SmartThings 
//*******************************************************************************
typedef void SmartThingsCallout_t(String message);

namespace st
{
	class SmartThings
	{
	private:

	protected:
		SmartThingsCallout_t *_calloutFunction;
		bool _isDebugEnabled;
		String _shieldType;
		int m_nTransmitInterval;

	public:

		//*******************************************************************************
		// SmartThings Constructor 
		//*******************************************************************************
		SmartThings(SmartThingsCallout_t *callout, String shieldType = "Unknown", bool enableDebug = false, int transmitInterval = 100);

		//*******************************************************************************
		// SmartThings Destructor 
		//*******************************************************************************
		virtual ~SmartThings();

		//*******************************************************************************
		/// Initialize SmartThings Library 
		//*******************************************************************************
		virtual void init(void) = 0; //all derived classes must implement this pure virtual function

		//*******************************************************************************
		/// Run SmartThings Library 
		//*******************************************************************************
		virtual void run(void) = 0; //all derived classes must implement this pure virtual function

		//*******************************************************************************
		/// Send Message to the Hub 
		//*******************************************************************************
		virtual void send(String message) = 0; //all derived classes must implement this pure virtual function

		//*******************************************************************************
		/// Send Message to the Hub 
		//*******************************************************************************
		virtual int getTransmitInterval() const { return m_nTransmitInterval; }

	};

}
#endif
