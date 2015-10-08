//*********************************************************
//	Link_Stats.cpp - Link Group Statistics
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Link_Group_Stats
//---------------------------------------------------------

void Validate::Link_Group_Stats (void)
{
	int i, n, lnk;
	String label;

	Group_Data *data;
	Group_Map_Itr map_itr;
	Statistics stats;
	Link_Data *link_ptr;
	Int_Set *list;
	Int_Set_Itr itr;
	Int_Map_Itr int_itr;

	for (i = 1; i <= link_equiv.Max_Group (); i++) {
		list = link_equiv.Group_List (i);

		if (list == 0) continue;

		for (itr = list->begin (); itr != list->end (); itr++) {
			lnk = *itr;
			int_itr = link_map.find (abs (lnk));
			if (int_itr == link_map.end ()) continue;

			link_ptr = &link_array [int_itr->second];

			if (lnk > 0 && link_ptr->AB_Dir () >= 0) {
				Sum_Group (i, link_ptr, link_ptr->AB_Dir ());
			} else if (lnk < 0 && link_ptr->BA_Dir () >= 0) {
				Sum_Group (i, link_ptr, link_ptr->BA_Dir ());
			}
		}
	}

	//---- print the page header ----
	
	Header_Number (LINK_GROUP);

	if (!Break_Check ((int) group_data.size () + 9)) {
		Print (1);
		Link_Group_Header ();
	}

	//---- print each link group ----

	for (map_itr = group_data.begin (); map_itr != group_data.end (); map_itr++) {
		data = &map_itr->second;
		n = data->number;
		if (n == 0) continue;
	
		Calc_Stats (data, stats);
		
		i = data->index;
		
		label = link_equiv.Group_Label (i);
		if (label.empty ()) {
			label = String ("Link Group %d") % i;
		}
		label = String ("%-25.25s") % label;
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
		Print (0, String (stats_format) % "TOTAL" % n % total.volume % total.count % 
			(total.volume - total.count) % stats.per_diff % stats.avg_error % stats.per_error % 
			stats.std_dev % stats.rmse % stats.r_sq % stats.avg_vc % total.max_vc);
		Print (0, "\n");
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Link_Group_Header
//---------------------------------------------------------

void Validate::Link_Group_Header (void)
{
	Page_Check (5);
	Print (0, String ("\n%44cSummary Statistics by Link Group\n") % BLANK);
	Print (0, header1);
	Print (0, String ("\nLink Group               ") + header2);
}

/**********************************************************|***********************************************************

                                            Summary Statistics by Link Group
									
                             Num.  ------Volume------ ---Difference---  --Abs.Error--    Std.     %     R   ----V/C----
Link Group                   Obs.  Estimate  Observed   Volume     %      Avg.    %      Dev.   RMSE   Sq.   Avg.  Max.

sssssssssssssssssssssssss ddddddd lllllllll lllllllll llllllll fffff.f lllllll ffff.f lllllll ffff.f  f.fff ff.ff ff.ff

TOTAL                     ddddddd lllllllll lllllllll llllllll fffff.f lllllll ffff.f lllllll ffff.f  f.fff ff.ff ff.ff

***********************************************************|***********************************************************/ 
