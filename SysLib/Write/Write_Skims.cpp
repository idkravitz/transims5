//*********************************************************
//	Write_Skims.cpp - write a new skim file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Skims
//---------------------------------------------------------

void Data_Service::Write_Skims (int period)
{
	int part, max_part, count, total, org, des, p, max_period;
	bool period_flag;

	Skim_File *file = (Skim_File *) System_File_Handle (NEW_SKIM);

	Int_Map *org_map, *des_map;
	Int_Map_Itr org_itr, des_itr;
	Skim_Record skim_rec;
	Skim_Data *skim_ptr;

	period_flag = (period >= 0);
	total = 0;

	if (file->Part_Flag () && !period_flag) {
		max_part = file->Num_Periods ();
		period = max_period = 0;
	} else {
		max_part = 1;
		if (period_flag && file->Part_Flag ()) {
			if (period > 0) {
				if (!file->Open (period)) return;
			}
			max_period = period;
		} else if (period_flag) {
			max_period = period;
		} else {
			max_period = file->Num_Periods () - 1;
		}
	}
	org_map = file->Org_Map ();
	des_map = file->Des_Map ();

	//---- process each partition ----

	for (part = 0; part < max_part; part++) {
		if (part > 0) {
			if (!file->Open (part)) break;
			period = max_period = part;
		}
		if (file->Part_Flag ()) {
			Show_Message (String ("Writing %s %d Period %d -- Record") % file->File_Type () % file->Part_Number () % period);
		} else if (period_flag) {
			Show_Message (String ("Writing %s Period %d -- Record") % file->File_Type () % period);
		} else {
			Show_Message (String ("Writing %s -- Record") % file->File_Type ());
		}
		Set_Progress ();
		count = 0;

		for (org_itr = org_map->begin (); org_itr != org_map->end (); org_itr++) {
			org = org_itr->second;

			skim_rec.Origin (org_itr->first);

			for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
				Show_Progress ();

				des = des_itr->second;

				skim_rec.Destination (des_itr->first);

				skim_ptr = file->Table (org, des);

				if (file->Part_Flag () || period_flag) {
					skim_ptr += part;
				}
				for (p=period; p <= max_period; p++, skim_ptr++) {
					if (skim_ptr->Count () == 0) continue;

					skim_rec.Period (p);
					skim_rec.Count (skim_ptr->Count ());
					skim_rec.Walk (skim_ptr->Walk ());
					skim_rec.Drive (skim_ptr->Drive ());
					skim_rec.Transit (skim_ptr->Transit ());
					skim_rec.Wait (skim_ptr->Wait ());
					skim_rec.Other (skim_ptr->Other ());
					skim_rec.Length (skim_ptr->Length ());
					skim_rec.Cost (skim_ptr->Cost ());
					skim_rec.Impedance (skim_ptr->Impedance ());

					count += Put_Skim_Data (*file, skim_rec);
					skim_ptr->Clear ();
				}
			}
		}
		Show_Progress (count);
		End_Progress ();
		total += count;
	}
	if (file->Part_Flag () || !period_flag) file->Close ();
	
	if (period_flag) {
		Print (2, String ("%s Period %d Records = %d") % file->File_Type () % period % total);
	} else {
		Print (2, String ("%s Records = %d") % file->File_Type () % total);
		if (part > 1) Print (0, String (" (%d files)") % part);
	}
}

//---------------------------------------------------------
//	Put_Skim_Data
//---------------------------------------------------------

int Data_Service::Put_Skim_Data (Skim_File &file, Skim_Record &data)
{
	file.Origin (data.Origin ());
	file.Destination (data.Destination ());
	file.Period (data.Period ());
	file.Data (data.Count ());
	if (file.Data_Type () == TIME_TABLE) {
		file.Time (Dtime (data.Time ()).Round_Seconds ());
	} else {
		file.Walk (Dtime (data.Walk ()).Round_Seconds ());
		file.Drive (Dtime (data.Drive ()).Round_Seconds ());
		file.Transit (Dtime (data.Transit ()).Round_Seconds ());
		file.Wait (Dtime (data.Wait ()).Round_Seconds ());
		file.Other (Dtime (data.Other ()).Round_Seconds ());
	}
	file.Length (UnRound (data.Length ()));
	file.Cost (UnRound (data.Cost ()));
	file.Impedance (data.Impedance ());

	if (!file.Write ()) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	return (1);
}
