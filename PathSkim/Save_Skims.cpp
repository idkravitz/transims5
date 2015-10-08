//*********************************************************
//	Save_Skims.cpp - summarize the path building results
//*********************************************************

#include "PathSkim.hpp"

//---------------------------------------------------------
//	Save_Skims
//---------------------------------------------------------

bool PathSkim::Save_Skims (One_To_Many *skim_ptr)
{
	if (skim_ptr == 0) return (false);

	if (!skim_flag) {
		delete skim_ptr;
		return (true);
	}
	if (skim_ptr->Problem ()) {
		Set_Problem ((Problem_Type) skim_ptr->Problem ());
		delete skim_ptr;
		return (true);
	}
	int org, des;
	Skim_Data *data_ptr;
	Many_Itr many_itr;
	Dtime time;

	org = des = 0;

	if (zone_skim_flag || district_flag) {
		if (forward_flag) {
			org = skim_file->Org_Index (skim_ptr->Zone ());
		} else {
			des = skim_file->Des_Index (skim_ptr->Zone ());
		}
	} else {
		if (forward_flag) {
			org = skim_file->Org_Index (skim_ptr->Location ());
		} else {
			des = skim_file->Des_Index (skim_ptr->Location ());
		}
	}
	for (many_itr = skim_ptr->begin (); many_itr != skim_ptr->end (); many_itr++) {
		if (many_itr->Problem ()) {
			Set_Problem ((Problem_Type) many_itr->Problem ());
			continue;
		}
		if (many_itr->Impedance () == 0) continue;

		if (zone_skim_flag || district_flag) {
			if (forward_flag) {
				des = skim_file->Des_Index (many_itr->Zone ());
			} else {
				org = skim_file->Org_Index (many_itr->Zone ());
			}
		} else {
			if (forward_flag) {
				des = skim_file->Des_Index (many_itr->Location ());
			} else {
				org = skim_file->Org_Index (many_itr->Location ());
			}
		}
		if (org < 0 || des < 0) continue;

		data_ptr = skim_file->Table (org, des);
		data_ptr->Add_Skim (many_itr->Walk (), many_itr->Drive (), many_itr->Transit (), many_itr->Wait (), 
						many_itr->Other (), many_itr->Length (), many_itr->Cost (), many_itr->Impedance ());
	}
	delete skim_ptr;
	return (true);
}
