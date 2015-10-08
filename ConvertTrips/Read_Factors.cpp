//*********************************************************
//	Read_Factors.cpp - read the trip factor file
//*********************************************************

#include "File_Group.hpp"

//---------------------------------------------------------
//	Read_Factors
//---------------------------------------------------------

void File_Group::Read_Factors (Zone_Equiv &zone_equiv)
{
	int org, des, period, num_periods;
	bool equiv_flag;

	Matrix_Index index;
	Factor_Periods factor_periods;
	Factor_Period factor_rec;
	Factor_Table_Stat fac_stat;
	Int_Map_Stat map_stat;
	Int_Map_Itr map_itr;

	if (factor_file == 0) return;

	//---- read the trip adjustment factors into memory ----

	factor_table.clear ();
	equiv_flag = (zone_equiv.Num_Groups () > 0);
	num_periods = factor_file->Num_Periods ();
	if (num_periods < 1) num_periods = 1;

	exe->Show_Message (0, String ("\tReading %s -- Record") % factor_file->File_Type ());
	exe->Set_Progress ();

	while (factor_file->Read ()) {
		exe->Show_Progress ();

		org = factor_file->Origin ();
		if (org == 0) continue;

		if (equiv_flag && zone_equiv.Group_List (org) == 0) {
			exe->Error (String ("Factor District %d is Not in the Zone Equivalence") % org);
		}
		des = factor_file->Destination ();
		if (des == 0) continue;

		if (equiv_flag && zone_equiv.Group_List (des) == 0) {
			exe->Error (String ("Factor District %d is Not in the Zone Equivalence") % des);
		}
		index.Origin (org);
		index.Destination (des);

		fac_stat = factor_table.insert (Factor_Table_Data (index, factor_periods));

		if (fac_stat.second) {
			fac_stat.first->second.assign (num_periods, factor_rec);
		}
		period = factor_file->Period ();
		fac_stat.first->second [period].Factor (factor_file->Factor ());
	}
	exe->End_Progress ();

	factor_file->Close ();
}
