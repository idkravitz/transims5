//*********************************************************
//	Write_Plans.cpp - write the travel plan file
//*********************************************************

#include "PlanSelect.hpp"

//---------------------------------------------------------
//	Write_Plans
//---------------------------------------------------------

void PlanSelect::Plan_Output::Write_Plans (int part)
{
	Plan_Data plan;
	Select_Map_Itr map_itr;

	//---- open the file partition ----

	if (!plan_file.Open (part)) {
		MAIN_LOCK 
		exe->Error (String ("Opening %s") % plan_file.Filename ()); 
		END_LOCK
	}
	if (!new_plan_file.Open (part)) {
		MAIN_LOCK 
		exe->Error (String ("Opening %s") % new_plan_file.Filename ()); 
		END_LOCK
	}
	if (thread_flag) {
		MAIN_LOCK
		if (new_plan_file.Part_Flag ()) {
			exe->Show_Message (String ("Writing %s %d") % new_plan_file.File_Type () % new_plan_file.Part_Number ());
		} else {
			exe->Show_Message (String ("Writing %s") % new_plan_file.File_Type ());
		}
		END_LOCK
	} else {
		if (new_plan_file.Part_Flag ()) {
			exe->Show_Message (String ("Writing %s %d -- Record") % new_plan_file.File_Type () % new_plan_file.Part_Number ());
		} else {
			exe->Show_Message (String ("Writing %s -- Record") % new_plan_file.File_Type ());
		}
		exe->Set_Progress ();
	}

	//---- read the plan file ----

	while (plan_file.Read_Plan (plan)) {
		if (!thread_flag) exe->Show_Progress ();

		map_itr = exe->select_map.find (plan.Get_Trip_Index ());
		if (map_itr != exe->select_map.end () && map_itr->second.Partition () >= 0) {
			new_plan_file.Write_Plan (plan);
		}
	}
	if (!thread_flag) exe->End_Progress ();

	plan_file.Close ();
	new_plan_file.Close ();
}
