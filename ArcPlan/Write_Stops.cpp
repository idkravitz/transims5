//*********************************************************
//	Write_Stops - draw transit boardings and alightings
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Write_Stops
//---------------------------------------------------------

void ArcPlan::Write_Stops (void)
{
	int index, board, alight, stop_field, board_field, alight_field, total_field;

	Int_Map_Itr map_itr;
	Point_Map_Itr pt_itr;

	stop_field = arcview_stop.Field_Number ("STOP");
	board_field = arcview_stop.Field_Number ("BOARD");
	alight_field = arcview_stop.Field_Number ("ALIGHT");
	total_field = arcview_stop.Field_Number ("TOTAL");

	arcview_stop.clear ();

	Show_Message (String ("Writing %s -- Record") % arcview_stop.File_Type ());
	Set_Progress ();

	//---- process each stop ----

	for (pt_itr = stop_pt.begin (); pt_itr != stop_pt.end (); pt_itr++) {
		Show_Progress ();

		map_itr = stop_map.find (pt_itr->first);
		if (map_itr == stop_map.end ()) continue;

		index = map_itr->second;

		board = board_data [index];
		alight = alight_data [index];

		if (board <= 0 && alight <= 0) continue;

		//---- write the stop record ----

		arcview_stop.Put_Field (stop_field, pt_itr->first);
		arcview_stop.Put_Field (board_field, board);
		arcview_stop.Put_Field (alight_field, alight);
		arcview_stop.Put_Field (total_field, (board + alight));

		arcview_stop.assign (1, pt_itr->second);

		if (!arcview_stop.Write_Record ()) {
			Error (String ("Writing %s") % arcview_stop.File_Type ());
		}
		num_stop++;
	}
	End_Progress ();

	arcview_stop.Close ();
}
