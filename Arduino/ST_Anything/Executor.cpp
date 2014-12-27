#include "Executor.h"

#include "Constants.h"
#include "Everything.h"

namespace
st
{
//private




//public
	//constructor
	Executor::Executor(const String &name):
		Device(name)
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