#include "Everything.h"


namespace st
{
//private


	void Everything::updateSensors()
	{
		for(unsigned int index=0; index<m_nSensorCount; ++index)
		{
			sendSmartString(m_Sensors[index]->update());
		}
	}
	
//public
	//constructor
	Everything::Everything():
		m_nSensorCount(0),
		m_nExecutorCount(0)
	{
	
	}
	
	//destructor
	Everything::~Everything()
	{
	
	}


}