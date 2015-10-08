//*********************************************************
//	Put_Shape_Data.cpp - write a new shape file
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Put_Shape_Data
//---------------------------------------------------------

int NetPrep::Put_Shape_Data (Shape_File &file, Shape_Data &data)
{
	if (drop_flag) {
		Int_Map_Itr map_itr = link_map.find (data.Link ());

		if (map_itr == link_map.end ()) return (0);

		Link_Data *link_ptr = &link_array [map_itr->second];

		if (link_ptr->Length () == 0) return (0);
	}
	return (Data_Service::Put_Shape_Data (file, data));
}
