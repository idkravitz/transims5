//********************************************************* 
//	Db_Base.cpp - database record
//*********************************************************

#include "Db_Base.hpp"

#include "String.hpp"

#include <stdio.h>
#include <string.h>
#include <math.h>

//-----------------------------------------------------------
//	Db_Base constructor
//-----------------------------------------------------------

Db_Base::Db_Base (Access_Type access, Format_Type format) : 
	Db_File (access, format)
{
	Time_Format (NO_UNITS);
	Dbase_Format (format);
}

//-----------------------------------------------------------
//	Db_Base constructor
//-----------------------------------------------------------

Db_Base::Db_Base (string filename, Access_Type access, Format_Type format) : 
	Db_File (access, format)
{
	Time_Format (NO_UNITS);
	Dbase_Format (format);

	Open (filename);
}

//-----------------------------------------------------------
//	Db_Base destructor
//-----------------------------------------------------------

Db_Base::~Db_Base (void)
{
	Clear_Fields ();
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Db_Base::Initialize (Access_Type access, Format_Type format) 
{
	Dbase_Format (format);

	Db_File::Initialize (access, format);
}

//---------------------------------------------------------
//	Dbase_Format
//---------------------------------------------------------

void Db_Base::Dbase_Format (Format_Type type) 
{
	if (type == DEFAULT_FORMAT && exe) {
		type = (Format_Type) exe->Default_Format ();
	}
	format_type = type;
	File_Format (type);
}

//-----------------------------------------------------------
//	Add_Field
//-----------------------------------------------------------

int Db_Base::Add_Field (const char *name, Field_Type type, double size, Units_Type units, bool binary, Nest_Type nest, int offset) 
{
	if (Dbase_Format () == DEFAULT_FORMAT) {
		Dbase_Format (DEFAULT_FORMAT);
	}
	if (field.capacity () == 0) {
		field.reserve (10);
	}
	if (time_format == NO_UNITS) {
		time_format = Static_Service::Time_Format ();
		if (type == DB_TIME && units == NO_UNITS) {
			units = time_format;
		}
	}
	int length, num;
	Field_Itr itr;

	switch (Dbase_Format ()) {
		case COMMA_DELIMITED:
		case SPACE_DELIMITED:
		case TAB_DELIMITED:
		case CSV_DELIMITED:
		case VERSION3:
			num = 1;
			length = 0;

			for (itr = field.begin (); itr != field.end (); itr++) {
				if ((nest != NESTED && !itr->Nested ()) || itr->Nest () == nest) {
					length += itr->Width () + 1;
					num++;
				}
			}
			if (binary) {
				Binary_Text_Size (type, &size);
			}
			length += (int) size;
			if (offset <= 0) offset = num;
			break;
		case UNFORMATED:
		case BINARY:
		case FIXED_COLUMN:
		case DBASE:
		case ARCVIEW:
		case SQLITE3:
		case TPPLUS:
		case TRANSCAD:
		default:
			if (offset < 0) {
				offset = (Dbase_Format () == DBASE || Dbase_Format () == ARCVIEW) ? 1 : 0;

				for (itr = field.begin (); itr != field.end (); itr++) {
					if ((nest != NESTED && !itr->Nested ()) || itr->Nest () == nest) {
						length = itr->Offset () + itr->Width ();
						if (offset < length) offset = length;
					}
				}
			}
			if (binary) {
				if (Record_Format () != BINARY) {
					Binary_Text_Size (type, &size);
				}
			} else if (Record_Format () == BINARY) {
				Text_Binary_Size (type, &size);
			}
			if (type == DB_TIME && Record_Format () == BINARY) {
				units = SECONDS;
			}
			length = offset + (int) size;
			break;
	}

	//---- convert to internal units ----

	if (File_Access () == CREATE) Internal_Units (1.0, &units);

	Db_Field fld (name, type, offset, size, units, nest);

	num = (int) field.size ();
	field.push_back (fld);

	Nested (nest);

	if (!Record_Size (length)) return (-1);

	return (num);
}

//---------------------------------------------------------
//	Required_Field
//---------------------------------------------------------

int Db_Base::Required_Field (string name)
{
	int num = Field_Number (name);

	if (num < 0 && exe->Send_Messages ()) {
		exe->Error (String ("%s does not include \"%s\" field") % File_Type () % name);
	}
	return (num);
}

//---------------------------------------------------------
//	Required_Field
//---------------------------------------------------------

int Db_Base::Required_Field (const char *name1, const char *name2, const char *name3, const char *name4, const char *name5)
{
	int num = Optional_Field (name1, name2, name3, name4, name5);

	if (num < 0 && exe->Send_Messages ()) {
		if (name5) {
			exe->Error (String ("%s does not include field:\n\t\t\"%s\", \"%s\", \"%s\", \"%s\" or \"%s\"") % 
						File_Type () % name1 % name2 % name3 % name4 % name5);
		} else if (name4) {
			exe->Error (String ("%s does not include field:\n\t\t\"%s\", \"%s\", \"%s\" or \"%s\"") % 
						File_Type () % name1 % name2 % name3 % name4);
		} else if (name3) {
			exe->Error (String ("%s does not include field:\n\t\t\"%s\", \"%s\" or \"%s\"") % 
						File_Type () % name1 % name2 % name3);
		} else if (name2) {
			exe->Error (String ("%s does not include \"%s\" or \"%s\" field") % File_Type () % name1 % name2);
		} else if (name1) {
			exe->Error (String ("%s does not include \"%s\" field") % File_Type () % name1);
		} else {
			exe->Error (String ("%s Required Field Name is NULL") % File_Type ());
		}
	}
	return (num);
}

//---------------------------------------------------------
//	Optional_Field
//---------------------------------------------------------

int Db_Base::Optional_Field (const char *name1, const char *name2, const char *name3, const char *name4, const char *name5)
{
	if (!name1) {
		Status (NULL_NAME);
		return (0);
	}
	int num = Field_Number (name1);

	if (num < 0 && name2) {
		num = Field_Number (name2);

		if (num < 0 && name3) {
			num = Field_Number (name3);

			if (num < 0 && name4) {
				num = Field_Number (name4);

				if (num < 0 && name5) {
					num = Field_Number (name5);
				}
			}
		}
	}
	return (num);
}

//-----------------------------------------------------------
//	Field - by name
//-----------------------------------------------------------

Field_Ptr Db_Base::Field (int fld)
{
	if (fld >= 0 && fld < (int) field.size ()) {
		return (&field [fld]); 
	} else {
		return (0);
	}
}

Field_Ptr Db_Base::Field (const char *name) 
{
	if (name) {
		Field_Itr itr;

		for (itr = field.begin (); itr != field.end (); itr++) {
			if (itr->Name ().Equals (name)) {
				return (&(*itr));
			}
		}
	}
	return (0);
}

//---------------------------------------------------------
//	Field_Number
//---------------------------------------------------------

int Db_Base::Field_Number (const char *name)
{
	if (name) {
		Field_Itr itr, itr0;

		for (itr = itr0 = field.begin (); itr != field.end (); itr++) {
			if (itr->Name ().Equals (name)) {
				return ((int) (itr - itr0));
			}
		}
	}
	return (-1);
}

//----------------------------------------------------------
//	Read_Field
//----------------------------------------------------------

bool Db_Base::Read_Field (Db_Field *fld, void *data, Field_Type type)
{
	if (data == 0) return (Status (NULL_POINTER));
	
	//---- initialize the data field ----

	switch (type) {
		case DB_INTEGER:
			*((int *) data) = 0;
			break;
		case DB_DOUBLE:
			*((double *) data) = 0.0;
			break;
		case DB_STRING:
			((string *) data)->clear ();
			break;
		case DB_TIME:
			*((Dtime *) data) = 0;
			break;
		default:
			return (Status (DB_ERROR));
	}
	if (fld == 0) return (Status (NO_FIELD, false));

	Nested (fld->Nested ());

	char *field = Record_String ();

	if (field == 0) return (Status (RECORD_SIZE));

	Field_Type fld_type;
	bool asc_flag = true;
	int len, lvalue = 0;
	double dvalue = 0.0;
	String svalue;
	Dtime tvalue = 0;

	int size = fld->Width ();
	int offset = fld->Offset ();

	switch (Record_Format ()) {
		case BINARY:
			if (fld->Type () != DB_STRING) {
				field += offset;
				asc_flag = false;
				break;
			} else {
				len = (int) strlen (field + offset);
				if (len < size) size = len;
			}
		case FIXED_COLUMN:
			svalue.assign (field + offset, size);
			svalue.Clean ();
			break;
		case COMMA_DELIMITED:
		case SPACE_DELIMITED:
		case TAB_DELIMITED:
		case CSV_DELIMITED:
			svalue (fld->Buffer ());
			size = (int) svalue.size ();
			break;
		default:
			svalue  = Get_Field_Number (offset);
			size = (int) svalue.size ();
			break;
	}

	//---- read the field from the current record ----

	switch (fld->Type ()) {
		default:
			return (Status (DB_ERROR));

		case DB_INTEGER:
			if (asc_flag) {
				lvalue = svalue.Integer ();
			} else if (size == sizeof (int)) {
				lvalue = *((int *) field);
			} else if (size == sizeof (short)) {
				lvalue = *((short *) field);
			} else if (size == sizeof (char)) {
				lvalue = *((char *) field);
			} else {
				return (Status (FIELD_BYTES));
			}
			fld_type = DB_INTEGER;
			break;
			
		case DB_UNSIGNED:
			if (asc_flag) {
				lvalue = svalue.Integer ();
			} else if (size == sizeof (int)) {
				lvalue = *((unsigned int *) field);
			} else if (size == sizeof (short)) {
				lvalue = *((unsigned short *) field);
			} else if (size == sizeof (char)) {
				lvalue = *((unsigned char *) field);
			} else {
				return (Status (FIELD_BYTES));
			}
			fld_type = DB_INTEGER;
			break;
			
		case DB_DOUBLE:
			if (asc_flag) {
				dvalue = svalue.Double ();
			} else if (size == sizeof (double)) {
				dvalue = *((double *) field);
			} else if (size == sizeof (float)) {
				dvalue = *((float *) field);
			} else {
				return (Status (FIELD_BYTES));
			}
			fld_type = DB_DOUBLE;
			break;
			
		case DB_FIXED:
			if (asc_flag) {
				dvalue = svalue.Double ();
				if (svalue.find ('.') == 0) {
					dvalue = dvalue / pow (10.0, fld->Decimal ());
				}
			} else {
				if (size == sizeof (int)) {
					lvalue = *((int *) field);
				} else if (size == sizeof (short)) {
					lvalue = *((short *) field);
				} else if (size == sizeof (char)) {
					lvalue = *((char *) field);
				} else {
					return (Status (FIELD_BYTES));
				}
				dvalue = (double) lvalue / pow (10.0, fld->Decimal ());
			}
			fld_type = DB_DOUBLE;
			break;
			
		case DB_STRING:
			fld_type = DB_STRING;
			break;

		case DB_CHAR:
			if (asc_flag) {
				if (svalue.empty ()) {
					svalue = " ";
				}
			} else {
				if (*field == '\0') {
					svalue = " ";
				} else {
					svalue.insert (0, field, 1);
				}
			}
			fld_type = DB_STRING;
			break;
			
		case DB_TIME:
			if (asc_flag) {
				tvalue.Time_String (svalue, fld->Units ());
			} else if (size == sizeof (Dtime)) {
				tvalue = *((int *) field);
			} else if (size == sizeof (short)) {
				tvalue = *((short *) field);
			} else {
				return (Status (FIELD_BYTES));
			}
			fld_type = DB_TIME;
			break;
	}

	//---- convert to internal units ----

	if (fld->Units () != NO_UNITS) {
		if (fld_type == DB_INTEGER) {
			lvalue = Internal_Units (lvalue, fld->Units ());
		} else if (fld_type == DB_DOUBLE) {
			dvalue = Internal_Units (dvalue, fld->Units ());
		} else if (fld->Units () < SECONDS || fld->Units () > TIME_CODE) {
			if (fld_type == DB_STRING && fld->Units () >= FACILITY_CODE) {
				if (type == DB_INTEGER) {
					lvalue = Internal_Units (svalue, fld->Units ());
					if (lvalue < -1) return (Status (FIELD_UNITS));
					if (lvalue >= 0) fld_type = DB_INTEGER;
				}
			} else {
				return (Status (FIELD_UNITS));
			}
		}
	}

	//---- convert to the output data type ----

	switch (type) {
		case DB_INTEGER:
			switch (fld_type) {
				case DB_INTEGER:
					*((int *) data) = lvalue;
					break;
				case DB_DOUBLE:
					*((int *) data) = DTOI (dvalue);
					break;
				case DB_STRING:
					if (svalue [0] < '0' || svalue [0] > '9' || svalue.find_first_of ("@:") != svalue.npos) {
						*((int *) data) = (int) Dtime (svalue);
					} else {
						*((int *) data) = svalue.Integer ();
					}
					break;
				case DB_TIME:
					*((int *) data) = (int) tvalue;
					break;
				default:
					return (Status (DB_ERROR));
			}
			break;

		case DB_DOUBLE:
			switch (fld_type) {
				case DB_INTEGER:
					*((double *) data) = lvalue;
					break;
				case DB_DOUBLE:
					*((double *) data) = dvalue;
					break;
				case DB_STRING:
					if (svalue [0] < '0' || svalue [0] > '9' || svalue.find_first_of ("@:") != svalue.npos) {
						*((double *) data) = (int) Dtime (svalue);
					} else {
						*((double *) data) = svalue.Double ();
					}
					break;
				case DB_TIME:
					*((double *) data) = (double) tvalue;
					break;
				default:
					return (Status (DB_ERROR));
			}
			break;

		case DB_STRING:
			switch (fld->Type ()) {
				case DB_TIME:
					svalue = tvalue.Time_String ();
					break;
				default:
					if (fld_type == DB_INTEGER) {
						svalue (lvalue);
					} else if (fld_type == DB_DOUBLE) {
						svalue (dvalue, fld->Decimal ());
					}
					break;
			}
			len = (int) svalue.size ();
			if (len > 0) {
				if (len > fld->Width ()) len = fld->Width ();

				*((string *) data) = svalue.substr (0, len);
			} else {
				(*((string *) data)).clear ();
			}
			break;

		case DB_TIME:
			switch (fld_type) {
				case DB_INTEGER:
					*((Dtime *) data) = Dtime (lvalue, fld->Units ());
					break;
				case DB_DOUBLE:
					*((Dtime *) data) = Dtime (dvalue, fld->Units ());
					break;
				case DB_STRING:
					*((Dtime *) data) = svalue;
					break;
				case DB_TIME:
					*((Dtime *) data) = tvalue;
					break;
				default:
					return (Status (DB_ERROR));
			}
			break;
		default:
			return (Status (DB_ERROR));
	}
	return (true);
}

//----------------------------------------------------------
//	Write_Field
//----------------------------------------------------------

bool Db_Base::Write_Field (Db_Field *fld, void *data, Field_Type type)
{
	if (data == 0) return (Status (NULL_POINTER));
	if (fld == 0) return (Status (NO_FIELD, false));
	if (!Record ().OK ()) return (Status (RECORD_SIZE));

	int lvalue = 0;
	unsigned uvalue = 0;
	double dvalue = 0.0;
	Dtime tvalue;
	String svalue;
	Field_Type fld_type;

	//---- convert the input data type to generic variables ----

	fld_type = fld->Type ();

	switch (fld_type) {
		default:
			return (Status (DB_ERROR));
			
		case DB_INTEGER:
			switch (type) {
				case DB_INTEGER:
					lvalue = *((int *) data);
					break;
				case DB_DOUBLE:
					dvalue = *((double *) data);
					if (dvalue > MAX_INTEGER) {
						lvalue = MAX_INTEGER;
					} else if (dvalue < -MAX_INTEGER) {
						lvalue = -MAX_INTEGER;
					} else {
						lvalue = DTOI (dvalue);
					}
					break;
				case DB_STRING:
					lvalue = ((String *) data)->Integer ();
					break;
				case DB_TIME:
					lvalue = (int) (*((Dtime *) data));
					break;
				default:
					return (Status (DB_ERROR));
			}
			break;

		case DB_UNSIGNED:
			switch (type) {
				case DB_INTEGER:
					uvalue = *((unsigned *) data);
					break;
				case DB_DOUBLE:
					dvalue = *((double *) data);
					if (dvalue > 2.0 * MAX_INTEGER) {
						uvalue = MAX_INTEGER;
						uvalue *= 2;
					} else if (dvalue < 0.0) {
						uvalue = 0;
					} else {
						uvalue = (unsigned) (dvalue + 0.5);
					}
					break;
				case DB_STRING:
					uvalue = ((String *) data)->Integer ();
					break;
				case DB_TIME:
					uvalue = (unsigned) (*((Dtime *) data));
					break;
				default:
					return (Status (DB_ERROR));
			}
			break;

		case DB_FIXED:
		case DB_DOUBLE:
			switch (type) {
				case DB_INTEGER:
					dvalue = (double) *((int *) data);
					break;
				case DB_DOUBLE:
					dvalue = *((double *) data);
					break;
				case DB_STRING:
					dvalue = ((String *) data)->Double ();
					break;
				case DB_TIME:
					dvalue = (double) *((Dtime *) data);
					break;
				default:
					return (Status (DB_ERROR));
			}
			break;
		
		case DB_STRING:
		case DB_CHAR:
			switch (type) {
				case DB_INTEGER:
					if (fld->Units () >= FACILITY_CODE) {
						External_Units (*((int *) data), fld->Units (), svalue);
					} else {
						svalue (*((int *) data));
					}
					break;
				case DB_DOUBLE:
					svalue (*((double *) data), fld->Decimal ());
					break;
				case DB_STRING:
					svalue = *((string *) data);
					break;
				case DB_TIME:
					svalue = ((Dtime *) data)->Time_String (fld->Units ());
					break;
				default:
					return (Status (DB_ERROR));
			}
			break;

		case DB_TIME:
			switch (type) {
				case DB_INTEGER:
					tvalue = *((int *) data);
					break;
				case DB_DOUBLE:
					tvalue = *((double *) data);
					break;
				case DB_STRING:
					tvalue = *((string *) data);
					break;
				case DB_TIME:
					tvalue = *((Dtime *) data);
					break;
				default:
					return (Status (DB_ERROR));
			}
			break;
	}

	//---- convert to external units ----

	if (fld->Units () != NO_UNITS) {
		if (fld_type == DB_INTEGER) {
			lvalue = (int) External_Units (lvalue, fld->Units ());
		} else if (fld_type == DB_UNSIGNED) {
			uvalue = (unsigned) External_Units ((int) uvalue, fld->Units ());
		} else if (fld_type == DB_DOUBLE || fld_type == DB_FIXED) {
			dvalue = External_Units (dvalue, fld->Units ());
		} else if ((fld_type == DB_STRING || fld_type == DB_CHAR) && fld->Units () < FACILITY_CODE) {
			return (Status (FIELD_UNITS));
		}
	}

	//---- place the data onto the data record -----

	Nested (fld->Nested ());

	char *field = Record_String ();

	bool asc_flag = false;
	bool justify = false;
	int len, position;
	int size = fld->Width ();
	int offset = fld->Offset ();

	switch (Record_Format ()) {
		case BINARY:
			field += offset;
			asc_flag = false;
			break;
		case FIXED_COLUMN:
			field += offset;
			memset (field, ' ', size);
			asc_flag = justify = true;
			break;
		case COMMA_DELIMITED:
		case SPACE_DELIMITED:
		case TAB_DELIMITED:
		case CSV_DELIMITED:
		default:		
			asc_flag = true;
			justify = false;
			break;
	}

	switch (fld_type) {
		default:
			return (Status (DB_ERROR));
			
		case DB_INTEGER:
			if (asc_flag) {
				svalue (lvalue);

				if (justify) {
					len = (int) svalue.size ();
					position = size - len;
					if (position < 0) {
						position = 0;
						len = size;
					}
					memcpy (field + position, svalue.c_str (), len);
				}
			} else if (size == sizeof (int)) {
				*((int *) field) = lvalue;
			} else if (size == sizeof (short)) {
				if (lvalue > 32767) lvalue = 32767;
				*((short *) field) = (short) lvalue;
			} else if (size == sizeof (char)) {
				if (lvalue > 127) lvalue = 127;
				*((char *) field) = (char) lvalue;
			} else {
				return (Status (FIELD_BYTES));
			}
			break;
			
		case DB_UNSIGNED:
			if (asc_flag) {
				svalue ((size_t) uvalue);

				if (justify) {
					len = (int) svalue.size ();
					position = size - len;
					if (position < 0) {
						position = 0;
						len = size;
					}
					memcpy (field + position, svalue.c_str (), len);
				}
			} else if (size == sizeof (int)) {
				*((unsigned int *) field) = (unsigned int) uvalue;
			} else if (size == sizeof (short)) {
				if (uvalue > 65535) uvalue = 65535;
				*((unsigned short *) field) = (unsigned short) uvalue;
			} else if (size == sizeof (char)) {
				if (uvalue > 255) uvalue = 255;
				*((unsigned char *) field) = (unsigned char) uvalue;
			} else {
				return (Status (FIELD_BYTES));
			}
			break;

		case DB_DOUBLE:
			if (asc_flag) {
				svalue (dvalue, fld->Decimal ());
				if (justify) {
					len = (int) svalue.size ();
					position = size - len;
					if (position < 0) {
						position = 0;
						len = size;
					}
					memcpy (field + position, svalue.c_str (), len);
				}
			} else if (size == sizeof (double)) {
				*((double *) field) = dvalue;
			} else if (size == sizeof (float)) {
				*((float *) field) = (float) dvalue;
			} else {
				return (Status (FIELD_BYTES));
			}
			break;

		case DB_FIXED:
			if (asc_flag) {
				svalue (dvalue, fld->Decimal ());

				if (justify) {
					len = (int) svalue.size ();
					position = size - len;
					if (position < 0) {
						position = 0;
						len = size;
					}
					memcpy (field + position, svalue.c_str (), len);
				}
			} else {
				if (dvalue < 0.0) {
					lvalue = (int) (dvalue * pow (10.0, fld->Decimal ()) - 0.5);
				} else {
					lvalue = (int) (dvalue * pow (10.0, fld->Decimal ()) + 0.5);
				}

				if (size == sizeof (int)) {
					*((int *) field) = lvalue;
				} else if (size == sizeof (short)) {
					*((short *) field) = (short) lvalue;
				} else if (size == sizeof (char)) {
					*((char *) field) = (char) lvalue;
				} else {
					return (Status (FIELD_BYTES));
				}
			}
			break;

		case DB_STRING:
			if (asc_flag) {
				if (justify) {
					len = (int) svalue.size ();
					if (len > size) len = size;
					memcpy (field, svalue.c_str (), len);
				}
			} else {
				memset (field, '\0', size);
				memcpy (field, svalue.c_str (), MIN ((int) svalue.size (), size));
			}
			break;
			
		case DB_CHAR:
			if (!asc_flag || justify) {
				memset (field, '\0', size);
				field [0] = svalue [0];
			}
			break;
			
		case DB_TIME:
			if (asc_flag) {
				svalue = tvalue.Time_String (fld->Units ());
				if (justify) {
					len = (int) svalue.size ();
					if (len > size) len = size;
					memcpy (field, svalue.c_str (), len);
				}
			} else if (size == sizeof (Dtime)) {
				*((Dtime *) field) = tvalue;
			} else if (size == sizeof (short)) {
				*((short *) field) = (short) tvalue;
			} else {
				return (Status (FIELD_BYTES));
			}
			break;
	}
	if (asc_flag && !justify) {
		if (Record_Format () == UNFORMATED) {
			return (Set_Field_Number (offset, svalue));
		} else {
			fld->Buffer (svalue);
		}
	}
	return (true);
}

//-----------------------------------------------------------
//	Blank_Field
//-----------------------------------------------------------

void Db_Base::Blank_Field (Db_Field *fld)
{
	if (fld == 0) return;

	if (Record_Format () == BINARY || Record_Format () == FIXED_COLUMN) {
		Nested (fld->Nested ());
		char *field = Record_String ();

		field += fld->Offset ();
		memset (field, ' ', fld->Width ());
	} else {
		fld->Buffer ().erase ();
	}
}

//-----------------------------------------------------------
//	Blank_Nested_Fields
//-----------------------------------------------------------

void Db_Base::Blank_Nested_Fields (void)
{
	Field_Itr itr;

	for (itr = field.begin (); itr != field.end (); itr++) {
		if (itr->Nested () || itr->Nest () == FLATTEN) Blank_Field (&(*itr));
	}
}

//---------------------------------------------------------
//	Copy_Fields
//---------------------------------------------------------

bool Db_Base::Copy_Fields (Db_Base &input, bool nest_flag)
{
	Field_Ptr in_fld, out_fld;

	bool nested = input.Nested ();
	int lvalue;
	double dvalue;
	string svalue;
	Dtime tvalue;

	if (Dbase_Format () == DEFAULT_FORMAT) {
		Dbase_Format (DEFAULT_FORMAT);
	}
	int num = input.Num_Fields ();

	for (int i=0; i < num; i++) {
		in_fld = input.Field (i);

		if (in_fld != 0) {
			if (nest_flag && input.Nest_Flag ()) {
				if (input.Nested ()) {
					if (in_fld->Nest () != input.Nest ()) continue;
				} else {
					if (in_fld->Nest () == input.Nest ()) continue;
				}
			}
			out_fld = Field (in_fld->Name ());

			if (out_fld != 0) {
				switch (in_fld->Type ()) {
					case DB_INTEGER:
					case DB_UNSIGNED:
						input.Read_Field (in_fld, (void *) &lvalue, DB_INTEGER);
						if (!Write_Field (out_fld, (void *) &lvalue, DB_INTEGER)) return (false);
						break;
					case DB_DOUBLE:
					case DB_FIXED:
						input.Read_Field (in_fld, (void *) &dvalue, DB_DOUBLE);
						if (!Write_Field (out_fld, (void *) &dvalue, DB_DOUBLE)) return (false);
						break;
					case DB_STRING:
					case DB_CHAR:
						input.Read_Field (in_fld, (void *) &svalue, DB_STRING);
						if (!Write_Field (out_fld, (void *) &svalue, DB_STRING)) return (false);
						break;
					case DB_TIME:
						input.Read_Field (in_fld, (void *) &tvalue, DB_TIME);
						if (!Write_Field (out_fld, (void *) &tvalue, DB_TIME)) return (false);
						break;
				}
			}
		}
	}
	input.Nested (nested);
	return (true);
}

//---------------------------------------------------------
//	Replicate_Fields
//---------------------------------------------------------

bool Db_Base::Replicate_Fields (Db_Base *file, bool notes_flag, bool nest_flag, bool drop_flag) 
{
	int i, offset, nfield;
	bool convert, binary;
	Nest_Type nest;
	Field_Ptr fld;

	if (file == 0) return (false);
	if (Dbase_Format () == DEFAULT_FORMAT) {
		Dbase_Format (DEFAULT_FORMAT);
	}
	convert = (file->Dbase_Format () != Dbase_Format () || Num_Fields () > 0);
	binary = (file->Record_Format () == BINARY);

	offset = END_OF_RECORD;
	nfield = file->Num_Fields ();

	for (i=0; i < nfield; i++) {
		fld = file->Field (i);
		if (fld == 0) continue;

		if (!convert) {
			offset = fld->Offset ();
		}
		if (notes_flag == false) {
			if (fld->Name ().Equals ("NOTES")) continue;
		}
		if (drop_flag && fld->Nested ()) continue;

		nest = (nest_flag) ? fld->Nest () : NO_NEST;

		Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Units (), binary, nest, offset);
	}
	return (Write_Fields ());
}

