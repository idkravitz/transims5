//*********************************************************
//	Time_Periods.cpp - data range functions
//*********************************************************

#include "Time_Periods.hpp"

#include "Execution_Service.hpp"

//---------------------------------------------------------
//	Time_Periods -- constructor
//---------------------------------------------------------

Time_Periods::Time_Periods (void) : Range_Array (1), Static_Service () 
{
	range_flag = wrap_flag = merge_flag = false;
	increment = start = end_time = 0;
	period_point = TRIP_START;
}

//---------------------------------------------------------
//	Period
//---------------------------------------------------------

int Time_Periods::Period (Dtime time)
{
	if (range_flag) {
		if (merge_flag) {
			return ((In_Index ((int) time) >= 0) ? 0 : -1);
		} else {
			return (In_Index ((int) time));
		}
	} else if (time >= start) {
		if (time >= end_time) {
			if (!wrap_flag || end_time <= start) return (-1);
			time = (time - start) % (end_time - start);
		} else {
			time -= start;
		}
		if (increment > 0 && !merge_flag) {
			return (time / increment);
		} else {
			return (0);
		}
	}
	return (-1);
}

//---------------------------------------------------------
//	At_Increment
//---------------------------------------------------------

bool Time_Periods::At_Increment (Dtime time)
{
	if (range_flag) {
		int low = In_Index ((int) time);
		if (low < 0) {
			low = In_Index ((int) (time-1));
			if (low < 0) return (false);
			low = at (low).High () + 1;
		} else if (low == 0) {
			low = at (low).High () + 1;
		} else {
			low = at (low).Low ();
		}
		return ((int) time == low);
	} else if (time >= start) {
		if (time > end_time) {
			if (!wrap_flag || end_time <= start) return (false);
			time = (time - start) % (end_time - start);
		} else {
			time -= start;
		}
		if (increment > 0) {
			return ((time % increment) == 0);
		} else {
			return ((int) time == 0);
		}
	}
	return (false);
}

//---------------------------------------------------------
//	Span_Range
//---------------------------------------------------------

bool Time_Periods::Span_Range (Dtime low, Dtime high)
{
	if (range_flag) {
		return (Range_Array::Span_Range (low, high));
	} else if (high >= start && low <= end_time) {
		return (true);
	} else if (wrap_flag && low > end_time) {
		Dtime day (MIDNIGHT, SECONDS);
		low = low % day;
		high = high % day;
		return (high >= start && low <= end_time);
	}
	return (false);
}

//---------------------------------------------------------
//	Num_Periods
//---------------------------------------------------------

int Time_Periods::Num_Periods (void)
{
	if (range_flag) {
		return (Num_Ranges ());
	} else if (increment <= 0) {
		if (end_time > start) {
			return (1);
		} else {
			return (0);
		}
	} else if (end_time > start) {
		return ((end_time - start - 1) / increment + 1);
	}
	return (0);
}

//---------------------------------------------------------
//	Range_Length
//---------------------------------------------------------

int Time_Periods::Range_Length (void)
{
	if (range_flag) {
		return (Max_Count ());
	} else if (end_time > start) {
		return (end_time - start);
	}
	return (0);
}

//---------------------------------------------------------
//	Set_Periods
//---------------------------------------------------------

