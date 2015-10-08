//*********************************************************
//	Plan_Processor.cpp - path builder thread manager
//*********************************************************

#include "Plan_Processor.hpp"

//---------------------------------------------------------
//	Plan_Processor -- Initialize
//---------------------------------------------------------

void Plan_Processor::Initialize (Router_Service *_exe, int _num_threads)
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
		plan_queue.Max_Records (100 + 50 * num_builders);

		for (int i=0; i < num_builders; i++) {
			path_builder [i] = new Path_Builder (&plan_queue, exe);
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
//	Plan_Processor -- destructor
//---------------------------------------------------------

Plan_Processor::~Plan_Processor (void)
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
//	Plan_Processor -- Start_Processing
//---------------------------------------------------------

void Plan_Processor::Start_Processing (void)
{
#ifdef BOOST_THREADS
	if (num_threads > 1) {
		plan_queue.Start_Work ();

		//---- create the path threads ----

		for (int i=0; i < num_builders; i++) {
			threads.create_thread (boost::ref (*(path_builder [i])));
		}
		threads.create_thread (boost::ref (save_results));
	}
#endif
}

//---------------------------------------------------------
//	Plan_Processor -- Start_Work
//---------------------------------------------------------

void Plan_Processor::Start_Work (void)
{
#ifdef BOOST_THREADS
	if (num_threads > 1) {
		plan_queue.Start_Work ();
	}
#endif
}

//---------------------------------------------------------
//	Plan_Processor -- Plan_Build
//---------------------------------------------------------

void Plan_Processor::Plan_Build (Plan_Ptr_Array *array_ptr) 
{ 
#ifdef BOOST_THREADS
	if (num_threads > 1) {
		plan_queue.Put_Work (array_ptr);
	} else {
		(*path_builder)->Array_Processing (array_ptr);
		exe->Save_Plans (array_ptr);
	}
#else
	path_builder.Array_Processing (array_ptr);
	exe->Save_Plans (array_ptr);
#endif
}

//---------------------------------------------------------
//	Plan_Processor -- Complete_Work
//---------------------------------------------------------

void Plan_Processor::Complete_Work (void)
{
#ifdef BOOST_THREADS
	if (num_threads > 1) {
		plan_queue.Complete_Work ();
	}
#endif
}

//---------------------------------------------------------
//	Plan_Processor -- Stop_Processing
//---------------------------------------------------------

void Plan_Processor::Stop_Processing (void)
{
#ifdef BOOST_THREADS
	if (num_threads > 1) {
		plan_queue.End_of_Work ();
		threads.join_all ();
	}
#endif
}

//---------------------------------------------------------
//	Plan_Processor -- Save_Flows
//---------------------------------------------------------

void Plan_Processor::Save_Flows (void)
{
#ifdef BOOST_THREADS
	if (num_threads > 1) {
		plan_queue.Complete_Work ();
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

void Plan_Processor::Save_Results::operator()()
{
	Plan_Ptr_Array *array_ptr;

	while ((array_ptr = ptr->plan_queue.Get_Result ())) {
		ptr->exe->Save_Plans (array_ptr);
		ptr->plan_queue.Result_Done ();
	}
}
#endif
