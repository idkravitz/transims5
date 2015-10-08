//*********************************************************
//	Skim_Processor.hpp - skim builder thread manager
//*********************************************************

#ifndef SKIM_PROCESSOR_HPP
#define SKIM_PROCESSOR_HPP

#include "Router_Service.hpp"
#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Skim_Processor - generate path skim data
//---------------------------------------------------------

class Skim_Processor
{
public:
	Skim_Processor (Router_Service *exe = 0, int num_threads = 0) { Initialize (exe, num_threads); }
	~Skim_Processor (void);

	void Initialize (Router_Service *_exe, int num_threads = 0);
	void Start_Processing (void);
	void Start_Work (void);
	void Skim_Build (One_To_Many *ptr);
	void Complete_Work (void);
	void Stop_Processing (void);
	void Save_Flows (void);
	int  Num_Threads (void)           { return (num_threads); }

private:
	Router_Service  *exe;
	int num_threads, num_builders;

#ifdef BOOST_THREADS
	boost::thread_group threads;
	Skim_Queue skim_queue;
	Path_Builder **path_builder;

	//---------------------------------------------------------
	//	Save_Results - process the path building results
	//---------------------------------------------------------

	class Save_Results
	{
	public:
		Save_Results (Skim_Processor *ptr = 0)   { Initialize (ptr); }

		void Initialize (Skim_Processor *_ptr)   { ptr = _ptr; }

		void operator()();

		Skim_Processor *ptr;
	} save_results;

#else	//---- BOOST_THREADS ----
	Path_Builder path_builder;
#endif	//---- non BOOST_THREADS ----
};
#endif
