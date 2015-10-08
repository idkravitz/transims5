//*********************************************************
//	Input_SpdCap.cpp - convert the input speed-capacity file
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Input_SpdCap
//---------------------------------------------------------

void NetPrep::Input_SpdCap (void)
{
	int at, at1, at2, fc, fc1, fc2, ln, ln1, ln2, lvalue; 
	int at1_fld, at2_fld, fc1_fld, fc2_fld, ln1_fld, ln2_fld, cap_fld, cap_flg, spd_fld, spd_flg;
	string cap_flag, spd_flag;
	char ch;
	double cap, spd, dvalue;

	//---- initialize the data fields ----

	if (spdcap_file.Num_Fields () == 0) {
		spdcap_file.Add_Field ("FROM_AT", DB_INTEGER, 2);
		spdcap_file.Add_Field ("TO_AT", DB_INTEGER, 2);
		spdcap_file.Add_Field ("FROM_FC", DB_INTEGER, 2);
		spdcap_file.Add_Field ("TO_FC", DB_INTEGER, 2);
		spdcap_file.Add_Field ("FROM_LANE", DB_INTEGER, 2);
		spdcap_file.Add_Field ("TO_LANE", DB_INTEGER, 2);
		spdcap_file.Add_Field ("CAP_FLAG", DB_CHAR, 1);
		spdcap_file.Add_Field ("CAPACITY", DB_DOUBLE, 5);
		spdcap_file.Add_Field ("SPD_FLAG", DB_CHAR, 1);
		spdcap_file.Add_Field ("SPEED", DB_DOUBLE, 4);
	}

	at1_fld = spdcap_file.Optional_Field ("FROM_AT", "FROMAT", "AT1", "AT", "AREATYPE");
	at2_fld = spdcap_file.Optional_Field ("TO_AT", "TOAT", "AT2", "AT", "AREATYPE");
	fc1_fld = spdcap_file.Optional_Field ("FROM_FC", "FROMFC", "FC1", "FC", "FUNCL");
	fc2_fld = spdcap_file.Optional_Field ("TO_FC", "TOFC", "FC2", "FC", "FUNCL");
	ln1_fld = spdcap_file.Optional_Field ("FROM_LANE", "FROMLANE", "LN1", "LN", "LANE");
	ln2_fld = spdcap_file.Optional_Field ("TO_LANE", "TOLANE", "LN2", "LN", "LANE");
	cap_flg = spdcap_file.Optional_Field ("CAP_FLAG");
	cap_fld = spdcap_file.Optional_Field ("CAPACITY", "CAP");
	spd_flg = spdcap_file.Optional_Field ("SPD_FLAG");
	spd_fld = spdcap_file.Optional_Field ("SPEED", "SPD");

	memset (cap_map, '\0', sizeof (cap_map));
	memset (spd_map, '\0', sizeof (spd_map));

	//---- read each node record----

	Show_Message (String ("Reading %s -- Record") % spdcap_file.File_Type ());
	Set_Progress ();

	while (spdcap_file.Read_Record ()) {
		Show_Progress ();

		//---- read the data fields ----

		at1 = spdcap_file.Get_Integer (at1_fld);
		at2 = spdcap_file.Get_Integer (at2_fld);
		fc1 = spdcap_file.Get_Integer (fc1_fld);
		fc2 = spdcap_file.Get_Integer (fc2_fld);
		ln1 = spdcap_file.Get_Integer (ln1_fld);
		ln2 = spdcap_file.Get_Integer (ln2_fld);
		cap_flag = spdcap_file.Get_String (cap_flg);
		cap = spdcap_file.Get_Double (cap_fld);
		spd_flag = spdcap_file.Get_String (spd_flg);
		spd = spdcap_file.Get_Double (spd_fld);

		//---- save the data ----

		for (at=at1; at <= at2; at++) {
			if (at < 0 || at >= MAX_AT) {
				Error (String ("Area Type %d is Out of Range (0..%d)") % at % (MAX_AT-1));
			}
			for (fc=fc1; fc <= fc2; fc++) {
				if (fc < 0 || fc >= MAX_FUNCL) {
					Error (String ("Facility Type %d is Out of Range (0..%d)") % fc % (MAX_FUNCL-1));
				}
				for (ln=ln1; ln <= ln2; ln++) {
					if (ln < 0 || ln >= MAX_LANE) {
						Error (String ("Lanes %d is Out of Range (0..%d)") % ln % (MAX_LANE-1));
					}
					ch = cap_flag [0];

					if (ch == ' ') {
						lvalue = (int) cap;
					} else {
						lvalue = cap_map [at] [fc] [ln];

						if (ch == '*') {
							lvalue = (int) (lvalue * cap + 0.5);
						} else if (ch == '+') {
							lvalue = (int) (lvalue + cap + 0.5);
						} else if (ch == '-') {
							lvalue = (int) (lvalue - cap + 0.5);
						}
					}
					cap_map [at] [fc] [ln] = lvalue;

					ch = spd_flag [0];

					if (ch == ' ') {
						dvalue = spd;
					} else {
						dvalue = UnRound (spd_map [at] [fc] [ln]);

						if (ch == '*') {
							dvalue *= spd;
						} else if (ch == '+') {
							dvalue += spd;
						} else if (ch == '-') {
							dvalue -= spd;
						}
					}
					spd_map [at] [fc] [ln] = Round (dvalue);
				}
			}
		}
	}
	End_Progress ();

	spdcap_file.Close ();

	//---- convert speeds from mph to internal units ----

	if (units_flag != Metric_Flag ()) {
		for (at=1; at < MAX_AT; at++) {
			for (fc=1; fc < MAX_FUNCL; fc++) {
				for (ln=1; ln < MAX_LANE; ln++) {
					if (units_flag) {
						spd_map [at] [fc] [ln] = Round (UnRound (spd_map [at] [fc] [ln]) * 1000.0 / MILETOMETER);
					} else {
						spd_map [at] [fc] [ln] = Round (UnRound (spd_map [at] [fc] [ln]) * MILETOMETER / 1000.0);
					}
				}
			}
		}
	}
	Print (2, String ("Number of %s Records = %d") % spdcap_file.File_Type () % Progress_Count ());
}
