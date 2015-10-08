//*********************************************************
//	New_Connects.cpp - update and insert link connections
//*********************************************************

#include "ExportNet.hpp"

//---------------------------------------------------------
//	New_Connections
//---------------------------------------------------------

void ExportNet::New_Connections (void)
{
	int index, link, link1, link2, link3, dir, dir1, dir2, dir3, size, count;

	Link_Data *link_ptr, *link1_ptr, *link2_ptr, *link3_ptr;
	Dir_Data *dir_ptr, *dir1_ptr, *dir2_ptr, *dir3_ptr;
	Connect_Data connect_rec, *connect_ptr;
	Split_Itr split_itr, split2_itr, end_itr;
	Link_Split_Itr link_itr;

	//---- add connections for each parking link ----

	count = 0;
	Set_Progress ();

	for (link=0, link_itr = link_splits.begin (); link_itr != link_splits.end (); link_itr++, link++) {
		if (link_itr->size () == 0) continue;
		Show_Progress ();

		link_ptr = &link_array [link];

		//---- update the existing connections ----

		split_itr = link_itr->begin ();

		end_itr = link_itr->end ();
		split2_itr = end_itr - 1;

		link1 = split_itr->link;
		link1_ptr = &link_array [link1];

		link2 = split2_itr->link;
		link2_ptr = &link_array [link2];

		dir = link_ptr->AB_Dir ();

		if (dir >= 0) {
			dir_ptr = &dir_array [dir];

			//---- enter anode ----

			if (link_ptr->Divided () != 2) {
				dir1 = link1_ptr->AB_Dir ();
				dir1_ptr = &dir_array [dir1];

				index = dir_ptr->First_Connect_From ();
				dir1_ptr->First_Connect_From (index);

				for (; index >= 0; index = connect_ptr->Next_From ()) {
					connect_ptr = &connect_array [index];
					connect_ptr->To_Index (dir1);
				}
			}

			//---- exit bnode ----

			if (link_ptr->Divided () != 1) {
				dir2 = link2_ptr->AB_Dir ();
				dir2_ptr = &dir_array [dir2];

				index = dir_ptr->First_Connect_To ();
				dir2_ptr->First_Connect_To (index);

				for (; index >= 0; index = connect_ptr->Next_To ()) {
					connect_ptr = &connect_array [index];
					connect_ptr->Dir_Index (dir2);
				}
			}
		}
		dir = link_ptr->BA_Dir ();

		if (dir >= 0) {
			dir_ptr = &dir_array [dir];

			//---- exit anode ----

			if (link_ptr->Divided () != 2) {
				dir1 = link1_ptr->BA_Dir ();
				dir1_ptr = &dir_array [dir1];

				index = dir_ptr->First_Connect_To ();
				dir1_ptr->First_Connect_To (index);

				for (; index >= 0; index = connect_ptr->Next_To ()) {
					connect_ptr = &connect_array [index];
					connect_ptr->Dir_Index (dir1);
				}
			}

			//---- enter bnode ----

			if (link_ptr->Divided () != 1) {
				dir2 = link2_ptr->BA_Dir ();
				dir2_ptr = &dir_array [dir2];

				index = dir_ptr->First_Connect_From ();
				dir2_ptr->First_Connect_From (index);

				for (; index >= 0; index = connect_ptr->Next_From ()) {
					connect_ptr = &connect_array [index];
					connect_ptr->To_Index (dir2);
				}
			}
		}

		//---- connect each parking link ----

		for (; split_itr != end_itr; split_itr++) {
			split2_itr = split_itr + 1;
			if (split2_itr == end_itr) break;

			link1 = split_itr->link;
			link1_ptr = &link_array [link1];

			//---- AB direction ----

			dir1 = link1_ptr->AB_Dir ();

			if (dir1 >= 0) {
				dir1_ptr = &dir_array [dir1];

				connect_rec.Clear ();

				//---- add thru lanes ----

				link2 = split2_itr->link;
				link2_ptr = &link_array [link2];

				dir2 = link2_ptr->AB_Dir ();
				dir2_ptr = &dir_array [dir2];

				connect_rec.Dir_Index (dir1);
				connect_rec.Low_Lane (dir1_ptr->Left ());
				connect_rec.High_Lane (dir1_ptr->Left () + dir1_ptr->Lanes () - 1);
				connect_rec.To_Index (dir2);
				connect_rec.To_Low_Lane (dir2_ptr->Left ());
				connect_rec.To_High_Lane (dir2_ptr->Left () + dir2_ptr->Lanes () - 1);
				connect_rec.Type (THRU);

				size = (int) connect_array.size ();

				connect_rec.Next_To (dir1_ptr->First_Connect_To ());
				dir1_ptr->First_Connect_To (size);

				connect_rec.Next_From (dir2_ptr->First_Connect_From ());
				dir2_ptr->First_Connect_From (size);

				connect_array.push_back (connect_rec);
				count++;

				//---- add right turn ----

				link3 = split_itr->link_ab;

				if (link3 >= 0) {
					link3_ptr = &link_array [link3];
				
					dir3 = link3_ptr->AB_Dir ();
					dir3_ptr = &dir_array [dir3];

					connect_rec.Dir_Index (dir1);
					connect_rec.Low_Lane (dir1_ptr->Left () + dir1_ptr->Lanes () - 1);
					connect_rec.High_Lane (connect_rec.Low_Lane ());
					connect_rec.To_Index (dir3);
					connect_rec.To_Low_Lane (0);
					connect_rec.To_High_Lane (0);
					connect_rec.Type (RIGHT);

					size = (int) connect_array.size ();

					connect_rec.Next_To (dir1_ptr->First_Connect_To ());
					dir1_ptr->First_Connect_To (size);

					connect_rec.Next_From (dir3_ptr->First_Connect_From ());
					dir3_ptr->First_Connect_From (size);

					connect_array.push_back (connect_rec);
					count++;

					dir3 = link3_ptr->BA_Dir ();
					dir3_ptr = &dir_array [dir3];

					connect_rec.Dir_Index (dir3);
					connect_rec.Low_Lane (0);
					connect_rec.High_Lane (0);
					connect_rec.To_Index (dir2);
					connect_rec.To_Low_Lane (dir2_ptr->Left () + dir2_ptr->Lanes () - 1);
					connect_rec.To_High_Lane (connect_rec.To_Low_Lane ());
					connect_rec.Type (RIGHT);

					size = (int) connect_array.size ();

					connect_rec.Next_To (dir3_ptr->First_Connect_To ());
					dir3_ptr->First_Connect_To (size);

					connect_rec.Next_From (dir2_ptr->First_Connect_From ());
					dir2_ptr->First_Connect_From (size);

					connect_array.push_back (connect_rec);
					count++;
				}

				//---- add left turn ----

				link3 = split_itr->link_ba;

				if (link3 >= 0) {
					link3_ptr = &link_array [link3];
				
					dir3 = link3_ptr->AB_Dir ();
					dir3_ptr = &dir_array [dir3];

					connect_rec.Dir_Index (dir1);
					connect_rec.Low_Lane (dir1_ptr->Left ());
					connect_rec.High_Lane (connect_rec.Low_Lane ());
					connect_rec.To_Index (dir3);
					connect_rec.To_Low_Lane (0);
					connect_rec.To_High_Lane (0);
					connect_rec.Type (LEFT);

					size = (int) connect_array.size ();

					connect_rec.Next_To (dir1_ptr->First_Connect_To ());
					dir1_ptr->First_Connect_To (size);

					connect_rec.Next_From (dir3_ptr->First_Connect_From ());
					dir3_ptr->First_Connect_From (size);

					connect_array.push_back (connect_rec);
					count++;

					dir3 = link3_ptr->BA_Dir ();
					dir3_ptr = &dir_array [dir3];

					connect_rec.Dir_Index (dir3);
					connect_rec.Low_Lane (0);
					connect_rec.High_Lane (0);
					connect_rec.To_Index (dir2);
					connect_rec.To_Low_Lane (dir2_ptr->Left ());
					connect_rec.To_High_Lane (connect_rec.To_Low_Lane ());
					connect_rec.Type (LEFT);

					size = (int) connect_array.size ();

					connect_rec.Next_To (dir3_ptr->First_Connect_To ());
					dir3_ptr->First_Connect_To (size);

					connect_rec.Next_From (dir2_ptr->First_Connect_From ());
					dir2_ptr->First_Connect_From (size);

					connect_array.push_back (connect_rec);
					count++;
				}
			}

			//---- BA direction ----

			dir1 = link1_ptr->BA_Dir ();

			if (dir1 >= 0) {
				dir1_ptr = &dir_array [dir1];

				connect_rec.Clear ();

				//---- add thru lanes ----

				link2 = split2_itr->link;
				link2_ptr = &link_array [link2];

				dir2 = link2_ptr->BA_Dir ();
				dir2_ptr = &dir_array [dir2];

				connect_rec.Dir_Index (dir2);
				connect_rec.Low_Lane (dir2_ptr->Left ());
				connect_rec.High_Lane (dir2_ptr->Left () + dir1_ptr->Lanes () - 1);
				connect_rec.To_Index (dir1);
				connect_rec.To_Low_Lane (dir1_ptr->Left ());
				connect_rec.To_High_Lane (dir1_ptr->Left () + dir1_ptr->Lanes () - 1);
				connect_rec.Type (THRU);

				size = (int) connect_array.size ();

				connect_rec.Next_To (dir2_ptr->First_Connect_To ());
				dir2_ptr->First_Connect_To (size);

				connect_rec.Next_From (dir1_ptr->First_Connect_From ());
				dir1_ptr->First_Connect_From (size);

				connect_array.push_back (connect_rec);
				count++;

				//---- add right turn ----

				link3 = split_itr->link_ba;

				if (link3 >= 0) {
					link3_ptr = &link_array [link3];
				
					dir3 = link3_ptr->AB_Dir ();
					dir3_ptr = &dir_array [dir3];

					connect_rec.Dir_Index (dir2);
					connect_rec.Low_Lane (dir2_ptr->Left () + dir2_ptr->Lanes () - 1);
					connect_rec.High_Lane (connect_rec.Low_Lane ());
					connect_rec.To_Index (dir3);
					connect_rec.To_Low_Lane (0);
					connect_rec.To_High_Lane (0);
					connect_rec.Type (RIGHT);

					size = (int) connect_array.size ();

					connect_rec.Next_To (dir2_ptr->First_Connect_To ());
					dir2_ptr->First_Connect_To (size);

					connect_rec.Next_From (dir3_ptr->First_Connect_From ());
					dir3_ptr->First_Connect_From (size);

					connect_array.push_back (connect_rec);
					count++;

					dir3 = link3_ptr->BA_Dir ();
					dir3_ptr = &dir_array [dir3];

					connect_rec.Dir_Index (dir3);
					connect_rec.Low_Lane (0);
					connect_rec.High_Lane (0);
					connect_rec.To_Index (dir1);
					connect_rec.To_Low_Lane (dir1_ptr->Left () + dir1_ptr->Lanes () - 1);
					connect_rec.To_High_Lane (connect_rec.To_Low_Lane ());
					connect_rec.Type (RIGHT);

					size = (int) connect_array.size ();

					connect_rec.Next_To (dir3_ptr->First_Connect_To ());
					dir3_ptr->First_Connect_To (size);

					connect_rec.Next_From (dir1_ptr->First_Connect_From ());
					dir1_ptr->First_Connect_From (size);

					connect_array.push_back (connect_rec);
					count++;
				}

				//---- add left turn ----

				link3 = split_itr->link_ab;

				if (link3 >= 0) {
					link3_ptr = &link_array [link3];
				
					dir3 = link3_ptr->AB_Dir ();
					dir3_ptr = &dir_array [dir3];

					connect_rec.Dir_Index (dir2);
					connect_rec.Low_Lane (dir2_ptr->Left ());
					connect_rec.High_Lane (connect_rec.Low_Lane ());
					connect_rec.To_Index (dir3);
					connect_rec.To_Low_Lane (0);
					connect_rec.To_High_Lane (0);
					connect_rec.Type (LEFT);

					size = (int) connect_array.size ();

					connect_rec.Next_To (dir2_ptr->First_Connect_To ());
					dir2_ptr->First_Connect_To (size);

					connect_rec.Next_From (dir3_ptr->First_Connect_From ());
					dir3_ptr->First_Connect_From (size);

					connect_array.push_back (connect_rec);
					count++;

					dir3 = link3_ptr->BA_Dir ();
					dir3_ptr = &dir_array [dir3];

					connect_rec.Dir_Index (dir3);
					connect_rec.Low_Lane (0);
					connect_rec.High_Lane (0);
					connect_rec.To_Index (dir1);
					connect_rec.To_Low_Lane (dir1_ptr->Left ());
					connect_rec.To_High_Lane (connect_rec.To_Low_Lane ());
					connect_rec.Type (LEFT);

					size = (int) connect_array.size ();

					connect_rec.Next_To (dir3_ptr->First_Connect_To ());
					dir3_ptr->First_Connect_To (size);

					connect_rec.Next_From (dir1_ptr->First_Connect_From ());
					dir1_ptr->First_Connect_From (size);

					connect_array.push_back (connect_rec);
					count++;
				}
			}
		}
	}
	Print (2, "Number of New Link Connections = ") << count++;
}
