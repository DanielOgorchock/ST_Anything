#include "Executor.h"

#include "Constants.h"
#include "Everything.h"

namespace
st
{
//private




//public
	//constructor
	Executor::Executor(byte id):
		Device(id)
	{
	
	}
	
	//destructor
	Executor::~Executor()
	{
		
	}

	const String& Executor::init()
	{
		return Constants::IGNORE_STRING;
	}
	

	bool Executor::debug=false; 
	
}