//*********************************************************
//	Read_Signs.cpp - read the sign file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Signs
//---------------------------------------------------------

void Data_Service::Read_Signs (void)
{
	Sign_File *file = (Sign_File *) System_File_Handle (SIGN);

	int count, node;
	Sign_Data sign_rec;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Dir_Data *dir_ptr;

	//---- store the turn data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();
	
	Initialize_Signs (*file);
	count = 0;

	while (file->Read ()) {
		Show_Progress ();

		sign_rec.Clear ();

		if (Get_Sign_Data (*file, sign_rec)) {
			dir_ptr = &dir_array [sign_rec.Dir_Index ()];
			link_ptr = &link_array [dir_ptr->Link ()];

			if (dir_ptr->Sign () > 0) {
				Warning ("Duplicate Sign Record on Link ") << link_ptr->Link ();
				continue;
			}
			if (dir_ptr->Dir () == 0) {
				node = link_ptr->Bnode ();
			} else {
				node = link_ptr->Anode ();
			}
			node_ptr = &node_array [node];

			if (node_ptr->Control () >= 0) {
				Warning (String ("Node %d has a Sign/Signal control conflict") % node_ptr->Node ());
				continue;
			}
			node_ptr->Control (-2);
			dir_ptr->Sign (sign_rec.Sign ());
			count++;
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	if (count && count != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % count);
	}
	if (count > 0) System_Data_True (SIGN);
}

//---------------------------------------------------------
//	Initialize_Signs
//---------------------------------------------------------

void Data_Service::Initialize_Signs (Sign_File &file)
{
	Required_File_Check (file, LINK);
}

//---------------------------------------------------------
//	Get_Sign_Data
//---------------------------------------------------------

bool Data_Service::Get_Sign_Data (Sign_File &file, Sign_Data &sign_rec)
{
	int link, dir, dir_index;
	Link_Data *link_ptr;

	//---- sign code ----

	sign_rec.Sign (file.Sign ());

	//---- don't keep unsigned approaches ----

	if (sign_rec.Sign () == NO_CONTROL || sign_rec.Sign () == SIGNAL_GROUP) return (false);

	//---- check/convert the link number ----

	link = file.Link ();
	dir = file.Dir ();

	link_ptr = Set_Link_Direction (file, link, dir);

	if (link_ptr == 0) return (false);

	if (dir) {
		dir_index = link_ptr->BA_Dir ();
	} else {
		dir_index = link_ptr->AB_Dir ();
	}
	if (dir_index < 0) {
		Warning (String ("Sign %d Link %d Direction %s was Not Found") % Progress_Count () % 
			link_ptr->Link () % ((dir) ? "BA" : "AB"));
		return (false);
	}
	sign_rec.Dir_Index (dir_index);

	sign_rec.Notes (file.Notes ());

	return (true);
}