//-----------------------------------------------------------
//	Input_Record
//-----------------------------------------------------------

bool Db_Base::Input_Record (void)
{
	if (File_Format () == SQLITE3) {
		int i, j, num, lvalue, result;
		const unsigned char *text;
		double dvalue;
		Db_Field *fld;

		//---- get the next record ----

		if (Nested ()) {
			if (new_nest_flag) {
				new_nest_flag = false;

				sqlite3_reset (read_nest);
				sqlite3_bind_int (read_nest, 1, parent_id);
			}
			result = sqlite3_step (read_nest);
		} else {
			Record_Number (0);
			parent_id = Record_Number ();
			new_nest_flag = true;

			sqlite3_reset (read_stmt);
			sqlite3_bind_int (read_stmt, 1, parent_id);
			result = sqlite3_step (read_stmt);
		}
		if (result == SQLITE_DONE) return (false);
		if (result != SQLITE_ROW) {
			exe->Warning ("SQLITE3 Read Problem: ") << sqlite3_errmsg (db_file);
		}
		num = Num_Fields ();

		for (i=0, j=0; i < num; i++) {
			fld = Field (i);
			if (Nested () != fld->Nested ()) continue;
			if (Nested ()) {
				if (fld->Type () == DB_INTEGER) {
					lvalue = sqlite3_column_int (read_nest, j+1);
					Put_Field (i, lvalue);
				} else if (fld->Type () == DB_DOUBLE) {
					dvalue = sqlite3_column_double (read_nest, j+1);
					Put_Field (i, dvalue);
				} else {
					text = sqlite3_column_text (read_nest, j+1);
					Put_Field (i, (char *) text);
				}
			} else {
				if (fld->Type () == DB_INTEGER) {
					lvalue = sqlite3_column_int (read_stmt, j);
					Put_Field (i, lvalue);
				} else if (fld->Type () == DB_DOUBLE) {
					dvalue = sqlite3_column_double (read_stmt, j);
					Put_Field (i, dvalue);
				} else {
					text = sqlite3_column_text (read_stmt, j);
					Put_Field (i, (char *) text);
				}
			}
			j++;
		}
	} else if (File_Format () == CSV_DELIMITED) {
		Strings fields;
		Db_Field *fld;

		int count = String (Record_String ()).Parse (fields, Delimiters ());

		for (int i=0; i < Num_Fields (); i++) {
			fld = Field (i);
			if (Nested () == fld->Nested ()) {
				if (fld->Offset () <= count) {
					fld->Buffer (fields [fld->Offset () - 1]);
				} else {
					fld->Buffer ("");
				}
			}
		}
	}
	return (true);
}

