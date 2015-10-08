//********************************************************* 
//	Link_Data_File.cpp - Link Data File Input/Output
//*********************************************************

#include "Link_Data_File.hpp"

//-----------------------------------------------------------
//	Link_Data_File constructors
//-----------------------------------------------------------

Link_Data_File::Link_Data_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format), Time_Periods ()
{
	Setup ();
}

Link_Data_File::Link_Data_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format), Time_Periods ()
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Link_Data_File::Setup (void)
{
	File_Type ("Link Data File");
	File_ID ("LinkData");

	set_flag = true;
	decimal = 0;
	units = NO_UNITS;
	Clear_Fields ();
	Custom_Header (true);
}

//-----------------------------------------------------------
//	Read_Header
//-----------------------------------------------------------

bool Link_Data_File::Read_Header (bool stat)
{
	int i;
	String name;

	Db_Field *fld;

	set_flag = false;

	Db_Header::Read_Header (stat);

	for (i=0; i < Num_Fields (); i++) {
		fld = Field (i);
		name = fld->Name ();

		if (!name.Starts_With ("AB_")) continue;
		name.erase (0, 3);
		
		if (!Add_Label (name)) return (false);

		//---- check for decimal points ----

		if (fld->Decimal () > decimal) {
			decimal = fld->Decimal ();
		}
		units = fld->Units ();
	}
	set_flag = true;

	return (Set_Field_Numbers ());
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------


bool Link_Data_File::Create_Fields (void) 
{
	int i, num;
	double size;
	String name;

	Add_Field ("LINK", DB_INTEGER, 10);
	Add_Field ("ANODE", DB_INTEGER, 10);
	Add_Field ("BNODE", DB_INTEGER, 10);

	name = String ("%d.%d") % (10 + decimal) % decimal;
	size = name.Double ();
	num = Num_Periods ();

	for (i=0; i < num; i++) {
		name = "AB_" + Range_Label (i);

		if (decimal == 0) {
			Add_Field (name, DB_INTEGER, 10, units);
		} else {
			Add_Field (name, DB_DOUBLE, size, units);
		}
	}
	for (i=0; i <  num; i++) {
		name = "BA_" + Range_Label (i);

		if (decimal == 0) {
			Add_Field (name, DB_INTEGER, 10, units);
		} else {
			Add_Field (name, DB_DOUBLE, size, units);
		}
	}
	return (Set_Field_Numbers ());
}

bool Link_Data_File::Create_Fields (int low, int high, int increment)
{
	if (low < 0 || low > high) return (false);

	if (!Add_Range (low, high-1, increment)) return (false);

	return (Create_Fields ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Link_Data_File::Set_Field_Numbers (void)
{
	int i, num;
	String name;

	Clear_Fields ();

	link = Required_Field ("LINK", "ID");

	if (link < 0) return (false);

	anode = Optional_Field ("ANODE", "NODEA", "A");
	bnode = Optional_Field ("BNODE", "NODEB", "B");

	//---- allocate field number space ----

	num = Num_Periods ();

	if (num != 0) {
		ab.assign (num, -1);
		ba.assign (num, -1);

		//---- identify the field number for each range ----

		for (i=0; i < num; i++) {
			name = "AB_" + Range_Format (i);

			ab [i] = Optional_Field (name.c_str ());

			if (ab [i] < 0) {
				name = "AB_" + Range_Label (i);

				if (set_flag) {
					ab [i] = Required_Field (name.c_str ());
				} else {
					ab [i] = Optional_Field (name.c_str ());
				}
			}
			name [0] = 'B';
			name [1] = 'A';

			if (set_flag) {
				ba [i] = Required_Field (name.c_str ());
			} else {
				ba [i] = Optional_Field (name.c_str ());
			}
			Set_Units (ab [i], units);
			Set_Units (ba [i], units);
		}
	}
	return (true);
}

//-----------------------------------------------------------
//	Zero_Fields
//-----------------------------------------------------------

void Link_Data_File::Zero_Fields (void)
{
	for (int i=0; i < Num_Fields (); i++) {
		Put_Field (i, 0);
	}
}

//-----------------------------------------------------------
//	Clear_Fields
//-----------------------------------------------------------

void Link_Data_File::Clear_Fields (void)
{
	link = anode = bnode = -1;
	ab.clear ();
	ba.clear ();
}
