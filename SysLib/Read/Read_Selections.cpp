//*********************************************************
//	Read_Selections.cpp - Read the household selection File
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Selections
//---------------------------------------------------------

void Data_Service::Read_Selections (void)
{
	int part, num, num_rec, last_hhold, part_num, first;
	Selection_File *file = (Selection_File *) System_File_Handle (SELECTION);

	Selection_Data selection_rec;
	Trip_Index trip_index;
	Select_Data select_data;
	Select_Map_Stat map_stat;

	Initialize_Selections (*file);
	num_rec = last_hhold = first = 0;

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
	
		//---- store the selection data ----

		if (file->Part_Flag ()) {
			part_num = file->Part_Number ();
			Show_Message (String ("Reading %s %d -- Record") % file->File_Type () % part_num);
		} else {
			part_num = part + first;
			Show_Message (String ("Reading %s -- Record") % file->File_Type ());
		}
		Set_Progress ();

		while (file->Read ()) {
			Show_Progress ();

			selection_rec.Clear ();

			if (Get_Selection_Data (*file, selection_rec, part_num)) {
				selection_rec.Get_Trip_Index (trip_index);

				select_data.Type (selection_rec.Type ());
				select_data.Partition (selection_rec.Partition ());

				//---- process the record ----

				map_stat = select_map.insert (Select_Map_Data (trip_index, select_data));

				if (!map_stat.second) {
					Warning (String ("Duplicate Selection Record = %d-%d-%d-%d") % 
						trip_index.Household () % trip_index.Person () % trip_index.Tour () % trip_index.Trip ());
				} else {
					if (selection_rec.Partition () > select_map.Max_Partition ()) {
						select_map.Max_Partition (selection_rec.Partition ());
					}
					if (selection_rec.Household () > select_map.Max_Household ()) {
						select_map.Max_Household (selection_rec.Household ());
					}
					if (selection_rec.Household () != last_hhold) {
						last_hhold = selection_rec.Household ();
						select_map.Add_Household ();
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

	num = (int) select_map.size ();

	if (num && num != num_rec) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) System_Data_True (SELECTION);
}

//---------------------------------------------------------
//	Initialize_Selections
//---------------------------------------------------------

void Data_Service::Initialize_Selections (Selection_File &file)
{
	file.Num_Records ();
}

//---------------------------------------------------------
//	Get_Selection_Data
//---------------------------------------------------------

bool Data_Service::Get_Selection_Data (Selection_File &file, Selection_Data &selection_rec, int partition)
{
	selection_rec.Household (file.Household ());
	if (selection_rec.Household () < 1) return (false);

	selection_rec.Person (file.Person ());
	selection_rec.Tour (file.Tour ());
	selection_rec.Trip (file.Trip ());
	selection_rec.Type (file.Type ());
	if (file.Partition_Flag ()) {
		selection_rec.Partition (file.Partition ());
	} else {
		selection_rec.Partition (partition);
	}
	if (Partition_Index (selection_rec.Partition ()) < 0) return (false);
	return (true);
}
