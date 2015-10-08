//*********************************************************
//	Plan_Output.cpp - plan output thread
//*********************************************************

#include "PlanSelect.hpp"

//---------------------------------------------------------
//	Plan_Output constructor
//---------------------------------------------------------

PlanSelect::Plan_Output::Plan_Output (PlanSelect *_exe)
{
	exe = _exe;
	thread_flag = (exe->Num_Threads () > 1);

	plan_file.File_Access (exe->plan_file->File_Access ());
	plan_file.Dbase_Format (exe->plan_file->Dbase_Format ());
	plan_file.Part_Flag (exe->plan_file->Part_Flag ());
	plan_file.Pathname (exe->plan_file->Pathname ());
	plan_file.Sort_Type (exe->plan_file->Sort_Type ());
	plan_file.First_Open (false);

	new_plan_file.File_Access (exe->new_plan_file->File_Access ());
	new_plan_file.Dbase_Format (exe->new_plan_file->Dbase_Format ());
	new_plan_file.Part_Flag (exe->new_plan_file->Part_Flag ());
	new_plan_file.Pathname (exe->new_plan_file->Pathname ());
	new_plan_file.Sort_Type (exe->new_plan_file->Sort_Type ());
	new_plan_file.First_Open (false);
}

//---------------------------------------------------------
//	Plan_Output operator
//---------------------------------------------------------

void PlanSelect::Plan_Output::operator()()
{
	int part = 0;
	while (exe->output_queue.Get (part)) {
		Write_Plans (part);
	}
	MAIN_LOCK
	exe->new_plan_file->Add_Counters (&new_plan_file);
	END_LOCK
}
