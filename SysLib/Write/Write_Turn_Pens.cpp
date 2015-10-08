//*********************************************************
//	Write_Turn_Pens.cpp - write a new turn penalty file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Turn_Pens
//---------------------------------------------------------

void Data_Service::Write_Turn_Pens (void)
{
	Turn_Pen_File *file = (Turn_Pen_File *) System_File_Handle (NEW_TURN_PENALTY);
	
	int dir, index, count;

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

			count += Put_Turn_Pen_Data (*file, dir_array [index]);
		}
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Turn_Pen_Data
//---------------------------------------------------------

int Data_Service::Put_Turn_Pen_Data (Turn_Pen_File &file, Dir_Data &data)
{
	int index, count;

	Turn_Pen_Data *turn_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Int_Map_Itr itr;
	Veh_Type_Data *veh_type_ptr;

	count = 0;

	link_ptr = &link_array [data.Link ()];

	file.Link (link_ptr->Link ());
	file.Dir (data.Dir ());

	for (index = data.First_Turn (); index >= 0; index = turn_ptr->Next_Index ()) {
		turn_ptr = &turn_pen_array [index];

		dir_ptr = &dir_array [turn_ptr->To_Index ()];

		link_ptr = &link_array [dir_ptr->Link ()];
		
		file.To_Link (link_ptr->Link ());

		file.Start (turn_ptr->Start ());
		file.End (turn_ptr->End ());
		file.Use (turn_ptr->Use ());
		file.Penalty (turn_ptr->Penalty ());

		if (turn_ptr->Min_Veh_Type () < 0) {
			file.Min_Veh_Type (0);
		} else if (veh_type_array.size () > 0) {
			veh_type_ptr = &veh_type_array [turn_ptr->Min_Veh_Type ()];
			file.Min_Veh_Type (veh_type_ptr->Type ());
		} else {
			file.Min_Veh_Type (turn_ptr->Min_Veh_Type ());
		}
		if (turn_ptr->Max_Veh_Type () < 0) {
			file.Max_Veh_Type (0);
		} else if (veh_type_array.size () > 0) {
			veh_type_ptr = &veh_type_array [turn_ptr->Max_Veh_Type ()];
			file.Max_Veh_Type (veh_type_ptr->Type ());
		} else {
			file.Max_Veh_Type (turn_ptr->Max_Veh_Type ());
		}
		file.Notes (turn_ptr->Notes ());

		if (!file.Write ()) {
			Error (String ("Writing %s") % file.File_Type ());
		}
		count++;
	}
	return (count);
}
