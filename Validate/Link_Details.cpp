//*********************************************************
//	Link_Details.cpp - Link Group Details
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Link_Group_Details
//---------------------------------------------------------

void Validate::Link_Group_Details (void)
{
	int i, j, n, lnk;
	String label, bearing;

	Group_Data *data;
	Group_Map_Itr map_itr;
	Statistics stats;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Int_Set *list;
	Int_Set_Itr itr;
	Int_Map_Itr int_itr;

	for (i = 1; i <= link_equiv.Max_Group (); i++) {
		list = link_equiv.Group_List (i);

		if (list == 0) continue;

		for (j=1, itr = list->begin (); itr != list->end (); itr++) {
			lnk = *itr;
			int_itr = link_map.find (abs (lnk));
			if (int_itr == link_map.end ()) continue;

			link_ptr = &link_array [int_itr->second];

			if (lnk > 0 && link_ptr->AB_Dir () >= 0) {
				Sum_Group (j++, link_ptr, link_ptr->AB_Dir ());
			} else if (lnk < 0 && link_ptr->BA_Dir () >= 0) {
				Sum_Group (j++, link_ptr, link_ptr->BA_Dir ());
			}
		}

		//---- get the link group label ----

		group_label = link_equiv.Group_Label (i);
		if (group_label.empty ()) {
			group_label = String ("Link Group %d") % i;
		}

		//---- print the page header ----
		
		Header_Number (GROUP_DETAILS);

		if (!Break_Check ((int) group_data.size () + 9)) {
			Print (1);
			Group_Detail_Header ();
		}

		//---- print each link ----

		for (j=1, itr = list->begin (); itr != list->end (); itr++) {
			lnk = *itr;
			int_itr = link_map.find (abs (lnk));
			if (int_itr == link_map.end ()) continue;

			link_ptr = &link_array [int_itr->second];

			if ((lnk > 0 && link_ptr->AB_Dir () < 0) || 
				(lnk < 0 && link_ptr->BA_Dir () < 0)) {
				continue;
			}
			map_itr = group_data.find (j);
			if (map_itr == group_data.end ()) continue;

			data = &map_itr->second;
			j++;

			n = data->number;
			if (n == 0) continue;

			Calc_Stats (data, stats);

			//---- get the link name ----

			label = link_ptr->Name ();
			if (label.empty ()) {
				label = String ("Link %d %s") % link_ptr->Link () % ((lnk > 0) ? "AB" : "BA");
			}

			//---- get the link direction ----
			
			if (lnk > 0) {
				dir_ptr = &dir_array [link_ptr->AB_Dir ()];
			} else {
				dir_ptr = &dir_array [link_ptr->BA_Dir ()];
			}
			bearing = compass.Point_Text (dir_ptr->Out_Bearing ());
			if (bearing.empty ()) {
				bearing = "B";
			}
			label = String ("%-22.22s %-2.2s") % label % bearing;

			//---- print the record ----

			Page_Check (1);
			Print (0, String (stats_format) % label % n % data->volume % data->count % 
				(data->volume - data->count) % stats.per_diff % stats.avg_error % stats.per_error % 
				stats.std_dev % stats.rmse % stats.r_sq % stats.avg_vc % data->max_vc);
		}
	
		//---- calculate the total statistics ----
		
		n = total.number;
		if (n > 0) {
			Calc_Stats (&total, stats);
			
			Page_Check (3);
			Print (0, "\n");
			label = String ("%-25.25s") % "TOTAL";
			Print (0, String (stats_format) % label % n % total.volume % total.count % 
				(total.volume - total.count) % stats.per_diff % stats.avg_error % stats.per_error % 
				stats.std_dev % stats.rmse % stats.r_sq % stats.avg_vc % total.max_vc);
			Print (0, "\n");
		}
		Header_Number (0);

		group_data.clear ();
		memset (&total, '\0', sizeof (total));
	}
}

//---------------------------------------------------------
//	Group_Detail_Header
//---------------------------------------------------------

void Validate::Group_Detail_Header (void)
{
	Page_Check (5);
	Print (0, String ("\n%35cDetailed Statistics for Link Group: %s\n") % BLANK % group_label);
	Print (0, header1);
	Print (0, String ("\nLink Name             Dir") + header2);
}

/**********************************************************|***********************************************************

                                   Detailed Statistics for Link Group: sssssssssssssssssssssssss
									
                             Num.  ------Volume------ ---Difference---  --Abs.Error--    Std.     %     R   ----V/C----
Link Name             Dir    Obs.  Estimate  Observed   Volume     %      Avg.    %      Dev.   RMSE   Sq.   Avg.  Max.

ssssssssssssssssssssss ss ddddddd lllllllll lllllllll llllllll fffff.f lllllll ffff.f lllllll ffff.f  f.fff ff.ff ff.ff

TOTAL                     ddddddd lllllllll lllllllll llllllll fffff.f lllllll ffff.f lllllll ffff.f  f.fff ff.ff ff.ff
   
***********************************************************|***********************************************************/ 
