//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "PlanSelect.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void PlanSelect::Program_Control (void)
{
	String key;

	//---- initialize the MPI thread range ----

	MPI_Setup ();

	//---- create the network files ----

	Data_Service::Program_Control ();

	Read_Select_Keys ();

	delay_flag = System_File_Flag (LINK_DELAY);
	if (delay_flag) {
		Link_Delay_File *file = (Link_Delay_File *) System_File_Handle (LINK_DELAY);
		turn_flag = file->Turn_Flag ();
	}
	plan_file = (Plan_File *) System_File_Handle (PLAN);
	if (!plan_file->Part_Flag ()) Num_Threads (1);

	new_plan_flag = System_File_Flag (NEW_PLAN);
	if (new_plan_flag) {
		new_plan_file = (Plan_File *) System_File_Handle (NEW_PLAN);
		new_plan_file->Sort_Type (plan_file->Sort_Type ());
		new_plan_file->Close ();

		if (Num_Threads () > 1 && !new_plan_file->Part_Flag ()) {
			Error ("Partitioned Input Plans require Partitioned Output Plans");
		}
	}
	if (plan_file->Part_Flag ()) {
		Selection_File *file = (Selection_File *) System_File_Handle (NEW_SELECTION);

		if (!file->Part_Flag ()) {
			if (Master ()) {
				file->Clear_Fields ();
				file->Partition_Flag (true);
				file->Create_Fields ();
				file->Write_Header ();
			} else {

				//---- remove the new selection file ----
				
				file->Close ();
				key = file->Filename ();
				remove (key.c_str ());
				key += ".def";
				remove (key.c_str ());
			}
		} else {
			select_parts = true;
		}
	}
	if (delay_flag && (!System_File_Flag (NODE) || !System_File_Flag (LINK))) {
		Error ("Link Delays required a Link and Node File");
	}
	if (select_org_zones || select_des_zones) {
		if (!System_File_Flag (LOCATION)) {
			Error ("Selecting by Zone requires a Location File");
		}
	}
	if (time_diff_flag) {
		if (!delay_flag) {
			Error ("Time Differences require a Link Delay File");
		}
	}
	Print (2, String ("%s Control Keys:") % Program ());

	//---- select parking lots ----

	key = Get_Control_Text (SELECT_PARKING_LOTS);

	if (!key.empty () && !key.Equals ("ALL")) {
		select_parking = true;
		if (!parking_range.Add_Ranges (key)) {
			Error ("Adding Parking Lot Ranges");
		}
	}

	//---- select vc ratios ----

	vc_ratio = Get_Control_Double (SELECT_VC_RATIOS);
	select_vc = (vc_ratio >= 1.0);
	
	if (select_vc && !delay_flag) {
		Error ("A Link Delay File is needed for VC Ratios");
	}

	//---- select time ratios ----

	time_ratio = Get_Control_Double (SELECT_TIME_RATIOS);
	select_ratio = (time_ratio >= 1.0);

	if (select_ratio && !delay_flag) {
		Error ("A Link Delay File is needed for Time Ratios");
	}
}
