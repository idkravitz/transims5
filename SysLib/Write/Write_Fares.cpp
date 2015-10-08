//*********************************************************
//	Write_Fares.cpp - write a new transit fare file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Fares
//---------------------------------------------------------

void Data_Service::Write_Fares (void)
{
	Fare_File *file = (Fare_File *) System_File_Handle (NEW_TRANSIT_FARE);

	int count = 0;
	Fare_Map_Itr itr;
	Fare_Index *fare_ptr;
	file->Header_Lines (2);
	file->Write_Header (fare_map.Period_String ());

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = fare_map.begin (); itr != fare_map.end (); itr++) {
		Show_Progress ();
		fare_ptr = (Fare_Index *) &(itr->first);
		file->From_Zone (fare_ptr->From_Zone ());
		file->To_Zone (fare_ptr->To_Zone ());
		file->From_Mode (fare_ptr->From_Mode ());
		file->To_Mode (fare_ptr->To_Mode ());
		file->Period (fare_ptr->Period ());
		file->Class (fare_ptr->Class ());
		file->Fare (itr->second);

		if (!file->Write ()) {
			Error (String ("Writing %s") % file->File_Type ());
		}
		count++;
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}
