//*********************************************************
//	Read_Skims.cpp - read the skim file
//*********************************************************

#include "TransitAccess.hpp"

//---------------------------------------------------------
//	Read_Skims
//---------------------------------------------------------

void TransitAccess::Read_Skims (Skim_Itr &itr)
{
	int org, ndes, table, number;
	Db_Mat_Ptr skim_ptr;
	Doubles table_data, *table_ptr;
	Dbls_Itr dbls_itr;
	double *data;

	skim_ptr = skim_files [itr->index];
	number = itr->number;

	Show_Message (String ("Reading %s -- Record") % skim_ptr->File_Type ());
	Set_Progress ();

	ndes = skim_ptr->Zones ();

	//---- allocate skim matrix memory ----

	if (itr->time.size () == 0) {
		itr->time.assign (nzones, table_data);
		itr->distance.assign (nzones, table_data);

		for (dbls_itr = itr->time.begin (); dbls_itr != itr->time.end (); dbls_itr++) {
			dbls_itr->assign (ndes, 0.0);
		}
		for (dbls_itr = itr->distance.begin (); dbls_itr != itr->distance.end (); dbls_itr++) {
			dbls_itr->assign (ndes, 0.0);
		}
	}

	//---- read the skim data into memory ----

	if (itr->time_table < itr->dist_table) {
		for (org=1; org <= nzones; org++) {
			Show_Progress ();

			table = itr->time_table;
			table_ptr = &itr->time [org-1];
			data = &table_ptr->at (0);

			if (!skim_ptr->Read (org, table, data)) {
				Error (String ("Reading Skim Group=%d, Time Table=%d, Origin=%d") % number % table % org);
			}
			table = itr->dist_table;
			table_ptr = &itr->distance [org-1];
			data = &table_ptr->at (0);

			if (!skim_ptr->Read (org, table, data)) {
				Error (String ("Reading Skim Group=%d, Distance Table=%d, Origin=%d") % number % table % org);
			}
		}
	} else {
		for (org=1; org <= nzones; org++) {
			Show_Progress ();

			table = itr->dist_table;
			table_ptr = &itr->distance [org-1];
			data = &table_ptr->at (0);

			if (!skim_ptr->Read (org, table, data)) {
				Error (String ("Reading Skim Group=%d, Distance Table=%d, Origin=%d") % number % table % org);
			}
			table = itr->time_table;
			table_ptr = &itr->time [org-1];
			data = &table_ptr->at (0);

			if (!skim_ptr->Read (org, table, data)) {
				Error (String ("Reading Skim Group=%d, Time Table=%d, Origin=%d") % number % table % org);
			}
		}
	}
	End_Progress ();
	skim_ptr->Close ();
}
