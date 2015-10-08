//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ExportPlans.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ExportPlans::Execute (void)
{
	//---- read the network data ----

	Data_Service::Execute ();

	//---- set selection flags ----

	select_flag = (select_nodes || select_subarea);

	if (select_flag) {
		Select_Links ();
	}

	//---- read the location map ----

	if (map_flag) {
		Read_Map ();
	}

	//---- read the subzone weights ----

	if (subzone_flag) {
		Read_Subzone ();
	}

	//---- read the plan file ----

	Read_Plans ();

	//---- write the vissim network ----

	if (vissim_flag) {
		Write_VISSIM ();
	}
	Exit_Stat (DONE);
}


