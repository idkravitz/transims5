//*********************************************************
//	Write_Links.cpp - write a new link file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Links
//---------------------------------------------------------

void Data_Service::Write_Links (void)
{
	Link_File *file = (Link_File *) System_File_Handle (NEW_LINK);

	int count = 0;
	Int_Map_Itr itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = link_map.begin (); itr != link_map.end (); itr++) {
		Show_Progress ();

		count += Put_Link_Data (*file, link_array [itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Link_Data
//---------------------------------------------------------

int Data_Service::Put_Link_Data (Link_File &file, Link_Data &data)
{
	Dir_Data *dir_ptr;

	if (data.Length () == 0) return (0);

	file.Link (data.Link ());
	file.Name (data.Name ());
	file.Node_A (node_array [data.Anode ()].Node ());
	file.Node_B (node_array [data.Bnode ()].Node ());
	file.Length (UnRound (data.Length ()));
	file.Setback_A (UnRound (data.Aoffset ()));
	file.Setback_B (UnRound (data.Boffset ()));
	file.Type (data.Type ());
	file.Divided (data.Divided ());
	file.Area_Type (data.Area_Type ());
	file.Use (data.Use ());
	file.Grade (UnRound (data.Grade ()));
	file.Notes (data.Notes ());

	if (data.AB_Dir () >= 0) {
		dir_ptr = &dir_array [data.AB_Dir ()];

		file.Lanes_AB (dir_ptr->Lanes ());
		file.Speed_AB (UnRound (dir_ptr->Speed ()));

		if (dir_ptr->Time0 () > 0) {
			file.Fspd_AB ((double) data.Length () / dir_ptr->Time0 ());
		} else {
			file.Fspd_AB (UnRound (dir_ptr->Speed ()));
		}
		if (file.Fspd_AB () > file.Speed_AB () && file.Speed_AB () > 0) {
			if ((file.Fspd_AB () - file.Speed_AB ()) > 0.5) {
				if (Metric_Flag ()) {
					Warning (String ("Link %d Free Flow Speed %.1lf > Maximum Speed %.1lf") % data.Link () % External_Units (file.Fspd_AB (), KPH) % 
						External_Units (file.Speed_AB (), KPH));
				} else {
					Warning (String ("Link %d Free Flow Speed %.1lf > Maximum Speed %.1lf") % data.Link () % External_Units (file.Fspd_AB (), MPH) % 
						External_Units (file.Speed_AB (), MPH));
				}
			}
			file.Fspd_AB (file.Speed_AB ());
		}
		file.Cap_AB (dir_ptr->Capacity ());
		file.Bearing_A (dir_ptr->In_Bearing ());
		file.Bearing_B (dir_ptr->Out_Bearing ());
	} else {
		file.Lanes_AB (0);
		file.Speed_AB (0);
		file.Fspd_AB (0);
		file.Cap_AB (0);
	}
	if (data.BA_Dir () >= 0) {
		dir_ptr = &dir_array [data.BA_Dir ()];

		file.Lanes_BA (dir_ptr->Lanes ());
		file.Speed_BA (UnRound (dir_ptr->Speed ()));

		if (dir_ptr->Time0 () > 0) {
			file.Fspd_BA ((double) data.Length () / dir_ptr->Time0 ());
		} else {
			file.Fspd_BA (UnRound (dir_ptr->Speed ()));
		}
		if (file.Fspd_BA () > file.Speed_BA () && file.Speed_BA () > 0) {
			if ((file.Fspd_BA () - file.Speed_BA ()) > 0.5) {
				if (Metric_Flag ()) {
					Warning (String ("Link %d Free Flow Speed %.1lf > Maximum Speed %.1lf") % data.Link () % External_Units (file.Fspd_BA (), KPH) % 
						External_Units (file.Speed_BA (), KPH));
				} else {
					Warning (String ("Link %d Free Flow Speed %.1lf > Maximum Speed %.1lf") % data.Link () % External_Units (file.Fspd_BA (), MPH) % 
						External_Units (file.Speed_BA (), MPH));
				}
			}
			file.Fspd_BA (file.Speed_BA ());
		}
		file.Cap_BA (dir_ptr->Capacity ());
		if (data.AB_Dir () < 0) {
			file.Bearing_B (compass.Flip (dir_ptr->In_Bearing ()));
			file.Bearing_A (compass.Flip (dir_ptr->Out_Bearing ()));
		}
	} else {
		file.Lanes_BA (0);
		file.Speed_BA (0);
		file.Fspd_BA (0);
		file.Cap_BA (0);
	}
	if (!file.Write ()) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	return (1);
}
