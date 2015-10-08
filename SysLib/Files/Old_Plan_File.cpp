//*********************************************************
//	Old_Plan_File.cpp - Version 3 Plan File Input/Output
//*********************************************************

#include "Old_Plan_File.hpp"

//---------------------------------------------------------
//	Old_Plan_File constructor
//---------------------------------------------------------

Old_Plan_File::Old_Plan_File (Access_Type access, Memory_Type memory, Sort_Type sort, int scale) : 
	Db_File (access)
{
	Setup (memory, sort, scale);
}

Old_Plan_File::Old_Plan_File (string filename, Access_Type access, Memory_Type memory, Sort_Type sort, int scale) : 
	Db_File (filename, access)
{
	Setup (memory, sort, scale);
}

//---------------------------------------------------------
//	Old_Plan_File destructor
//---------------------------------------------------------

Old_Plan_File::~Old_Plan_File (void)
{
	Close ();

	Delete_Plan ();
}

//---------------------------------------------------------
//	Setup
//---------------------------------------------------------

void Old_Plan_File::Setup (Memory_Type memory, Sort_Type sort, int scale)
{
	num_record = num_plan = num_traveler = num_trip = max_data = 0;
	plan = 0;

	File_Type ("Old Plan File");
	File_ID ("OldPlan");
	Version (40);

	Setup_Record ();

	Plan_Memory (memory);
	Plan_Sort (sort);
	Traveler_Scale (scale);
	Node_Based_Flag (true);
}

//---------------------------------------------------------
//	Open
//---------------------------------------------------------

bool Old_Plan_File::Open (int ext_number)
{
	return (Db_File::Open (ext_number));
}

bool Old_Plan_File::Open (string filename)
{
	bool stat;

	if (!filename.empty ()) {
		Filename (filename);
	}
	filename = Filename ();

	if (!filename.empty () && File_Access () != CREATE) {
		stat = Read_Plan_Def ();
	} else {
		stat = false;
	}

	//---- open the file ----

	if (Db_File::Open ()) {
		if (!stat || File_Access () == CREATE) {
			stat = Write_Plan_Def ();
		}
		return (true);
	}
	return (false);
}

//---------------------------------------------------------
//	Read
//---------------------------------------------------------

bool Old_Plan_File::Read (long offset)
{
	//---- check the file status ----

	if (!Check_File ()) return (false);
	if (plan == 0) return (Status (RECORD_SIZE));
	if (File_Access () != READ) return (Status (DB_ERROR));

	//---- move to a specified location in the file ----

	if (offset >= 0) {
		if (!Offset (offset)) return (false);
	}

	//---- allocate space ----

	if (allocate_memory) {
		if (!Setup_Record ()) return (false);
	}

	//---- read the next plan ----

	if (Record_Format () == BINARY) {
		int num_token;

		if (!Db_File::Read (plan, (sizeof (Old_Plan_Data) - sizeof (int)))) return (false);

		plan->key2 = Round (plan->key2);
		plan->duration = Round (plan->duration);
		plan->stop_time = Round (plan->stop_time);

		if (time_sort) {
			int temp = plan->key1;
			plan->key1 = plan->key2;
			plan->key2 = temp;
		}
		num_record++;
		num_plan++;

		if (Leg () == 2) {
			num_trip++;
		} else if (Leg () == 1 && Trip () == 1) {
			num_traveler++;
		}
		num_token = Tokens ();
		if (num_token > 0) {
			if (!Check_Size (num_token)) return (false);
			if (!Db_File::Read (&(plan->data [0]), num_token * sizeof (int))) return (Status (PLAN_FIELDS));

			num_record++;
		}
		return (true);
	} else {

		int field, max_field, value;
		String record, token;

		field = max_field = 0;

		while (Db_File::Read ()) {
			num_record++;

			record (Record_String ());

			//---- check for a blank record ----

			if (record.empty ()) continue;

			//---- process the plan record ----

			while (record.Split (token)) {
				field++;
				value = token.Integer ();

				switch (field) {
					case 1:		//---- traveler id ----
						Traveler (value);
						num_plan++;
						break;
					case 2:		//---- user field ----
						break;
					case 3:		//---- trip id ----
						Trip (value);
						break;
					case 4:		//---- leg id ----
						Leg (value);
						if (value == 2) {
							num_trip++;
						} else if (value == 1 && Trip () == 1) {
							num_traveler++;
						}
						break;
					case 5:		//---- time ----
						value = Round (value);
						Time (value);
						break;
					case 6:		//---- start id ----
						Start_ID (value);
						break;
					case 7:		//---- start type ----
						Start_Type (value);
						break;
					case 8:		//---- end id ----
						End_ID (value);
						break;
					case 9:		//---- end type ----
						End_Type (value);
						break;
					case 10:	//---- duration ----
						value = Round (value);
						Duration (value);
						break;
					case 11:	//---- stop time ----
						value = Round (value);
						Stop_Time (value);
						break;
					case 12:	//---- max time flag ----
						break;
					case 13:	//---- cost ----
						Cost (value);
						break;
					case 14:	//---- gcf ----
						GCF (value);
						break;
					case 15:	//---- driver flag ----
						Driver_Flag (value);
						break;
					case 16:	//---- mode ----
						Mode (value);
						break;
					case 17:	//---- number of tokens ----
						Tokens (value);
						max_field = value + 17;

						if (value == 0) return (true);
						if (!Check_Size (value)) return (false);
						break;

					default:	//---- token value ----

						if (field > max_field) {
							Status (PLAN_FIELDS);
							return (false);
						}
						plan->data [field - 18] = value;

						if (field == max_field) return (true);
						break;
				}
			}
		}
		if (field != 0) {
			return (Status (PLAN_FIELDS));
		}
		return (false);
	}
}

