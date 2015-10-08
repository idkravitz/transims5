//*********************************************************
//	Data_Queue.hpp - thread safe data queue
//*********************************************************

#ifndef DATA_QUEUE_HPP
#define DATA_QUEUE_HPP

#include "Execution_Service.hpp"

#ifdef BOOST_THREADS
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#endif

#include <deque>
using namespace std;

//---------------------------------------------------------
//	Data_Queue -- template class definition
//---------------------------------------------------------
//	Data_Queue <Type> (void);
//
//	void Put (Type data);
//
//  bool Get (Type &data);
//
//	void End_of_Queue (void);
//  void Reset (void);
//---------------------------------------------------------

template <typename Type>
class Data_Queue
{
	typedef deque <Type> Type_Array;

public:

	//---------------------------------------------------------
	//	Data_Queue constructor
	//---------------------------------------------------------

	Data_Queue (void)
	{
		end_of_queue = false;
	}

	//---------------------------------------------------------
	//	Put
	//---------------------------------------------------------

	void Put (Type data)
	{
#ifdef BOOST_THREADS
		boost::mutex::scoped_lock lock (queue_mutex);
#endif
		type_array.push_back (data);

#ifdef BOOST_THREADS
		queue_empty.notify_one ();
#endif
	}

	//---------------------------------------------------------
	//	Get
	//---------------------------------------------------------

	bool Get (Type &data)
	{
#ifdef BOOST_THREADS
		boost::mutex::scoped_lock lock (queue_mutex);
#endif
		while (!end_of_queue && type_array.empty ()) {
#ifdef BOOST_THREADS
			queue_empty.wait (lock);
#endif
		}
		if (type_array.empty ()) {
#ifdef BOOST_THREADS
			queue_empty.notify_all ();
#endif
			return (false);
		}
		data = type_array.front ();
		type_array.pop_front ();
		return (true);
	}

	//---------------------------------------------------------
	//	End_of_Queue
	//---------------------------------------------------------

	void End_of_Queue (void)
	{ 
#ifdef BOOST_THREADS
		boost::mutex::scoped_lock lock (queue_mutex);
		queue_empty.notify_one ();
#endif
		end_of_queue = true;
	}

	//---------------------------------------------------------
	//	Reset
	//---------------------------------------------------------

	void Reset (void)
	{
		end_of_queue = false;
	}

private:
	bool end_of_queue;

#ifdef BOOST_THREADS
	boost::condition_variable  queue_empty;
	boost::mutex  queue_mutex;
#endif

	Type_Array type_array;
};

#endif
