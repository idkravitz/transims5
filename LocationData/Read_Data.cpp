//*********************************************************
//	Read_Data.cpp - read the data file into memory
//*********************************************************

#include "LocationData.hpp"

//---------------------------------------------------------
//	Read_Data
//---------------------------------------------------------

void LocationData::Read_Data (void)
{
	int id, field, count;

	Db_Sort_Array *data;
	Db_Header *file;
	Data_Itr data_itr;

	for (data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {
		file = data_itr->file;
		data = data_itr->data_db;
		field = data_itr->join_field;

		//---- read the data file ----

		Show_Message (String ("Reading %s -- Record") % file->File_Type ());
		Set_Progress ();

		count = 0;

		while (file->Read ()) {
			Show_Progress ();

			//---- get the join field id ----

			id = file->Get_Integer (field);
			if (id == 0) continue;

			//---- copy the data fields ----

			data->Copy_Fields (*file);

			//---- set the record index and location count ----

			data->Put_Field (0, id);
			data->Put_Field (1, 0);

			//---- save the database record ----

			if (!data->Write_Record (id)) {
				Error ("Writing Data File Database");
			}
			count++;
		}
		End_Progress ();

		Print (2, String ("Number of Data #%d Records = %d") % data_itr->group % count);
		
		file->Close ();
	}
}
