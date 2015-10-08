//*********************************************************
//	Vol_Stats.cpp - Volume Level Statistics
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Volume_Stats
//---------------------------------------------------------

void Validate::Volume_Stats (void)
{
	int i, n;
	String label;

	Group_Data *data;
	Group_Map_Itr map_itr;
	Statistics stats;
	Link_Itr link_itr;

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		if (link_itr->AB_Dir () >= 0) {
			Sum_Group (-1, &(*link_itr), link_itr->AB_Dir ());
		}
		if (link_itr->BA_Dir () >= 0) {
			Sum_Group (-1, &(*link_itr), link_itr->BA_Dir ());
		}
	}

	//---- print the page header ----

	Header_Number (VOLUME_LEVEL);

	if (!Break_Check ((int) group_data.size () + 9)) {
		Print (1);
		Volume_Header ();
	}
	
	//---- print each volume level ----

	for (map_itr = group_data.begin (); map_itr != group_data.end (); map_itr++) {
		data = &map_itr->second;
		n = data->number;
		if (n == 0) continue;

		i = data->index;
		if (i == 0) {
			label = String ("0 to %d") % volume_level [i];
		} else {
			label = String ("%d to %d") % volume_level [i-1] % volume_level [i];
		}
		Calc_Stats (data, stats);

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
		Print (0, String (stats_format) % "TOTAL" %	n % total.volume % total.count % 
			(total.volume - total.count) % stats.per_diff % stats.avg_error % stats.per_error % 
			stats.std_dev % stats.rmse % stats.r_sq % stats.avg_vc % total.max_vc);
		Print (0, "\n");
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Volume_Header
//---------------------------------------------------------

void Validate::Volume_Header (void)
{
	Page_Check (5);
	Print (0, String ("\n%42cSummary Statistics by Volume Level\n") % BLANK);
	Print (0, header1);
	Print (0, String ("\n             Volume Level") + header2);
}

/**********************************************************|**********************************************************

                                          Summary Statistics by Volume Level

                             Num.  ------Volume------ ---Difference---  --Abs.Error--    Std.     %     R   ----V/C----
            Volume Level     Obs.  Estimate  Observed   Volume     %      Avg.    %      Dev.   RMSE   Sq.   Avg.  Max.

          xxxxx to xxxxx  ddddddd lllllllll lllllllll llllllll fffff.f lllllll ffff.f lllllll ffff.f  f.fff ff.ff ff.ff

                   TOTAL  ddddddd lllllllll lllllllll llllllll fffff.f lllllll ffff.f lllllll ffff.f  f.fff ff.ff ff.ff
    
***********************************************************|**********************************************************/ 
