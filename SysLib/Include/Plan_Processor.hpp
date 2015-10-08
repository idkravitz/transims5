//*********************************************************
//	Plan_Processor.hpp - path builder thread manager
//*********************************************************

#ifndef PLAN_PROCESSOR_HPP
#define PLAN_PROCESSOR_HPP

#include "Router_Service.hpp"
#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Plan_Processor - builds/updates plan data
//---------------------------------------------------------

class SYSLIB_API Plan_Processor
{
public:
	Plan_Processor (Router_Service *exe = 0, int num_threads = 0) { Initialize (exe, num_threads); }
	~Plan_Processor (void);

	void Initialize (Router_Service *_exe, int num_threads = 0);
	void Start_Processing (void);
	void Start_Work (void);
	void Plan_Build (Plan_Ptr_Array *ptr);
	void Complete_Work (void);
	void Stop_Processing (void);
	void Save_Flows (void);
	int  Num_Threads (void)           { return (num_threads); }

private:
	Router_Service  *exe;
	int num_threads, num_builders;

#ifdef BOOST_THREADS
	boost::thread_group threads;
	Plan_Queue plan_queue;
	Path_Builder **path_builder;

	//---------------------------------------------------------
	//	Save_Results - process the path building results
	//---------------------------------------------------------

	class Save_Results
	{
	public:
		Save_Results (Plan_Processor *ptr = 0)   { Initialize (ptr); }

		void Initialize (Plan_Processor *_ptr)   { ptr = _ptr; }

		void operator()();

		Plan_Processor *ptr;
	} save_results;

#else	//---- BOOST_THREADS ----
	Path_Builder path_builder;
#endif	//---- non BOOST_THREADS ----
};
#endif
