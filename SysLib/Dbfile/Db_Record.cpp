//********************************************************* 
//	Db_Record.cpp - database record
//*********************************************************

#include "Db_Record.hpp"

#include "System_Defines.hpp"

#include <stdio.h>
#include <string.h>

//-----------------------------------------------------------
//	Db_Record constructor
//-----------------------------------------------------------

Db_Record::Db_Record (Format_Type format) : Db_Status ()
{
	delimiters = 0;
	nested = false;
	nest = NO_NEST;
	
	Record_Format (format);
}

//-----------------------------------------------------------
//	Record_Format
//-----------------------------------------------------------

void Db_Record::Record_Format (Format_Type format)
{
	if (format == DEFAULT_FORMAT && exe) {
		format = (Format_Type) exe->Default_Format ();
	}
	record_format = format;

	switch (record_format) {
		case DBASE:
		case ARCVIEW:
			record_format = FIXED_COLUMN;
			break;
		case COMMA_DELIMITED:
			delimiters = COMMA_DELIMITERS;
			break;
		case SPACE_DELIMITED:
			delimiters = SPACE_DELIMITERS;
			break;
		case VERSION3:
			record_format = TAB_DELIMITED;
		case TAB_DELIMITED:
			delimiters = TAB_DELIMITERS;
			break;
		case SQLITE3:
		case TPPLUS:
		case TRANSCAD:
			record_format = BINARY;
			break;
		default:	//---- CSV ----	
			delimiters = CSV_DELIMITERS;
			break;
	}
	Reset_Buffers ();
}

//-----------------------------------------------------------
//	Reset_Buffers
//-----------------------------------------------------------

void Db_Record::Reset_Buffers (void)
{
	if (record_format == BINARY || record_format == FIXED_COLUMN) {
		record.Max_Size (0);
		if (Nest () == NESTED) {
			nest_rec.Max_Size (0);
		}
	} else {
		record.Max_Size (4094);
		if (Nest () == NESTED) {
			nest_rec.Max_Size (4094);
		}
	}
}

//-----------------------------------------------------------
//	Record
//-----------------------------------------------------------

bool Db_Record::Record (void *_record, int size)
{
	Buffer &buf = Record ();

	if (String_Flag ()) {
		if (size <= 0) {
			size = (int) strlen ((char *) _record);
			if (size > buf.Max_Size ()) {
				size = buf.Max_Size ();
			}
		}
	} else {
		if (size <= 0) size = buf.Max_Size ();
	}
	return (buf.Data (_record, size));
}

//-----------------------------------------------------------
//	Reset_Record
//-----------------------------------------------------------

Buffer & Db_Record::Reset_Record (void)
{
	char fill = (record_format == FIXED_COLUMN) ? ' ' : '\0';

	Buffer &buf = Record ();

	buf.Fill (fill);
	return (buf);
}

//----------------------------------------------------------
//	Get_Field_Number
//----------------------------------------------------------

string Db_Record::Get_Field_Number (int number)
{
	String field, buffer;

	if (!Record ().OK ()) {
		Status (RECORD_SIZE);
		return (0);
	}
	buffer = Record_String ();

	for (int i=0; i < number; i++) {
		if (!buffer.Split (field, Delimiters ())) break;
	}
	return (field);
}

//----------------------------------------------------------
//	Set_Field_Number
//----------------------------------------------------------

bool Db_Record::Set_Field_Number (int number, string text)
{
	Buffer &rec = Record ();

	if (!rec.OK ()) return (Status (RECORD_SIZE));

	int i, max_num, num;

	Strings fields;
	String buffer = rec.Data ();

	String_Ptr (text)->Trim ();

	num = buffer.Parse (fields, delimiters);

	max_num = MAX (num, number);
	number--;

	buffer.clear ();

	for (i=0; i < max_num; i++) {
		if (i > 0) buffer += *delimiters;
		if (*delimiters == ' ') {
			buffer += '\"';
		}
		if (i == number) {
			buffer += text;
		} else if (i < num) {
			buffer += fields [i];
		}
		if (*delimiters == ' ') {
			buffer += '\"';
		}
	}
	return (Record ((void *) buffer.data (), (int) buffer.size ()));
}
