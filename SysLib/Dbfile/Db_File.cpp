//*********************************************************
//	Db_File.cpp - standard database interface
//*********************************************************

#include "Db_File.hpp"

#include "String.hpp"

//---------------------------------------------------------
//	Db_File constructors
//---------------------------------------------------------

Db_File::Db_File (Access_Type access, Format_Type format) : Db_Record ()
{
	Initialize (access, format);
}

Db_File::Db_File (string filename, Access_Type access, Format_Type format) : Db_Record ()
{
	Initialize (access, format);

	Open (filename);
}

//---------------------------------------------------------
//	Db_File destructor
//---------------------------------------------------------

Db_File::~Db_File (void)
{
	Close ();
	if (read_stmt != 0) {
		sqlite3_finalize (read_stmt);
		read_stmt = 0;
	}
	if (read_nest != 0) {
		sqlite3_finalize (read_nest);
		read_nest = 0;
	}
	if (insert_stmt != 0) {
		sqlite3_finalize (insert_stmt);
		insert_stmt = 0;
	}
	if (insert_nest != 0) {
		sqlite3_finalize (insert_nest);
		insert_nest = 0;
	}
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Db_File::Initialize (Access_Type access, Format_Type format) 
{
	db_file = 0;
	insert_stmt = insert_nest = 0;
	read_stmt = read_nest = 0;
	first_offset = 0;
	num_files = num_records = record_num = max_record_num = parent_id = part_num = extend = 0;
	new_nest_flag = first_open = clean_flag = true;
	part_flag = false;

	Version (VERSION_CODE);
	File_Access (access);
	File_Format (format);

	switch (format) {
		default:
			File_Type ("Text File");
			break;
		case BINARY:
			File_Type ("Binary File");
			break;
		case FIXED_COLUMN:
			File_Type ("Fixed Column File");
			break;
		case COMMA_DELIMITED:
			File_Type ("Comma Delimited File");
			break;
		case SPACE_DELIMITED:
			File_Type ("Space Delimited File");
			break;
		case VERSION3:
		case TAB_DELIMITED:
			File_Type ("Tab Delimited File");
			break;
		case CSV_DELIMITED:
			File_Type ("CSV Delimited File");
			break;
		case DBASE:
			File_Type ("dBase File");
			break;
		case ARCVIEW:
			File_Type ("ArcView File");
			break;
		case SQLITE3:
			File_Type ("SQLite3 Database");
			break;
		case TPPLUS:
			File_Type ("TPPlus Matrix");
			break;
		case TRANSCAD:
			File_Type ("TransCAD Matrix");
			break;
	}
}

//---------------------------------------------------------
//	File_Format
//---------------------------------------------------------

void Db_File::File_Format (Format_Type format)
{
	if (format == DEFAULT_FORMAT && exe) {
		format = (Format_Type) exe->Default_Format ();
	}
	Record_Format (format);

	if (format == COMMA_DELIMITED || format == SPACE_DELIMITED || 
		format == TAB_DELIMITED || format == VERSION3) {
		file_format = CSV_DELIMITED;
	} else if (format == DBASE || format == ARCVIEW) {
		file_format = BINARY;
	} else {
		file_format = format;
	}
}

void Db_File::File_Format (string text)
{ 
	File_Format (Format_Code (text)); 
}

//---------------------------------------------------------
//	Filename
//---------------------------------------------------------

bool Db_File::Filename (string rootname)
{
	part_flag = false;
	extend = 0;
	if (rootname == filename) return (true);

	filename.clear ();
	pathname.clear ();

	if (rootname.empty ()) return (false);

	filename = rootname;

	//---- check for a partition type ----

	size_t index = filename.find (".*");

	if (index != filename.npos) {
		part_flag = true;
		filename.erase (index);
		if (f_exist (filename + ".AA")) {
			Version (40);
			extend = 1;
		} else if (f_exist (filename + ".tAA")) {
			Version (40);
			extend = 2;
		}
	} else {
		index = filename.find (".t*");

		if (index != filename.npos) {
			part_flag = true;
			filename.erase (index);

			if (f_exist (filename + ".tAA")) {
				Version (40);
				extend = 2;
			}
		} else {
			part_flag = false;
		}
	}
	if (part_flag) {
		if (exe->Single_Partition ()) {
			filename += Extension (exe->First_Partition ());
			part_flag = false;
			extend = 3;
		}
	} else if (exe->MPI_Size () > 1 && exe->Slave () && File_Access () == CREATE) {
		filename += String (".%d") % exe->MPI_Rank ();
	}
	pathname = filename;

	return (Filename (0));
}

bool Db_File::Filename (int part_index)
{
	if (part_flag) {
		if (part_index < 0) {
			return (Status (EXTENSION));
		} else if (pathname.empty ()) {
			return (Status (NULL_POINTER));
		} else {
			part_index = exe->Partition_Number (part_index);
			if (part_index < 0) {
				return (Status (NOT_OPEN));
			} else {
				filename = pathname + Extension (part_index);
			}
		}
	} else if (part_index != 0) {
		return (Status (EXTENSION));
	} else if (pathname.empty ()) {
		return (Status (NULL_POINTER));
	} else {
		filename = pathname;
	}
	return (true);
}

//---------------------------------------------------------
//	Find_File
//---------------------------------------------------------

bool Db_File::Find_File (int part_index)
{
	string name = pathname;
	
	if (part_flag) {
		part_index = exe->Partition_Number (part_index);
		if (part_index < 0) return (false);
		name += Extension (part_index);
	}
	return (f_exist (name));
}

//---------------------------------------------------------
//	Num_Parts
//---------------------------------------------------------

int Db_File::Num_Parts (void)
{
	if (part_flag) {
		int num;
		String name;

		for (int part=0; ; part++) {
			num = exe->Partition_Number (part);
			if (num < 0) return (part);
			name = pathname + Extension (num);
			if (!f_exist (name)) return (part);
		}
	} else {
		return (1);
	}
}

//---------------------------------------------------------
//	Open
//---------------------------------------------------------

bool Db_File::Open (string path)
{
	if (!path.empty ()) {
		Filename (path);
	} else if (filename.empty ()) {
		return (false);
	}
	if (Is_Open ()) {
		Close ();
	} else if (File_Format () == DEFAULT_FORMAT) {
		File_Format (DEFAULT_FORMAT);
	}
	bool create_flag, exist_flag;

	if (File_Access () == CREATE) {
		create_flag = true;
		exist_flag = f_exist (filename);
		num_records = record_num = max_record_num = 0;
	} else {
		create_flag = exist_flag = false;
	}
	if (!Db_Open (filename)) {
		if (exe->Send_Messages ()) {
			string message (((create_flag) ? "Creating " : "Opening "));
			message += File_Type ();
			exe->File_Error (message, filename);
		}
		return (Status (NOT_OPEN));
	} else {
		if (create_flag && !File_Type ().Starts_With ("New") && !File_Type ().Starts_With ("Output")) {
			File_Type (String ("New %s") % File_Type ());
		
			if (File_Format () != SQLITE3) {
				File_ID (String ("New%s") % File_ID ());
			}
		}

		//---- first open processing ----

		if (first_open) {
			string name;

			num_files = 1;

			//---- print the filename ----

			if (exe->Send_Messages ()) {
				if (!part_flag) {
					name = filename;
				} else {
					name = pathname + ".*";
					part_num = 0;
				}
				exe->Print_Filename (1, File_Type (), name);
			}

			//---- delete existing files ----

			if (exist_flag && part_flag) {
				int num;
				for (int part=1; part < 1000; part++) {
					num = exe->Partition_Number (part);
					if (num < 0) break;
					name = pathname + Extension (num);
					if (!f_exist (name)) break;
					remove (name.c_str ());
					name += ".def";
					remove (name.c_str ());
				}
			}
		}
		
		if (File_Format () == SQLITE3) {
			if (sqlite3_exec (db_file, "PRAGMA synchronous = OFF", 0, 0, 0) != SQLITE_OK) {
				return (Status (NOT_OPEN));
			}
		}
		return (Status (OK));
	}
}

bool Db_File::Open (int part_index)
{
	if (Is_Open ()) {
		Close ();
	} else if (File_Format () == DEFAULT_FORMAT) {
		File_Format (DEFAULT_FORMAT);
	}
	bool messages = exe->Send_Messages ();
	if (part_index > 0) exe->Send_Messages (false);

	bool stat = Filename (part_index);

	if (stat) {
		int hold = part_num;

		if (part_flag) {
			part_num = exe->Partition_Number (part_index);
		} else {
			part_num = -1;
		}
		stat = Open ();
		if (stat) {
			if (part_index >= num_files) num_files = part_index + 1;
			if (!part_flag) part_num = part_index;
		} else {
			part_num = hold;
		}
	}
	exe->Send_Messages (messages);
	return (stat);
}

//---------------------------------------------------------
//	Db_Open
//---------------------------------------------------------

bool Db_File::Db_Open (string filename)
{
	ios_base::openmode code;
	int  sqlite_code = 0;
	bool sqlite_flag = (File_Format () == SQLITE3);

	file.clear ();

	if (sqlite_flag && Nest_Flag ()) {
		exe->File_Error ("SQLITE3 does not support Nested Files", filename);
		return (Status (NOT_OPEN));
	}
	switch (File_Access ()) {
		default:
			return (Status (DB_ERROR));
			break;
		case READ:
			if (sqlite_flag) {
				sqlite_code = SQLITE_OPEN_READONLY;
			} else {
				code = ios_base::in;
			}
			break;
		case MODIFY:
			if (sqlite_flag) {
				sqlite_code = SQLITE_OPEN_READWRITE;
			} else {
				code = ios_base::in | ios_base::out;
			}
			break;
		case APPEND:
			if (sqlite_flag) {
				sqlite_code = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
			} else {
				code = ios_base::in | ios_base::out | ios_base::app;
			}
			break;
		case CREATE:
			if (sqlite_flag) {
				remove (filename.c_str ());
				sqlite_code = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
			} else {
				code = ios_base::in | ios_base::out | ios_base::trunc;
			}
			break;
	}

	if (sqlite_flag) {
		return (sqlite3_open_v2 (filename.c_str (), &db_file, sqlite_code, 0) == SQLITE_OK);
	} else {
		if (File_Format () == BINARY) {
			code |= ios_base::binary;
		}
		file.open (filename.c_str (), code);
		return (file.is_open ());
	}
}

//---------------------------------------------------------
//	Read
//---------------------------------------------------------

bool Db_File::Read (void)
{
	//---- read the record into the buffer ----

	bool stat = Read (Record_String (), Max_Read ());
	if (!stat) return (stat);

	if (File_Format () == CSV_DELIMITED || File_Format () == SQLITE3) {
		if (!Input_Record ()) return (Status (NO_FIELD));
	}
	return (true);
}

bool Db_File::Read (void *record, int size)
{
	//---- check the file status ----

	if (!Check_File ()) return (false);
	if (record == 0) return (Status (NULL_POINTER));
	if (size <= 0) return (Status (RECORD_SIZE));
	
	//---- read the record ----

	switch (File_Format ()) {
		case BINARY:
			if (file.read ((char *) record, size).fail ()) {
				memset (record, '\0', size);
				return (file.eof () ? false : Status (FILE_IO));
			}
			break;
		case SQLITE3:
			return (true);
		case FIXED_COLUMN:
			memset (record, ' ', size);
			file.getline ((char *) record, size + 2);
			if (file.eof ()) return (false);
			((char *) record) [size] = '\0';
			break;
		default:
			file.getline ((char *) record, size);
			if (clean_flag) Clean ((char *) record);
			size = (int) strlen ((char *) record);
			if (file.eof () && size == 0) return (false);
			break;
	}
	return (Record (record, size));
}

bool Db_File::Read (void *record, int size, off_t offset)
{
	if (!Offset (offset)) return (false);

	return (Read (record, size));
}

bool Db_File::Read_Record (int number) 
{
	if (!Record_Number (number)) return (false);

	return (Read ());
}

//---------------------------------------------------------
//	Write
//---------------------------------------------------------

bool Db_File::Write (void)
{
	if (!Check_File ()) return (false);

	if (File_Format () == CSV_DELIMITED || File_Format () == SQLITE3) {
		if (!Output_Record ()) return (Status (NO_FIELD));
	}
	return (Write (Record_String (), Record_Size ()));
}
	
bool Db_File::Write (bool nested)
{
	if (!nested && Nest () == FLATTEN) return (true);
	Nested (nested); 
	return (Write ());
}

bool Db_File::Write (void *record, int size)
{
	//---- check the file status ----

	if (!Check_File ()) return (false);
	if (record == 0) return (Status (NULL_POINTER));
	if (size <= 0) return (Status (RECORD_SIZE));
		
	//---- write the record ----

	if (File_Format () == SQLITE3) return (true);

	switch (File_Format ()) {
		case BINARY:
			file.write ((char *) record, size);
			break;
		case FIXED_COLUMN:
			file << (String ("%-*.*s") % size % size % (char *) record) << endl;
			break;
		case CSV_DELIMITED:
			file << (String ("%.*s") % size % (char *) record) << endl;
			break;
		default:
			file << (char *) record << endl;
			break;
	}
	if (file.fail ()) {
		return (Status (FILE_IO));
	}
	return (true);
}

bool Db_File::Write (void *record, int size, off_t offset)
{
	if (!Offset (offset)) return (false);

	return (Write (record, size));
}

bool Db_File::Write_Record (int number) 
{
	if (!Record_Number (number)) return (false);

	return (Write ());
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

bool Db_File::Close (void)
{
	bool stat = true;

	if (Is_Open ()) {
		first_open = false;

		if (File_Format () == SQLITE3) {
			if (read_stmt != 0) {
				sqlite3_finalize (read_stmt);
				read_stmt = 0;
			}
			if (read_nest != 0) {
				sqlite3_finalize (read_nest);
				read_nest = 0;
			}
			if (insert_stmt != 0) {
				sqlite3_finalize (insert_stmt);
				insert_stmt = 0;
			}
			if (insert_nest != 0) {
				sqlite3_finalize (insert_nest);
				insert_nest = 0;
			}
			if (db_file != 0) {
				sqlite3_exec (db_file, "COMMIT", 0, 0, 0);
				sqlite3_close (db_file);
				db_file = 0;
			}
		} else {
			file.close ();
		}
	}
	return (stat);
}

//---------------------------------------------------------
//	Rewind
//---------------------------------------------------------

bool Db_File::Rewind (void)
{
	Record_Number (-1); 

	if (File_Format () == BINARY) {
		return (Offset (First_Offset ()));
	} else if (File_Format () != SQLITE3) {
		file.clear ();
		file.seekg (0);

		if (First_Offset () > 0) {
			int len = 0;

			while (!file.getline (Record_String (), Record ().Max_Read ()).fail ()) {
				Set_Size ();
				len += Record_Size ();

				if (len >= First_Offset ()) {
					return (true);
				}
			}
			return (Status (FILE_IO));
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Offset
//---------------------------------------------------------

bool Db_File::Offset (off_t offset)
{
	if (Check_File ()) {
		if (File_Format () == SQLITE3) {
			if (offset == 0) {
				Record_Number (-1);
			} else if (Max_Size () > 0) {
				Record_Number ((int) ((offset / Max_Size ()) + 1));
			} else {
				exe->Warning (String ("Db_File::Offset on a SQLITE3 Database offset=%d") % (size_t) offset);
				return (Status (POSITIONING));
			}
		} else if (File_Format () != TPPLUS && File_Format () != TRANSCAD) {
			if (offset >= 0) {
				file.clear ();
				file.seekg (offset);
				if (!file.fail ()) return (true);
			}
			return (Status (POSITIONING));
		}
		return (true);
	}
	return (false);
}

off_t Db_File::Offset (void)
{
	off_t offset = -1;

	if (Check_File ()) {
		if (File_Format () == SQLITE3) {
			offset = 0;
			exe->Warning ("Db_File::Offset on a SQLITE3 Database");
		} else if (File_Format () != TPPLUS && File_Format () != TRANSCAD) {
			offset = (off_t) file.tellg ();

			if (offset < 0) {
				if (File_Format () == BINARY) {
					Status (POSITIONING);
				} else {
					char *rec = Record_String ();

					if (rec != 0) {
						offset = (int) strlen (rec) + 1;
					} else {
						offset = 0;
					}
				}
			}
		}
	}
	return (offset);
}

//-----------------------------------------------------------
//	File_Size
//-----------------------------------------------------------

off_t Db_File::File_Size (void)
{
	off_t offset = -1;

	if (Check_File ()) {
		if (File_Format () == SQLITE3) {
			exe->Warning ("Db_File::File_Size on a SQLITE3 Database");
			return (Status (POSITIONING));
		} else if (File_Format () != TPPLUS && File_Format () != TRANSCAD) {
			off_t current = (off_t) file.tellg ();

			file.seekg (0, ios_base::end);
			offset = (off_t) file.tellg ();

			if (offset < 0) {
				Status (POSITIONING);
			}
			if (current >= 0) {
				file.seekg (current);
			}
		}
	}
	return (offset);
}

//-----------------------------------------------------------
//	Num_Records
//-----------------------------------------------------------

int Db_File::Num_Records (void)
{
	if (!num_records && Check_File ()) {
		if (File_Format () == SQLITE3) {
			sqlite3_stmt *record_stmt;
			String sql;

			//---- get the number of rows ----

			sql ("SELECT COUNT(*) FROM %s;") % File_ID ();

			if (sqlite3_prepare_v2 (db_file, sql.c_str (), -1, &record_stmt, 0) != SQLITE_OK) {
				exe->Warning ("Preparing Record Statement: ") << sqlite3_errmsg (db_file);
			}
			sqlite3_step (record_stmt);
			num_records = sqlite3_column_int (record_stmt, 0);

			sqlite3_finalize (record_stmt);
		} else if (File_Format () == BINARY || File_Format () == FIXED_COLUMN) {
			if (Max_Size () > 0) {
				num_records = (int) (File_Size () - First_Offset () + Max_Size () - 1) / Max_Size ();
			}
		} else {
			FILE *temp = f_open (Filename (), "rt");

			if (temp != 0) {
				char buffer [4096];

				while (fgets (buffer, sizeof (buffer), temp) != 0) {
					num_records++;
				}
				fclose (temp);
			}
		}
	}
	return (num_records);
}

//-----------------------------------------------------------
//	Estimate_Records
//-----------------------------------------------------------

int Db_File::Estimate_Records (void)
{
	int num = 0;

	if (Check_File ()) {
		if (File_Format () == SQLITE3) {
			num = Num_Records ();
		} else if (File_Format () == BINARY || File_Format () == FIXED_COLUMN) {
			if (Max_Size () > 0) {
				num = (File_Size () - First_Offset () + Max_Size () - 1) / Max_Size ();
			}
		} else {
			FILE *temp = f_open (Filename (), "rt");

			if (temp != 0) {
				int cum_size = 0;
				off_t offset;
				char buffer [4096];

				while (fgets (buffer, sizeof (buffer), temp) != 0) {
					cum_size += (int) strlen (buffer);
					if (++num == 100) break;
				}
				if (cum_size > 0) {
					if (!fseek (temp, 0, SEEK_END)) {
						offset = ftell (temp);

						if (!fseek (temp, -10 * (int) sizeof (buffer), SEEK_END)) {
							int size = 0;
							int n = 0;

							while (fgets (buffer, sizeof (buffer), temp) != 0) {
								if (n != 0) size += (int) strlen (buffer);
								if (++n == 100) break;
							}
							if (n > 1) {
								cum_size = (size / (--n) + cum_size / num) / 2 + 1;
								num = 1;
							}
						}
						num *= (int) (offset / cum_size) + 1;
					}
				}
				fclose (temp);
			}
		}
	}
	return (num);
}

//-----------------------------------------------------------
//	Record_Number
//-----------------------------------------------------------

bool Db_File::Record_Number (int number)
{
	if (number == 0) {
		record_num++;
	} else if (number < 0) {
		record_num = 0;
	} else {
		if (number != record_num + 1 && (File_Format () == BINARY || File_Format () == FIXED_COLUMN)) {
			off_t offset = First_Offset () + (number - 1) * Record_Size ();
			if (!Offset (offset)) return (false);
		}
		record_num = number;
	}
	if (record_num > max_record_num) max_record_num = record_num;
	return (true);
}

//---------------------------------------------------------
//	Binary_Read
//---------------------------------------------------------

bool Db_File::Binary_Read (void *buffer, int num_rec, bool nested)
{
	//---- check the file status ----

	if (buffer == 0 || num_rec < 1 || !Check_File () || File_Format () != BINARY) return (false);

	int size = Nest_Size (nested);
	if (size <= 0) return (Status (RECORD_SIZE));
	
	//---- read the record ----

	file.read ((char *) buffer, num_rec * size);

	int num = (int) file.gcount () / size;

	if (record_num < 0) {
		record_num = num;
	} else {
		record_num += num;
	}
	if (num != num_rec) {
		char *ptr = ((char *) buffer) + num * size;
		memset (ptr, '\0', (num_rec - num) * size);
		return (file.eof () ? false : Status (FILE_IO));
	}
	return (true);
}
//---------------------------------------------------------
//	Binary_Write
//---------------------------------------------------------

bool Db_File::Binary_Write (void *buffer, int num_rec, bool nested)
{
	//---- check the file status ----

	if (buffer == 0 || num_rec < 1 || !Check_File () || File_Format () != BINARY) return (false);

	int size = Nest_Size (nested);
	if (size <= 0) return (Status (RECORD_SIZE));
	
	//---- write the record ----

	file.write ((char *) buffer, num_rec * size);

	int num = (int) file.gcount () / size;

	if (record_num < 0) {
		record_num = num;
	} else {
		record_num += num;
	}
	if (num != num_rec) {
		return (Status (FILE_IO));
	}
	return (true);
}

//---------------------------------------------------------
//	Extension
//---------------------------------------------------------

String Db_File::Extension (int part_number)
{
	String extension;

	if (part_flag && part_number >= 0) {
		if (Version () <= 40) {
			if (extend == 1) {
				extension (".%c%c") % (char) ('A' + (part_number / 26)) % (char) ('A' + (part_number % 26));
			} else if (extend == 2) {
				extension (".t%c%c") % (char) ('A' + (part_number / 26)) % (char) ('A' + (part_number % 26));
			}
		} else {
			extension (".%d") % part_number;
		}
	}
	return (extension);
}
