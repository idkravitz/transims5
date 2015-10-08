//*********************************************************
//	Write_Group.cpp - Write the Zone Group Travel File
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Write_Group
//---------------------------------------------------------

void LinkSum::Write_Group (void)
{
	int i, j, nrec, group, index;
	String buffer, base;
	double factor;
	Units_Type units;

	Doubles vmt, vht;
	Int_Set *set_ptr;
	Int_Set_Itr itr;
	Int_Map_Itr map_itr;

	Show_Message (String ("Writing %s -- Record") % group_file.File_Type ());
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

	group_file.Add_Field ("GROUP", DB_INTEGER, 10);

	for (i=0; i < num_inc; i++) {
		buffer = base + sum_periods.Range_Label (i);

		group_file.Add_Field (buffer, DB_DOUBLE, 16.2, units);

		buffer [0] = 'H';

		group_file.Add_Field (buffer, DB_DOUBLE, 16.2, HOURS);
	}
	group_file.Write_Header ();

	//---- process each group ----

	nrec = 0;

	for (group=1; group <= zone_equiv.Num_Groups (); group++) {
		Show_Progress ();

		set_ptr = zone_equiv.Group_List (group);
		if (set_ptr == 0) continue;

		vmt.assign (num_inc, 0.0);
		vht.assign (num_inc, 0.0);

		//---- process each zone in the group ----

		for (itr = set_ptr->begin (); itr != set_ptr->end (); itr++) {
			map_itr = zone_list.find (*itr);
			if (map_itr == zone_list.end ()) continue;

			index = map_itr->second;

			for (i=0; i < num_inc; i++) {
				vmt [i] += zone_vmt [i] [index];
				vht [i] += zone_vht [i] [index];
			}
		}

		//---- save the group data ----

		group_file.Put_Field (0, group);

		for (i=0, j=1; i < num_inc; i++) {
			group_file.Put_Field (j++, vmt [i] * factor);
			group_file.Put_Field (j++, vht [i] / 3600.0);
		}
		if (!group_file.Write ()) {
			Error (String ("Writing %s") % group_file.File_Type ());
		}
		nrec++;
	}
	End_Progress ();

	Print (2, "Number of Group Travel Records = ") << nrec;
}
