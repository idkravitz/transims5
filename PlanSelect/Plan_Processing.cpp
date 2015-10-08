//*********************************************************
//	Plan_Processing.cpp - plan processing thread
//*********************************************************

#include "PlanSelect.hpp"

//---------------------------------------------------------
//	Plan_Processing constructor
//---------------------------------------------------------

PlanSelect::Plan_Processing::Plan_Processing (PlanSelect *_exe)
{
	exe = _exe;
	thread_flag = (exe->Num_Threads () > 1);
	num_trips = 0;

	plan_file.File_Access (exe->plan_file->File_Access ());
	plan_file.Dbase_Format (exe->plan_file->Dbase_Format ());
	plan_file.Part_Flag (exe->plan_file->Part_Flag ());
	plan_file.Pathname (exe->plan_file->Pathname ());
	plan_file.Sort_Type (exe->plan_file->Sort_Type ());
	plan_file.First_Open (false);
}

//---------------------------------------------------------
//	Plan_Processing operator
//---------------------------------------------------------

void PlanSelect::Plan_Processing::operator()()
{
	int part = 0;
	while (exe->partition_queue.Get (part)) {
		Read_Plans (part);
	}
	MAIN_LOCK
	exe->plan_file->Add_Counters (&plan_file);

	exe->num_trips += num_trips;

	if (thread_flag) {
		Select_Map_Itr map_itr;
		Select_Map_Stat map_stat;
		
		for (map_itr = select_map.begin (); map_itr != select_map.end (); map_itr++) {
			map_stat = exe->select_map.insert (*map_itr);

			if (!map_stat.second) {
				Trip_Index index = map_stat.first->first;
				exe->Warning (String ("Duplicate Plan Record %d-%d-%d-%d") % 
					index.Household () % index.Person () % index.Tour () % index.Trip ());
			}
		}
	}
	END_LOCK
}
