//*********************************************************
//	Link_Report.cpp - Create a Link Volume Report
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Link_Report
//---------------------------------------------------------

void LinkSum::Link_Report (double min_volume)
{
	bool flag;
	int i, ab_index, ba_index, ab_flow, ba_flow, an, bn;
	double flow;
	String ab, ba;
	
	Link_Itr link_itr;
	Link_Perf_Period_Itr period_itr;
	Flow_Time_Data flow_data;

	Show_Message ("Creating a Link Event Report -- Record");
	Set_Progress ();

	//---- find volumes greater than min_volume ----

	header_value = DTOI (min_volume);

	Header_Number (LINK_REPORT);
	New_Page ();

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (select_flag && link_itr->Use () == 0) continue;

		ab_index = (link_itr->Use () != -2) ? link_itr->AB_Dir () : -1;
		ba_index = (link_itr->Use () != -1) ? link_itr->BA_Dir () : -1;

		ab_flow = (ab_index >= 0) ? dir_array [ab_index].Flow_Index () : -1;
		ba_flow = (ba_index >= 0) ? dir_array [ba_index].Flow_Index () : -1;

		//---- scan each period volume ----

		for (i=0, period_itr = link_perf_array.begin (); period_itr != link_perf_array.end (); period_itr++, i++) {
			ab.clear ();
			ba.clear ();
			flag = false;

			if (ab_index >= 0) {
				flow_data = period_itr->Total_Flow_Time (ab_index, ab_flow);

				flow = flow_data.Flow ();
				if (flow >= min_volume) {
					ab = String ("%d") % DTOI (flow);
					flag = true;
				}
			}
			if (ba_index >= 0) {
				flow_data = period_itr->Total_Flow_Time (ba_index, ba_flow);

				flow = flow_data.Flow ();
				if (flow >= min_volume) {
					ba = String ("%d") % DTOI (flow);
					flag = true;
				}
			}
			if (!flag) continue;

			Show_Progress ();

			//---- print the data record ----
		
			an = node_array [link_itr->Anode ()].Node ();
			bn = node_array [link_itr->Bnode ()].Node ();

			Print (1, String ("%10d%10d%10d  %12.12s%10.10s %10.10s") % link_itr->Link () % an % 
				bn % sum_periods.Range_Format (i) % ab % ba);
		}
	}
	End_Progress ();

	if (Progress_Count ()) {
		Print (2, "Number of Records in the Report = ") << Progress_Count ();
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Link_Report_Header
//---------------------------------------------------------

void LinkSum::Link_Report_Header (void)
{
	Print (1, String ("Link Volumes Greater Than %.0lf") % header_value);
	Print (2, "      Link    A-Node    B-Node   Time-of-Day  Volume_AB  Volume_BA");
	Print (1);
}

/*********************************************|***********************************************

	Link Volume Greater Than dddddd

	      Link    A-Node    B-Node   Time-of-Day  Volume_AB  Volume_BA

	dddddddddd  dddddddd  dddddddd  dd:dd..dd:dd  dddddddd   dddddddd

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
