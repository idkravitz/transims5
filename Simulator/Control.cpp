//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "Simulator.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Simulator::Program_Control (void)
{
	String key;

	//---- create the network files ----

	if (Slave ()) {
		System_File_False (PLAN);

		if (System_File_Flag (NEW_PROBLEM)) {
			problem_output.Output_Flag (true);
			System_File_False (NEW_PROBLEM);
		}
	}
	Simulator_Base::Program_Control ();

	if (Master ()) {
		if (System_File_Flag (PLAN)) {
			plan_file = (Plan_File *) System_File_Handle (PLAN);
		}
		Print (2, String ("%s Control Keys:") % Program ());
	}

#ifdef DEBUG_KEYS
	//---- debug time range ----

	key = Get_Control_Text (DEBUG_TIME_RANGE);

	if (!key.empty () && !key.Equals ("ALL")) {
		if (!debug_time.Add_Ranges (key)) {
			Error ("Adding Debug Time Range");
		}
		debug_flag = true;
	}

	//---- debug vehicle_list ----

	bool flag = false;
	key = Get_Control_Text (DEBUG_VEHICLE_LIST);

	if (!key.empty ()) {
		if (!key.Equals ("ALL")) {
			if (!debug_list.Add_Ranges (key)) {
				Error ("Adding Debug Vehicle List");
			}
		} else {
			flag = true;
		}
	}

	//---- debug link_list ----

	key = Get_Control_Text (DEBUG_LINK_LIST);

	if (!key.empty () && !key.Equals ("ALL")) {
		if (!debug_link.Add_Ranges (key)) {
			Error ("Adding Debug Link List");
		}
	}

	if (debug_flag && debug_list.Num_Ranges () == 0 && debug_link.Num_Ranges () == 0) {
		if (flag) {
			debug_list.Add_Ranges ("ALL");
		} else {
			Error ("Debug Link or Vehicle Range Missing");
		}
	}
#endif
}
