//*********************************************************
//	Write_Link_Data.cpp - Write the Link Data File
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Write_Link_Data
//---------------------------------------------------------

void LinkSum::Write_Link_Data (Data_Group_Itr &group)
{
	int i, j, k, nrec, lanes, lane, index, flow_index, tod_list;
	bool save, ab_flag;	
	double data, cap, capacity;
	Dtime low, high, tod;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Lane_Use_Period *use_ptr;
	Int_Map_Itr map_itr;
	Link_Perf_Period_Itr period_itr;
	Link_Perf_Data perf_data;

	nrec = tod_list = lanes = 0;
	capacity = cap = data = 0.0;

	Show_Message (String ("Writing %s -- Record") % group->file->File_Type ());
	Set_Progress ();

	//---- process each link in sorted order ----

	for (map_itr = link_map.begin (); map_itr != link_map.end (); map_itr++) {
		Show_Progress ();

		link_ptr = &link_array [map_itr->second];

		if (select_flag && link_ptr->Use () == 0) continue;

		save = false;
		group->file->Zero_Fields ();

		//---- process each direction ----

		for (i=0; i < 2; i++) {

			if (i) {
				if (link_ptr->Use () == -1) continue;
				index = link_ptr->BA_Dir ();
				ab_flag = false;
			} else {
				if (link_ptr->Use () == -2) continue;
				index = link_ptr->AB_Dir ();
				ab_flag = true;
			}
			if (index < 0) continue;
			dir_ptr = &dir_array [index];

			flow_index = dir_ptr->Flow_Index ();

			//---- get the capacity data ----

			if (group->field == VC_DATA) {
				capacity = dir_ptr->Capacity ();

				if (capacity < 20) capacity = 1000.0;
				cap = capacity *= cap_factor;

				tod_list = dir_ptr->First_Lane_Use ();

				lanes = dir_ptr->Lanes ();
				if (lanes < 1) lanes = 1;
			}

			for (j=0, period_itr = link_perf_array.begin (); period_itr != link_perf_array.end (); period_itr++, j++) {
				perf_data = period_itr->Total_Link_Perf (index, flow_index);

				switch (group->field) {
					case FLOW_DATA:
						data = perf_data.Flow ();
						break;
					case TTIME_DATA:
						data = perf_data.Time ();
						break;
					case VC_DATA:
						data = perf_data.Flow ();

						if (data > 0.0) {
							if (tod_list >= 0) {
								sum_periods.Period_Range (j, low, high);

								tod = (low + high + 1) / 2;
								cap = capacity;
								k = tod_list;

								for (use_ptr = &use_period_array [k]; ; use_ptr = &use_period_array [++k]) {
									if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
										lane = use_ptr->Lanes0 () + use_ptr->Lanes1 ();
										cap = capacity * lane / lanes;
										break;
									}
									if (use_ptr->Periods () == 0) break;
								}
							}
							if (cap > 0.0) {
								data = data / cap;
							} else {
								data = 0.0;
							}
						}
						break;
					case RATIO_DATA:
						if (dir_ptr->Time0 () > 0) {
							data = (double) perf_data.Time () / dir_ptr->Time0 ();
						} else {
							data = 1.0;
						}
						break;
					case SPEED_DATA:
						if (perf_data.Time () > 0) {
							data = (double) link_ptr->Length () / perf_data.Time ();
						} else {
							data = (double) link_ptr->Length () / dir_ptr->Time0 ();
						}
						if (data < 0.1) data = 0.1;
						break;
					case DELAY_DATA:
						data = perf_data.Time () - dir_ptr->Time0 ();
						break;
					case DENSITY_DATA:
						data = perf_data.Density ();
						break;
					case MAX_DENSITY_DATA:
						data = perf_data.Max_Density ();
						break;
					case QUEUE_DATA:
						data = perf_data.Queue ();
						break;
					case MAX_QUEUE_DATA:
						data = perf_data.Max_Queue ();
						break;
					case FAILURE_DATA:
						data = perf_data.Failure ();
						break;
					case VMT_DATA:
						data = perf_data.Flow () * UnRound (link_ptr->Length ());
						break;
					case VHT_DATA:
						data = perf_data.Time () * perf_data.Flow ();
						break;
					case CONG_VMT_DATA:
						data = perf_data.Ratio_VMT ();
						break;
					case CONG_VHT_DATA:
						data = perf_data.Ratio_VHT ();
						break;
					case CONG_TIME_DATA:
						data = perf_data.Occupancy ();
						if (data <= 0.0) data = 1.0;
						data = 100.0 * perf_data.Ratio_Count () / data;
						break;
				}
				if (data != 0.0) {
					if (ab_flag) {
						group->file->Data_AB (j, data);
					} else {
						group->file->Data_BA (j, data);
					}
					save = true;
				}
			}
		}

		//---- save non-zero records ----

		if (save) {
			nrec++;

			group->file->Link (link_ptr->Link ());
			group->file->Anode (node_array [link_ptr->Anode ()].Node ());
			group->file->Bnode (node_array [link_ptr->Bnode ()].Node ());

			if (!group->file->Write ()) {
				Error (String ("Writing %s") % group->file->File_Type ());
			}
		}
	}
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % group->file->File_Type () % nrec);
	Print (1, "Number of Time Periods = ") << group->file->Num_Periods ();

	group->file->Close ();
}
