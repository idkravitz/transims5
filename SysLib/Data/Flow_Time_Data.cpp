//*********************************************************
//	Flow_Time_Data.cpp - flow rate and travel time data
//*********************************************************

#include "Flow_Time_Data.hpp"
#include "Data_Service.hpp"

//---------------------------------------------------------
//	Combine_Flow
//---------------------------------------------------------

void Flow_Time_Data::Combine_Flow (double flw)
{
	if (time > 0) {
		flow = (float) ((flow * time + flw) / (time + 1));
		time = time + 1;
	} else {
		flow = (float) flw;
		time = 1;
	}
}

//---------------------------------------------------------
//	Combine_Flow
//---------------------------------------------------------

void  Flow_Time_Data::Combine_Flow_Time (double flw, Dtime tim)
{
	if (flow > 0) {
		double tot = flow + flw;
		time = (int) ((time * flow + tim * flw) / tot + 0.5); 
		flow = (float) tot;
	} else {
		flow = (float) flw;
		time = tim;
	}
}

//---------------------------------------------------------
//	Total_Flow_Time
//---------------------------------------------------------

Flow_Time_Data Flow_Time_Array::Total_Flow_Time (int index) 
{
	return (Total_Flow_Time (index, dat->dir_array [index].Flow_Index ()));
}

Flow_Time_Data Flow_Time_Array::Total_Flow_Time (int index, int flow_index) 
{
	Flow_Time_Data rec = at (index);

	if (flow_index >= 0) {
		Flow_Time_Data *ptr = &at (flow_index);
		rec.Combine_Flow_Time (ptr->Flow (), ptr->Time ());
	}
	return (rec);
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Flow_Time_Period_Array::Initialize (Time_Periods *time_periods, int num)
{
	periods = time_periods;

	if (num > 0) {
		num_records = num;
	} else {
		num_records = (int) dat->dir_array.size () + dat->Num_Lane_Use_Flows ();
	}
	if (num_records > 0) {
		Flow_Time_Period_Itr period_itr;
		Flow_Time_Array period_rec;
		Flow_Time_Data flow_data;

		assign (periods->Num_Periods (), period_rec);

		for (period_itr = begin (); period_itr != end (); period_itr++) {
			period_itr->assign (num_records, flow_data);
		}
	}
}

//---------------------------------------------------------
//	Replicate
//---------------------------------------------------------

void Flow_Time_Period_Array::Replicate (Flow_Time_Period_Array &period_array)
{
	periods = period_array.periods;
	num_records = period_array.num_records;

	if (num_records > 0) {
		Flow_Time_Period_Itr period_itr;
		Flow_Time_Array period_rec;
		Flow_Time_Data flow_data;

		assign (periods->Num_Periods (), period_rec);

		for (period_itr = begin (); period_itr != end (); period_itr++) {
			period_itr->assign (num_records, flow_data);
		}
	}
}

//---------------------------------------------------------
//	Set_Time0
//---------------------------------------------------------

void Flow_Time_Period_Array::Set_Time0 (void)
{
	if (num_records > 0) {
		int num, index, size;
		Flow_Time_Period_Itr period_itr;
		Flow_Time_Itr flow_itr;

		size = (int) dat->dir_array.size ();

		for (period_itr = begin (); period_itr != end (); period_itr++) {
			for (num=0, flow_itr = period_itr->begin (); flow_itr != period_itr->end (); flow_itr++, num++) {
				if (num < size) {
					index = num;
				} else {
					index = dat->lane_use_flow_index [num - size];
				}
				flow_itr->Time (dat->dir_array [index].Time0 ());
			}
		}
	}
}

//---------------------------------------------------------
//	Zero_Flows
//---------------------------------------------------------

void Flow_Time_Period_Array::Zero_Flows (void)
{
	if (num_records > 0) {
		Flow_Time_Period_Itr period_itr;
		Flow_Time_Itr flow_itr;

		for (period_itr = begin (); period_itr != end (); period_itr++) {
			for (flow_itr = period_itr->begin (); flow_itr != period_itr->end (); flow_itr++) {
				flow_itr->Flow (0.0);
			}
		}
	}
}

//---------------------------------------------------------
//	Copy_Flow_Data
//---------------------------------------------------------

void Flow_Time_Period_Array::Copy_Flow_Data (Flow_Time_Period_Array &period_array, bool zero_flag)
{
	if (num_records == 0) Replicate (period_array);

	if (num_records > 0) {
		int p, i;
		Flow_Time_Period_Itr period_itr;
		Flow_Time_Array *period_ptr;
		Flow_Time_Itr flow_itr;
		Flow_Time_Data *flow_ptr;

		for (p=0, period_itr = begin (); period_itr != end (); period_itr++, p++) {
			period_ptr = &period_array [p];

			for (i=0, flow_itr = period_itr->begin (); flow_itr != period_itr->end (); flow_itr++, i++) {
				flow_ptr = period_ptr->Data_Ptr (i);

				flow_itr->Flow (flow_ptr->Flow ());
				flow_itr->Time (flow_ptr->Time ());

				if (zero_flag) flow_ptr->Flow (0.0);
			}
		}
	}
}

//---------------------------------------------------------
//	Combine_Flows
//---------------------------------------------------------

void Flow_Time_Period_Array::Combine_Flows (Flow_Time_Period_Array &period_array, bool zero_flag)
{
	if (num_records == 0) Replicate (period_array);

	if (num_records > 0) {
		int p, i;
		Flow_Time_Period_Itr period_itr;
		Flow_Time_Array *period_ptr;
		Flow_Time_Itr flow_itr;
		Flow_Time_Data *flow_ptr;

		for (p=0, period_itr = begin (); period_itr != end (); period_itr++, p++) {
			period_ptr = &period_array [p];

			for (i=0, flow_itr = period_itr->begin (); flow_itr != period_itr->end (); flow_itr++, i++) {
				flow_ptr = period_ptr->Data_Ptr (i);

				flow_itr->Add_Flow (flow_ptr->Flow ());
				if (zero_flag) flow_ptr->Flow (0.0);
			}
		}
	}
}

//---------------------------------------------------------
//	Period_Ptr
//---------------------------------------------------------

Flow_Time_Array * Flow_Time_Period_Array::Period_Ptr (Dtime time)
{
	int period = periods->Period (time);

	if (period >= 0) {
		return (&at (period));
	} else {
		return (0);
	}
}