//-----------------------------------------------------------
//	Output_Record
//-----------------------------------------------------------

bool Db_Base::Output_Record (void)
{
	if (File_Format () == SQLITE3) {
		int i, j, num, lvalue;
		double dvalue;
		string svalue;
		Db_Field *fld;

		num = Num_Fields ();

		for (i=0, j=1; i < num; i++) {
			fld = Field (i);
			if (Nested () != fld->Nested ()) continue;

			if (Nested ()) {
				if (fld->Type () == DB_INTEGER) {
					Get_Field (i, lvalue);
					sqlite3_bind_int (insert_nest, j+1, lvalue);
				} else if (fld->Type () == DB_DOUBLE) {
					Get_Field (i, dvalue);
					sqlite3_bind_double (insert_nest, j+1, dvalue);
				} else {
					Get_Field (i, svalue);
					sqlite3_bind_text (insert_nest, j+1, svalue.c_str (), -1, SQLITE_TRANSIENT);
				}
			} else {
				if (fld->Type () == DB_INTEGER) {
					Get_Field (i, lvalue);
					sqlite3_bind_int (insert_stmt, j, lvalue);
				} else if (fld->Type () == DB_DOUBLE) {
					Get_Field (i, dvalue);
					sqlite3_bind_double (insert_stmt, j, dvalue);
				} else {
					Get_Field (i, svalue);
					sqlite3_bind_text (insert_stmt, j, svalue.c_str (), -1, SQLITE_TRANSIENT);
				}
			}
			j++;
			
		}
		if (Nested ()) {
			sqlite3_bind_int (insert_nest, 1, parent_id);

			if (sqlite3_step (insert_nest) != SQLITE_DONE) {
				exe->Warning ("Inserting Database: ") << sqlite3_errmsg (db_file);
			}
			if (sqlite3_reset (insert_nest) != SQLITE_OK) {
				exe->Warning ("Inserting Database: ") << sqlite3_errmsg (db_file);
			}
		} else {
			if (sqlite3_step (insert_stmt) != SQLITE_DONE) {
				exe->Warning ("Inserting Database: ") << sqlite3_errmsg (db_file);
			}
			parent_id = (int) sqlite3_last_insert_rowid (db_file);
			if (sqlite3_reset (insert_stmt) != SQLITE_OK) {
				exe->Warning ("Inserting Database: ") << sqlite3_errmsg (db_file);
			}
		}
	} else if (File_Format () == CSV_DELIMITED) {
		int i, j, max;
		string *ptr;
		char *delimiter, *record;
		bool first = true;
		Db_Field *fld;

		delimiter = Delimiters ();
		record = Record_String ();

		for (i=0; i < Num_Fields (); i++) {
			fld = Field (i);
			if (Nested () == fld->Nested ()) {
				ptr = &fld->Buffer ();
				max = (int) ptr->size ();
				if (max > fld->Width ()) max = fld->Width ();

				if (first) {
					first = false;
				} else {
					*record++ = *delimiter;
				}
				for (j=0; j < max; j++) {
					*record++ = (*ptr) [j];
				}
				*record = '\0';
			}
		}
		Set_Size ();
	}
	return (true);
}

