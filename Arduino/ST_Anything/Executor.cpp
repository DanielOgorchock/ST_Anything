#include "Executor.h"

#include "Constants.h"

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

	String Executor::init()
	{
		return Constants::IGNORE_STRING;
	}
	

	bool Executor::debug=false; 
	
}