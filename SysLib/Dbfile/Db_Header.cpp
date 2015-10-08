//********************************************************* 
//	Db_Header.cpp - general database interface
//*********************************************************

#include "Db_Header.hpp"

#include "String.hpp"

//-----------------------------------------------------------
//	Db_Header constructors
//-----------------------------------------------------------

Db_Header::Db_Header (Access_Type access, Format_Type format) : 
	Db_Base (access, format)
{
	Header_Lines (1);
	Scan_Lines ();
	Custom_Header (false);
	LinkDir_Type (LINK_DIR);
	Sort_Type (UNKNOWN_SORT);
	notes = num_nest = -1;
}

Db_Header::Db_Header (string filename, Access_Type access, Format_Type format) : 
	Db_Base (access, format)
{
	Header_Lines (1);
	Scan_Lines ();
	Custom_Header (false);
	LinkDir_Type (LINK_DIR);
	Sort_Type (UNKNOWN_SORT);
	notes = num_nest = -1;

	Open (filename);
}
//---------------------------------------------------------
//	Db_Header destructor
//---------------------------------------------------------

Db_Header::~Db_Header (void)
{
	Close ();
}

//-----------------------------------------------------------
//	Open
//-----------------------------------------------------------

bool Db_Header::Open (string filename)
{
	bool stat;
	Format_Type format_type;

	if (Dbase_Format () == DEFAULT_FORMAT) {
		Dbase_Format (DEFAULT_FORMAT);
	}
	if (!filename.empty ()) {
		Filename (filename);
	}

	//---- read the definition file if it exists ----

	filename = Filename ();

	if (!filename.empty () && File_Access () != CREATE) {
		stat = Read_Def_Header ();
	} else {
		stat = false;
	}

	//---- open the file ----

	if (Db_File::Open ()) {
		if (File_Access () != CREATE) {

			//---- process the header ----

			if (!stat && Header_Lines () == 0 && Num_Fields () != 0) {
				stat = Write_Def_Header ("");
			}
			if (!Read_Header (stat)) {
				return (Status (OPEN_DEF));
			}
		} else {
			if (!Custom_Header ()) {
				//if (Part_Number () <= 0 || Num_Fields () == 0) {
				if (Num_Fields () == 0) {
					Create_Fields ();
				}
				if (Num_Fields () > 0) {
					Write_Header ();
				}
			}
		}
		format_type = Dbase_Format ();

		if (format_type != exe->Default_Format () && First_Open () && exe->Send_Messages ()) {
			String text, result, nesting;
			text = File_Type ();

			for (;;) {
				text.Split_Last (result);
				if (!result.Starts_With ("#")) break;
				if (!nesting.empty ()) {
					result += " " + nesting;
				}
				nesting = result;
			}
			if (nesting.empty ()) {
				exe->Print (1, text) << " Format = " << Format_Code (format_type);
			} else {
				exe->Print (1, text) << " Format " << nesting << " = " << Format_Code (format_type);
			}
		}
		return (true);
	}
	return (false);
}

bool Db_Header::Open (int ext_number)
{
	if (File_Access () != CREATE && Num_Fields () > 0) Clear_Fields ();

	return (Db_File::Open (ext_number)); 
}

//-----------------------------------------------------------
//	Close
//-----------------------------------------------------------

bool Db_Header::Close (void)
{
	if (!Is_Open ()) return (Db_File::Close ());

	if ((Dbase_Format () == DBASE || Dbase_Format () == ARCVIEW) && File_Access () != READ) {
		int records = Max_Record_Number ();
		if (records < Num_Records ()) {
			records = Num_Records ();
		}
		if (records > 0) {
			dBase_Header header;
			if (!Read (&header, sizeof (header), 0L)) {
				return (Status (FILE_HEADER));
			}
			//---- update the database header ----

			if (header.num_records < records) {
				header.num_records = records;
				if (!Write (&header, sizeof (header), 0L)) {
					return (Status (HEADER_WRITE));
				}
			}
		}
	}
	return (Db_File::Close ());
}

//-----------------------------------------------------------
//	Read_Header
//-----------------------------------------------------------

