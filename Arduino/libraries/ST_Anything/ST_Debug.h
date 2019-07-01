//******************************************************************************************
//  File: ST_debug
//  Authors: Daniel Terryn
//
//  Summary:  Defines global prototypes for logging messages
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2019-07-01  Dan.t		 	 Original Creation
//
//
//******************************************************************************************
#ifndef ST_DEBUG_H
#define ST_DEBUG_H

	typedef void debugPrint_t(String message);

	typedef void debugPrintln_t(String message);

namespace st
{
	
	debugPrint_t debugPrint;
	debugPrintln_t debugPrintln;
}

#endif