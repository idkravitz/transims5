//*********************************************************
//	Read_Plan.cpp - Read the Plan File
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	Read_Plan
//---------------------------------------------------------

void PlanTrips::Read_Plan (void)
{
	int part, num_trips, num_update;

	Plan_File *plan_file;
	Plan_Data plan;
	Trip_Index trip_index;
	Trip_Map_Itr trip_map_itr;
	Trip_Data *trip_ptr;
		
	plan_file = (Plan_File *) System_File_Handle (PLAN);

	num_update = num_trips = 0;

	//---- process each partition ----

	for (part=0; ; part++) {
		if (part > 0) {
			if (!plan_file->Open (part)) break;
		}
		if (plan_file->Part_Flag ()) {
			Show_Message (String ("Reading %s %d -- Record") % plan_file->File_Type () % plan_file->Part_Number ());
		} else {
			Show_Message (String ("Reading %s -- Record") % plan_file->File_Type ());
		}
		Set_Progress ();

		while (plan_file->Read_Plan (plan)) {
			Show_Progress ();

			plan.Get_Trip_Index (trip_index);
			num_trips++;

			trip_map_itr = trip_map.find (trip_index);

			if (trip_map_itr != trip_map.end ()) {
				trip_ptr = &trip_array [trip_map_itr->second];

				trip_ptr->Start (plan.Depart ());
				trip_ptr->End (plan.Arrive ());
				trip_ptr->Duration (plan.Activity ());
				num_update++;
			}
		}
		End_Progress ();
	}
	plan_file->Close ();

	Print (2, String ("Number of %s Trips = %d") % plan_file->File_Type () % num_trips);
	if (part > 1) Print (0, String (" (%d files)") % part);

	Print (1, "Number of Trips Updated = ") << num_update;
}
