//*********************************************************
//	Record_Queue.cpp - queue of record numbers
//*********************************************************

#include "Record_Queue.hpp"

#include <string.h>
#include <iostream>
using namespace std;

//---------------------------------------------------------
//	Record_Queue constructor
//---------------------------------------------------------

Record_Queue::Record_Queue (int num_rec)
{
	records = 0;
	first = last = -1;
	num_records = 0;
	exit_queue = end_queue = false;
	
	Num_Records (num_rec);
}

//---------------------------------------------------------
//	Record_Queue destructor
//---------------------------------------------------------

Record_Queue::~Record_Queue (void)
{
	if (records != 0) {
		delete [] records;
		records = 0;
	}
	num_records = 0;
	first = last = -1;
}

//---------------------------------------------------------
//	Put -- add a record to the end of the queue
//---------------------------------------------------------

void Record_Queue::Put (int record)
{
#ifdef BOOST_THREADS
	boost::mutex::scoped_lock lock (queue_mutex);
#endif
	if (Check (record)) {
		if (last < 0) {
			first = last = record;
		} else {
			records [last] = record;
			last = record;
		}
#ifdef BOOST_THREADS
		queue_empty.notify_one ();
#endif
	}
}

//---------------------------------------------------------
//	Get -- get a record from the front of the queue
//---------------------------------------------------------

int  Record_Queue::Get (void)
{
	for (;;) {
#ifdef BOOST_THREADS
		boost::mutex::scoped_lock lock (queue_mutex);

		while (!exit_queue && !end_queue && last < 0) {
			queue_empty.wait (lock);
		}
		if (last < 0) {
			if (exit_queue) {
				queue_empty.notify_all ();
			}
			if (end_queue) {
				end_queue = false;
				end_wait.notify_one ();
				continue;
			}
		}
#endif
		if (last < 0) return (-1);

		int record = first;
		if (first == last) {
			first = last = -1;
		} else {
			first = records [record];
		}
		records [record] = -2;
		return (record);
	}
}

//---------------------------------------------------------
//	Check -- check if the record has been processed
//---------------------------------------------------------

bool Record_Queue::Check (int record)
{
	if (record >= 0 && record < num_records) {
		return (records [record] == -1 && record != last);
	} else {
		return (false);
	}
}

//---------------------------------------------------------
//	Reset -- re-initialize the queue
//---------------------------------------------------------

void Record_Queue::Reset (void)
{
#ifdef BOOST_THREADS
	boost::mutex::scoped_lock lock (queue_mutex);
#endif
	first = last = -1;
	if (num_records > 0) {
		memset (records, -1, num_records * sizeof (int));
	}
}

//---------------------------------------------------------
//	Num_Records -- allocate memory
//---------------------------------------------------------

bool Record_Queue::Num_Records (int num_rec)
{
	if (records != 0) {
		delete [] records;
		num_records = 0;
	}
	if (num_rec > 0) {
		records = new int [num_rec];
		if (records == 0) return (false);
		num_records = num_rec;
	}
	Reset ();
	return (true);
}

//---------------------------------------------------------
//	End_Queue
//---------------------------------------------------------

void Record_Queue::End_Queue (void)
{ 
#ifdef BOOST_THREADS
	boost::mutex::scoped_lock lock (queue_mutex);
	if (last >= 0) {
		end_queue = true;
		end_wait.wait (lock);
	}
	end_queue = false;
#endif
}

//---------------------------------------------------------
//	Exit_Queue
//---------------------------------------------------------

void Record_Queue::Exit_Queue (void)
{ 
#ifdef BOOST_THREADS
	boost::mutex::scoped_lock lock (queue_mutex);
	exit_queue = true;

	if (last < 0) {
		queue_empty.notify_all ();
	} else {
		queue_empty.notify_one ();
	}
#endif
}
