//*********************************************************
//	Write_Parking - draw parking lots demands
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Write_Parking
//---------------------------------------------------------

void ArcPlan::Write_Parking (void)
{
	int index, in, out, park_field, out_field, in_field, tot_field;

	Int_Map_Itr map_itr;
	Point_Map_Itr pt_itr;

	park_field = arcview_parking.Field_Number ("PARKING");
	out_field = arcview_parking.Field_Number ("DEPART");
	in_field = arcview_parking.Field_Number ("ARRIVE");
	tot_field = arcview_parking.Field_Number ("TOTAL");

	arcview_parking.clear ();

	Show_Message (String ("Writing %s -- Record") % arcview_parking.File_Type ());
	Set_Progress ();

	//---- process each parking lot ----

	for (pt_itr = parking_pt.begin (); pt_itr != parking_pt.end (); pt_itr++) {
		Show_Progress ();

		map_itr = parking_map.find (pt_itr->first);
		if (map_itr == parking_map.end ()) continue;

		index = map_itr->second;

		out = parking_out [index];
		in = parking_in [index];

		if (in <= 0 && out <= 0) continue;

		//---- write the parking record ----

		arcview_parking.Put_Field (park_field, pt_itr->first);
		arcview_parking.Put_Field (out_field, out);
		arcview_parking.Put_Field (in_field, in);
		arcview_parking.Put_Field (tot_field, (out + in));

		arcview_parking.assign (1, pt_itr->second);

		if (!arcview_parking.Write_Record ()) {
			Error (String ("Writing %s") % arcview_parking.File_Type ());
		}
		num_parking++;
	}
	End_Progress ();

	arcview_parking.Close ();
}
