//*********************************************************
//	Zone_Stats.cpp - Zone Group Statistics
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Zone_Group_Stats
//---------------------------------------------------------

void Validate::Zone_Group_Stats (void)
{
	int i, n, type;
	String label;

	Group_Data *data;
	Group_Map_Itr map_itr;
	Statistics stats;
	Link_Itr link_itr;
	Str_Map_Itr str_itr;

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		if (link_itr->AB_Dir () >= 0) {
			n = node_zone [link_itr->Anode ()];
			type = zone_equiv.Zone_Group (n);

			Sum_Group (type, &(*link_itr), link_itr->AB_Dir ());
		}
		if (link_itr->BA_Dir () >= 0) {
			n = node_zone [link_itr->Bnode ()];
			type = zone_equiv.Zone_Group (n);

			Sum_Group (type, &(*link_itr), link_itr->BA_Dir ());
		}
	}

	//---- print the page header ----
	
	Header_Number (ZONE_GROUP);

	if (!Break_Check ((int) group_data.size () + 9)) {
		Print (1);
		Zone_Group_Header ();
	}

	//---- print each zone group ----

	for (map_itr = group_data.begin (); map_itr != group_data.end (); map_itr++) {
		data = &map_itr->second;
		n = data->number;
		if (n == 0) continue;

		Calc_Stats (data, stats);

		i = data->index;
		
		label = zone_equiv.Group_Label (i);
		if (label.empty ()) {
			label = String ("Zone Group %d") % i;
		}
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
		Print (0, String (stats_format) % "TOTAL" % n % total.volume % total.count % 
			(total.volume - total.count) % stats.per_diff % stats.avg_error % stats.per_error % 
			stats.std_dev % stats.rmse % stats.r_sq % stats.avg_vc % total.max_vc);
		Print (0, "\n");
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Zone_Group_Header
//---------------------------------------------------------

void Validate::Zone_Group_Header (void)
{
	Page_Check (5);
	Print (0, String ("\n%44cSummary Statistics by Zone Group\n") % BLANK);
	Print (0, header1);
	Print (0, String ("\n               Zone Group") + header2);
}

/**********************************************************|**********************************************************

                                            Summary Statistics by Zone Group
									
                             Num.  ------Volume------ ---Difference---  --Abs.Error--    Std.     %     R   ----V/C----
               Zone Group    Obs.  Estimate  Observed   Volume     %      Avg.    %      Dev.   RMSE   Sq.   Avg.  Max.

sssssssssssssssssssssssss ddddddd lllllllll lllllllll llllllll fffff.f lllllll ffff.f lllllll ffff.f  f.fff ff.ff ff.ff

                    TOTAL ddddddd lllllllll lllllllll llllllll fffff.f lllllll ffff.f lllllll ffff.f  f.fff ff.ff ff.ff
    

***********************************************************|**********************************************************/ 
