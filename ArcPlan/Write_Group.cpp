//*********************************************************
//	Write_Group - draw stop group boardings and alightings
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Write_Group
//---------------------------------------------------------

void ArcPlan::Write_Group (void)
{
	int i, num, board, alight, max_group;
	int group_field, board_field, alight_field, total_field, name_field;

	XYZ_Point pt;

	Int_Map_Itr map_itr;
	Point_Map_Itr pt_itr;
	Int_Set *group;
	Int_Set_Itr itr;

	group_field = arcview_group.Field_Number ("GROUP");
	name_field = arcview_group.Field_Number ("NAME");
	board_field = arcview_group.Field_Number ("BOARD");
	alight_field = arcview_group.Field_Number ("ALIGHT");
	total_field = arcview_group.Field_Number ("TOTAL");

	arcview_group.clear ();

	Show_Message (String ("Writing %s -- Record") % arcview_group.File_Type ());
	Set_Progress ();

	//---- process each stop group ----

	max_group = stop_equiv.Max_Group ();

	for (i=1; i <= max_group; i++) {

		group = stop_equiv.Group_List (i);
		if (group == 0) continue;

		Show_Progress ();

		board = alight = num = 0;
		pt.x = pt.y = pt.z = 0.0;

		//---- sum the stops in the group ----

		for (itr = group->begin (); itr != group->end (); itr++) {

			pt_itr = stop_pt.find (*itr);
			if (pt_itr == stop_pt.end ()) continue;
			
			map_itr = stop_map.find (pt_itr->first);
			if (map_itr == stop_map.end ()) continue;

			pt.x += pt_itr->second.x;
			pt.y += pt_itr->second.y;
			pt.z += pt_itr->second.z;

			board += board_data [map_itr->second];
			alight += alight_data [map_itr->second];
			num++;
		}
		if (board <= 0 && alight <= 0) continue;

		//---- write the stop record ----

		pt.x = pt.x / num;
		pt.y = pt.y / num;
		pt.z = pt.z / num;

		arcview_group.Put_Field (group_field, i);
		arcview_group.Put_Field (name_field, stop_equiv.Group_Label (i));
		arcview_group.Put_Field (board_field, board);
		arcview_group.Put_Field (alight_field, alight);
		arcview_group.Put_Field (total_field, (board + alight));

		arcview_group.assign (1, pt);

		if (!arcview_group.Write_Record ()) {
			Error (String ("Writing %s") % arcview_group.File_Type ());
		}
		num_group++;
	}
	End_Progress ();

	arcview_group.Close ();
}
