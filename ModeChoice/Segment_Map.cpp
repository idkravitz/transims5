//*********************************************************
//	Segment_Map.cpp - build the segment map
//*********************************************************

#include "ModeChoice.hpp"

//---------------------------------------------------------
//	Segment_Map
//---------------------------------------------------------

void ModeChoice::Segment_Map (void)
{
	int org, des, segment, max_org, max_des, count, org_field, des_field, seg_field;
	bool first;
	Int2_Map_Stat map_stat;

	Show_Message (String ("Reading %s -- Record") % segment_file.File_Type ());
	Set_Progress ();

	max_org = max_des = num_market = 0;
	org_field = segment_file.Optional_Field ("ORIGIN", "ORG", "O", "I");
	des_field = segment_file.Optional_Field ("DESTINATION", "DES", "D", "J");
	seg_field = segment_file.Optional_Field ("SEGMENT", "SEG", "MARKET", "S");

	if (org_field < 0) org_field = 0;
	if (des_field < 0) des_field = 1;
	if (seg_field < 0) seg_field = 2;

	while (segment_file.Read ()) {
		Show_Progress ();

		org = segment_file.Get_Integer (org_field);
		if (org <= 0) continue;
		if (org > max_org) max_org = org;

		des = segment_file.Get_Integer (des_field);
		if (des <= 0) continue;
		if (des > max_des) max_des = des;

		segment = segment_file.Get_Integer (seg_field);
		if (segment <= 0) continue;
		if (segment > num_market) num_market = segment;

		map_stat = segment_map.insert (Int2_Map_Data (Int2_Key (org, des), segment));

		if (!map_stat.second) {
			Warning ("Duplicate Segment Map between ") << org << " and " << des;
		}
	}
	End_Progress ();
	segment_file.Close ();

	Print (1);
	Write (1, "Number of Market Segments = ") << num_market;
	Show_Message (1);

	//---- scan for missing interchanges ----

	first = true;
	count = 0;

	for (org=1; org <= max_org; org++) {
		for (des=1; des <= max_des; des++) {
			if (segment_map.find (Int2_Key (org, des)) == segment_map.end ()) {
				if (first) {
					first = false;
					Warning ("Missing Segment Interchanges:");
				}
				if ((++count % 10) == 1) {
					Write (1, "\t") << org << "-" << des;
				} else {
					Write (0, ", ") << org << "-" << des;
				}
			}
		}
	}
	if (count > 0) {
		Write (2, "\tNumber of Missing Interchanges = ") << count;
		Show_Message (1);
	}
}
