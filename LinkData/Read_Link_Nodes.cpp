//*********************************************************
//	Read_Link_Nodes.cpp - Read the Link Node List File
//*********************************************************

#include "LinkData.hpp"

//---------------------------------------------------------
//	Read_Equiv
//---------------------------------------------------------

void LinkData::Read_Link_Nodes (void)
{
	int link, anode, bnode;
	bool flag;
	String record;
	Strings parse;
	Str_Itr str_itr;
	Int2_Map_Stat map_stat;

	Data data_rec;

	//---- initialize the data records ----

	if (data_flag) {
		data_rec.link = 0;
		data_rec.anode = 0;
		data_rec.bnode = 0;
		data_rec.num_ab = 0;
		data_rec.num_ba = 0;

		data_rec.volume_ab.assign (num_fields, 0.0);
		data_rec.volume_ba.assign (num_fields, 0.0);
		if (speed_flag) {
			data_rec.speed_ab.assign (num_fields, 0.0);
			data_rec.speed_ba.assign (num_fields, 0.0);
		}
	}

	//---- process each link ----

	Show_Message (String ("Reading %s -- Record") % link_node.File_Type ());
	Set_Progress ();

	while (link_node.Read ()) {
		Show_Progress ();

		//---- read the field ----

		record = link_node.Record_String ();

		record.Parse (parse);

		link = anode = bnode = 0;
		flag = false;

		for (str_itr = parse.begin (); str_itr != parse.end (); str_itr++) {
			if (link == 0) {
				data_rec.link = link = str_itr->Integer ();
			} else if (!str_itr->Equals ("=")) {
				if (anode == 0) {
					data_rec.anode = anode = str_itr->Integer ();
				} else {
					data_rec.bnode = bnode = str_itr->Integer ();
					if (bnode == 0) continue;

					map_stat = data_map.insert (Int2_Map_Data (Int2_Key (anode, bnode), (int) data_array.size ()));

					if (!map_stat.second) {
						Warning (String ("Duplicate AB Nodes (%d-%d)") % anode % bnode);
					} else {
						flag = true;

					}
					anode = bnode;
					nab++;
				}
			}
		}
		if (flag) {
			data_array.push_back (data_rec);
		}
	}
	End_Progress ();
	nequiv = Progress_Count ();

	link_node.Close ();
}
