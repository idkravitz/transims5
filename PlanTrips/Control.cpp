//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void PlanTrips::Program_Control (void)
{
	String key;

	//---- open network and demand files ----

	Data_Service::Program_Control ();
	
	Read_Select_Keys ();

	select_flag = System_File_Flag (SELECTION);

	Print (2, String ("%s Control Keys:") % Program ());
	Print (1);
} 
