//*********************************************************
//	Plan_Processing.cpp - plan processing thread
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Plan_Processing constructor
//---------------------------------------------------------

NewFormat::Plan_Processing::Plan_Processing (NewFormat *_exe)
{
	exe = _exe;
	thread_flag = (exe->Num_Threads () > 1);

	old_plan.File_Access (exe->old_plan.File_Access ());
	old_plan.File_Format (exe->old_plan.File_Format ());
	old_plan.Part_Flag (exe->old_plan.Part_Flag ());
	old_plan.Extend (exe->old_plan.Extend ());
	old_plan.Pathname (exe->old_plan.Pathname ());
	old_plan.Plan_Memory (exe->old_plan.Plan_Memory ());
	old_plan.Plan_Sort (exe->old_plan.Plan_Sort ());
	old_plan.Traveler_Scale (exe->old_plan.Traveler_Scale ());
	old_plan.Node_Based_Flag (exe->old_plan.Node_Based_Flag ());
	old_plan.First_Open (false);

	Plan_File *file = (Plan_File *) exe->System_File_Handle (NEW_PLAN);

	new_plan.File_Type (file->File_Type ());
	new_plan.File_Access (file->File_Access ());
	new_plan.Dbase_Format (file->Dbase_Format ());
	new_plan.Part_Flag (file->Part_Flag ());
	new_plan.Pathname (file->Pathname ());
	new_plan.First_Open (false);
}

//---------------------------------------------------------
//	Plan_Processing operator
//---------------------------------------------------------

void NewFormat::Plan_Processing::operator()()
{
	int part = 0;
	while (exe->partition_queue.Get (part)) {
		Travel_Plans (part);
	}
	MAIN_LOCK
	exe->old_plan.Add_Counters (&old_plan);

	Plan_File *file = (Plan_File *) exe->System_File_Handle (NEW_PLAN);
	file->Add_Counters (&new_plan);
	END_LOCK
}