bool Time_Periods::Set_Periods (Dtime _increment, Dtime _start, Dtime _end, bool _wrap)
{
	increment = _increment;
	wrap_flag = _wrap;
	range_flag = false;

	if (_start == 0 && _end == 0) {
		if (end_time == 0) {
			start = exe->Model_Start_Time ();
			if (wrap_flag) {
				end_time = MIN ((start + Round (MIDNIGHT)), exe->Model_End_Time ());
			} else {
				end_time = exe->Model_End_Time ();
			}
		}
	} else if (_start > _end) {
		if (_end != 0) return (false);
		start = _start;
		if (wrap_flag) {
			end_time = MIN ((start + Round (MIDNIGHT)), exe->Model_End_Time ());
		} else {
			end_time = exe->Model_End_Time ();
		}
	} else {
		start = _start;
		if (wrap_flag) {
			end_time = MIN ((start + Round (MIDNIGHT)), _end);
		} else {
			end_time = _end;
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Add_Label
//---------------------------------------------------------

bool Time_Periods::Add_Label (string label)
{
	Dtime low, high;
	size_t i;
	String text, low_text, high_text;

	text = label;
	text.Trim ();
	if (text.empty ()) return (false);
	range_flag = true;
	
	i = text.find ("_");

	if (i == text.npos) {
		if (!text.Range (low_text, high_text)) goto label_error;
	} else {
		low_text = text.substr (0, i);
		high_text = text.substr (i + 1);
	}
	low.Time_Label (low_text);
	high.Time_Label (high_text);

	return (Add_Range ((int) low, (int) high - 1, increment));

label_error:
	if (exe->Send_Messages ()) {
		exe->Error (String ("Label Value is Missing from Time Label %s") % label);
	}
	return (false);
}

//---------------------------------------------------------
//	Add_Ranges
//---------------------------------------------------------

bool Time_Periods::Add_Ranges (string range)
{
	Dtime low, high;
	int periods;
	String text, low_text, high_text;
	Strings ranges;
	Str_Itr itr;
	Units_Type units;

	text = range;
	text.Trim ();
	if (text.empty ()) return (false);

	if (text.Equals ("All")) {
		if (range_flag) {
			low = exe->Model_Start_Time ();
			high = exe->Model_End_Time ();

			return (Add_Range ((int) low, (int) high - 1, increment));
		} else {
			return (Set_Periods (increment, start, end_time, wrap_flag));
		}
	}

	//---- unpack the range string ----	

	units = exe->Parse_Units (text);

	periods = text.Parse (ranges);

	range_flag = (periods > 1 || range_flag);

	for (itr = ranges.begin (); itr != ranges.end (); itr++) {
		if (!itr->Range (low_text, high_text)) return (false);
		
		if (units != NO_UNITS) {
			low.Time_String (low_text, units);
			high.Time_String (high_text, units);
		} else {
			low = low_text;
			high = high_text;
		}

		if (range_flag) {
			if (!Add_Range ((int) low, (int) high - 1, increment)) return (false);
		} else {
			return (Set_Periods (increment, low, high, wrap_flag));
		}
	}
	return (Num_Ranges () > 0);
}

//---------------------------------------------------------
//	Add_Breaks
//---------------------------------------------------------

bool Time_Periods::Add_Breaks (string range)
{
	Dtime low, high;
	String text;
	Strings breaks;
	Str_Itr itr;
	Units_Type units;

	text = range;
	text.Trim ();
	if (text.empty ()) return (false);

	if (text.Equals ("None")) {
		return (Set_Periods (0));
	}

	//---- unpack the break string ----	

	units = exe->Parse_Units (text);

	text.Parse (breaks);
	range_flag = true;

	for (itr = breaks.begin (); itr != breaks.end (); itr++) {
		if (units != NO_UNITS) {
			high.Time_String (*itr, units);
		} else {
			high = *itr;
		}
		if (low > high) goto break_error;

		if (low < high) {
			if (!Add_Range ((int) low, (int) high - 1)) return (false);
		}
		low = high;
	}
	high = exe->Model_End_Time ();

	if (low < high) {
		return (Add_Range ((int) low, (int) high - 1));
	}
	return (Num_Ranges () > 0);

break_error:
	if (exe->Send_Messages ()) {
		exe->Error (String ("Range Breaks %s are Illogical") % range);
	}
	return (false);
}

//---------------------------------------------------------
//	Range_Format
//---------------------------------------------------------

string Time_Periods::Range_Format (int num)
{
	string buffer;

	if (num >= 0 && num < Num_Periods ()) {
		Dtime low, high;

		if (range_flag) {
			Range_Data &ref = at (num);

			low = ref.Low ();
			high = ref.High () + 1;
		} else if (increment <= 0) {
			low = start;
			high = end_time;
		} else {
			low = start + num * (int) increment;
			high = low +  increment;
			if (high > end_time) high = end_time;
		}
		buffer = low.Time_String () + ".." + high.Time_String ();
	}
	return (buffer);
}

//---------------------------------------------------------
//	Range_Label
//---------------------------------------------------------

string Time_Periods::Range_Label (int num, bool pad_flag)
{
	string buffer;

	if (num >= 0 && num < Num_Periods ()) {
		Dtime low, high;

		if (range_flag) {
			Range_Data &ref = at (num);

			low = ref.Low ();
			high = ref.High () + 1;
		} else if (increment <= 0) {
			low = start;
			high = end_time;
		} else {
			low = start + num * (int) increment;
			high = low + increment;
			if (high > end_time) high = end_time;
		}
		buffer = low.Time_Label (pad_flag) + "_" + high.Time_Label (pad_flag);
	}
	return (buffer);
}

//---------------------------------------------------------
//	Break_String
//---------------------------------------------------------

string Time_Periods::Break_String (void)
{
	Dtime time;
	string text;
	Range_Array_Itr itr;
	bool first = true;

	for (itr = begin (); itr != end (); itr++) {
		if (first) {
			time = itr->Low ();
			text = time.Time_String ();
			first = false;
		}
		time = itr->High () + 1;
		text += " ";
		text += time.Time_String ();
	}
	return (text);
}

//---------------------------------------------------------
//	Period_Range
//---------------------------------------------------------

bool Time_Periods::Period_Range (int num, Dtime &low, Dtime &high)
{
	low = high = 0;
	if (num < 0 || num >= Num_Periods ()) return (false);

	if (range_flag) {
		Range_Data &ref = at (num);

		low = ref.Low ();
		high = ref.High () + 1;
	} else if (increment <= 0) {
		low = start;
		high = end_time;
	} else {
		low = start + num * (int) increment;
		high = low + increment;
		if (high > end_time) high = end_time;
	}
	return (true);
}

bool Time_Periods::Period_Range (Dtime low, Dtime high, int &p1, int &p2)
{
	p1 = p2 = -1;

	if (range_flag) {
		p1 = Span_Low_Index ((int) low, (int) high);
		p2 = Span_High_Index ((int) low, (int) high);
	} else {
		if (wrap_flag && low > end_time) {
			Dtime day (MIDNIGHT, SECONDS);
			low = low % day;
			high = high % day;
		}
		if (high >= start && low <= end_time) {
			if (low > start) {
				p1 = Period (low);
			} else {
				p1 = Period (start);
			}
			if (high < end_time) {
				p2 = Period (high - 1);
			} else {
				p2 = Period (end_time - 1);
			}
		}
	}
	return (p1 >= 0 && p2 >= 0);
}

//---------------------------------------------------------
//	Period_Time
//---------------------------------------------------------

Dtime Time_Periods::Period_Time (int num)
{
	Dtime low, high;
	Period_Range (num, low, high);
	return ((low + high) / 2);
}

//---------------------------------------------------------
//	Copy_Periods
//---------------------------------------------------------

bool Time_Periods::Copy_Periods (Time_Periods &periods)
{
	if (periods.Num_Periods () == 0) return (false);

	range_flag = periods.Range_Flag ();

	if (range_flag) {
		assign (periods.begin (), periods.end ());
	} else {
		Set_Periods (periods.Increment (), periods.Start (), periods.End (), periods.Wrap_Flag ());
	}
	return (true);
}

//---------------------------------------------------------
//	Period_Offset
//---------------------------------------------------------

double Time_Periods::Period_Offset (int num, Dtime time)
{
	Dtime p1, p2;

	if (range_flag) {
		Range_Data *range_ptr = &at (num);
		p1 = range_ptr->Low ();
		p2 = range_ptr->High () + 1;
	} else if (increment > 0) {
		p1 = start + num * increment;
		p2 = p1 + increment;
	} else {
		p1 = start;
		p2 = end_time + 1;
	}
	if (time <= p1) return (0.0);
	if (time >= p2) return (1.0);
	return (((double) (time - p1)) / (p2 - p1));
}
