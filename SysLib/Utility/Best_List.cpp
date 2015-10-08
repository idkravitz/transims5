//*********************************************************
//	Best_List.cpp - vector of integers/doubles/bools
//*********************************************************

#include "Best_List.hpp"

#include "Static_Service.hpp"

//---------------------------------------------------------
//	Best
//---------------------------------------------------------

int Integer_List::Best (int num)
{
	if (num < 0) return (0);
	int max = (int) size ();
	if (max == 0) return (0);
	return ((num < max) ? at (num) : at (max-1)); 
}

double Double_List::Best (int num)
{
	if (num < 0) return (0.0);
	int max = (int) size ();
	if (max == 0) return (0.0);
	return ((num < max) ? at (num) : at (max-1)); 
}

Dtime Dtime_List::Best (int num)
{
	if (num < 0) return (0);
	int max = (int) size ();
	if (max == 0) return (0);
	return ((num < max) ? at (num) : at (max-1)); 
}

String String_List::Best (int num) 
{
	if (num < 0) return ("");
	int max = (int) size ();
	if (max == 0) return ("");
	return ((num < max) ? at (num) : at (max-1)); 
}

bool Bool_List::Best (int num)
{
	if (num < 0) return (false);
	int max = (int) size ();
	if (max == 0) return (false);
	return ((num < max) ? at (num) : at (max-1)); 
}

//---------------------------------------------------------
//	Combine
//---------------------------------------------------------

bool Integer_List::Combine (Integer_List &list, int index, bool round_flag)
{
	int i, n, value;
	Int_Itr itr;
	bool flag = false;

	n = (int) size ();
	if (n > 0) {
		value = *(end () - 1);
	} else {
		value = list [0];
		if (round_flag) {
			value = Static_Service::Round (value);
		}
	}
	if (value > 0) {
		flag = true;
	}
	while (n < index) {
		push_back (value);
		n++;
	}
	for (i=index, itr = list.begin () + 1; itr != list.end (); itr++, i++) {
		if (round_flag) {
			value = Static_Service::Round (*itr);
		} else {
			value = *itr;
		}
		if (value > 0) {
			flag = true;
		}
		if (n > i) {
			at (i) = value;
		} else {
			push_back (value);
		}
	}
	return (flag);
}

//---------------------------------------------------------
//	Combine
//---------------------------------------------------------

bool Double_List::Combine (Double_List &list, int index, bool round_flag)
{
	int i, n;
	double value;
	Dbl_Itr itr;
	bool flag = false;

	n = (int) size ();
	if (n > 0) {
		value = *(end () - 1);
	} else {
		value = list [0];
		if (round_flag) {
			value = Static_Service::Scale (value);
		}
	}
	if (value > 0.0) {
		flag = true;
	}
	while (n < index) {
		push_back (value);
		n++;
	}
	for (i=index, itr = list.begin () + 1; itr != list.end (); itr++, i++) {
		if (round_flag) {
			value = Static_Service::Scale (*itr);
		} else {
			value = *itr;
		}
		if (value > 0.0) {
			flag = true;
		}
		if (n > i) {
			at (i) = value;
		} else {
			push_back (value);
		}
	}
	return (flag);
}

//---------------------------------------------------------
//	Combine
//---------------------------------------------------------

bool Dtime_List::Combine (Dtime_List &list, int index, bool round_flag)
{
	int i, n;
	Dtime value;
	Dtime_Itr itr;
	bool flag = false;

	n = (int) size ();
	if (n > 0) {
		value = *(end () - 1);
	} else {
		value = list [0];
		if (round_flag) {
			value = value.Round_Seconds ();
		}
	}
	if (value > 0) {
		flag = true;
	}
	while (n < index) {
		push_back (value);
		n++;
	}
	for (i=index, itr = list.begin () + 1; itr != list.end (); itr++, i++) {
		if (round_flag) {
			value = itr->Round_Seconds ();
		} else {
			value = *itr;
		}
		if (value > 0) {
			flag = true;
		}
		if (n > i) {
			at (i) = value;
		} else {
			push_back (value);
		}
	}
	return (flag);
}

//---------------------------------------------------------
//	Combine
//---------------------------------------------------------

bool String_List::Combine (String_List &list, int index)
{
	int i, n;
	String value;
	Str_Itr itr;
	bool flag = false;

	n = (int) size ();
	if (n > 0) {
		value = *(end () - 1);
	} else {
		value = "";
	}
	if (value.length () > 0) {
		flag = true;
	}
	while (n < index) {
		push_back (value);
		n++;
	}
	for (i=index, itr = list.begin () + 1; itr != list.end (); itr++, i++) {
		if (itr->length () > 0) {
			flag = true;
		}
		if (n > i) {
			at (i) = *itr;
		} else {
			push_back (*itr);
		}
	}
	return (flag);
}
