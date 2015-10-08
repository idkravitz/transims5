//*********************************************************
//	PlanCompare.hpp - travel plan comparison utility
//*********************************************************

#ifndef PLANCOMPARE_HPP
#define PLANCOMPARE_HPP

#include "APIDefs.hpp"
#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Difference_Data.hpp"
#include "Trip_Gap_Data.hpp"
#include "Data_Queue.hpp"

//---------------------------------------------------------
//	PlanCompare - execution class definition
//---------------------------------------------------------

class PlanCompare : public Data_Service, public Select_Service
{
public:

	PlanCompare (void);
	virtual ~PlanCompare (void);

	virtual void Execute (void);
	virtual void Page_Header (void);

protected:
	enum PlanCompare_Keys { 
		COMPARE_PLAN_FILE = 1, COMPARE_PLAN_FORMAT, COMPARE_GENERALIZED_COSTS, SELECTION_METHOD, MERGE_PLAN_FILES,
		NEW_TIME_DISTRIBUTION_FILE, NEW_COST_DISTRIBUTION_FILE, NEW_TRIP_TIME_GAP_FILE, NEW_TRIP_COST_GAP_FILE,
		NEW_TRIP_MATCH_FILE, NEW_TRIP_MATCH_FORMAT
	};
	virtual void Program_Control (void);

private:
	enum PlanCompare_Reports { TOTAL_TIME = 1, PERIOD_TIME, TOT_TIME_SUM, PER_TIME_SUM, 
								TOTAL_COST, PERIOD_COST, TOT_COST_SUM, PER_COST_SUM, TRP_TIME_GAP, TRP_COST_GAP};
	
	Plan_File *plan_file, *new_plan_file;
	Plan_File compare_file;
	Selection_File match_file;

	bool select_flag, new_plan_flag, cost_flag, time_sum_flag, cost_sum_flag, time_gap_flag, cost_gap_flag;
	bool merge_flag, select_parts, time_sort, match_flag;
	int num_trips, num_compare, method, num_select;

	Difference_Data time_diff, cost_diff;
	Trip_Gap_Data time_gap, cost_gap;

	Select_Map selected, matched;

	typedef Data_Queue <int> Partition_Queue;

	Partition_Queue partition_queue;
	Partition_Queue output_queue;

	void MPI_Setup (void);
	void MPI_Processing (void);
	void Select_Plans (void);
	void MPI_Write (void);
	void MPI_Close (void);
	void Write_Match (void);
	
	//---------------------------------------------------------
	//	Plan_Processing - process plan partitions
	//---------------------------------------------------------

	class Plan_Processing
	{
	public:
		Plan_Processing (PlanCompare *_exe);
		void operator()();

	private:
		PlanCompare *exe;
		bool thread_flag, time_sort;
		int num_trips, num_compare;

		Plan_File  plan_file;
		Plan_File  compare_file;
		Select_Map selected, matched;
		Difference_Data time_diff, cost_diff;
		Trip_Gap_Data time_gap, cost_gap;

		void Read_Plans (int part);
	};
	Plan_Processing **plan_processing;

	//---------------------------------------------------------
	//	Plan_Output - write plan partitions
	//---------------------------------------------------------

	class Plan_Output
	{
	public:
		Plan_Output (PlanCompare *_exe);
		void operator()();

	private:
		PlanCompare *exe;
		bool thread_flag, time_sort;

		Plan_File plan_file, compare_file, new_plan_file;

		void Merge_Plans (int part);
		void Write_Plans (int part);
	};
	Plan_Output **plan_output;
};
#endif
