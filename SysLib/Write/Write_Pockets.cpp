//*********************************************************
//	Write_Pockets.cpp - write a new pocket file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Pockets
//---------------------------------------------------------

void Data_Service::Write_Pockets (void)
{
	int dir, count, index;

	Pocket_File *file = (Pocket_File *) System_File_Handle (NEW_POCKET);

	Link_Data *link_ptr;
	Int_Map_Itr itr;

	count = 0;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = link_map.begin (); itr != link_map.end (); itr++) {
		link_ptr = &link_array [itr->second];

		for (dir=0; dir < 2; dir++) {
			index = (dir) ? link_ptr->BA_Dir () : link_ptr->AB_Dir ();
			if (index < 0) continue;

			Show_Progress ();

			count += Put_Pocket_Data (*file, dir_array [index]);
		}
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Pocket_Data
//---------------------------------------------------------

int Data_Service::Put_Pocket_Data (Pocket_File &file, Dir_Data &data)
{
	int count, index, length, offset;

	Pocket_Data *pocket_ptr;
	Link_Data *link_ptr;
	Int_Map_Itr itr;

	count = 0;

	link_ptr = &link_array [data.Link ()];

	file.Link (link_ptr->Link ());
	file.Dir (data.Dir ());

	for (index = data.First_Pocket (); index >= 0; index = pocket_ptr->Next_Index ()) {
		pocket_ptr = &pocket_array [index];

		file.Type (pocket_ptr->Type ());
		file.Lanes (pocket_ptr->Lanes ());

		length = pocket_ptr->Length ();
		offset = pocket_ptr->Offset ();

		if (pocket_ptr->Type () == RIGHT_TURN || pocket_ptr->Type () == LEFT_TURN) {
			offset = 0;
			length -= (data.Dir () == 0) ? link_ptr->Boffset () : link_ptr->Aoffset ();
		} else if (pocket_ptr->Type () == RIGHT_MERGE || pocket_ptr->Type () == LEFT_MERGE) {
			offset = 0;
			length -= (data.Dir () == 0) ? link_ptr->Aoffset () : link_ptr->Boffset ();
		} else if (pocket_ptr->Type () == AUX_LANE) {
			offset = length = 0;
		}
		file.Length (UnRound (length));
		file.Offset (UnRound (offset));
		file.Notes (pocket_ptr->Notes ());

		if (!file.Write ()) {
			Error (String ("Writing %s") % file.File_Type ());
		}
		count++;
	}
	return (count);
}
