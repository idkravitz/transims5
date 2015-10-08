//*********************************************************
//	Get_Trip_Gap.cpp - compare trip impedances
//*********************************************************

#include "Router.hpp"

#include <math.h>

//---------------------------------------------------------
//	Get_Trip_Gap
//---------------------------------------------------------

double Router::Get_Trip_Gap (void)
{
	int i, num, period, parts;
	double diff, imp, sum_diff, total, gap;
	bool report_flag, gap_flag;

	Trip_Gap_Map_Itr itr;
	Gap_Data gap_data;
	Doubles period_diff, period_sum;
	
	num = 0;
	sum_diff = total = gap = 0.0;

	report_flag = Report_Flag (TRIP_GAP);
	memset (&gap_data, '\0', sizeof (gap_data));

	gap_flag = (trip_gap_flag || report_flag);

	if (gap_flag) {
		num = sum_periods.Num_Periods ();
		period_diff.assign (num, 0);
		period_sum.assign (num, 0);
	}

	//---- process each trip ----
	
	if (part_processor.Thread_Flag ()) {
		parts = Num_Partitions ();
	} else {
		parts = 1;
	}
	for (i=0; i < parts; i++) {
		Trip_Gap_Map *trip_gap_map_ptr;

		if (part_processor.Thread_Flag ()) {
			trip_gap_map_ptr = trip_gap_map_array [i];
		} else {
			trip_gap_map_ptr = &trip_gap_map;
		}
		for (itr = trip_gap_map_ptr->begin (); itr != trip_gap_map_ptr->end (); itr++) {
			diff = abs (itr->second.current - itr->second.previous);
			imp = itr->second.current;
			sum_diff += diff;
			total += imp;

			itr->second.previous = itr->second.current;
			itr->second.current = 0;

			if (report_flag) {
				gap_data.count++;
				gap_data.diff += diff;
				gap_data.diff_sq += diff * diff;
				gap_data.total += imp;
			}
			if (gap_flag) {
				period = sum_periods.Period (itr->second.time);

				if (period >= 0) {
					period_diff [period] += diff;
					period_sum [period] += imp;
				}
			}
		}
	}

	//---- process the iteration data ----

	if (Master ()) {
#ifdef MPI_EXE

		//---- gather the results from the slaves ----

		for (i=1; i < MPI_Size (); i++) {
			Get_MPI_Buffer (i);
			if (mpi_buffer.Size () == 0) continue;

			mpi_buffer.Get_Data (&diff, sizeof (double));
			sum_diff += diff;

			mpi_buffer.Get_Data (&imp, sizeof (double));
			total += imp;

			if (report_flag) {
				Gap_Data gap_temp;
				mpi_buffer.Get_Data (&gap_temp, sizeof (gap_temp));

				gap_data.count += gap_temp.count;
				gap_data.diff += gap_temp.diff;
				gap_data.diff_sq += gap_temp.diff_sq;
				gap_data.total += gap_temp.total;
			}
			if (gap_flag) {
				for (period=0; period < num; period++) {
					mpi_buffer.Get_Data (&diff, sizeof (double));
					period_diff [period] += diff;

					mpi_buffer.Get_Data (&imp, sizeof (double));
					period_sum [period] += imp;
				}
			}
		}
#endif
		if (total > 0) {
			gap = sum_diff / total;
		} else if (sum_diff > 0) {
			gap = 1.0;
		} else {
			gap = 0.0;
		}

		//---- write the trip gap file ----

		if (gap_flag) {
			for (i=0; i < num; i++) {
				imp = period_sum [i];
				diff = period_diff [i];

				if (imp > 0) {
					diff = diff / imp;
				} else if (diff > 0) {
					diff = 1.0;
				} else {
					diff = 0.0;
				}
				if (report_flag && diff > gap_data.max_gap) gap_data.max_gap = diff;
				if (trip_gap_flag) trip_gap_file.File () << "\t" << diff;
			}
			if (trip_gap_flag) trip_gap_file.File () << "\t" << gap << endl;

			if (report_flag) {
				trip_gap_array.push_back (gap_data);
			}
		}
#ifdef MPI_EXE

		//---- send the convergence result ----

		mpi_buffer.Data (&gap, sizeof (double));

		for (i=1; i < MPI_Size (); i++) {
			Send_MPI_Buffer (i);
		}

	} else {	//----- slave processing ----

		mpi_buffer.Data (&sum_diff, sizeof (double));
		mpi_buffer.Add_Data (&total, sizeof (double));

		if (report_flag) {
			mpi_buffer.Add_Data (&gap_data, sizeof (gap_data));
		}
		if (gap_flag) {
			for (period=0; period < num; period++) {
				mpi_buffer.Add_Data (&period_diff [period], sizeof (double));
				mpi_buffer.Add_Data (&period_sum [period], sizeof (double));
			}
		}
		Send_MPI_Buffer ();

		//---- retrieve the convergence result ----

		Get_MPI_Buffer ();
		mpi_buffer.Get_Data (&gap, sizeof (double));
#endif
	}
	return (gap);
}

