//*********************************************************
//	Set_Time.cpp - set the trip time
//*********************************************************

#include "ConvertTrips.hpp"

#include "math.h"

//---------------------------------------------------------
//	Set_Time
//---------------------------------------------------------

bool ConvertTrips::Set_Time (File_Group *group, Dtime &start, Dtime &end, Dtime ttim, int num)
{
	int t, t1, t2;
	double prob, share, share1, total;
	int retry_count;
	
	Share_Data *share_ptr;

	if (start > end) return (false);
	t1 = (int) start.Minutes ();
	t2 = (int) end.Minutes ();

	if (group->Diurnal_Periods () < t2) {
		t2 = group->Diurnal_Periods ();
		if (t1 >= t2) return (false);
	}

	//---- distribute the trips to the time targets ----

	if (group->Num_Shares () < 0) num = 0;
	total = 0.0;

	for (t=t1; t < t2; t++) {
		total += group->Share (t, num);
	}
	if (total == 0.0) return (false);

	for (t=t1; t < t2; t++) {
		share_ptr = group->Share_Ptr (t, num);
		share = share_ptr->Share () / total;
		share_ptr->Add_Target (share);
	}
	retry_count = 0;

retry:
	prob = random_tod.Probability ();

	share = share1 = 0.0;

	for (t=t1; t < t2; t++) {
		share_ptr = group->Share_Ptr (t, num);
		share1 = share + share_ptr->Share () / total;

		if (prob < share1) {
			if (share_ptr->Target () < share_ptr->Trips () + 0.5) {
				if (retry_count < 10) {
					retry_count++;
					goto retry;
				}
			}
			group->Diurnal_Time_Range (t, start, end);
			break;
		}
		share = share1;
	}
	if (share != share1) {
		start += (int) ((end - start) * (prob - share) / (share1 - share) + 0.5);
	}

	//---- time methods ----

	if (group->Time_Point () == TRIP_END) {
		start = start - ttim;
		if (start < 0) start = 1;
	} else if (group->Time_Point () == MID_TRIP) {
		start = start - ttim / 2;
		if (start < 0) start = 1;
	}
	end = start + ttim;
	return (true);
}
