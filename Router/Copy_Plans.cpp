//*********************************************************
//	Copy_Plans.cpp - Copy the Plan File
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Copy_Plans
//---------------------------------------------------------

bool Router::Copy_Plans (int part, Plan_Processor *plan_process_ptr)
{
	int last_hhold, hhold, partition;
	string process_type;
	bool keep_flag;

	Plan_File *plan_file;
	Plan_Data *plan_ptr;
	Plan_Ptr_Array *plan_ptr_array;

	//---- process each partition ----
		
	plan_ptr_array = new Plan_Ptr_Array ();
	plan_ptr = new Plan_Data ();

	partition = part;

	if (plan_set_flag) {
		plan_file = plan_set [part];
	} else {
		plan_file = Router::plan_file;
	}
	if (update_flag) {
		process_type = "Updating";
	} else if (reroute_flag) {
		process_type = "Re-Routing";
	} else {
		process_type = "Copying";
	}
	if (thread_flag) {
		MAIN_LOCK
		if (plan_file->Part_Flag ()) {
			Show_Message (String ("%s %s %d") % process_type % plan_file->File_Type () % plan_file->Part_Number ());
		} else {
			Show_Message (String ("%s %s") % process_type % plan_file->File_Type ());
		}
		END_LOCK
	} else {
		if (plan_file->Part_Flag ()) {
			Show_Message (String ("%s %s %d -- Trip") % process_type % plan_file->File_Type () % plan_file->Part_Number ());
		} else {
			Show_Message (String ("%s %s -- Trip") % process_type % plan_file->File_Type ());
		}
		Set_Progress ();
	}
	last_hhold = 0;

	while (plan_file->Read_Plan (*plan_ptr)) {
		if (thread_flag) {
			Show_Dot ();
		} else {
			Show_Progress ();
		}

		//---- check the household id ----

		hhold = plan_ptr->Household ();
		if (hhold < 1) continue;

		if (hhold != last_hhold) {
			if (last_hhold > 0 && plan_ptr_array->size () > 0) {
				plan_process_ptr->Plan_Build (plan_ptr_array);

				plan_ptr_array = new Plan_Ptr_Array ();
			}
			last_hhold = hhold;
		}
		keep_flag = Selection (plan_ptr);

		partition = plan_ptr->Partition ();

		if (!update_flag && !reroute_flag && !keep_flag) continue;

		//---- update the paths ----

		if (update_flag && keep_flag) {
			plan_ptr->Method (RESKIM_PATH);
		} else if (reroute_flag) {
			if (plan_ptr->Depart () >= reroute_time) {
				plan_ptr->Method (BUILD_PATH);
			} else if (plan_ptr->Arrive () < reroute_time) {
				if (Link_Flows ()) {
					plan_ptr->Method (PATH_FLOWS);
				} else {
					plan_ptr->Method (COPY_PATH);
				}
			} else {
				plan_ptr->Method (REROUTE_PATH);
				plan_ptr->Arrive (reroute_time);
			}
		} else if (Link_Flows ()) {
			plan_ptr->Method (PATH_FLOWS);
		} else {
			plan_ptr->Method (COPY_PATH);
		}
		plan_ptr_array->push_back (plan_ptr);

		plan_ptr = new Plan_Data ();
	}
	if (last_hhold > 0 && plan_ptr_array->size () > 0) {
		plan_process_ptr->Plan_Build (plan_ptr_array);

		plan_ptr_array = new Plan_Ptr_Array ();
	}
	if (!thread_flag) {
		End_Progress ();
	}
	plan_file->Close ();

	delete plan_ptr_array;
	delete plan_ptr;
	return (true);
}
