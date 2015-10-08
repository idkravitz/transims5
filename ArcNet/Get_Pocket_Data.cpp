//*********************************************************
//	Get_Pocket_Data.cpp - additional pocket lane processing
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Initialize_Pockets
//---------------------------------------------------------

void ArcNet::Initialize_Pockets (Pocket_File &file)
{
	Data_Service::Initialize_Pockets (file);

	if (pocket_db.capacity () == 0) {
		pocket_db.reserve (pocket_array.capacity ());
	}
}

//---------------------------------------------------------
//	Get_Pocket_Data
//---------------------------------------------------------

bool ArcNet::Get_Pocket_Data (Pocket_File &file, Pocket_Data &pocket_rec)
{
	if (Data_Service::Get_Pocket_Data (file, pocket_rec)) {

		pocket_db.Copy_Fields (file);
		pocket_db.Put_Field ("POCKET_INDEX", (int) pocket_array.size ());

		if (!pocket_db.Add_Record ()) {
			Error ("Writing Pocket Database");
		}
		return (true);
	}
	return (false);
}
