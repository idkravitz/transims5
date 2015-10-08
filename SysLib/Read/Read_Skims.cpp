//*********************************************************
//	Read_Skims.cpp - Read the Skim File
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Skims
//---------------------------------------------------------

void Data_Service::Read_Skims (void)
{
	int part, num, num_rec, org, des, period;

	Skim_File *file = (Skim_File *) System_File_Handle (SKIM);

	Skim_Record skim_rec;
	Skim_Data *skim_ptr;
	Dtime *ttime;

	Initialize_Skims (*file);
	num_rec = 0;

	//---- process each partition ----

	for (part=0; ; part++) {
		if (part > 0) {
			if (!file->Open (part)) break;
		}

		//---- store the skim data ----

		if (file->Part_Flag ()) {
			Show_Message (String ("Reading %s %d -- Record") % file->File_Type () % file->Part_Number ());
		} else {
			Show_Message (String ("Reading %s -- Record") % file->File_Type ());
		}
		Set_Progress ();

		while (file->Read ()) {
			Show_Progress ();

			skim_rec.Clear ();

			if (Get_Skim_Data (*file, skim_rec, part)) {

				org = file->Add_Org (skim_rec.Origin ());
				des = file->Add_Des (skim_rec.Destination ());
				period = skim_rec.Period ();

				//---- process the record ----

				if (Time_Table_Flag ()) {
					ttime = file->Time_Skim (org, des, period);
					ttime->Seconds (skim_rec.Time ());
				} else {
					skim_ptr = file->Table (org, des, period);
					skim_ptr->Add_Skim (skim_rec);
				}
			}
		}
		End_Progress ();
		num_rec += Progress_Count ();
	}
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % num_rec);
	if (part > 1) Print (0, String (" (%d files)") % part);

	num = file->Table_Size ();

	if (num && num != num_rec) {
		Print (1, String ("Number of %s Table Records = %d") % file->File_ID () % num);
	}
	if (num > 0) System_Data_True (SKIM);
}

//---------------------------------------------------------
//	Initialize_Skims
//---------------------------------------------------------

void Data_Service::Initialize_Skims (Skim_File &file)
{
	if (file.OD_Units () == LOCATION_OD) {
		Required_File_Check (file, LOCATION);
	}
	file.Time_Table_Flag (Time_Table_Flag ());

	if (!file.Set_Table ()) Mem_Error (file.File_ID ());
}

//---------------------------------------------------------
//	Get_Skim_Data
//---------------------------------------------------------

bool Data_Service::Get_Skim_Data (Skim_File &file, Skim_Record &skim_rec, int partition)
{
	int org, des;

	//---- convert the origin and destination ----

	org = file.Origin ();
	des = file.Destination ();

	if (org < 1 || des < 1) return (false);

	if (file.OD_Units () == LOCATION_OD) {
		Int_Map_Itr map_itr = location_map.find (org);

		if (map_itr == location_map.end ()) {
			Warning (String ("Skim %d Origin %d was Not Found") % Progress_Count () % org);
			return (false);
		}
		map_itr = location_map.find (des);

		if (map_itr == location_map.end ()) {
			Warning (String ("Skim %d Destination %d was Not Found") % Progress_Count () % des);
			return (false);
		}
	}
	skim_rec.Origin (org);
	skim_rec.Destination (des);

	skim_rec.Period (file.Period ());
	if (partition > 0 && skim_rec.Period () == 0) skim_rec.Period (partition);

	if (file.Data_Type () == TIME_TABLE) {
		skim_rec.Time (file.Time ());
	} else {
		skim_rec.Walk (file.Walk ());
		skim_rec.Drive (file.Drive ());
		skim_rec.Transit (file.Transit ());
		skim_rec.Wait (file.Wait ());
		skim_rec.Other (file.Other ());
	}
	skim_rec.Length (DTOI (file.Length ()));
	skim_rec.Cost (file.Cost ());
	skim_rec.Impedance (file.Impedance ());
	return (true);
}