//-----------------------------------------------------------
//	Reset_Record
//-----------------------------------------------------------

char * Db_Base::Reset_Record (void)
{
	if (File_Format () == CSV_DELIMITED) {
		Field_Itr itr;

		for (itr = field.begin (); itr != field.end (); itr++) {
			if (Nested () == itr->Nested ()) {
				itr->Buffer ().erase ();
			}
		}
	}
	return (Db_Record::Reset_Record ().Data ());
}

//---------------------------------------------------------
//	Binary_Text_Size
//---------------------------------------------------------

void Db_Base::Binary_Text_Size (Field_Type type, double *size) 
{
	size_t length = (size_t) *size;
	int decimal = DTOI ((*size - length) * 10.0);

	switch (type) {
		case DB_INTEGER:
		case DB_UNSIGNED:
			if (length == sizeof (char)) {
				*size = 3;
			} else if (length == sizeof (short)) {
				*size = 5;
			} else {
				*size = 10;
			}
			break;
		case DB_DOUBLE:
			if (decimal == 0) {
				if (length == sizeof (float)) {
					*size = 8.2;
				} else {
					*size = 12.4;
				}
			} else {
				if (length == sizeof (float)) {
					*size = ((int) ((decimal + 7) * 10 + decimal)) / 10.0;
				} else {
					*size = ((int) ((decimal + 10) * 10 + decimal)) / 10.0;
				}
			}
			break;
		case DB_FIXED:
			if (decimal == 0) {
				if (length == sizeof (char)) {
					*size = 4.2;
				} else if (length == sizeof (short)) {
					*size = 6.2;
				} else {
					*size = 8.2;
				}
			} else {
				if (length == sizeof (char)) {
					*size = 4 + decimal / 10.0;
				} else if (length == sizeof (short)) {
					*size = 6 + decimal / 10.0;
				} else {
					*size = 10 + decimal / 10.0;
				}
			}
			break;
		case DB_STRING:
		case DB_CHAR:
			break;
		case DB_TIME:
			*size = 12;
			break;
	}
}

//---------------------------------------------------------
//	Text_Binary_Size
//---------------------------------------------------------

void Db_Base::Text_Binary_Size (Field_Type type, double *size) 
{
	int length = (int) *size;
	double decimal = *size - length;

	switch (type) {
		case DB_INTEGER:
		case DB_UNSIGNED:
			if (length < 3) {
				*size = sizeof (char);
			} else if (length < 5) {
				*size = sizeof (short);
			} else {
				*size = sizeof (int);
			}
			break;
		case DB_DOUBLE:
			if (length < 9) {
				*size = sizeof (float);
			} else {
				*size = sizeof (double);
			}
			break;
		case DB_FIXED:
			if (length < 4) {
				*size = sizeof (char);
			} else if (length < 6) {
				*size = sizeof (short);
			} else {
				*size = sizeof (int);
			}
			break;
		case DB_STRING:
		case DB_CHAR:
			break;
		case DB_TIME:
			*size = sizeof (Dtime);
			break;
	}
	*size += decimal;
}
