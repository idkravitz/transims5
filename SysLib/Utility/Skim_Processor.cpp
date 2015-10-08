//*********************************************************
//	Skim_Processor.cpp - skim builder thread manager
//*********************************************************

#include "Skim_Processor.hpp"

//---------------------------------------------------------
//	Skim_Processor -- Initialize
//---------------------------------------------------------

void Skim_Processor::Initialize (Router_Service *_exe, int _num_threads)
{
	exe = _exe;
	if (exe == 0) return;
#ifdef BOOST_THREADS
	path_builder = 0;
	num_builders = num_threads = (_num_threads > 0) ? _num_threads : 1;
	if (num_builders > 4) num_builders--;

	path_builder = new Path_Builder * [num_builders];

	//---- create path builders ----

	if (num_threads > 1) {
		skim_queue.Max_Records (100 + 20 * num_builders);

		for (int i=0; i < num_builders; i++) {
			path_builder [i] = new Path_Builder (&skim_queue, exe);
		}
		save_results.Initialize (this);
	} else {
		*path_builder = new Path_Builder (exe);
	}
#else
	num_builders = num_threads = _num_threads = 1;
	path_builder.Initialize (exe);
#endif
}

//---------------------------------------------------------
//	Skim_Processor -- destructor
//---------------------------------------------------------

Skim_Processor::~Skim_Processor (void)
{
#ifdef BOOST_THREADS
	if (exe && num_threads > 1) {
		for (int i=0; i < num_builders; i++) {
			delete path_builder [i];
			path_builder [i] = 0;
		}
		delete path_builder;
	}
#endif
}

//---------------------------------------------------------
//	Skim_Processor -- Start_Processing
//---------------------------------------------------------

void Skim_Processor::Start_Processing (void)
{
#ifdef BOOST_THREADS
	if (num_threads > 1) {
		skim_queue.Start_Work ();

		//---- create the path threads ----

		for (int i=0; i < num_builders; i++) {
			threads.create_thread (boost::ref (*(path_builder [i])));
		}
		threads.create_thread (boost::ref (save_results));
	}
#endif
}

//---------------------------------------------------------
//	Skim_Processor -- Start_Work
//---------------------------------------------------------

void Skim_Processor::Start_Work (void)
{
#ifdef BOOST_THREADS
	if (num_threads > 1) {
		skim_queue.Start_Work ();
	}
#endif
}

//---------------------------------------------------------
//	Skim_Processor -- Skim_Build
//---------------------------------------------------------

void Skim_Processor::Skim_Build (One_To_Many *ptr) 
{ 
#ifdef BOOST_THREADS
	if (num_threads > 1) {
		skim_queue.Put_Work (ptr);
	} else {
		(*path_builder)->Skim_Build (ptr);
		exe->Save_Skims (ptr);
	}
#else
	path_builder.Skim_Build (ptr);
	exe->Save_Skims (ptr);
#endif
}

//---------------------------------------------------------
//	Skim_Processor -- Complete_Work
//---------------------------------------------------------

void Skim_Processor::Complete_Work (void)
{
#ifdef BOOST_THREADS
	if (num_threads > 1) {
		skim_queue.Complete_Work ();
	}
#endif
}

//---------------------------------------------------------
//	Skim_Processor -- Stop_Processing
//---------------------------------------------------------

void Skim_Processor::Stop_Processing (void)
{
#ifdef BOOST_THREADS
	if (num_threads > 1) {
		skim_queue.End_of_Work ();
		threads.join_all ();
	}
#endif
}

//---------------------------------------------------------
//	Skim_Processor -- Save_Flows
//---------------------------------------------------------

void Skim_Processor::Save_Flows (void)
{
#ifdef BOOST_THREADS
	if (num_threads > 1) {
		skim_queue.Complete_Work ();
		for (int i=0; i < num_builders; i++) {
			path_builder [i]->Save_Flows ();
		}
	} else {
		(*path_builder)->Save_Flows ();
	}
#endif
}

#ifdef BOOST_THREADS
//---------------------------------------------------------
//	Save_Results -- operator
//---------------------------------------------------------

void Skim_Processor::Save_Results::operator()()
{
	One_To_Many *skim_ptr;

	while ((skim_ptr = ptr->skim_queue.Get_Result ())) {
		ptr->exe->Save_Skims (skim_ptr);
		ptr->skim_queue.Result_Done ();
	}
}
#endif
