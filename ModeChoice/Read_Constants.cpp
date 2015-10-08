//*********************************************************
//	Read_Constants.cpp - read the mode constants file
//*********************************************************

#include "ModeChoice.hpp"

//---------------------------------------------------------
//	Read_Constants
//---------------------------------------------------------

void ModeChoice::Read_Constants (void)
{
	int i, mode, segment, tab, mode_field, segment_field, const_field;
	String text;
	double constant;
	bool table_flag;
	Str_ID_Itr id_itr;

	Show_Message (String ("Reading %s -- Record") % constant_file.File_Type ());
	Set_Progress ();

	//---- identify the data fields ----

	mode_field = constant_file.Optional_Field ("MODE", "M", "MOD", "MODES");
	segment_field = constant_file.Optional_Field ("SEGMENT", "SEG", "MARKET", "S");
	const_field = constant_file.Optional_Field ("CONSTANT", "CONST");

	if (mode_field < 0 || const_field < 0) {
		Error ("Mode Constannt Field Names are Not Defined");
	}
	text = trip_file->Field (table_map [0])->Name ();

	table_flag = (constant_file.Optional_Field (text.c_str ()) >= 0);

	//---- initialize the model constants ----

	while (constant_file.Read ()) {
		Show_Progress ();

		text = constant_file.Get_String (mode_field);
		if (text.empty ()) continue;

		id_itr = mode_id.find (text);
		if (id_itr == mode_id.end ()) {
			Warning ("Constant Mode ") << text << " was Not Defined";
		}
		mode = id_itr->second;

		segment = constant_file.Get_Integer (segment_field);

		if (!segment_flag && segment != 0) {
			Error ("A Segment Map File is needed for Segment Constants");
		} else if (segment < 0 || segment > num_market) {
			Error (String ("Segment %d is Out of Range (1..%d)") % segment % num_market);
		}
		constant = constant_file.Get_Double (const_field);

		if (segment_flag && segment == 0) {
			for (i=0; i <= num_market; i++) {
				seg_constant [i] [mode] [num_tables] = constant;
			}
		} else {
			seg_constant [segment] [mode] [num_tables] = constant;
		}

		if (table_flag) {
			for (tab=0; tab < num_tables; tab++) {
				text = trip_file->Field (table_map [tab])->Name ();

				constant = constant_file.Get_Double (text.c_str ());

				if (segment_flag && segment == 0) {
					for (i=0; i <= num_market; i++) {
						seg_constant [i] [mode] [tab] = constant;
					}
				} else {
					seg_constant [segment] [mode] [tab] = constant;
				}
			}
		}
	}
	End_Progress ();
	constant_file.Close ();
}
