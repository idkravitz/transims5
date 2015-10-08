//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Router::Execute (void)
{
	//---- read the network data ----

	Router_Service::Execute ();

	//---- build link delay arrays ----

	if (Flow_Updates () || Time_Updates ()) {
		if (Master ()) Build_Flow_Time_Arrays (old_link_array, old_turn_array);

		if (!System_File_Flag (LINK_DELAY)) {
			Build_Flow_Time_Arrays ();
		} else {
			Link_Delay_File *file = (Link_Delay_File *) System_File_Handle (LINK_DELAY);

			if (Turn_Updates () && turn_delay_array.size () == 0) {
				Build_Turn_Arrays ();
			}
			if (Master ()) {

				//---- copy existing flow data ----

				old_link_array.Copy_Flow_Data (link_delay_array, true);

				if (Turn_Flows () && file->Turn_Flag ()) {
					old_turn_array.Copy_Flow_Data (turn_delay_array, true);
				}

			} else {	//---- MPI slave ----

				//---- clear flows data ----

				link_delay_array.Zero_Flows ();

				if (Turn_Flows () && file->Turn_Flag ()) {
					turn_delay_array.Zero_Flows ();
				}
			}
		}
	}

	//---- create the file partitions ----

	Set_Partitions ();

	//---- initialize the plan processor ---

	if (trip_flag) {
		Iteration_Loop ();
	} else {
		part_processor.Copy_Plans ();

		if (Time_Updates () && System_File_Flag (NEW_LINK_DELAY)) {
			Update_Travel_Times ();
			num_time_updates++;
		}
	}

	//---- save the link delays ----

	if (System_File_Flag (NEW_LINK_DELAY)) {
		if (max_iteration > 1) Show_Message (1);
		Write_Link_Delays ();
	}

	//---- gather summary statistics ----

	if (trip_flag && trip_set_flag) {
		for (int i=0; i < num_file_sets; i++) {
			trip_file->Add_Counters (trip_set [i]);
			trip_set [i]->Close ();
		}
		if (trip_file->Num_Files () == 0) trip_file->Num_Files (num_file_sets);
	}
	if (plan_flag && plan_set_flag) {
		for (int i=0; i < num_file_sets; i++) {
			plan_file->Add_Counters (plan_set [i]);
			plan_set [i]->Close ();
		}
		if (plan_file->Num_Files () == 0) plan_file->Num_Files (num_file_sets);
	}
	if (new_plan_flag && plan_set_flag) {
		for (int i=0; i < num_file_sets; i++) {
			new_plan_file->Add_Counters (new_plan_set [i]);
			new_plan_set [i]->Close ();
		}
		if (new_plan_file->Num_Files () == 0) new_plan_file->Num_Files (num_file_sets);
	}
	if (problem_set_flag) {
		for (int i=0; i < num_file_sets; i++) {
			problem_file->Add_Counters (problem_set [i]);
			problem_set [i]->Close ();
		}
		if (problem_file->Num_Files () == 0) problem_file->Num_Files (num_file_sets);
	}

	//---- collect data from MPI slaves ----

	MPI_Processing ();

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case LINK_GAP:			//---- Link Gap Report ----
				if (iteration_flag) Link_Gap_Report ();
				break;
			case TRIP_GAP:			//---- Trip Gap Report ----
				if (iteration_flag) Trip_Gap_Report ();
				break;
			default:
				break;
		}
	}

	//---- print summary statistics ----
	
	if (Time_Updates ()) Print (2, "Number of Travel Time Updates = ") << num_time_updates;

	if (trip_flag) {
		trip_file->Print_Summary ();

		if (num_trip_sel != num_trip_rec) {
			Print (1, "Number of Records Selected = ") << num_trip_sel;
		}
	}
	if (plan_flag) {
		plan_file->Print_Summary ();
	}
	if (new_plan_flag) {
		new_plan_file->Print_Summary ();
	}
	if (problem_flag) {
		if (problem_file->Num_Trips () > 0) {
			problem_file->Print_Summary ();
		}
	}

	//---- end the program ----

	Report_Problems (total_records);
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void Router::Page_Header (void)
{
	switch (Header_Number ()) {
		case LINK_GAP:			//---- Link Gap Report ----
			Link_Gap_Header ();
			break;
		case TRIP_GAP:			//---- Trip Gap Report ----
			Trip_Gap_Header ();
			break;
		default:
			break;
	}
}

