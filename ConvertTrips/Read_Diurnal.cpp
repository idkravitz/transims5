//*********************************************************
//	Read_Diurnal.cpp - read the time distribution file
//*********************************************************

#include "File_Group.hpp"

//---------------------------------------------------------
//	Read_Diurnal
//---------------------------------------------------------

void File_Group::Read_Diurnal (void)
{
	int i, j, k, k1, num, num_rec, num_shares;
	Dtime start, end, minute, t, t1;
	double value, share, total;
	bool share_flag;

	Range_Data range;
	Diurnal_Data diurnal_rec;
	Diurnal_Itr diurnal_itr;
	Share_Data *share_ptr;

	minute = Dtime (60, SECONDS);

	//---- read the trip time into memory ----

	diurnal_array.clear ();

	num_shares = Num_Shares ();
	share_flag = (num_shares > 0);
	if (!share_flag) num_shares = 1;

	diurnal_rec.Num_Shares (num_shares);

	exe->Show_Message (0, String ("\tReading %s -- Record") % diurnal_file->File_Type ());
	exe->Set_Progress ();

	num = 0;

	while (diurnal_file->Read ()) {
		exe->Show_Progress ();

		start = diurnal_file->Start ();
		end = diurnal_file->End ();

		if (start == 0 && end == 0) continue;
		if (start >= end) {
			exe->Error (String ("Time Range is Inappropriate (%d >= %d)") % start % end);
		}
		share = (double) (end - start) / minute;
		if (share < 1.0) share = 1.0;
		if (!share_flag) {
			diurnal_rec [0].Share (diurnal_file->Share () / share);
		}
		num++;

		for (t = start; t < end; t = t1) {
			t1 = t + minute;

			if (!In_Range (t, t1)) continue;

			diurnal_rec.Start_Time (t);
			diurnal_rec.End_Time (t1);

			//---- get the share data ----

			if (share_flag) {
				for (i=0; i < num_shares; i++) {
					Db_Field *fld = diurnal_header->Field (i);
					value = diurnal_file->Get_Double (fld->Name ().c_str ());
					diurnal_rec [i].Share (value / share);
				}
			}
			diurnal_array.push_back (diurnal_rec);
		}
	}
	exe->End_Progress ();

	diurnal_file->Close ();

	//---- smooth, normalize and convert to a cumulative distribution ----

	num_rec = (int) diurnal_array.size ();
	num = num_rec / num;
	num = (num + 1) / 2;
	if (num > 100) num = 100;
	num *= num;

	for (i=0; i < num_shares; i++) {

		//---- smooth the distribution ----

		for (j=0; j < num; j++) {
			for (diurnal_itr = diurnal_array.begin (); diurnal_itr != diurnal_array.end (); diurnal_itr++) {
				share_ptr = &diurnal_itr->at (i);
				share_ptr->Target (share_ptr->Share ());
				share_ptr->Share (0.0);
			}
			for (k=0; k < num_rec; k++) {
				share_ptr = &diurnal_array [k] [i];

				total = share_ptr->Target ();
				share = total * 0.2;

				share_ptr->Add_Share (total - 2.0 * share);

				k1 = k - 1;
				if (k1 < 0) k1 = 0;

				diurnal_array [k1] [i].Add_Share (share);

				k1 = k + 1;
				if (k1 >= num_rec) k1 = num_rec - 1;

				diurnal_array [k1] [i].Add_Share (share);
			}
		}
		total = 0.0;

		for (diurnal_itr = diurnal_array.begin (); diurnal_itr != diurnal_array.end (); diurnal_itr++) {
			total += diurnal_itr->at (i).Share ();
		}
		if (total == 0.0) {
			exe->Error (String ("Total Diurnal Distribution is Zero for Group %d") % group);
		}
		share = 1.0 / total;
		total = 0.0;

		for (diurnal_itr = diurnal_array.begin (); diurnal_itr != diurnal_array.end (); diurnal_itr++) {
			share_ptr = &diurnal_itr->at (i);

			total = share_ptr->Share () * share;

			share_ptr->Share (total);
			share_ptr->Target (0.0);
			share_ptr->Trips (0);
		}
	}
}