//---------------------------------------------------------
//	Write
//---------------------------------------------------------

bool Old_Plan_File::Write (Old_Plan_Data *data)
{
	int num_token;

	//---- check the file status ----

	if (!Check_File ()) return (false);
	if (File_Access () == READ) return (Status (DB_ERROR));

	Old_Plan_Data *backup = 0;

	if (data != 0) {
		backup = plan;
		plan = data;
	} else {
		if (plan == 0) return (Status (RECORD_SIZE));
	}

	//---- write the plan data ----

	fstream &file = File ();
	num_token = Tokens ();

	if (Record_Format () == BINARY) {
		if (time_sort) {
			int size, temp;
			size = sizeof (Old_Plan_Data) - sizeof (int);

			memcpy (backup, plan, size);
			
			temp = backup->key1;
			backup->key1 = backup->key2;
			backup->key2 = temp;

			if (!Db_File::Write (backup, size)) goto reset;
		} else {
			if (!Db_File::Write (plan, (sizeof (Old_Plan_Data) - sizeof (int)))) goto reset;
		}

		num_record++;
		num_plan++;

		if (Leg () == 2) {
			num_trip++;
		} else if (Leg () == 1 && Trip () == 1) {
			num_traveler++;
		}
		if (num_token > 0) {
			if (!Db_File::Write (&(plan->data [0]), num_token * sizeof (int))) goto reset;
			num_record++;
		}

	} else {

		file << Traveler () << " 0 " << Trip () << " " << Leg () << endl;
		file << Time () << " " << Start_ID () << " " << Start_Type () << " " << End_ID () << " " << End_Type () << endl;
		file << Duration () << " " << Stop_Time () << " 1 " << Cost () << " " << GCF () << endl;
		file << Driver_Flag () << " " << Mode () << endl;
		file << num_token << endl;

		if (file.fail ()) goto reset;

		num_record += 5;
		num_plan++;

		if (Leg () == 2) {
			num_trip++;
		} else if (Leg () == 1 && Trip () == 1) {
			num_traveler++;
		}

		//---- write tokens ----

		if (num_token > 0) {
			int field;
			int i = 0;

			switch (Mode ()) {
				case AUTO_MODE:		//---- auto ----
					if (Driver_Flag ()) {

						//---- vehicle ID and number of passengers ----

						i = 2;
						file << plan->data [0] << " " << plan->data [1] << endl;
						if (file.fail ()) goto reset;
						num_record++;
					}
					break;
				case TRANSIT_MODE:		//---- transit ----
					if (Driver_Flag ()) {

						//---- schedule pairs, vehicle ID, and route ID ----

						i = 3;
						file << plan->data [0] << " " << plan->data [1] << " " << plan->data [2] << endl;
						if (file.fail ()) goto reset;
						num_record++;
					}
					break;
				default:
					break;
			}

			//---- print the rest of the fields in groups of 10 ----

			for (field=0; i < num_token; i++, field++) {
				if (!field) {
					file << plan->data [i];
					if (file.fail ()) goto reset;
					num_record++;
				} else if (!(field % 10)) {
					file << endl << plan->data [i];
					if (file.fail ()) goto reset;
					num_record++;
				} else {
					file << " " << plan->data [i];
					if (file.fail ()) goto reset;
				}
			}
			if (field) {
				file << endl;
			}
		}

		//---- add a blank line at the end of the plan ----

		file << endl;
		if (file.fail ()) goto reset;
		num_record++;

		Flush ();
	}
	if (data != 0) {
		plan = backup;
	}
	return (true);

reset:
	if (data != 0) {
		plan = backup;
	}
	return (false);
}