bool Db_Header::Read_Header (bool stat)
{
	Offset (0L);

	if (File_Format () == CSV_DELIMITED) {
		if (!Read_CSV_Header (stat)) {
			return (Status (FILE_HEADER));
		}
	} else if (Dbase_Format () == DBASE || Dbase_Format () == ARCVIEW) {
		if (!Read_dBase_Header (stat)) return (false);
	} else if (File_Format () == SQLITE3) {
		if (!Read_SQLite3_Header (stat)) return (false);
	} else if (!stat) {
		return (Status (OPEN_DEF));
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Nesting
//-----------------------------------------------------------

void Db_Header::Set_Nesting (bool flag) 
{
	if (flag) {
		Nest (NESTED);
		Header_Lines (2);
	} else {
		Nest (NO_NEST);
		Header_Lines (1);
	}
}

//-----------------------------------------------------------
//	Write_Header
//-----------------------------------------------------------

bool Db_Header::Write_Header (string user_lines) 
{
	Offset (0L);

	if (user_lines.empty ()) {
		user_lines = Header_Record ();
	}
	if (File_Format () == CSV_DELIMITED) {
		if (!Write_CSV_Header (user_lines)) return (false);
	} else if (Dbase_Format () == DBASE || Dbase_Format () == ARCVIEW) {
		if (!Write_dBase_Header ()) return (false);
	} else if (File_Format () == SQLITE3) {
		if (!Write_SQLite3_Header ()) return (false);
	}
	if (!Write_Def_Header (user_lines)) return (false);

	return (Set_Field_Numbers ());
}

//---------------------------------------------------------
//	Flatten_File
//---------------------------------------------------------

void Db_Header::Flatten_File (void) 
{
	if (!Nest_Flag ()) return;

	Field_Array temp_fld;
	Field_Itr fld_itr;
	Nest_Type nest;

	temp_fld.swap (field);

	Nest (FLATTEN);
	if (Header_Lines () > 1) {
		Header_Lines (Header_Lines () - 1);
	}
	for (int i=0; i < 2; i++) {
		for (fld_itr = temp_fld.begin (); fld_itr != temp_fld.end (); fld_itr++) {
			if (fld_itr->Nested ()) {
				if (i == 0) continue;
				nest = FLATTEN;
			} else {
				if (i == 1) continue;
				nest = fld_itr->Nest ();
			}
			Add_Field (fld_itr->Name (), fld_itr->Type (), fld_itr->Size (), fld_itr->Units (), false, nest);
		}
	}
	Write_Header ();
}

//-----------------------------------------------------------
//	Read_CSV_Header
//-----------------------------------------------------------

bool Db_Header::Read_CSV_Header (bool stat)
{
	int i, j, k, len, nfield, decimal, exponent;
	String buffer, field;
	char *ptr, *p, ch;
	Db_Field *fld_ptr;
	Format_Type type;

	if (Header_Lines () == 0) return (stat);

	type = Dbase_Format ();
	File_Format (UNFORMATED);
	Record_Format (type);

	//---- read the database header ----

	len = 0;

	if (Nest_Flag ()) {
		for (i=2; i < Header_Lines (); i++) {
			if (!Read (false)) return (false);
			buffer += Record_String ();
			len += Record_Size ();
		}
		if (!Read (false)) return (false);
		len += Record_Size ();

		if (!Read (true)) return (false);
		len += Record_Size ();

		Nested (false);
	} else {
		for (i=1; i <= Header_Lines (); i++) {
			if (!Read ()) return (false);
			len += Record_Size ();

			if (i < Header_Lines ()) {
				buffer += Record_String ();
			}
		}
	}
	Header_Record (buffer);
	First_Offset (len);

	//---- validate the definition file ----

	if (stat) {
		Db_Field *fld_ptr;

		nfield = 0;

		for (i=0; i < 2; i++) {
			buffer = Record_String ();

			while (buffer.Split (field, Delimiters ())) {
				fld_ptr = Field (nfield++);

				if (fld_ptr == 0 || !field.Equals (fld_ptr->Name ())) {
					exe->Write (1, String ("Field %d does not match %s != %s") % nfield % 
						((fld_ptr != 0) ? fld_ptr->Name () : "NULL") % field);
					nfield = 0;
					break;
				}
			}
			if (!Nest_Flag () || nfield == 0) break;
			Nested (true);
		}
		if (nfield != Num_Fields ()) {
			Clear_Fields ();
			stat = false;
		}
	}

	//---- if a definition file was read, skip the field scan ----

	if (!stat) {
		if (Nest_Flag ()) {
			return (Status (DEF_NEST));
		}
		ptr = Record_String ();

		//---- refine the delimiter type ----

		if (Record_Format () == CSV_DELIMITED) {
			
			//---- scan for tabs ----

			i = 0;
			p = ptr;

			while (p != 0) {
				p = strchr (p, '\t');
				if (p != 0) {
					i++;
					p++;
				}
			}

			//---- scan for commas ----

			j = 0;
			p = ptr;

			while (p != 0) {
				p = strchr (p, ',');
				if (p != 0) {
					j++;
					p++;
				}
			}

			//---- reset the delimiter ----

			if (i == 0 && j == 0) {
				Record_Format (type = SPACE_DELIMITED);
			} else if (i == 0 || j > i * 2) {
				Record_Format (type = COMMA_DELIMITED);
			} else if (j == 0 || i > j * 2) {
				Record_Format (type = TAB_DELIMITED);
			}
		}

		//---- process the header record ----

		buffer = ptr;

		while (buffer.Split (field, Delimiters ())) {
			ch = field [0];
			if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_') {
				if (Add_Field (field, DB_INTEGER, 1) < 0) return (false);
			} else {
				File_Format (type);
				return (false);
			}
		}
		nfield = Num_Fields ();

		if (nfield == 0) return (Status (NO_FIELD));

		//---- scan the file for field information ----

		for (k=0; k < scan_lines || scan_lines == SCAN_ALL_RECORDS; k++) {
			if (!Read ()) break;

			//---- read the data fields ----

			buffer = Record_String ();

			for (i=0; i < nfield; i++) {
				if (!buffer.Split (field, Delimiters ())) continue;

				//---- change NA to zero ----

				if (field.Equals ("NA")) {
					field = "0";
				}
				fld_ptr = Field (i);
				if (fld_ptr == 0) return (Status (NO_FIELD));

				//---- check the field size ----

				len = (int) field.size ();
				if (scan_lines != SCAN_ALL_RECORDS && len < 10) {
					if (fld_ptr->Width () < 10) {
						fld_ptr->Size (10);
					}
				} else {
					if (fld_ptr->Width () < len) fld_ptr->Size (len);
				}

				//---- check for non-numbers and decimal points ----

				decimal = exponent = 0;

				for (j=0; j < len; j++) {
					ch = field [j];

					if (ch < '0' || ch > '9') {
						if (ch == '.' && decimal == 0) {
							decimal = len - j - 1;
							continue;
						}
						if (ch == '-') {
							if (j == 0) continue;
							if (exponent > 0 && j == exponent + 1) continue;
						}
						if (ch == 'E' && exponent == 0 && decimal != 0) {
							exponent = j;
							continue;
						}
						break;
					}
				}
				if (j < len) {
					fld_ptr->Type (DB_STRING);
				} else if (decimal) {
					fld_ptr->Type (DB_DOUBLE);
					if (decimal > fld_ptr->Decimal ()) {
						fld_ptr->Size ((double) fld_ptr->Width () + (decimal / 10.0));
					}
				}
			}
		}

		//---- set the number of records ----

		if (scan_lines == SCAN_ALL_RECORDS) {
			Num_Records (k);
		}
	}

	//---- check the header alignment ----

	if (scan_lines != 0) {
		Rewind ();
	}

	//---- restore the file format ----

	File_Format (type);

	if (!stat) {
		Write_Def_Header (Header_Record ());
	}
	return (Record_Number (-1));
}

//-----------------------------------------------------------
//	Write_CSV_Header
//-----------------------------------------------------------

bool Db_Header::Write_CSV_Header (string user_lines) 
{
	int i, l, len, nfield, lines;
	char *name, *ptr, *nptr, delimiter;
	bool first;
	Db_Field *fld_ptr;

	if (Header_Lines () <= 0) return (true);

	fstream &fh = File ();
	if (!fh.is_open ()) return (Status (NOT_OPEN));

	//---- write extra header lines ----

	if (Nest_Flag ()) {
		lines = 2;
	} else {
		lines = 1;
	}

	if (Header_Lines () > lines) {
		ptr = (char *) user_lines.c_str ();

		for (i=lines; i < Header_Lines (); i++) {
			if (ptr != 0) {
				nptr = strchr (ptr, '\n');

				if (nptr != 0) {
					len = (int) (nptr - ptr);
					fh << (String ("%*.*s") % len % len % ptr) << endl;
					ptr = nptr + 1;
				} else {
					fh << ptr << endl;
					ptr = 0;
				}
			} else {
				fh << endl;
			}
		}
	}
	nfield = Num_Fields ();
	if (nfield == 0) return (Status (NO_FIELD));

	//---- setup the output record ----

	delimiter = *(Delimiters ());

	//--- write each field name ----

	for (l=0; l < lines; l++) {
		first = true;

		for (i=0; i < nfield; i++) {
			fld_ptr = Field (i);

			if (fld_ptr->Nest () != NO_NEST && fld_ptr->Nest () != FLATTEN) {
				if (l == 0) continue;
			} else if (l == 1) {
				continue;
			}
			if (!first) {
				fh << delimiter;
			} else {
				first = false;
			}
			name = (char *) fld_ptr->Name ().c_str ();

			if (name != 0) {
				if (strstr (name, Delimiters ())) {
					fh << "\"" << name << "\"";
				} else {
					fh << name;
				}
			}
		}
		fh << endl;
	}

	//---- end the header line ----
	
	First_Offset (Offset ());

	return (Record_Number (-1));
}

//-----------------------------------------------------------
//	Read_dBase_Header
//-----------------------------------------------------------

bool Db_Header::Read_dBase_Header (bool stat)
{
	int i, nfield, offset;
	double size;
	char marker;
	String name;
	Field_Type type;

	dBase_Header header;
	dBase_Field field;

	//---- read the database header ----

	if (!Read (&header, sizeof (header), 0L)) {
		return (Status (FILE_HEADER));
	}
	nfield = (header.header_size - sizeof (header) - 1) / sizeof (field);
	offset = 1;		//---- delete marker ----

	//---- validate the definition file ----

	if (stat) {
		if (Num_Fields () == nfield && Num_Fields () > 0) {
			Db_Field *fld_ptr;

			for (i=0; i < nfield; i++) {
				if (!Read (&field, sizeof (field))) {
					return (Status (FIELD_LIST));
				}
				fld_ptr = Field (i);
				name = field.name;

				if (fld_ptr == 0 || !fld_ptr->Name ().Starts_With (name)) {
					stat = false;
					break;
				}
			}
			if (!stat) {
				Read (&header, sizeof (header), 0L);
				Clear_Fields ();
			}
		} else {
			stat = false;
			Clear_Fields ();
		}
	}

	//---- read each field ----

	if (!stat) {
		for (i=0; i < nfield; i++) {
			if (!Read (&field, sizeof (field))) {
				return (Status (FIELD_LIST));
			}
			marker = field.type;
			if (marker > 90) marker -= 32;

			switch (marker) {
				case 'C':
					type = DB_STRING;
					break;
				case 'D':
					type = DB_STRING;
					break;
				case 'L':
					type = DB_CHAR;
					break;
				case 'N':
					if (field.decimal) {
						type = DB_DOUBLE;
					} else {
						type = DB_INTEGER;
					}
					break;
				case 'F':
					type = DB_DOUBLE;
					break;
				default:
					offset += field.width;
					continue;
			}
			name = field.name;
			size = (double) field.width + (field.decimal / 10.0);

			if (Add_Field (name, type, size, NO_UNITS, false, NO_NEST, offset) < 0) {
				return (false);
			}
			offset += field.width;
		}
	}

	//---- read the end of header marker ----

	Read (&marker, sizeof (char));

	if (marker != 13) {
		return (Status (RECORD_SYNTAX));
	}

	//---- set the record information ----

	Max_Size (header.record_size);
	First_Offset (header.header_size);
	Num_Records (header.num_records);

	if (!stat) {
		Write_Def_Header ("");
	}
	return (Rewind ());
}

//-----------------------------------------------------------
//	Write_dBase_Header
//-----------------------------------------------------------

bool Db_Header::Write_dBase_Header (void) 
{
	char marker, *name;
	int i, j, nfield, size, max_size, num_field;
	time_t ltime;
	struct tm *dt;
	Db_Field *fld;

	dBase_Header header;
	dBase_Field field;

	nfield = Num_Fields ();

	if (nfield == 0) return (Status (NO_FIELD));

	//---- get the record size ----

	max_size = num_field = 0;

	for (i=0; i < nfield; i++) {
		fld = Field (i);
		if (fld == 0) continue;

		size = fld->Offset () + fld->Width ();

		if (size > max_size) {
			max_size = size;
		}
		num_field++;
	}

	//---- write the header ----

	time (&ltime);
	dt = local_time (&ltime);

	memset (&header, '\0', sizeof (header));

	header.date [0] = (char) dt->tm_year;
	header.date [1] = (char) (dt->tm_mon + 1);
	header.date [2] = (char) dt->tm_mday;

	header.version = 3;
	header.num_records = Num_Records ();
	header.header_size = (short) (sizeof (header) + num_field * sizeof (dBase_Field) + 1);
	header.record_size = (short) max_size;
	
	if (!Write (&header, sizeof (header), 0L)) {
		return (Status (HEADER_WRITE));
	}

	//---- write each field ----

	for (int i=0; i < nfield; i++) {
		fld = Field (i);
		if (fld == 0) continue;

		memset (&field, '\0', sizeof (field));

		switch (fld->Type ()) {
			case DB_TIME:
			case DB_STRING:
			case DB_CHAR:
				field.type = 'C';
				break;
			case DB_DOUBLE:
				field.type = 'F';
				break;
			case DB_INTEGER:
			case DB_UNSIGNED:
			case DB_FIXED:
				field.type = 'N';
				break;
		}
		name = (char *) fld->Name ().c_str ();
		size = (int) fld->Name ().size ();
		
		if (size > (int) sizeof (field.name)) {
			size = (int) sizeof (field.name);
		}
		for (j=0; j < size; j++, name++) {
			field.name [j] = *name;
		}
		field.width = (char) (fld->Width () & 0xff);
		field.decimal = (char) (fld->Decimal () & 0xff);

		if (!Write (&field, sizeof (field))) {
			return (Status (HEADER_WRITE));
		}
	}
	marker = 13;

	if (!Write (&marker, sizeof (char))) {
		return (Status (HEADER_WRITE));
	}
	Record_Size (0);
	Record_Size (header.record_size);
	First_Offset (header.header_size);

	return (Record_Number (-1));
}

//-----------------------------------------------------------
//	Read_SQLite3_Header
//-----------------------------------------------------------

bool Db_Header::Read_SQLite3_Header (bool stat)
{
	int i, nfield, nnest, num_rows, length;
	String sql, name;
	Field_Type type = DB_INTEGER;

	//---- get the number of rows ----

	First_Offset (0);
	num_rows = Num_Records ();

	//---- create the read statements ----

	sql ("SELECT * FROM %s WHERE ROWID=?;") % File_ID ();

	if (sqlite3_prepare_v2 (db_file, sql.c_str (), -1, &read_stmt, 0) != SQLITE_OK) {
		exe->Warning ("Preparing Read Statement: ") << sqlite3_errmsg (db_file);
	}
	nfield = sqlite3_column_count (read_stmt);

	if (Nest_Flag ()) {
		sql ("SELECT * FROM %s_Nest WHERE PARENTID=?;") % File_ID ();

		if (sqlite3_prepare_v2 (db_file, sql.c_str (), -1, &read_nest, 0) != SQLITE_OK) {
			exe->Warning ("Preparing Read Statement: ") << sqlite3_errmsg (db_file);
		}
		nnest = sqlite3_column_count (read_nest) - 1;
	} else {
		nnest = 0;
	}

	//---- validate the definition file ----

	if (stat) {
		if (Num_Fields () == (nfield + nnest) && Num_Fields () > 0) {
			for (i=0; i < nfield; i++) {
				name = sqlite3_column_name (read_stmt, i);
				if (Field_Number (name) != i) {
					stat = false;
					break;
				}
			}
			for (i=0; i < nnest; i++) {
				name = sqlite3_column_name (read_nest, i);
				if (Field_Number (name) != (nfield + i)) {
					stat = false;
					break;
				}
			}
			if (!stat) {
				Clear_Fields ();
			}
		} else {
			stat = false;
			Clear_Fields ();
		}
	}

	//---- read each field ----

	if (!stat) {
		int result, sqlite_type;
		double size = 0;

		sqlite3_bind_int (read_stmt, 1, num_rows);
		result = sqlite3_step (read_stmt);

		if (result != SQLITE_ROW && result != SQLITE_DONE) {
			exe->Warning ("SQLITE3 Database Table ") << File_ID () << ": " << sqlite3_errmsg (db_file);
			return (false);
		}
		length = 0;
		Record_Size (0);

		for (i=0; i < nfield; i++) {
			sqlite_type = sqlite3_column_type (read_stmt, i);

			switch (sqlite_type) {
				case SQLITE_INTEGER:
					type = DB_INTEGER;
					size = 4;
					break;
				case SQLITE_FLOAT:
					type = DB_DOUBLE;
					size = 8.2;
					break;
				case SQLITE_TEXT:
				case SQLITE_BLOB:
				case SQLITE_NULL:
				default:
					type = DB_STRING;
					size = STRING_FIELD_SIZE;
					break;
			}
			name = sqlite3_column_name (read_stmt, i);

			if (Add_Field (name, type, size, NO_UNITS, true, NO_NEST, length) < 0) {
				return (false);
			}
			length += (int) size;
		}
		sqlite3_reset (read_stmt);

		//---- nested fields ----

		if (Nest_Flag ()) {
			sqlite3_bind_int (read_nest, 1, num_rows);
			result = sqlite3_step (read_nest);

			if (result != SQLITE_ROW && result != SQLITE_DONE) {
				exe->Warning (String ("SQLITE3 Database Table %s_Nest: %s") % File_ID () % sqlite3_errmsg (db_file));
				return (false);
			}
			length = 0;
			Record_Size (0);

			for (i=0; i < nnest; i++) {
				sqlite_type = sqlite3_column_type (read_nest, i);

				switch (sqlite_type) {
					case SQLITE_INTEGER:
						type = DB_INTEGER;
						size = 4;
						break;
					case SQLITE_FLOAT:
						type = DB_DOUBLE;
						size = 8.2;
						break;
					case SQLITE_TEXT:
					case SQLITE_BLOB:
					case SQLITE_NULL:
						type = DB_STRING;
						size = STRING_FIELD_SIZE;
						break;
				}
				name = sqlite3_column_name (read_nest, i);

				if (Add_Field (name, type, size, NO_UNITS, true, NO_NEST, length) < 0) {
					return (false);
				}
				length += (int) size;
			}
			sqlite3_reset (read_nest);
		}
	}

	//---- set the record information ----

	if (!stat) {
		Write_Def_Header ("");
	}
	return (Record_Number (-1));
}

//-----------------------------------------------------------
//	Write_SQLite3_Header
//-----------------------------------------------------------

bool Db_Header::Write_SQLite3_Header (void) 
{
	String name, sql;
	int i, j, nfield;
	Db_Field *fld;

	nfield = Num_Fields ();

	if (nfield == 0) return (Status (NO_FIELD));

	//---- drop the table if it currently exists ----

	sql ("DROP TABLE %s;") % File_ID ();
	sqlite3_exec (db_file, sql.c_str (), 0, 0, 0);

	//---- create the database table ----

	sql ("CREATE TABLE %s (") % File_ID ();

	for (i=0, j=1; i < nfield; i++) {
		fld = Field (i);
		if (!fld->Nested ()) {
			if (fld->Type () == DB_INTEGER) {
				name = "INTEGER";
			} else if (fld->Type () == DB_DOUBLE) {
				name = "FLOAT";
			} else {
				name = "TEXT";
			}
			sql += (j++ == 1) ? " " : ", ";
			sql += fld->Name ();
			sql += " ";
			sql += name;
		}
	}
	sql += " );";

	if (sqlite3_exec (db_file, sql.c_str (), 0, 0, 0) != SQLITE_OK) {
		exe->Warning ("Creating Database Table: ") << sqlite3_errmsg (db_file);
	}

	//---- create the read statements ----

	sql ("SELECT * FROM %s WHERE ROWID=?;") % File_ID ();

	if (sqlite3_prepare_v2 (db_file, sql.c_str (), -1, &read_stmt, 0) != SQLITE_OK) {
		exe->Warning ("Preparing Read Statement: ") << sqlite3_errmsg (db_file);
	}

	//---- create the insert statements ----

	sql ("INSERT INTO %s VALUES (") % File_ID ();

	for (i=0, j=1; i < nfield; i++) {
		fld = Field (i);
		if (!fld->Nested ()) {
			sql += (j++ == 1) ? "?" : ",?";
		}
	}
	sql += " );";

	if (sqlite3_prepare_v2 (db_file, sql.c_str (), -1, &insert_stmt, 0) != SQLITE_OK) {
		exe->Warning ("Preparing Insert Statement: ") << sqlite3_errmsg (db_file);
	}

	//---- create the nested table ----
	
	if (Nest_Flag ()) {

		//---- drop the table if it currently exists ----

		sql ("DROP TABLE %s_Nest;") % File_ID ();
		sqlite3_exec (db_file, sql.c_str (), 0, 0, 0);

		//---- create the database table ----

		sql ("CREATE TABLE %s_Nest (PARENTID INTEGER") % File_ID ();

		for (i=0; i < nfield; i++) {
			fld = Field (i);
			if (fld->Nested ()) {
				if (fld->Type () == DB_INTEGER) {
					name = "INTEGER";
				} else if (fld->Type () == DB_DOUBLE) {
					name = "FLOAT";
				} else {
					name = "TEXT";
				}
				sql += ", ";
				sql += fld->Name ();
				sql += " ";
				sql += name;
			}
		}
		sql += " );";

		if (sqlite3_exec (db_file, sql.c_str (), 0, 0, 0) != SQLITE_OK) {
			exe->Warning ("Creating Database Table: ") << sqlite3_errmsg (db_file);
		}

		//---- create the read statements ----

		sql ("SELECT * FROM %s_Nest WHERE PARENTID=?;") % File_ID ();

		if (sqlite3_prepare_v2 (db_file, sql.c_str (), -1, &read_nest, 0) != SQLITE_OK) {
			exe->Warning ("Preparing Read Statement: ") << sqlite3_errmsg (db_file);
		}

		//---- create the insert statements ----

		sql ("INSERT INTO %s_Nest VALUES (?") % File_ID ();

		for (i=0; i < nfield; i++) {
			fld = Field (i);
			if (fld->Nested ()) {
				sql += ",?";
			}
		}
		sql += " );";

		if (sqlite3_prepare_v2 (db_file, sql.c_str (), -1, &insert_nest, 0) != SQLITE_OK) {
			exe->Warning ("Preparing Insert Statement: ") << sqlite3_errmsg (db_file);
		}
	}

	//---- inialized the processing block ----

	sqlite3_exec (db_file, "BEGIN", 0, 0, 0);

	return (Record_Number (-1));
}

//---------------------------------------------------------
//	Def_Format
//---------------------------------------------------------

Format_Type Db_Header::Def_Format (string filename) 
{
	Format_Type type = UNFORMATED;
	char buffer [512];
	char *delim = ",\t";
	String name, record, token;
	FILE *def;

	//---- construct the definition filename ----

	if (filename.empty ()) return (type);

	name = filename + ".def";

	def = f_open (name, "rt");
	if (def == 0) return (type);

	//---- get the header line ----

	if (fgets (buffer, sizeof (buffer), def) == 0) return (type);

	record = buffer;
	fclose (def);

	//---- TRANSIMS version code ----

	record.Split (token, delim);

	if (!token.Starts_With ("TRANSIMS")) return (type);

	//---- file format ----

	record.Split (token, delim);

	return (Format_Code (token));
}

//-----------------------------------------------------------
//	Read_Def_Header
//-----------------------------------------------------------

bool Db_Header::Read_Def_Header (void) 
{
	bool binary, matrix;
	int num, width, decimal, offset, lines;
	double size;
	Nest_Type nest;
	Field_Type type;
	Units_Type units;
	char buffer [512];
	char *delim = ",\t";
	String name, record, token;
	FILE *def;

	//---- construct the definition filename ----

	name = Filename () + ".def";

	//---- open the definition file ----

	def = f_open (name, "rt");

	if (def == 0) return (Default_Definition ());

	//---- get the header line ----

	if (fgets (buffer, sizeof (buffer), def) == 0) goto error;

	record = buffer;

	//---- TRANSIMS version code ----

	record.Split (token, delim);

	if (token.Starts_With ("AECOM HEADER")) {
		Version (40);
	} else {
		if (!token.Starts_With ("TRANSIMS")) goto error;
		token.erase (0, 8);
		num = token.Integer ();
		if (num < 40 || num > 99) goto error;
		Version (num);
	}

	//---- file format ----

	record.Split (token, delim);

	Dbase_Format (token);

	matrix = (Dbase_Format () == TPPLUS || Dbase_Format () == TRANSCAD);
	binary = (matrix || Dbase_Format () == BINARY || Dbase_Format () == SQLITE3);

	//---- number of header lines ----

	record.Split (token, delim);

	Header_Lines (token.Integer ());

	//---- nested file flag ----

	if (record.Split (token, delim)) {
		if (Check_Nest_Code (token)) {
			nest = Nest_Code (token);
			if (nest == NESTED) {
				lines = 2;
			} else {
				lines = 1;
			}
			if (record.Split (token, delim)) {
				Sort_Type (Trip_Sort_Code (token));
			}
		} else {
			nest = NO_NEST;
			lines = 1;

			Sort_Type (Trip_Sort_Code (token));
		}
	} else {
		nest = NO_NEST;
		lines = 1;
	}
	Nest (nest);

	if (binary || Dbase_Format () == FIXED_COLUMN) {
		Header_Lines (Header_Lines () + lines);
	}

	for (int i=lines; i < Header_Lines (); i++) {
		if (fgets (buffer, sizeof (buffer), def) == 0) {
			exe->Error ("Reading the Header Lines");
		}
		Header_Record (buffer);
	}

	//---- read the fields ----

	while (fgets (buffer, sizeof (buffer), def) != 0) {
		record = buffer;
		record.Clean ();

		record.Split (name, delim);
		if (name.empty ()) continue;

		record.Split (token, delim);
		type = Field_Code (token);

		record.Split (token, delim);
		offset = token.Integer ();
		if (matrix) {
			if (offset != Num_Fields () + 1) goto error;
			offset = -1;
		}
		units = NO_UNITS;

		if (Version () > 40) {
			record.Split (token, delim);
			size = token.Double ();

			if (record.Split (token)) {
				units = Units_Code (token);
			}
		} else {
			record.Split (token, delim);
			width = token.Integer ();

			record.Split (token, delim);
			decimal = token.Integer ();

			size = (double) width + (decimal / 10.0);
		}
		record.Split (token, delim);
		if (!token.empty ()) {
			nest = Nest_Code (token);
		} else {
			nest = NO_NEST;
		}
		if (Nest_Flag () && units == NEST_COUNT) {
			num_nest = Num_Fields ();
		}
		if (Add_Field (name, type, size, units, binary, nest, offset) < 0) goto error;
	}
	fclose (def);
	return (true);

error:
	return (Status (DEF_FORMAT));
}

//-----------------------------------------------------------
//	Write_Def_Header
//-----------------------------------------------------------

bool Db_Header::Write_Def_Header (string user_lines) 
{
	int i, l, num_lines, lines, len, offset;
	bool matrix;
	string filename;
	char *ptr, *nptr;
	FILE *def;
	Field_Ptr fld;

	//---- construct the definition filename ----

	filename = Filename () + ".def";

	//---- create the definition file ----

	def = f_open (filename, "wt");

	if (def == 0) {
		return (Status (OPEN_DEF));
	}

	if (Nest_Flag ()) {
		lines = 2;
	} else {
		lines = 1;
	}
	num_lines = Header_Lines ();

	matrix = (Dbase_Format () == TPPLUS || Dbase_Format () == TRANSCAD);

	if (matrix || Dbase_Format () == BINARY || Dbase_Format () == FIXED_COLUMN || Dbase_Format () == SQLITE3) {
		num_lines -= lines;
	}

	//---- write the header line ----

	if (Nest () != NO_NEST) {
		if (fprintf (def, "TRANSIMS%d, %s, %d, %s", Version (), Format_Code (Dbase_Format ()), 
			num_lines, Nest_Code (Nest ())) < 0) goto error;
	} else {
		if (fprintf (def, "TRANSIMS%d, %s, %d", Version (), Format_Code (Dbase_Format ()), 
			num_lines) < 0) goto error;
	}
	if (!Sorted ()) {
		fprintf (def, "\n");
	} else {
		fprintf (def, ", %s\n", Trip_Sort_Code (Sort_Type ()));
	}
	
	//---- write extra header lines ----

	if (Header_Lines () > lines) {
		ptr = (char *) user_lines.c_str ();

		for (i=lines; i < Header_Lines (); i++) {
			if (ptr != 0) {
				nptr = strchr (ptr, '\n');

				if (nptr != 0) {
					len = (int) (nptr - ptr);
					fprintf (def, "%*.*s\n", len, len, ptr);
					ptr = nptr + 1;
				} else {
					fprintf (def, "%s\n", ptr);
					ptr = 0;
				}
			} else {
				fprintf (def, "\n");
			}
		}
	}

	//---- write the fields ----

	for (l=0; l < lines; l++) {
		for (i=0; i < Num_Fields (); i++) {
			fld = Field (i);

			if (fld->Nest () == NO_NEST || fld->Nest () == FLATTEN) {
				if (l > 0) continue;
			} else {
				if (l == 0) continue;
			}
			if (matrix) {
				offset = i + 1;
			} else {
				offset = fld->Offset ();
			}
			if (fprintf (def, "%s, %s, %d, %d", fld->Name ().c_str (), Field_Code (fld->Type ()), 
				offset, fld->Width ()) < 0) goto error;

			if (fld->Type () == DB_DOUBLE || fld->Type () == DB_FIXED || fld->Decimal () > 0) {
				if (fprintf (def, ".%d", fld->Decimal ()) < 0) goto error;
			}
			if (fld->Units () != NO_UNITS || fld->Nest () != NO_NEST) {
				if (fprintf (def, ", %s", Units_Code (fld->Units ())) < 0) goto error;

				if (fld->Nest () != NO_NEST) {
					if (fprintf (def, ", %s", Nest_Code (fld->Nest ())) < 0) goto error;
				}
			}
			fprintf (def, "\n");
		}
	}
	fclose (def);
	return (true);

error:
	return (Status (DEF_FORMAT));
}

//-----------------------------------------------------------
//	Add_LinkDir_Field
//-----------------------------------------------------------

void Db_Header::Add_LinkDir_Field (Nest_Type nest)
{
	if (Dbase_Format () == VERSION3 || LinkDir_Type () == LINK_NODE) {
		Add_Field ("NODE", DB_INTEGER, 10, NO_UNITS, false, nest);
	} else if (LinkDir_Type () == LINK_DIR) {
		Add_Field ("DIR", DB_INTEGER, 1, NO_UNITS, false, nest);
	}
}

//-----------------------------------------------------------
//	LinkDir_Type_Field
//-----------------------------------------------------------

int Db_Header::LinkDir_Type_Field (void)
{
	int dir = Optional_Field ("DIR", "NODE");
	if (dir >= 0) {
		if (Optional_Field ("DIR") < 0) {
			LinkDir_Type (LINK_NODE);
		} else {
			LinkDir_Type (LINK_DIR);
		}
	} else {
		LinkDir_Type (LINK_SIGN);
	}
	return (dir);
}

//-----------------------------------------------------------
//	Set_Units
//-----------------------------------------------------------

void Db_Header::Set_Units (int fld, Units_Type units)
{
	if (fld >= 0) {
		Field_Ptr field = Field (fld);
		if (field && field->Units () == NO_UNITS) field->Units (units);
	}
}

//-----------------------------------------------------------
//	Get_Units
//-----------------------------------------------------------

Units_Type Db_Header::Get_Units (int fld)
{
	if (fld >= 0) {
		Field_Ptr field = Field (fld);
		if (field) return (field->Units ());
	}
	return (NO_UNITS);
}

//-----------------------------------------------------------
//	Num_Nest_Field
//-----------------------------------------------------------

void Db_Header::Num_Nest_Field (int fld)
{
	Field_Ptr field;

	if (fld < 0) {
		if (num_nest < 0) {
			for (int i=0; i < Num_Fields (); i++) {
				field = Field (i);
				if (field->Units () == NEST_COUNT) {
					num_nest = i;
					break;
				}
			}
		}
	} else {
		num_nest = fld;

		field = Field (fld);
		if (field && field->Units () == NO_UNITS) {
			field->Units (NEST_COUNT);
		}
	}
}
