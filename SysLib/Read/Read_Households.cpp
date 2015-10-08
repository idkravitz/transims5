//*********************************************************
//	Read_Households.cpp - Read the Household File
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Households
//---------------------------------------------------------

void Data_Service::Read_Households (void)
{
	int i, part, num, count, num_rec, part_num, first, index;
	bool keep_flag;
	Household_File *file = (Household_File *) System_File_Handle (HOUSEHOLD);

	Int_Map_Stat map_stat;
	Household_Data household_rec;
	Person_Itr person_itr;
	Person_Index person_index;
	Person_Map_Stat person_stat;

	count = num_rec = first = 0;
	Initialize_Households (*file);

	//---- check the partition number ----

	if (file->Part_Flag () && First_Partition () != file->Part_Number ()) {
		file->Open (0);
	} else if (First_Partition () >= 0) {
		first = First_Partition ();
	}

	//---- process each partition ----

	for (part=0; ; part++) {
		if (part > 0) {
			if (!file->Open (part)) break;
		}
	
		//---- store the household data ----

		if (file->Part_Flag ()) {
			part_num = file->Part_Number ();
			Show_Message (String ("Reading %s %d -- Record") % file->File_Type () % part_num);
		} else {
			part_num = part + first;
			Show_Message (String ("Reading %s -- Record") % file->File_Type ());
		}
		Set_Progress ();

		while (file->Read (false)) {
			Show_Progress ();

			household_rec.Clear ();

			keep_flag = Get_Household_Data (*file, household_rec, part_num);

			num = file->Num_Nest ();
			if (num > 0) household_rec.reserve (num);

			for (i=1; i <= num; i++) {
				if (!file->Read (true)) {
					Error (String ("Number of Nested Records for Household %d") % file->Household ());
				}
				Show_Progress ();

				Get_Household_Data (*file, household_rec, part_num);
			}
			if (keep_flag) {
				index = (int) hhold_array.size ();

				map_stat = hhold_map.insert (Int_Map_Data (household_rec.Household (), index));

				if (!map_stat.second) {
					Warning ("Duplicate Household Number = ") << household_rec.Household ();
				} else {
					hhold_array.push_back (household_rec);
					count += (int) household_rec.size ();

					//---- create a person index ----

					if (Person_Map_Flag ()) {
						person_index.Household (household_rec.Household ());

						for (person_itr = household_rec.begin (); person_itr != household_rec.end (); person_itr++) {
							person_index.Person (person_itr->Person ());
							person_stat = person_map.insert (Person_Map_Data (person_index, index));

							if (!person_stat.second) {
								Warning ("Duplicate Person Number = ") << person_index.Household () << "." << person_index.Person ();
							}
						}
					}
				}
			}
		}
		End_Progress ();
		num_rec += Progress_Count ();
	}
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % num_rec);
	if (part > 1) Print (0, String (" (%d files)") % part);

	num = (int) hhold_array.size ();

	if (num && num != num_rec) {
		Print (1, "Number of Households = ") << num;
		Print (1, "Number of Persons = ") << count;
	}
	if (num > 0) System_Data_True (HOUSEHOLD);
}

//---------------------------------------------------------
//	Initialize_Households
//---------------------------------------------------------

void Data_Service::Initialize_Households (Household_File &file)
{
	Required_File_Check (file, LOCATION);

	int percent = System_Data_Reserve (HOUSEHOLD);

	if (hhold_array.capacity () == 0 && percent > 0) {
		int num = file.Estimate_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		} else if (file.Version () > 40) {
			num = (int) (num / 2.0);
		}
		if (num > 1) {
			hhold_array.reserve (num);
			if (num > (int) hhold_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Household_Data
//---------------------------------------------------------

bool Data_Service::Get_Household_Data (Household_File &file, Household_Data &household_rec, int partition)
{
	int hhold, loc;

	//---- process a header line ----

	if (!file.Nested ()) {
		Int_Map_Itr map_itr;

		hhold = file.Household ();
		if (hhold <= 0) return (false);

		household_rec.Household (hhold);

		loc = file.Location ();
		if (loc == 0) return (false);

		map_itr = location_map.find (loc);
		if (map_itr == location_map.end ()) {
			Warning (String ("Household %d Location %d was Not Found") % hhold % loc);
			return (false);
		}
		household_rec.Location (map_itr->second);

		household_rec.Persons (file.Persons ());
		household_rec.Workers (file.Workers ());
		household_rec.Vehicles (file.Vehicles ());
		household_rec.Type (file.Type ());
		//household_rec.Notes (file.Notes ());

		if (file.Partition_Flag ()) {
			household_rec.Partition (file.Partition ());
		} else {
			household_rec.Partition (partition);
		}
		return (true);
	}

	//---- process a nested person record ----

	Person_Data person_rec;

	person_rec.Person (file.Person ());
	person_rec.Age (file.Age ());
	person_rec.Relate (file.Relate ());
	person_rec.Gender (file.Gender ());
	person_rec.Work (file.Work ());
	person_rec.Drive (file.Drive ());

	household_rec.push_back (person_rec);
	return (true);
}
