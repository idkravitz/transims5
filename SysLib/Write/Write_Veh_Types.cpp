//*********************************************************
//	Write_Veh_Types.cpp - write a new vehicle type file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Veh_Types
//---------------------------------------------------------

void Data_Service::Write_Veh_Types (void)
{
	Veh_Type_File *file = (Veh_Type_File *) System_File_Handle (NEW_VEHICLE_TYPE);

	int count = 0;
	Int_Map_Itr itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = veh_type_map.begin (); itr != veh_type_map.end (); itr++) {
		Show_Progress ();

		count += Put_Veh_Type_Data (*file, veh_type_array [itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Veh_Type_Data
//---------------------------------------------------------

int Data_Service::Put_Veh_Type_Data (Veh_Type_File &file, Veh_Type_Data &data)
{
	file.Type (data.Type ());
	file.Length (UnRound (data.Length ()));
	file.Max_Speed (UnRound (data.Max_Speed ()));
	file.Max_Accel (UnRound (data.Max_Accel ()));
	file.Max_Decel (UnRound (data.Max_Decel ()));
	file.Op_Cost (UnRound (data.Op_Cost ()));
	file.Use (data.Use ());
	file.Capacity (data.Capacity ());
	file.Loading (UnRound (data.Loading ()));
	file.Unloading (UnRound (data.Unloading ()));
	file.Method (data.Method ());
	file.Min_Dwell (data.Min_Dwell ());
	file.Max_Dwell (data.Max_Dwell ());
	
	for (int i=0; i < 10; i++) {
		file.Grade (i, data.Grade (i));
	}
	file.Notes (data.Notes ());

	if (!file.Write ()) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	return (1);
}
