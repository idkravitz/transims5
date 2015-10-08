//*********************************************************
//	Get_Detector_Data.cpp - additional detector processing
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Get_Detector_Data
//---------------------------------------------------------

bool ArcNet::Get_Detector_Data (Detector_File &file, Detector_Data &detector_rec)
{
	int dir, center, lane, lane1, lane2, index, type, pocket;
	double length, offset, start, end, side, width, setbacka, setbackb, off, len;
	bool dir_flag, left_flag;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Pocket_Data *pocket_ptr;
	Points_Itr pt_itr;

	if (Data_Service::Get_Detector_Data (file, detector_rec)) {
		if (arcview_detector.Is_Open ()) {
			arcview_detector.Copy_Fields (file);

			arcview_detector.parts.clear ();
			arcview_detector.clear ();

			dir_ptr = &dir_array [detector_rec.Dir_Index ()];
			dir = dir_ptr->Dir ();

			link_ptr = &link_array [dir_ptr->Link ()];

			offset = UnRound (detector_rec.Offset ());
			length = UnRound (detector_rec.Length ());

			if (offset == 0.0 && length == 0.0) {
				offset = 0.0;
				length = UnRound (link_ptr->Length ());
			}
			start = offset;
			end = start + length;

			length = UnRound (link_ptr->Length ());

			if (dir == 0) {
				setbacka = UnRound (link_ptr->Aoffset ());
				setbackb = UnRound (link_ptr->Boffset ());
				dir_flag = (link_ptr->BA_Dir () >= 0);
			} else {
				setbacka = UnRound (link_ptr->Boffset ());
				setbackb = UnRound (link_ptr->Aoffset ());
				dir_flag = (link_ptr->AB_Dir () >= 0);
			}
			if (start < setbacka) start = setbacka;

			side = length - setbackb;
			if (end > side) end = side;

			length = end - start;
			offset = start;

			//---- draw the lanes ----

			if (lanes_flag) {
				if (center_flag) {
					if (!dir_flag) {
						center = dir_ptr->Lanes () + dir_ptr->Left () + dir_ptr->Right () + 1;
					} else {
						center = 1;
					}
				} else {
					center = 1;
				}

				//---- set the lane range ----

				lane1 = detector_rec.Low_Lane ();
				lane2 = detector_rec.High_Lane ();

				width = lane_width / 2.0;

				for (lane = lane1; lane <= lane2; lane++) {
					side = (2 + 2 * lane - center) * width;
					left_flag = (lane < dir_ptr->Left ());
					off = offset;
					len = length;

					if (left_flag || lane >= (dir_ptr->Left () + dir_ptr->Lanes ())) {
						for (index = dir_ptr->First_Pocket (); index >= 0; index = pocket_ptr->Next_Index ()) {
							pocket_ptr = &pocket_array [index];

							type = pocket_ptr->Type ();
							pocket = pocket_ptr->Lanes ();

							if (type == LEFT_TURN || type == LEFT_MERGE) {
								if (!left_flag) continue;
								if (lane < (dir_ptr->Left () - pocket)) continue;
							} else {
								if (left_flag) continue;
								if (lane >= dir_ptr->Left () + dir_ptr->Lanes () + pocket) continue;
							}
							off = UnRound (pocket_ptr->Offset ());
							len = UnRound (pocket_ptr->Length ());

							if (off > end || (off + len) < start) continue;

							len += off;
							if (off < start) off = start;
							if (len > end) len = end;
							len -= off;
							if (len < 0) len = 0;
							break;
						}
						if (index < 0) continue;
					}

					Link_Shape (link_ptr, dir, points, off, len, side);

					arcview_detector.parts.push_back ((int) arcview_detector.size ());

					for (pt_itr = points.begin (); pt_itr != points.end (); pt_itr++) {
						arcview_detector.push_back (*pt_itr);
					}
				}
			} else {
				arcview_detector.parts.push_back ((int) arcview_detector.size ());

				Link_Shape (link_ptr, dir, arcview_detector, offset, length, 0.0);
			}
			if (arcview_detector.size () > 0) {
				if (!arcview_detector.Write_Record ()) {
					Error (String ("Writing %s") % arcview_detector.File_Type ());
				}
			} else {
				Warning (String ("Detector %d cound not be Drawn") % detector_rec.Detector ());
				return (false);
			}
		}
		return (true);
	}
	return (false);
}
