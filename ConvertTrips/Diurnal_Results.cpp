//*********************************************************
//	Diurnal_Results.cpp - Output the Diurnal Allocations
//*********************************************************

#include "ConvertTrips.hpp"

#include "fstream"

//---------------------------------------------------------
//	Diurnal_Results
//---------------------------------------------------------

void ConvertTrips::Diurnal_Results (void)
{
	int i, num, p, periods;

	Share_Data *share_ptr;
	File_Group_Itr group_itr;

	fstream &fh = diurnal_file.File ();

	fh << "MINUTE";
	periods = 0;

	for (group_itr = file_group.begin (); group_itr != file_group.end (); group_itr++) {
		num = group_itr->Num_Shares ();
		if (num < 1) num = 1;

		if (group_itr->Diurnal_Periods () > periods) periods = group_itr->Diurnal_Periods ();

		for (i=1; i <= num; i++) {
			fh << "\tSHARE" << group_itr->Group () << "_" << i;
			fh << "\tTARGET" << group_itr->Group () << "_" << i;
			fh << "\tTRIPS" << group_itr->Group () << "_" << i;
		}
	}
	fh << endl;

	//---- print the data ----

	for (p=0; p < periods; p++) {
		fh << (p+1);

		for (group_itr = file_group.begin (); group_itr != file_group.end (); group_itr++) {
			num = group_itr->Num_Shares ();
			if (num < 1) num = 1;

			for (i=0; i < num; i++) {
				share_ptr = group_itr->Share_Ptr (p, i);

				fh << "\t" << share_ptr->Share ();
				fh << "\t" << share_ptr->Target ();
				fh << "\t" << share_ptr->Trips ();
			}
		}
		fh << endl;
	}
	diurnal_file.Close ();
}
