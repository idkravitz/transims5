//*********************************************************
//	Read_Pockets.cpp - read the pocket lane file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Pockets
//---------------------------------------------------------

void Data_Service::Read_Pockets (void)
{
	Pocket_File *file = (Pocket_File *) System_File_Handle (POCKET);

	int num;
	Pocket_Data pocket_rec;

	//---- store the pocket data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();
	
	Initialize_Pockets (*file);

	while (file->Read ()) {
		Show_Progress ();

		pocket_rec.Clear ();

		if (Get_Pocket_Data (*file, pocket_rec)) {
			pocket_array.push_back (pocket_rec);
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	num = (int) pocket_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) {
		System_Data_True (POCKET);

		Pocket_Data *pocket_ptr;
		Dir_Data *dir_ptr;

		//---- create directional link list ---- 

		while (num--) {
			pocket_ptr = &pocket_array [num];
			dir_ptr = &dir_array [pocket_ptr->Dir_Index ()];

			pocket_ptr->Next_Index (dir_ptr->First_Pocket ());
			dir_ptr->First_Pocket (num);

			if (pocket_ptr->Type () == LEFT_TURN || pocket_ptr->Type () == LEFT_MERGE) {
				if (dir_ptr->Left () < pocket_ptr->Lanes ()) {
					dir_ptr->Left (pocket_ptr->Lanes ());
				}
			} else if (dir_ptr->Right () < pocket_ptr->Lanes ()) {
				dir_ptr->Right (pocket_ptr->Lanes ());
			}
		}
	}
}

//---------------------------------------------------------
//	Initialize_Pockets
//---------------------------------------------------------

void Data_Service::Initialize_Pockets (Pocket_File &file)
{
	Required_File_Check (file, LINK);
	
	int percent = System_Data_Reserve (POCKET);
	
	if (pocket_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		} else if (file.Version () <= 40) {
			num = (int) (num / 1.5);
		}
		if (num > 1) {
			pocket_array.reserve (num);
			if (num > (int) pocket_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Pocket_Data
//---------------------------------------------------------

bool Data_Service::Get_Pocket_Data (Pocket_File &file, Pocket_Data &pocket_rec)
{
	int lanes, link, dir, offset, dir_index;

	Link_Data *link_ptr;

	//---- check/convert the link number and direction ----
	
	link = file.Link ();
	dir = file.Dir ();
	offset = Round (file.Offset ());

	link_ptr = Set_Link_Direction (file, link, dir, offset);

	if (link_ptr == 0) return (false);

	if (dir) {
		dir_index = link_ptr->BA_Dir ();
	} else {
		dir_index = link_ptr->AB_Dir ();
	}
	if (dir_index < 0) {
		Warning (String ("Pocket %d Link %d Direction %s was Not Found") % Progress_Count () % 
			link_ptr->Link () % ((dir) ? "BA" : "AB"));
		return (false);
	}
	pocket_rec.Dir_Index (dir_index);

	//---- set the pocket type ----
	
	pocket_rec.Type (file.Type ());

	//----- pocket length and offset ----

	pocket_rec.Length (file.Length ());
	pocket_rec.Offset (offset);

	//---- lane number ----

	lanes = file.Lanes ();
	if (lanes < 1) {
		Warning (String ("Pocket %d Link %d Lanes %d are Out of Range") % Progress_Count () % 
			link_ptr->Link () % lanes);
		return (false);
	}
	if (file.Version () <= 40) {
		Dir_Data *dir_ptr;

		if (dir) {
			dir_ptr = &dir_array [dir_index];
		} else {
			dir_ptr = &dir_array [dir_index];
		}
		if (lanes <= dir_ptr->Left ()) {
			if (pocket_rec.Type () == RIGHT_MERGE) {
				pocket_rec.Type (LEFT_MERGE);
			}
		} else {
			if (pocket_rec.Type () == LEFT_TURN) {
				pocket_rec.Type (RIGHT_TURN);
			}
		}
		if (lanes <= dir_ptr->Left ()) {
			lanes = dir_ptr->Left () - lanes + 1;
		} else {
			lanes = lanes - dir_ptr->Lanes () - dir_ptr->Left ();
		}
		if (pocket_rec.Type () == POCKET_LANE) {
			pocket_rec.Offset (link_ptr->Length () - pocket_rec.Offset ());
		}
	}
	if (pocket_rec.Type () == LEFT_TURN || pocket_rec.Type () == RIGHT_TURN) {
		offset = (dir == 0) ? link_ptr->Boffset () : link_ptr->Aoffset ();
		pocket_rec.Length (pocket_rec.Length () + offset);
		pocket_rec.Offset (link_ptr->Length () - pocket_rec.Length ());
		if (pocket_rec.Offset () < 0) {
			Warning (String ("Link %d Pocket Length %.1lf is Too Long") % link_ptr->Link () % UnRound (pocket_rec.Length ()));
			pocket_rec.Offset (0);
		}
	} else if (pocket_rec.Type () == RIGHT_MERGE || pocket_rec.Type () == LEFT_MERGE) {
		offset = (dir == 0) ? link_ptr->Aoffset () : link_ptr->Boffset ();
		pocket_rec.Offset (0);
		pocket_rec.Length (pocket_rec.Length () + offset);
		if (pocket_rec.Length () > link_ptr->Length ()) {
			Warning (String ("Link %d Pocket Length %.1lf is Too Long") % link_ptr->Link () % UnRound (pocket_rec.Length ()));
			pocket_rec.Length (link_ptr->Length ());
		}
	} else if (pocket_rec.Type () == AUX_LANE) {
		pocket_rec.Offset (0);
		pocket_rec.Length (link_ptr->Length ());
	}
	pocket_rec.Lanes (lanes);
	pocket_rec.Notes (file.Notes ());

	return (true);
}
