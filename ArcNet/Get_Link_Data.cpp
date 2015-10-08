//*********************************************************
//	Get_Link_Data.cpp - additonal link processing
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Initialize_Links
//---------------------------------------------------------

void ArcNet::Initialize_Links (Link_File &file)
{
	Data_Service::Initialize_Links (file);
		
	if (arcview_link.Is_Open () || arcview_center.Is_Open ()) {
		if (link_db.capacity () == 0) {
			link_db.reserve (link_array.capacity ());
		}
	}
}

//---------------------------------------------------------
//	Get_Link_Data
//---------------------------------------------------------

bool ArcNet::Get_Link_Data (Link_File &file, Link_Data &data, Dir_Data &ab, Dir_Data &ba)
{
	if (Data_Service::Get_Link_Data (file, data, ab, ba)) {
		if (arcview_link.Is_Open () || arcview_center.Is_Open ()) {
			link_db.Copy_Fields (file);

			if (!link_db.Add_Record ()) {
				Error ("Writing Link Database");
			}
		}
		return (true);
	}
	return (false);
}
