//*********************************************************
//	Read_Details.cpp - read the link detail file
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Read_Details
//---------------------------------------------------------

void TransimsNet::Read_Details (void)
{
	int link, index, lanes, low, high;
	Link_Data *link_ptr;
	Int_Map_Itr map_itr;
	Link_Detail_Map_Stat map_stat;
	Link_Detail_Data detail_rec, *detail_ptr;

	//---- read link details----

	Show_Message (String ("Reading %s -- Record") % detail_file.File_Type ());
	Set_Progress ();

	while (detail_file.Read_Record ()) {
		Show_Progress ();

		//---- get the link number ----

		link = detail_file.Link ();
		if (link == 0) continue;

		map_itr = link_map.find (link);
		if (map_itr == link_map.end ()) {
			Warning (String ("Link Detail Link %d was Not Found") % link);
			continue;
		}
		link_ptr = &link_array [map_itr->second];

		detail_rec.Link (map_itr->second);
		detail_rec.Dir (detail_file.Dir ());

		if (detail_rec.Dir () == 0) {
			index = link_ptr->AB_Dir ();
		} else {
			index = link_ptr->BA_Dir ();
		}
		if (index < 0) {
			Warning (String ("Link Detail Link %d Direction %s was Not Found") % link % ((detail_rec.Dir () == 0) ? "AB" : "BA"));
			continue;
		}
		map_stat = detail_map.insert (Link_Detail_Map_Data (index, detail_rec));
		if (!map_stat.second) {
			Warning ("Duplicate Link Detail Record for Link = ") << link;
			continue;
		}
		detail_ptr = &map_stat.first->second;

		detail_ptr->Control (detail_file.Control ());
		detail_ptr->Group (detail_file.Group ());
		detail_ptr->LM_Length (detail_file.LM_Length ());
		detail_ptr->Left_Merge (detail_file.Left_Merge ());
		detail_ptr->LT_Length (detail_file.LT_Length ());
		detail_ptr->Left (detail_file.Left ());
		detail_ptr->Left_Thru (detail_file.Left_Thru ());
		detail_ptr->Thru (detail_file.Thru ());
		detail_ptr->Right_Thru (detail_file.Right_Thru ());
		detail_ptr->Right (detail_file.Right ());
		detail_ptr->RT_Length (detail_file.RT_Length ());
		detail_ptr->Right_Merge (detail_file.Right_Merge ());
		detail_ptr->RM_Length (detail_file.RM_Length ());
		detail_ptr->Use (detail_file.Use ());

		//---- lane number ----

		lanes = detail_file.Lanes ();

		if (detail_file.Version () <= 40 && lanes > 0) {
			low = high = lanes - 1;
		} else {
			Convert_Lane_Range (index, lanes, low, high);
		}
		detail_ptr->Low_Lane (low);
		detail_ptr->High_Lane (high);

		detail_ptr->Period (detail_file.Period ());
	}
	End_Progress ();

	detail_file.Close ();

	Print (2, "Number of ") << detail_file.File_Type () << " Records = " << detail_map.size ();
}
