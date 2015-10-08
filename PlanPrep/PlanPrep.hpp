//*********************************************************
//	PlanPrep.hpp - travel plan processing utility
//*********************************************************

#ifndef PLANPREP_HPP
#define PLANPREP_HPP

#include "APIDefs.hpp"
#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Data_Queue.hpp"

//---------------------------------------------------------
//	PlanPrep - execution class definition
//---------------------------------------------------------

class PlanPrep : public Data_Service, public Select_Service
{
public:

	PlanPrep (void);
	virtual ~PlanPrep (void);

	virtual void Execute (void);
	virtual void Page_Header (void);

protected:
	enum PlanPrep_Keys { 
		MERGE_PLAN_FILE = 1, MERGE_PLAN_FORMAT, MAXIMUM_SORT_SIZE
	};
	virtual void Program_Control (void);

private:
	enum PlanPrep_Reports { FIRST_REPORT = 1, SECOND_REPORT };

	Plan_File *plan_file, *new_plan_file;
	Plan_File merge_file;

	int sort_size, new_format;
	bool select_flag, merge_flag, combine_flag, output_flag, new_plan_flag;
	String pathname;

	void MPI_Setup (void);
	void MPI_Processing (void);
	void Combine_Plans (bool mpi_flag = false);
	void Time_Combine (Plan_File *temp_file, int num_temp);
	void Trip_Combine (Plan_File *temp_file, int num_temp);

	typedef Data_Queue <int> Partition_Queue;

	Partition_Queue partition_queue;

	//---------------------------------------------------------
	//	Plan_Processing - process plan partitions
	//---------------------------------------------------------

	class Plan_Processing
	{
	public:
		Plan_Processing (PlanPrep *_exe);
		void operator()();

	private:
		PlanPrep *exe;
		int  num_temp;
		bool thread_flag;

		Plan_Ptr_Array  plan_ptr_array;
		Trip_Map        traveler_sort;
		Time_Map        time_sort;
		Plan_File       plan_file;
		Plan_File       new_plan_file;
		Plan_File       merge_file;

		void Read_Plans (int part);
		void Write_Temp (void);
		void Trip_Write (void);
		void Time_Write (void);
		void Temp_Trip_Write (int part);
		void Temp_Time_Write (int part);
	};
	Plan_Processing **plan_processing;
};
#endif
