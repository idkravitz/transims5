//*********************************************************
//	Area_Stats.cpp - Area Type Statistics
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Area_Stats
//---------------------------------------------------------

void Validate::Area_Stats (void)
{
	int i, n, type;
	String label;

	Group_Data *data;
	Group_Map_Itr map_itr;
	Statistics stats;
	Link_Itr link_itr;
	Str_Map_Itr str_itr;
	Zone_Data *zone_ptr;

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		if (link_itr->AB_Dir () >= 0) {
			type = link_itr->Area_Type ();

			if (type == 0 && zone_flag) {
				n = node_zone [link_itr->Anode ()];
				if (n >= 0) {
					zone_ptr = &zone_array [n];
					type = zone_ptr->Area_Type ();
				}
			}
			Sum_Group (type, &(*link_itr), link_itr->AB_Dir ());
		}
		if (link_itr->BA_Dir () >= 0) {
			type = link_itr->Area_Type ();

			if (type == 0 && zone_flag) {
				n = node_zone [link_itr->Bnode ()];
				if (n >= 0) {
					zone_ptr = &zone_array [n];
					type = zone_ptr->Area_Type ();
				}
			}
			Sum_Group (type, &(*link_itr), link_itr->BA_Dir ());
		}
	}

	//---- print the page header ----

	Header_Number (AREA_TYPE);

	if (!Break_Check ((int) group_data.size () + 9)) {
		Print (1);
		Area_Header ();
	}

	//---- print each area type ----

	for (map_itr = group_data.begin (); map_itr != group_data.end (); map_itr++) {
		data = &map_itr->second;
		n = data->number;
		if (n == 0) continue;
	
		Calc_Stats (data, stats);
		
		i = data->index;

		str_itr = area_type.find (i);
		if (str_itr == area_type.end ()) {
			label = String ("Area Type %d") % i;
		} else {
			label = str_itr->second;
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
//	Area_Header
//---------------------------------------------------------

void Validate::Area_Header (void)
{
	Page_Check (5);
	Print (0, String ("\n%44cSummary Statistics by Area Type\n") % BLANK);
	Print (0, header1);
	Print (0, String ("\n                Area Type") + header2);
}
	 
/**********************************************************|***********************************************************

                                            Summary Statistics by Area Type
									
                             Num.  ------Volume------ ---Difference---  --Abs.Error--    Std.     %     R   ----V/C----
                Area Type    Obs.  Estimate  Observed   Volume     %      Avg.    %      Dev.   RMSE   Sq.   Avg.  Max.

sssssssssssssssssssssssss ddddddd lllllllll lllllllll llllllll fffff.f lllllll ffff.f lllllll ffff.f  f.fff ff.ff ff.ff

                    TOTAL ddddddd lllllllll lllllllll llllllll fffff.f lllllll ffff.f lllllll ffff.f  f.fff ff.ff ff.ff
    
***********************************************************|***********************************************************/ 
