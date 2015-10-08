//*********************************************************
//	Lane_Use.cpp - process lane use records
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Lane_Use
//---------------------------------------------------------

void TransimsNet::Lane_Use (void)
{
	int i, num;
	Dtime start, end;

	//---- get the pocket length ----

	Link_Detail_Map_Itr detail_itr;
	Dir_Data *dir_ptr;
	Lane_Use_Data lane_use_rec, *lane_use_ptr;

	Show_Message (String ("Creating Lane Use Data -- Record"));
	Set_Progress ();

	for (detail_itr = detail_map.begin (); detail_itr != detail_map.end (); detail_itr++) {

		if (Use_Permission (detail_itr->second.Use (), ANY)) continue;
		num = detail_itr->second.Period ()->Num_Periods ();

		lane_use_rec.Dir_Index (detail_itr->first);
		lane_use_rec.Low_Lane (detail_itr->second.Low_Lane ());
		lane_use_rec.High_Lane (detail_itr->second.High_Lane ());

		if (Use_Permission (detail_itr->second.Use (), NONE)) {
			lane_use_rec.Type (PROHIBIT);
			lane_use_rec.Use (ANY);
		} else {
			lane_use_rec.Type (LIMIT);
			lane_use_rec.Use (detail_itr->second.Use ());
		}
		for (i=0; i < num; i++) {
			detail_itr->second.Period ()->Period_Range (i, start, end);

			lane_use_rec.Start (start);
			lane_use_rec.End (end);

			lane_use_array.push_back (lane_use_rec);			
		}
	}
	num = (int) lane_use_array.size ();

	if (num > 0) {
		while (num--) {
			lane_use_ptr = &lane_use_array [num];
			dir_ptr = &dir_array [lane_use_ptr->Dir_Index ()];

			lane_use_ptr->Next_Index (dir_ptr->First_Lane_Use ());
			dir_ptr->First_Lane_Use (num);
		}
	}
} 
