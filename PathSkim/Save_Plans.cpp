//*********************************************************
//	Save_Plans.cpp -  write the path building results
//*********************************************************

#include "PathSkim.hpp"

//---------------------------------------------------------
//	Save_Plans
//---------------------------------------------------------

bool PathSkim::Save_Plans (Plan_Ptr_Array *array_ptr)
{
	if (array_ptr == 0) return (false);

	Plan_Ptr_Itr itr;
	Plan_Data *plan_ptr;

	for (itr = array_ptr->begin (); itr != array_ptr->end (); itr++) {
		plan_ptr = *itr;

		total_records++;

		if (plan_ptr->Problem () == 0) {
			if (plan_flag) {
				plan_file->Write_Plan (*plan_ptr);
			}
			if (skim_flag) {
				int org, des;
				Skim_Data *skim_ptr;

				org = skim_file->Org_Index (plan_ptr->Origin ());
				des = skim_file->Des_Index (plan_ptr->Destination ());

				if (org >= 0 && des >= 0) {
					skim_ptr = skim_file->Table (org, des);

					skim_ptr->Add_Skim (plan_ptr->Walk (), plan_ptr->Drive (), plan_ptr->Transit (), plan_ptr->Wait (), 
									plan_ptr->Other (), plan_ptr->Length (), plan_ptr->Cost (), plan_ptr->Impedance ());
				}
			}
		} else {
			Set_Problem ((Problem_Type) plan_ptr->Problem ());

			if (problem_flag) {
				problem_file->Problem (plan_ptr->Problem ());
				problem_file->Household (plan_ptr->Household ());
				problem_file->Person (plan_ptr->Person ());
				problem_file->Tour (plan_ptr->Tour ());
				problem_file->Trip (plan_ptr->Trip ());
				problem_file->Start (plan_ptr->Start ());
				problem_file->End (plan_ptr->End ());
				problem_file->Duration (plan_ptr->Duration ());
				problem_file->Origin (plan_ptr->Origin ());
				problem_file->Destination (plan_ptr->Destination ());
				problem_file->Purpose (plan_ptr->Purpose ());
				problem_file->Mode (plan_ptr->Mode ());
				problem_file->Constraint (plan_ptr->Constraint ());
				problem_file->Priority (plan_ptr->Priority ());
				problem_file->Vehicle (plan_ptr->Vehicle ());
				problem_file->Passengers (plan_ptr->Passengers ());
				problem_file->Notes ((char *) Problem_Code ((Problem_Type) plan_ptr->Problem ()));

				if (!problem_file->Write ()) {
					Warning ("Writing ") << problem_file->File_Type ();
					problem_flag = false;
				}
				problem_file->Add_Trip (plan_ptr->Household (), plan_ptr->Person (), plan_ptr->Tour ());
			}
		}
		delete plan_ptr;
	}
	delete array_ptr;
	return (true);
}
