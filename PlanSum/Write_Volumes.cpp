//*********************************************************
//	Write_Volumes.cpp - Write the Link Volume File
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Write_Volume
//---------------------------------------------------------

void PlanSum::Write_Volumes (void)
{
	int i, j, nrec, index;
	bool save, ab_flag;	
	double flow;

	Link_Data *link_ptr;
	Int_Map_Itr map_itr;
	Flow_Time_Period_Itr period_itr;
	Flow_Time_Data flow_data;

	nrec = 0;
	flow = 0.0;

	Show_Message (String ("Writing %s -- Record") % volume_file.File_Type ());
	Set_Progress ();

	//---- process each link in sorted order ----

	for (map_itr = link_map.begin (); map_itr != link_map.end (); map_itr++) {
		Show_Progress ();

		link_ptr = &link_array [map_itr->second];

		save = false;
		volume_file.Zero_Fields ();

		//---- process each direction ----

		for (i=0; i < 2; i++) {

			if (i) {
				index = link_ptr->BA_Dir ();
				ab_flag = false;
			} else {
				index = link_ptr->AB_Dir ();
				ab_flag = true;
			}
			if (index < 0) continue;

			for (j=0, period_itr = link_delay_array.begin (); period_itr != link_delay_array.end (); period_itr++, j++) {
				flow_data = period_itr->Total_Flow_Time (index);
						
				flow = flow_data.Flow ();

				if (flow != 0.0) {
					if (ab_flag) {
						volume_file.Data_AB (j, flow);
					} else {
						volume_file.Data_BA (j, flow);
					}
					save = true;
				}
			}
		}

		//---- save non-zero records ----

		if (save) {
			nrec++;

			volume_file.Link (link_ptr->Link ());
			volume_file.Anode (node_array [link_ptr->Anode ()].Node ());
			volume_file.Bnode (node_array [link_ptr->Bnode ()].Node ());

			if (!volume_file.Write ()) {
				Error (String ("Writing %s") % volume_file.File_Type ());
			}
		}
	}
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % volume_file.File_Type () % nrec);
	Print (1, "Number of Time Periods = ") << volume_file.Num_Periods ();

	volume_file.Close ();
}
