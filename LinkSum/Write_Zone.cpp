//*********************************************************
//	Write_Zone.cpp - Write the Zone Travel File
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Write_Zone
//---------------------------------------------------------

void LinkSum::Write_Zone (void)
{
	int i, j, nrec, zone, index;
	double factor;
	String buffer, base;
	Units_Type units;

	Int_Map_Itr itr;

	Show_Message (String ("Writing %s -- Record") % zone_file.File_Type ());
	Set_Progress ();

	if (Metric_Flag ()) {
		factor = 1.0 / 1000.0;
		base = "K";
		units = KILOMETERS;
	} else {
		factor = 1.0 / MILETOFEET;
		base = "M";
		units = MILES;
	}

	//---- set the data fields and file header ----

	zone_file.Add_Field ("ZONE", DB_INTEGER, 10);

	for (i=0; i < num_inc; i++) {
		buffer = base + sum_periods.Range_Label (i);

		zone_file.Add_Field (buffer, DB_DOUBLE, 16.2, units);

		buffer [0] = 'H';

		zone_file.Add_Field (buffer, DB_DOUBLE, 16.2, HOURS);
	}
	zone_file.Write_Header ();

	//---- process each zone ----

	nrec = 0;

	for (itr = zone_list.begin (); itr != zone_list.end (); itr++) {
		zone = itr->first;
		index = itr->second;

		zone_file.Put_Field (0, zone);

		for (i=0, j=1; i < num_inc; i++) {
			zone_file.Put_Field (j++, zone_vmt [i] [index] * factor);
			zone_file.Put_Field (j++, zone_vht [i] [index] / 3600.0);
		}
		if (!zone_file.Write ()) {
			Error (String ("Writing %s") % zone_file.File_Type ());
		}
		nrec++;
	}
	End_Progress ();

	Print (2, "Number of Zone Travel Records = ") << nrec;
}