//---------------------------------------------------------
//	Plan
//---------------------------------------------------------

bool Old_Plan_File::Plan (Old_Plan_Data *data)
{
	if (data == 0) return (Status (NULL_POINTER));

	int size = data->tokens;

	if (!Check_Size (size)) return (false);

	if (size) size--;		

	size = sizeof (Old_Plan_Data) + size * sizeof (int);

    memcpy (plan, data, size);
	return (true);
}

//---------------------------------------------------------
//	Delete_Plan
//---------------------------------------------------------

void Old_Plan_File::Delete_Plan (Old_Plan_Data *data)
{
	if (data == 0) {
		data = plan;
		max_data = 0;
	}
	if (data != 0) {
		free (data);
		data = 0;
	}
}

//---------------------------------------------------------
//	Setup_Record
//---------------------------------------------------------

bool Old_Plan_File::Setup_Record (void)
{
	max_data = 0;
	plan = (Old_Plan_Data *) malloc (sizeof (Old_Plan_Data));

	if (plan == 0) return (Status (RECORD_SIZE));

	max_data = 1;
	memset (plan, '\0', sizeof (Old_Plan_Data));
	return (true);
}

//---------------------------------------------------------
//	Check_Size
//---------------------------------------------------------

bool Old_Plan_File::Check_Size (int size)
{
	if (size <= max_data) return (true);

	max_data = 0;

	size_t mem_size = sizeof (Old_Plan_Data) + (size - 1) * sizeof (int);

	plan = (Old_Plan_Data *) realloc (plan, mem_size);

	if (plan == 0) return (Status (RECORD_SIZE));

	max_data = size;

	return (true);
}

//---------------------------------------------------------
//	Data
//---------------------------------------------------------

int * Old_Plan_File::Data (int *max_token)
{
	if (plan != 0) {
		*max_token = max_data;
		return (plan->data);
	} else {
		*max_token = 0;
		return (0);
	}
}

//---------------------------------------------------------
//	Path
//---------------------------------------------------------

int * Old_Plan_File::Path (int *num_path)
{
	int offset = 0;
	*num_path = 0;

	//---- mode specific adjustments ----

	switch (Mode ()) {
		case AUTO_MODE:		//---- auto ----
			if (Driver_Flag ()) {	//---- driver ----
				offset = 2;
				//*num_path = Tokens () - offset - plan->data [1];
				*num_path = Tokens () - offset;
			}
			break;
		case TRANSIT_MODE:		//---- transit ----
			if (Driver_Flag ()) {	//---- driver ----
				offset = 3;
				*num_path = Tokens () - offset - 2 * plan->data [0];
			}
			break;
		case BIKE_MODE:		//---- bike ----
		case WALK_MODE:		//---- walk ----
			*num_path = Tokens ();
			break;
		default:	//---- other ----
			offset = 1;
			*num_path = Tokens () - offset;
			break;
	}
	return (plan->data + offset);
}

//---------------------------------------------------------
//	Num_Path
//---------------------------------------------------------

void Old_Plan_File::Num_Path (int num)
{
	//---- mode specific adjustments ----

	switch (Mode ()) {
		case AUTO_MODE:		//---- auto ----
			if (Driver_Flag ()) {	//---- driver ----
				Tokens (num + 2);
			} else {				//---- passenger ----
				Tokens (1);
			}
			break;
		case TRANSIT_MODE:		//---- transit ----
			if (Driver_Flag ()) {	//---- driver ----
				Tokens (num + 3);
			} else {
				Tokens (1);
			}
			break;
		case BIKE_MODE:		//---- bike ----
		case WALK_MODE:		//---- walk ----
			Tokens (num);
			break;
		default:	//---- other ----
			Tokens (num + 1);
			break;
	}
}

//---------------------------------------------------------
//	Mode_ID
//---------------------------------------------------------

int Old_Plan_File::Mode_ID (void)
{
	int mode_id;

	switch (Mode ()) {
		case AUTO_MODE:		//---- auto ----
			mode_id = plan->data [0];
			break;
		case TRANSIT_MODE:		//---- transit ----
			if (Driver_Flag ()) {	//---- driver ----
				mode_id = plan->data [2];	//---- route id ----
			} else {				//---- passenger ----
				mode_id = plan->data [0];		//---- route id ----
			}
			break;
		case BIKE_MODE:		//---- bike ----
		case MAGIC_MODE:		//---- magic move ----	
			mode_id = plan->data [0];		//---- type ----
			break;
		default:	//---- other ----
			mode_id = 0;	//---- not used ----
			break;
	}
	return (mode_id);
}

