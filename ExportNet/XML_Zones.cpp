//*********************************************************
//	XML_Zones.cpp - convert the zone data
//*********************************************************

#include "ExportNet.hpp"

//---------------------------------------------------------
//	XML_Zones
//---------------------------------------------------------

void ExportNet::XML_Zones (void)
{
	int node, dir, index, new_loc, count;
	double x, y;

	Int_Map_Itr map_itr;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Location_Data *location_ptr, loc_rec;
	
	map_itr = --location_map.end ();
	new_loc = map_itr->first;
	new_loc = ((new_loc + 1) / 100 + 1) * 100;
	index = (int) location_array.size ();

	fstream &fh = xml_file.File ();

	Set_Progress ();

	fh << "<ZONES>" << endl;

	for (map_itr = location_map.begin (); map_itr != location_map.end (); map_itr++) {
		Show_Progress ();

		location_ptr = &location_array [map_itr->second];

		link_ptr = &link_array [location_ptr->Link ()];
		if (link_ptr->Divided () == 0) continue;

		x = UnRound (location_ptr->X ());
		y = UnRound (location_ptr->Y ());

		fh << "<ZONE NO=\"" << location_ptr->Location ();
		fh << (String ("\" XCOORD=\"%.2lf\" YCOORD=\"%.2lf\" >") % x %  y) << endl;

		if (zone_loc_flag) {
			zone_loc_file.File () << (String ("%d\t%.2lf\t%.2lf") % location_ptr->Location () % x % y) << endl;
		}

		//---- find the closest node ----

		if (location_ptr->Offset () > link_ptr->Length () / 2) {
			dir = 1 - location_ptr->Dir (); 
		} else {
			dir = location_ptr->Dir ();
		}
		if (dir == 1) {
			node = link_ptr->Bnode ();
		} else {
			node = link_ptr->Anode ();
		}
		node_ptr = &node_array [node];

		//---- access links ----

		fh << "<CONNECTORS>" << endl;

		fh << "<CONNECTOR NODENO=\"" << node_ptr->Node () << "\" DIRECTION=\"false\" />" << endl;
		fh << "<CONNECTOR NODENO=\"" << node_ptr->Node () << "\" DIRECTION=\"true\" />" << endl;

		fh << "</CONNECTORS>" << endl;
		fh << "</ZONE>" << endl;
	}

	//---- add external stations ----

	count = 0;
	Set_Progress ();

	for (map_itr = link_map.begin (); map_itr != link_map.end (); map_itr++) {
		Show_Progress ();

		link_ptr = &link_array [map_itr->second];

		if (link_ptr->Divided () != 1 && link_ptr->Divided () != 2) continue;

		if (link_ptr->Divided () == 1) {
			node_ptr = &node_array [link_ptr->Bnode ()];
			dir = 0;
		} else {
			node_ptr = &node_array [link_ptr->Anode ()];
			dir = 1;
		}
		loc_rec.Location (new_loc);
		loc_rec.Link (map_itr->second);
		loc_rec.Dir (dir);
		loc_rec.Offset (link_ptr->Length ());
		loc_rec.Setback (0);
		
		location_map.insert (Int_Map_Data (new_loc, index++));
		location_array.push_back (loc_rec);
		count++;
		
		x = UnRound (node_ptr->X ());
		y = UnRound (node_ptr->Y ());
		
		if (zone_loc_flag) {
			zone_loc_file.File () << (String ("%d\t%.2lf\t%.2lf") % new_loc % x % y) << endl;
		}
		fh << "<ZONE NO=\"" << new_loc++;
		fh << (String ("\" XCOORD=\"%.2lf\" YCOORD=\"%.2lf\" >") % x % y) << endl;

		//---- access links ----

		fh << "<CONNECTORS>" << endl;

		fh << "<CONNECTOR NODENO=\"" << node_ptr->Node () << "\" DIRECTION=\"false\" />" << endl;
		fh << "<CONNECTOR NODENO=\"" << node_ptr->Node () << "\" DIRECTION=\"true\" />" << endl;

		fh << "</CONNECTORS>" << endl;
		fh << "</ZONE>" << endl;

	}
	fh << "</ZONES>" << endl;

	Print (2, "Number of New Location Records = ") << count;
}
