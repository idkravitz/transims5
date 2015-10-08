//*********************************************************
//	Set_Files.cpp - Setup Data Files for User Program
//*********************************************************

#include "ZoneData.hpp"

//---------------------------------------------------------
//	Set_Files
//---------------------------------------------------------

void ZoneData::Set_Files (void)
{
	int i, nfld, key;
	bool binary;

	Db_Field *fld;
	Db_Base *file;
	Db_Sort_Array *data;
	Data_Itr data_itr;
	Polygon_Itr poly_itr;
	Sum_Itr sum_itr;

	//---- create the program data structure ----

	data_rec.push_back (input_file);
	data_rec.push_back (output_file);

	//---- convert each data file to binary ----

	for (data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {
		file = data_itr->file;
		data = data_itr->data_db;

		data_rec.push_back ((Db_Base *) data);

		data->File_ID (file->File_ID ());
		data->File_Type (file->File_Type ());

		nfld = file->Num_Fields ();
		key = data_itr->join_field;
	
		binary = (file->Record_Format () == BINARY);

		fld = file->Field (key);

		data->Add_Field (fld->Name (), DB_INTEGER, 10);
		data->Add_Field ("Z_COUNT", DB_INTEGER, 10);

		for (i=0; i < nfld; i++) {
			if (i == key) continue;
			fld = file->Field (i);
			if (fld != 0) {
				data->Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Units (), binary);
			}
		}
	}

	//---- read the arcview boundary file ----

	for (poly_itr = polygons.begin (); poly_itr != polygons.end (); poly_itr++) {
		file = poly_itr->file;
		data = poly_itr->data_db;

		data_rec.push_back ((Db_Base *) data);

		data->File_ID (file->File_ID ());
		data->File_Type (file->File_Type ());

		nfld = file->Num_Fields ();
		binary = (file->Record_Format () == BINARY);

		data->Add_Field ("INDEX", DB_INTEGER, 10);

		for (i=0; i < nfld; i++) {
			fld = file->Field (i);
			if (fld != 0) {
				data->Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Units (), binary);
			}
		}
	}

	//---- add the sum distance databases ----

	for (sum_itr = sum_group.begin (); sum_itr != sum_group.end (); sum_itr++) {
		data_rec.push_back ((Db_Base *) sum_itr->data_db);
	}

	Write (1, "Compiling Conversion Script");

	if (Report_Flag (PRINT_SCRIPT)) {
		Header_Number (PRINT_SCRIPT);

		if (!Break_Check (10)) {
			Print (1);
			Page_Header ();
		}
	}
	program.Initialize (data_rec, random.Seed () + 1);

	if (!program.Compile (program_file, Report_Flag (PRINT_SCRIPT))) {
		Error ("Compiling Conversion Script");
	}
	if (Report_Flag (PRINT_STACK)) {
		Header_Number (PRINT_STACK);

		program.Print_Commands (false);
	}
	Header_Number (0);
} 
