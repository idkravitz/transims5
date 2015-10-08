//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "LinkData.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void LinkData::Execute (void)
{
	//---- read the data service ----

	Data_Service::Execute ();

	//---- read the link node list file ----

	Read_Link_Nodes ();

	//---- read the directional data file ----

	Read_Dir_Data ();

	//---- write the link data file ----

	if (data_flag) {
		Write_Dir_Data ();
	}
	Print (2, "Number of Link Node Equivalence Records = ") << nequiv;
	Print (1, "Number of From-To Link Combinations = ") << nab;
	Print (1, "Number of Directional Data Records = ") << ndir;
	Print (1, "Number of Link Data Records Written = ") << nlink;

	Exit_Stat (DONE);
}
