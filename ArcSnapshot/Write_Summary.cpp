//*********************************************************
//	Write_Summary.cpp - write link summary records
//*********************************************************

#include "ArcSnapshot.hpp"

//---------------------------------------------------------
//	Write_Summary
//---------------------------------------------------------

void ArcSnapshot::Write_Summary (int period)
{
	int i, index;
	Dtime low, high;

	Int_Map_Itr map_itr;
	Link_Data *link_ptr;

	sum_periods.Period_Range (period, low, high);

	summary_file.Put_Field (start_fld, low);
	summary_file.Put_Field (end_fld, high);

	for (map_itr = link_map.begin (); map_itr != link_map.end (); map_itr++) {
		index = map_itr->second;
		if (vehicles [0] [index] <= 0) continue;

		link_ptr = &link_array [index];

		summary_file.Put_Field (link_fld, link_ptr->Link ());

		for (i=0; i < max_type; i++) {
			if (i > 0 && select_vehicles && !vehicle_range.In_Range (i)) continue;

			summary_file.Put_Field (veh_fld [i], vehicles [i] [index]);
			summary_file.Put_Field (per_fld [i], persons [i] [index]);
		}
		if (!summary_file.Write ()) {
			Error (String ("Writing %s") % summary_file.File_Type ());
		}
		num_sum++;
	}
}
