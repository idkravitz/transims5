//*********************************************************
//	Update_Plans.cpp - update plan travel time data
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Update_Plans
//---------------------------------------------------------

void EventSum::Update_Plans (void)
{
	int nfile;
	Dtime time, diff;

	Plan_File *plan_file = (Plan_File *) System_File_Handle (PLAN);
	Plan_File *new_plan_file = (Plan_File *) System_File_Handle (NEW_PLAN);

	Trip_Index trip_index;
	Trip_Map_Itr map_itr;
	Travel_Time_Data *time_ptr;
	Plan_Data plan_data;

	//---- read the plan file ----

	for (nfile=0; ; nfile++) {
		if (!plan_file->Open (nfile)) break;

		if (new_plan_file->Part_Flag ()) {
			if (!new_plan_file->Open (nfile)) {
				Error (String ("Opening %s") % new_plan_file->File_Type ());
			}
		}
		if (plan_file->Part_Flag ()) {
			Show_Message (String ("Updating %s %d -- Plan") % plan_file->File_Type () % plan_file->Part_Number ());
		} else {
			Show_Message (String ("Updating %s -- Plan") % plan_file->File_Type ());
		}
		Set_Progress ();

		//---- read each plan ----

		while (plan_file->Read_Plan (plan_data)) {
			Show_Progress ();

			plan_data.Get_Trip_Index (trip_index);

			map_itr = travel_time_map.find (trip_index);
			if (map_itr == travel_time_map.end ()) continue;

			time_ptr = &travel_time_array [map_itr->second];

			if (time_ptr->Constraint () == START_TIME || time_ptr->Constraint () == FIXED_TIME) {

				diff = time_ptr->Trip_Start () - plan_data.Depart ();

				if (diff != 0 || time_ptr->Trip_Start_Diff () != 0) {
					plan_data.Depart (time_ptr->Trip_Start ());
					plan_data.Arrive (time_ptr->Trip_Start () + time_ptr->Travel_Time ());
				}
				if (time_ptr->Constraint () == FIXED_TIME) {
					plan_data.Arrive (time_ptr->Trip_End ());
				}
			} else if (time_ptr->Constraint () == END_TIME) {

				diff = time_ptr->Trip_End () - plan_data.Arrive ();

				if (diff != 0 || time_ptr->Trip_End_Diff () != 0) {
					plan_data.Arrive (time_ptr->Trip_End ());
					plan_data.Depart (time_ptr->Trip_End () - time_ptr->Travel_Time ());
					if (plan_data.Depart () < 0) plan_data.Depart () == 0;
				}
			} else if (time_ptr->Constraint () == DURATION) {

				diff = plan_data.Activity () - plan_data.Duration ();

				plan_data.Arrive (time_ptr->Ended () + diff);
				plan_data.Depart (plan_data.Arrive () - time_ptr->Travel_Time ());
			}

			//---- write the updated plan to the new plan file ----

			if (!new_plan_file->Write_Plan (plan_data)) {
				Error (String ("Writing %s") % new_plan_file->File_Type ());
			}
		}
		End_Progress ();
	}
	plan_file->Close ();
	new_plan_file->Close ();

	//---- write summary statistics ----

	plan_file->Print_Summary ();
	new_plan_file->Print_Summary ();
	Print (1);
}