void Old_Plan_File::Mode_ID (int id)
{
	switch (Mode ()) {
		case AUTO_MODE:		//---- auto ----
			plan->data [0] = id;
			if (Driver_Flag ()) {
				plan->data [1] = 0;		//---- passengers ----
			}
			break;
		case TRANSIT_MODE:		//---- transit ----
			if (Driver_Flag ()) {	//---- driver ----
				plan->data [0] = 0;		//---- schedule pairs ----
				plan->data [1] = 0;		//---- vehicle id ----
				plan->data [2] = id;	//---- route id ----
			} else {				//---- passenger ----
				plan->data [0] = id;		//---- route id ----
			}
			break;
		case BIKE_MODE:		//---- bike ----
		case MAGIC_MODE:		//---- magic move ----	
			plan->data [0] = id;		//---- type ----
			break;
		default:	//---- other ----
			break;
	}
}

//---------------------------------------------------------
//	Vehicle
//---------------------------------------------------------

int Old_Plan_File::Vehicle (void)
{
	switch (Mode ()) {
		case AUTO_MODE:		//---- auto ----
			return (plan->data [0]);
			break;
		case TRANSIT_MODE:		//---- transit ----
			if (Driver_Flag ()) {	//---- driver ----
				return (plan->data [1]);	//---- vehicle id ----
			}
			break;
		default:	//---- other ----
			break;
	}
	return (0);
}

void Old_Plan_File::Vehicle (int veh)
{
	switch (Mode ()) {
		case AUTO_MODE:		//---- auto ----
			plan->data [0] = veh;
			break;
		case TRANSIT_MODE:		//---- transit ----
			if (Driver_Flag ()) {	//---- driver ----
				plan->data [1] = veh;	//---- vehicle id ----
			} else {				//---- passenger ----
			}
			break;
		case BIKE_MODE:		//---- bike ----
		case MAGIC_MODE:		//---- magic move ----	
			break;
		default:	//---- other ----
			break;
	}
}
//---------------------------------------------------------
//	Route
//---------------------------------------------------------

int Old_Plan_File::Route (void)
{
	if (Mode () == TRANSIT_MODE) {
		if (Driver_Flag ()) {
			return (plan->data [2]);
		} else {
			return (plan->data [0]);
		}
	}
	return (0);
}

void Old_Plan_File::Route (int id)
{
	if (Mode () == TRANSIT_MODE) {
		if (Driver_Flag ()) {
			plan->data [2] = id;
		} else {
			plan->data [0] = id;
		}
	}
}

//-----------------------------------------------------------
//	Read_Plan_Def
//-----------------------------------------------------------

bool Old_Plan_File::Read_Plan_Def (void) 
{
	char buffer [512];
	String name, record, token;
	FILE *def;

	//---- construct the definition filename ----

	name = Filename () + ".def";

	//---- open the definition file ----

	def = f_open (name, "rt");

	if (def == 0) return (true);

	//---- get the header line ----

	if (fgets (buffer, sizeof (buffer), def) == 0) return (Status (DEF_FORMAT));

	record = buffer;

	//---- TRANSIMS version code ----

	record.Split (token, ",\t");

	if (token.Starts_With ("AECOM HEADER")) {
		Version (40);
	} else {
		return (Status (DEF_FORMAT));
	}

	//---- file format ----

	record.Split (token);

	File_Format (token);

	if (!token.Equals ("BINARY") && !token.Equals ("VERSION3")) {
		return (Status (DEF_FORMAT));
	}

	//---- number of scaling factor ----

	record.Split (token);

	scale = token.Integer ();
	if (scale < 2 || scale > 100) return (Status (DEF_FORMAT));

	//---- get the path base ----

	record.Split (token);

	if (token.Equals ("NODE_BASED")) {
		Node_Based_Flag (true);
	} else if (token.Equals ("LINK_BASED")) {
		Node_Based_Flag (false);
	} else {
		return (Status (DEF_FORMAT));
	}
	fclose (def);
	return (true);
}

//-----------------------------------------------------------
//	Write_Plan_Def
//-----------------------------------------------------------

bool Old_Plan_File::Write_Plan_Def (void) 
{
	string filename;
	FILE *def;

	//---- construct the definition filename ----

	filename = Filename () + ".def";

	//---- create the definition file ----

	def = f_open (filename, "wt");

	if (def == 0) return (Status (OPEN_DEF));

	if (fprintf (def, "AECOM HEADER, %s, %d, %s\n", ((File_Format () == BINARY) ? "BINARY" : "VERSION3"), 
		scale, ((Node_Based_Flag ()) ? "NODE_BASED" : "LINK_BASED")) < 0) {
		return (Status (DEF_FORMAT));
	}
	fclose (def);
	return (true);
}
