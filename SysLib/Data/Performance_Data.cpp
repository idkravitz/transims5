//*********************************************************
//	Performance_Data.cpp - network performance data
//*********************************************************

#include "Performance_Data.hpp"
#include "Data_Service.hpp"

//---------------------------------------------------------
//	Total_Flow_Time
//---------------------------------------------------------

Flow_Time_Data Link_Perf_Array::Total_Flow_Time (int index) 
{
	return (Total_Flow_Time (index, dat->dir_array [index].Flow_Index ()));
}

Flow_Time_Data Link_Perf_Array::Total_Flow_Time (int index, int flow_index) 
{
	Flow_Time_Data rec = at (index);

	if (flow_index >= 0) {
		Flow_Time_Data *ptr = &at (flow_index);
		rec.Combine_Flow_Time (ptr->Flow (), ptr->Time ());
	}
	return (rec);
}
//---------------------------------------------------------
//	Total_Link_Perf
//---------------------------------------------------------

Link_Perf_Data Link_Perf_Array::Total_Link_Perf (int index) 
{
	return (Total_Link_Perf (index, dat->dir_array [index].Flow_Index ()));
}

Link_Perf_Data Link_Perf_Array::Total_Link_Perf (int index, int flow_index) 
{
	Link_Perf_Data rec = at (index);

	if (flow_index >= 0) {
		Link_Perf_Data *ptr = &at (flow_index);

		rec.Combine_Flow_Time (ptr->Flow (), ptr->Time ());

		rec.Add_Density (ptr->Density ());
		rec.Add_Max_Density (ptr->Max_Density ());
		rec.Add_Queue (ptr->Queue ());
		rec.Add_Max_Queue (ptr->Max_Queue ());
		rec.Add_Failure (ptr->Failure ());
		rec.Add_Occupant ();

		if (dat->Ratio_Flag ()) {
			Dir_Data *dir_ptr = &dat->dir_array [index];
			
			int time_ratio = dat->Round (ptr->Time () * 100.0 / dir_ptr->Time0 ());

			if (time_ratio >= dat->Congested_Ratio ()) {
				Link_Data *link_ptr = &dat->link_array [dir_ptr->Link ()];

				rec.Add_Ratio ();
				rec.Add_Ratio_VMT (dat->DTOI (ptr->Flow () * link_ptr->Length ()));
				rec.Add_Ratio_VHT (dat->DTOI (ptr->Time () * ptr->Flow ()));
			}
		}
	}
	return (rec);
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Link_Perf_Period_Array::Initialize (Time_Periods *time_periods, int num)
{
	periods = time_periods;

	if (num > 0) {
		num_records = num;
	} else {
		num_records = (int) dat->dir_array.size () + dat->Num_Lane_Use_Flows ();
	}
	if (num_records > 0) {
		Link_Perf_Period_Itr period_itr;
		Link_Perf_Array period_rec;
		Link_Perf_Data perf_data;

		assign (periods->Num_Periods (), period_rec);

		for (period_itr = begin (); period_itr != end (); period_itr++) {
			period_itr->assign (num_records, perf_data);
		}
	}
}

//---------------------------------------------------------
//	Replicate
//---------------------------------------------------------

void Link_Perf_Period_Array::Replicate (Link_Perf_Period_Array &period_array)
{
	periods = period_array.periods;
	num_records = period_array.num_records;

	if (num_records > 0) {
		Link_Perf_Period_Itr period_itr;
		Link_Perf_Array period_rec;
		Link_Perf_Data perf_data;

		assign (periods->Num_Periods (), period_rec);

		for (period_itr = begin (); period_itr != end (); period_itr++) {
			period_itr->assign (num_records, perf_data);
		}
	}
}

//---------------------------------------------------------
//	Period_Ptr
//---------------------------------------------------------

Link_Perf_Array * Link_Perf_Period_Array::Period_Ptr (Dtime time)
{
	int period = periods->Period (time);

	if (period >= 0) {
		return (&at (period));
	} else {
		return (0);
	}
}
