//*********************************************************
//	Get_Access_Data.cpp - additional access processing
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Get_Access_Data
//---------------------------------------------------------

bool ArcNet::Get_Access_Data (Access_File &file, Access_Data &access_rec)
{
	if (arcview_access.Is_Open ()) {
		int i, id, type;

		XYZ_Point point;
		Point_Map_Itr map_itr;

		access_rec.From_ID (0);

		arcview_access.clear ();
		arcview_access.Copy_Fields (file);

		for (i=0; i < 2; i++) {
			if (i) {
				type = file.To_Type ();;
				id = file.To_ID ();
			} else {
				type = file.From_Type ();;
				id = file.From_ID ();
			}
			if (type == LOCATION_ID) {
				map_itr = location_pt.find (id);
				if (map_itr != location_pt.end ()) point = map_itr->second;
			} else if (type == PARKING_ID) {
				map_itr = parking_pt.find (id);
				if (map_itr != parking_pt.end ()) point = map_itr->second;
			} else if (type == STOP_ID) {
				if (!System_File_Flag (TRANSIT_STOP)) return (false);
				map_itr = stop_pt.find (id);
				if (map_itr != stop_pt.end ()) point = map_itr->second;
			} else if (type == NODE_ID) {
				Int_Map_Itr itr = node_map.find (id);
				if (itr != node_map.end ()) {
					Node_Data *node_ptr = &node_array [itr->second];

					point.x = UnRound (node_ptr->X ());
					point.y = UnRound (node_ptr->Y ());
					point.z = UnRound (node_ptr->Z ());
				}
			} else {
				return (false);
			}
			arcview_access.push_back (point);
		}
		if (!arcview_access.Write_Record ()) {
			Error (String ("Writing %s") % arcview_access.File_Type ());
		}
	}
	return (false);
}
