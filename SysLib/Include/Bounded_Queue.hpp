//*********************************************************
//	Bounded_Queue.hpp - fixed length circular queue
//*********************************************************

#ifdef BOOST_THREADS
#ifndef BOUNDED_QUEUE_HPP
#define BOUNDED_QUEUE_HPP

#include "APIDefs.hpp"

#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include "Execution_Service.hpp"
#include <vector>
using namespace std;

//---------------------------------------------------------
//	Bounded_Queue -- template class definition
//---------------------------------------------------------
//	Bounded_Queue <Type> (int max_records = 200);
//
//	void Put (Type &data);
//
//  bool Get (Type &data);
//
//  void operator ();
//  void End_Queue (void);
//  void Exit_Queue (void);
//
//	bool Max_Records (int max_records);
//---------------------------------------------------------

template <typename Type>
class SYSLIB_API Bounded_Queue
{
	typedef vector <Type> Type_Array;

public:

	//---------------------------------------------------------
	//	Bounded_Queue constructor
	//---------------------------------------------------------

	Bounded_Queue (int _max_records = 200)
	{
		Reset ();
		Max_Records (_max_records);
	}

	//---------------------------------------------------------
	//	operator ()
	//---------------------------------------------------------

	void operator()()
	{
		boost::mutex::scoped_lock lock (queue_mutex);
		exit_wait.wait (lock);
	}

	//---------------------------------------------------------
	//	Put
	//---------------------------------------------------------

	void Put (Type &data)
	{
		boost::mutex::scoped_lock lock (queue_mutex);

		while (num_records == max_records) {
			queue_full.wait (lock);
		}
		num_records++;
		type_array [last_index++] = data;

		if (last_index >= max_records) last_index = 0;

		queue_empty.notify_one ();
	}

	//---------------------------------------------------------
	//	Get
	//---------------------------------------------------------

	bool Get (Type &data)
	{
		while (1) {
			boost::mutex::scoped_lock lock (queue_mutex);

			while (!exit_flag && !end_flag && num_records == 0) {
				queue_empty.wait (lock);
			}
			if (num_records == 0) {
				if (exit_flag) {
					queue_empty.notify_all ();
					return (false);
				}
				if (end_flag) {
					end_flag = false;
					end_wait.notify_one ();
					continue;
				}
			}
			num_records--;

			data = type_array [first_index++];

			if (first_index >= max_records) first_index = 0;

			queue_full.notify_one ();
			return (true);
		}
	}

	//--------------------------------------------------------
	//	End_Queue
	//--------------------------------------------------------

	void End_Queue (void)
	{
		boost::mutex::scoped_lock lock (queue_mutex);

		if (num_records > 0) {
			end_flag = true;
			end_wait.wait (lock);
		}
		end_flag = false;
	}

	//---------------------------------------------------------
	//	Exit_Queue
	//---------------------------------------------------------

	void Exit_Queue (void)
	{ 
		boost::mutex::scoped_lock lock (queue_mutex);
		exit_flag = true;
		queue_empty.notify_all ();
	}
	
	//---------------------------------------------------------
	//	Reset
	//---------------------------------------------------------

	void Reset (void)
	{
		num_records = first_index = last_index = 0;
		exit_flag = end_flag = false;
	}

	//---------------------------------------------------------
	//	Max_Records
	//---------------------------------------------------------

	bool Max_Records (int max_size)
	{
		type_array.resize (max_size);
		max_records = (int) type_array.size ();
		return (max_records > 0);
	}

private:

	//---- data ----

	int num_records, max_records, first_index, last_index;
	bool exit_flag, end_flag;

	boost::condition_variable  queue_full, queue_empty, exit_wait, end_wait;
	boost::mutex  queue_mutex;

	Type_Array type_array;
};

#endif
#endif
