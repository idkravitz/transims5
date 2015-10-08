//********************************************************* 
//	Link_Dir_File.cpp - Link Direction File Input/Output
//*********************************************************

#include "Link_Dir_File.hpp"

//-----------------------------------------------------------
//	Link_Direction_File constructors
//-----------------------------------------------------------

Link_Direction_File::Link_Direction_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format), Time_Periods ()
{
	Setup ();
}

Link_Direction_File::Link_Direction_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format), Time_Periods ()
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Link_Direction_File::Setup (void)
{
	File_Type ("Link Direction File");
	File_ID ("LinkDir");

	set_flag = true;
	decimal = 0;
	units = NO_UNITS;
	Ignore_Periods (false);
	Join_Flag (false);
	Difference_Flag (false);
	Direction_Index (false);
	Clear_Fields ();
	Custom_Header (true);
}

//-----------------------------------------------------------
//	Read_Header
//-----------------------------------------------------------

bool Link_Direction_File::Read_Header (bool stat)
{
	int i;
	String name;

	Db_Field *fld;

	set_flag = false;

	Db_Header::Read_Header (stat);

	Join_Flag (false);
	Difference_Flag (false);

	if (!Ignore_Periods ()) {
		for (i=0; i < Num_Fields (); i++) {
			fld = Field (i);
			name = fld->Name ();

			if (name [0] == 'P' && name [1] >= '0' && name [1] <= '9') {
				name.erase (0, 1);
					
				if (!Add_Label (name)) return (false);

				//---- check for decimal points ----

				if (fld->Decimal () > decimal) {
					decimal = fld->Decimal ();
				}
				units = fld->Units ();
			} else if (name [0] == 'B' && name [1] >= '0' && name [1] <= '9') {
				Join_Flag (true);
			} else if (name [0] == 'D' && name [1] >= '0' && name [1] <= '9') {
				Difference_Flag (true);
			}
		}
	}
	set_flag = true;

	return (Set_Field_Numbers ());
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Link_Direction_File::Create_Fields (void) 
{
	int i, num;
	double size;
	String name;

	if (Direction_Index ()) {
		Add_Field ("LINKDIR", DB_INTEGER, 10);
	}
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_LinkDir_Field ();
	
	name = String ("%d.%d") % (10 + decimal) % decimal;
	size = name.Double ();
	num = Num_Periods ();

	for (i=0; i < num; i++) {
		name = "P" + Range_Label (i);

		if (decimal == 0) {
			Add_Field (name, DB_INTEGER, 10, units);
		} else {
			Add_Field (name, DB_DOUBLE, size, units);
		}
	}
	if (Join_Flag ()) {
		for (i=0; i < num; i++) {
			name = "B" + Range_Label (i);

			if (decimal == 0) {
				Add_Field (name, DB_INTEGER, 10, units);
			} else {
				Add_Field (name, DB_DOUBLE, size, units);
			}
		}
	}
	if (Difference_Flag ()) {
		for (i=0; i < num; i++) {
			name = "D" + Range_Label (i);

			if (decimal == 0) {
				Add_Field (name, DB_INTEGER, 10, units);
			} else {
				Add_Field (name, DB_DOUBLE, size, units);
			}
		}
	}
	return (Set_Field_Numbers ());
}

bool Link_Direction_File::Create_Fields (int low, int high, int increment)
{
	if (low < 0 || low > high) return (false);

	if (!Add_Range (low, high-1, increment)) return (false);

	return (Create_Fields ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Link_Direction_File::Set_Field_Numbers (void)
{
	int i, num;
	String name;

	Clear_Fields ();

	link = Required_Field ("LINK", "ID");

	if (link < 0) return (false);

	link_dir = Optional_Field ("LINKDIR", "LINK_DIR", "INDEX");
	dir = LinkDir_Type_Field ();	

	if (Ignore_Periods ()) {
		num = 0;
	} else {
		num = Num_Periods ();
	}

	//---- allocate field number space ----

	if (num != 0) {
		data.assign (num, -1);

		//---- identify the field number for each range ----

		for (i=0; i < num; i++) {
			name = "P" + Range_Format (i);

			data [i] = Optional_Field (name.c_str ());

			if (data [i] < 0) {
				name = "P" + Range_Label (i);

				if (set_flag) {
					data [i] = Required_Field (name.c_str ());
				} else {
					data [i] = Optional_Field (name.c_str ());
				}
			}
			Set_Units (data [i], units);
		}
	}

	//---- find the join fields ----

	if (Join_Flag () && num != 0) {
		data2.assign (num, -1);

		//---- identify the field number for each range ----

		for (i=0; i < num; i++) {
			name = "B" + Range_Format (i);

			data2 [i] = Optional_Field (name.c_str ());

			if (data2 [i] < 0) {
				name = "B" + Range_Label (i);

				if (set_flag) {
					data2 [i] = Required_Field (name.c_str ());
				} else {
					data2 [i] = Optional_Field (name.c_str ());
				}
			}
			Set_Units (data2 [i], units);
		}
	} 

	//---- difference fields ----

	if (Difference_Flag () && num != 0) {
		diff.assign (num, -1);

		//---- identify the field number for each range ----

		for (i=0; i < num; i++) {
			name = "D" + Range_Format (i);

			diff [i] = Optional_Field (name.c_str ());

			if (diff [i] < 0) {
				name = "D" + Range_Label (i);

				if (set_flag) {
					diff [i] = Required_Field (name.c_str ());
				} else {
					diff [i] = Optional_Field (name.c_str ());
				}
			}
			Set_Units (diff [i], units);
		}
	}
	return (true);
}

//-----------------------------------------------------------
//	Zero_Fields
//-----------------------------------------------------------

void Link_Direction_File::Zero_Fields (void)
{
	for (int i=0; i < Num_Fields (); i++) {
		Put_Field (i, 0);
	}
}

//-----------------------------------------------------------
//	Difference
//-----------------------------------------------------------

void Link_Direction_File::Difference (void)
{
	if (Difference_Flag () && Join_Flag ()) {
		double difference;

		for (int i=0; i < Num_Periods (); i++) {
			difference = Data (i) - Data2 (i);
			Diff (i, difference);
		}
	}
}

//-----------------------------------------------------------
//	Clear_Fields
//-----------------------------------------------------------

void Link_Direction_File::Clear_Fields (void)
{
	link_dir = link = dir = -1;
	data.clear ();
	data2.clear ();
	diff.clear ();
}

