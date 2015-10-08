//*********************************************************
//	Barrier.cpp - coordinated thread barrier
//*********************************************************

#include "Barrier.hpp"
#include "Execution_Service.hpp"

//---------------------------------------------------------
//	Barrier constructor
//---------------------------------------------------------

Barrier::Barrier (int num)
{
	num_barrier = num;
	exit_flag = false;

#ifdef BOOST_THREADS
	num_go = num_end = end_id = 0;
	start_flag = finish_flag = false;
#endif
}

//---------------------------------------------------------
//	Go -- start processing
//---------------------------------------------------------

bool Barrier::Go (int &num)
{
	num = 0;
#ifdef BOOST_THREADS
	if (num_barrier > 0) {
		boost::mutex::scoped_lock lock (barrier_mutex);

		while (!start_flag && !exit_flag) {
			start_wait.wait (lock);
		}
		num = num_go++;

		if (num_go == num_barrier) {
			start_flag = false;
		}
	}
#endif
	return (!exit_flag);
}

//---------------------------------------------------------
//	Start
//---------------------------------------------------------

bool Barrier::Start (void)
{
#ifdef BOOST_THREADS
	if (num_barrier > 0) {
		boost::mutex::scoped_lock lock (barrier_mutex);
		int num = end_id;

		start_flag = true;
		finish_flag = false;
		num_go = num_end = 0;
		start_wait.notify_all ();

		while (num == end_id) {
			finish_wait.wait (lock);
		}
		return (finish_flag);
	}
#endif
	return (true);
}

//---------------------------------------------------------
//	Finish
//---------------------------------------------------------

bool Barrier::Finish (bool status)
{ 
#ifdef BOOST_THREADS
	if (num_barrier > 0) {
		boost::mutex::scoped_lock lock (barrier_mutex);
		int num = end_id;
		if (status) finish_flag = true;

		if (++num_end == num_barrier) {
			start_flag = false;
			end_id++;
			finish_wait.notify_all ();
			return (status);
		}
		while (num == end_id) {
			finish_wait.wait (lock);
		}
	}
#endif
	return (status);	
}

//---------------------------------------------------------
//	Result
//---------------------------------------------------------

bool Barrier::Result (bool status)
{ 
#ifdef BOOST_THREADS
	if (num_barrier > 0) {
		boost::mutex::scoped_lock lock (barrier_mutex);
		if (status) finish_flag = true;

		if (++num_end == num_barrier) {
			end_id++;
			finish_wait.notify_one ();
		}
	}
#endif
	return (status);	
}

//---------------------------------------------------------
//	Exit
//---------------------------------------------------------

void Barrier::Exit (void)
{
#ifdef BOOST_THREADS
	if (num_barrier > 0) {
		boost::mutex::scoped_lock lock (barrier_mutex);
		exit_flag = true;
		start_wait.notify_all ();
		return;
	}
#endif
	exit_flag = true;
}
