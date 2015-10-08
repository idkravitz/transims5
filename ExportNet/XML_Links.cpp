//*********************************************************
//	XML_Links.cpp - convert the link data
//*********************************************************

#include "ExportNet.hpp"

//---------------------------------------------------------
//	XML_Links
//---------------------------------------------------------

void ExportNet::XML_Links (void)
{
	int i, max_link;
	bool flip, shape_flag;
	String speed;

	Int_Map_Itr map_itr;
	Node_Data *anode_ptr, *bnode_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Shape_Data *shape_ptr;
	XYZ_Itr xyz_itr;
	XYZ_RItr xyz_ritr;
	Split_Array *split_ptr;

	fstream &fh = xml_file.File ();

	Set_Progress ();

	shape_flag = System_File_Flag (SHAPE);

	//---- write the facility types ----

	fh << "<LINKTYPES>" << endl;

	for (int type=1; type <= EXTERNAL; type++) {
		fh << "<LINKTYPE NO=\"" << type << "\" NAME=\"" << Facility_Code ((Facility_Type) type) << "\" DRIVINGBEHAVIOR=\"Urban\" />" << endl;
	}
	fh << "</LINKTYPES>" << endl;

	//---- write the link data ----

	fh << "<LINKS>" << endl;
	
	max_link = (int) link_splits.size ();

	for (map_itr = link_map.begin (); map_itr != link_map.end (); map_itr++) {
		Show_Progress ();

		link_ptr = &link_array [map_itr->second];
		if (link_ptr->Divided () == 0) continue;
		
		if (map_itr->second < max_link) {
			split_ptr = &link_splits [map_itr->second];
			if (split_ptr->size () != 0) continue;
		}

		flip = (link_ptr->Anode () > link_ptr->Bnode ());

		anode_ptr = &node_array [link_ptr->Anode ()];
		bnode_ptr = &node_array [link_ptr->Bnode ()];

		if (shape_flag && link_ptr->Shape () > 0) {
			shape_ptr = &shape_array [link_ptr->Shape ()];
		} else {
			shape_ptr = 0;
		}

		//---- AB direction ----

		if (link_ptr->AB_Dir () >= 0) {
			dir_ptr = &dir_array [link_ptr->AB_Dir ()];

			speed ("%.1lf") % External_Units (UnRound (dir_ptr->Speed ()), ((Metric_Flag ()) ? KPH : MPH));

			fh << "<LINK ID=\"" << link_ptr->Link () << ((flip) ? "B" : "A") << "\"";
			fh << " FROMNODENO=\"" << anode_ptr->Node ();
			fh << "\" TONODENO=\"" << bnode_ptr->Node ();
			fh << "\" NAME=\"" << ((link_ptr->Name () != 0) ? link_ptr->Name () : " ");
			fh << "\" LINKTYPENO=\"" << link_ptr->Type ();
			fh << "\" SPEED=\"" << speed;
			fh << "\" NUMLANES=\"" << dir_ptr->Lanes ();

			if (link_ptr->BA_Dir () >= 0) {
				fh << "\" REVERSELINK=\"" << link_ptr->Link () << ((flip) ? "A" : "B") << "\">" << endl;
			} else {
				fh << "\" >" << endl;
			}
			if (shape_ptr != 0) {
				fh << "<LINKPOLY>" << endl;
				i = 1;

				fh << "<POINT INDEX=\"" << i++;
				fh << (String ("\" XCOORD=\"%.2lf\" YCOORD=\"%.2lf\" />") % UnRound (anode_ptr->X ()) % UnRound (anode_ptr->Y ())) << endl;

				for (xyz_itr = shape_ptr->begin (); xyz_itr != shape_ptr->end (); xyz_itr++) {
					fh << "<POINT INDEX=\"" << i++;
					fh << (String ("\" XCOORD=\"%.2lf\" YCOORD=\"%.2lf\" />") % UnRound (xyz_itr->x) % UnRound (xyz_itr->y)) << endl;
				}
				fh << "<POINT INDEX=\"" << i++;
				fh << (String ("\" XCOORD=\"%.2lf\" YCOORD=\"%.2lf\" />") % UnRound (bnode_ptr->X ()) % UnRound (bnode_ptr->Y ())) << endl;

				fh << "</LINKPOLY>" << endl;
			}
			fh << "</LINK>" << endl;
		}

		//---- BA direction ----

		if (link_ptr->BA_Dir () >= 0) {
			dir_ptr = &dir_array [link_ptr->BA_Dir ()];
			
			speed ("%.1lf") % External_Units (UnRound (dir_ptr->Speed ()), ((Metric_Flag ()) ? KPH : MPH));

			fh << "<LINK ID=\"" << link_ptr->Link () << ((flip) ? "A" : "B") << "\"";
			fh << " FROMNODENO=\"" << bnode_ptr->Node ();
			fh << "\" TONODENO=\"" << anode_ptr->Node ();
			fh << "\" NAME=\"" << ((link_ptr->Name () != 0) ? link_ptr->Name () : " ");
			fh << "\" LINKTYPENO=\"" << link_ptr->Type ();
			fh << "\" SPEED=\"" << speed;
			fh << "\" NUMLANES=\"" << dir_ptr->Lanes ();

			if (link_ptr->BA_Dir () >= 0) {
				fh << "\" REVERSELINK=\"" << link_ptr->Link () << ((flip) ? "B" : "A") << "\">" << endl;
			} else {
				fh << "\" >" << endl;
			}
			if (shape_ptr != 0) {
				fh << "<LINKPOLY>" << endl;
				i = 1;

				fh << "<POINT INDEX=\"" << i++;
				fh << (String ("\" XCOORD=\"%.2lf\" YCOORD=\"%.2lf\" />") % UnRound (bnode_ptr->X ()) % UnRound (bnode_ptr->Y ())) << endl;

				for (xyz_ritr = shape_ptr->rbegin (); xyz_ritr != shape_ptr->rend (); xyz_ritr++) {
					fh << "<POINT INDEX=\"" << i++;
					fh << (String ("\" XCOORD=\"%.2lf\" YCOORD=\"%.2lf\" />") % UnRound (xyz_ritr->x) % UnRound (xyz_ritr->y)) << endl;
				}
				fh << "<POINT INDEX=\"" << i++;
				fh << (String ("\" XCOORD=\"%.2lf\" YCOORD=\"%.2lf\" />") % UnRound (anode_ptr->X ()) % UnRound (anode_ptr->Y ())) << endl;

				fh << "</LINKPOLY>" << endl;
			}
			fh << "</LINK>" << endl;
		}
	}
	fh << "</LINKS>" << endl;
}
