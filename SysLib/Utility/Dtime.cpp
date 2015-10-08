//*********************{***********************************
//	Dtime.hpp - day-time data type
//*********************************************************

#include "Dtime.hpp"

#include "TypeDefs.hpp"
#include "Execution_Service.hpp"

bool Dtime::warning_flag = false;

//---------------------------------------------------------
//	Dtime constructors
//---------------------------------------------------------

Dtime::Dtime (int time, Units_Type units)
{
	if (units == SECONDS) {
		dtime = Round (time);
	} else if (units == MINUTES) {
		dtime = Round (time * 60);
	} else if (units == HOURS) {
		dtime = Round (time * 3600);
	} else {
		dtime = time;
	}
}

Dtime::Dtime (double time, Units_Type units)
{
	if (units == SECONDS) {
		dtime = Round (time);
	} else if (units == MINUTES) {
		dtime = Round (time * 60.0);
	} else if (units == HOURS) {
		dtime = Round (time * 3600.0);
	} else {
		dtime = DTOI (time);
	}
}

//---------------------------------------------------------
//	Time_String - converts a string to dtime
//---------------------------------------------------------

void  Dtime::Time_String (string text, bool duration, Units_Type time_format)
{
	//---- NOON ----
	//---- MIDNIGHT ----
	//---- d@hh:mm:ss.x -----
	//---- d@hh:mm -----
	//---- d@hh.xxx ----
	//---- d@ssssss ----
	//---- hh:mm:ss -----
	//---- hh:mm -----
	//---- hh:mmpm -----
	//---- hh.xxx ----
	//---- ssssss ----
	//---- dddhh:mm ----
	
	static const char *day_text [] = {
		"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT", "WKE", "WKD", "ALL", 0
	};
	int hour, min_time, hour_time, day_time;
	size_t index;
	char ch;
	bool sign;
	String result, time;
	Strings clock;
	Str_Itr itr;

	if (time_format == NO_UNITS) time_format = Time_Format ();

	dtime = 0;
	time = text;
	time.Trim (" \t");

	if (time.empty ()) return;

	//---- negative ----

	if (time [0] == '-') {
		if (!duration && time_format != SECONDS && time_format != MINUTES) {
			if (!warning_flag) goto time_error;
			return;
		}
		time.erase (0, 1);
		sign = true;
	} else {
		sign = false;
	}
	min_time = Round (60);
	hour_time = min_time * 60;
	day_time = hour_time * 24;

	//---- day ----

	if (time.find ("@") != time.npos) {
		if (time.Split (result, "@")) {
			if (result.empty () || result [0] < '0' || result [0] > '9') goto time_error;

			dtime = result.Integer () * day_time;
			if (time.empty ()) goto finish;
		}
	}

	//---- check for special codes ----

	if (time [0] < '0' || time [0] > '9') {
		if (time.Equals ("NOON")) {
			dtime += day_time / 2;
			goto finish;
		} 
		if (time.Equals ("MIDNIGHT")) {
			goto finish;
		}

		//---- VERSION3 Time Codes ----

		for (int i=0; day_text [i] != 0; i++) {
			if (time.Starts_With (day_text [i])) {
				time.erase (0, strlen (day_text [i]));

				if (time.empty ()) goto finish;
				if (time.find (":") != time.npos) {
					if (time.Split (result, ":")) {
						dtime = Round (result.Integer () * 3600 + time.Integer () * 60);
					}
				} else if (time [0] < '0' || time [0] > '9') {
					goto time_error;
				} else {
					dtime = Round (time.Integer () * 3600);
				}
				goto finish;
			}
		}
	}

	//---- check time units ----

	if (time.Parse (clock, ":") < 1) goto finish;

	itr = clock.begin ();
	ch = (*itr) [0];

	if (clock.size () == 1) {
		if ((ch < '0' || ch > '9') && ch != '.') goto time_error;

		if (time_format == SECONDS) {
			dtime += Round (itr->Double ()); 
		} else if (time_format == MINUTES) {
			dtime += DTOI (itr->Double () * min_time);
		} else if (time_format == HOURS) {
			dtime += DTOI (itr->Double () * hour_time); 
		} else if (itr->find ('.') != itr->npos) {
			dtime += DTOI (itr->Double () * hour_time); 
		} else {
			dtime += Round (itr->Double ());
		}
		goto finish;
	}

	//---- hour of the day ----

	if (ch < '0' || ch > '9') goto time_error;

	hour = itr->Integer ();

	dtime += hour * hour_time;

	if (++itr == clock.end ()) goto finish;

	//---- minute of the hour ----
	
	ch = (*itr) [0];
	if (ch < '0' || ch > '5' || itr->length () != 2) goto time_error;

	dtime += itr->Integer () * min_time;

	if (++itr == clock.end ()) goto finish;

	//---- seconds of the minute ----

	ch = (*itr) [0];
	if (ch < '0' || ch > '5') goto time_error;

	index = itr->find ('.');

	if (index != itr->npos) {
		if (index != 2) goto time_error;
		dtime += Round (itr->Double ()); 
	} else {
		if (itr->length () != 2) goto time_error;
		dtime += Round (itr->Double ());
	}
finish:
	if (sign) dtime = -dtime;
	return;

time_error:
	exe->Warning ("Time Conversion ") << text;
	warning_flag = true;
}

//---------------------------------------------------------
//	Time_String - converts dtime to a time string
//---------------------------------------------------------

string Dtime::Time_String (Units_Type time_format)
{
	int day, hour, minute, min_time, hour_time, day_time;
	double time;
	String text;

	if (time_format == NO_UNITS) time_format = Time_Format ();

	time = UnRound (dtime);
	min_time = 60;
	hour_time = min_time * 60;
	day_time = hour_time * 24;

	if (time >= MAX_INTEGER) {
		time = MIDNIGHT;
	} else if (time > 3 * MIDNIGHT) {
		time = 3 * MIDNIGHT;
	}
	if (time_format == HOURS) {
		text (time / hour_time);

		if (text.find ('.') == text.npos) {
			text += ".0";
		}
		return (text);
	} else if (time_format == MINUTES) {
		return (text (time / min_time, 2));
	} else if (time_format == SECONDS) {
		return (text (time));
	} else if (time_format == TIME_CODE) {
		hour = (int) (time / hour_time);
		time -= hour * hour_time;
		minute = (int) (time / 60);
		return (text ("ALL%02d:%02d") % hour % minute);
	}

	//---- check for negative values ----

	if (time < 0) {
		text = "-";
		time = -time;
	}

	//---- clock formating ----

	if (time >= day_time) {
		if (time == day_time) {
			text += "24:00";
			return (text);
		}
		if (time_format == DAY_TIME) {
			text += "%d@";
			day = (int) (time / day_time);
			text % day;
			time -= day * day_time;
		}
	}
	hour = (int) (time / hour_time);
	time -= hour * hour_time;
	minute = (int) (time / min_time);
	time -= minute * min_time;

	text += "%d:%02d";
	text % hour % minute;

	if (time > 0) {
		int second = (int) time;
		time -= second;

		text += ":%02d";
		text % second;

		if (time > 0) {
			text += ".%d";
			text % Round (time);
		}
	}
	return (text);
}

//---------------------------------------------------------
//	Time_Label - converts a time label to dtime
//---------------------------------------------------------

void Dtime::Time_Label (string label, Units_Type time_format)
{
	String time;

	if (time_format == NO_UNITS) time_format = Time_Format ();

	dtime = 0;
	time = label;
	time.Trim (" \t");

	if (time.empty ()) {
		exe->Warning ("Time Label Conversion ");
		return;
	}
	if (time.find_first_of (".:") == time.npos) {
		size_t len = time.length ();
			
		//---- scan for clock time ----

		if (len >= 3 && len <= 4) {
			bool flag = false;

			if (time [0] == '0') {
				flag = true;
			} else if (len == 4) {
				if (time [0] <= '2' && time [2] <= '5') {
					flag = true;
				}
			} else if (time [1] <= '5') {
				flag = true;
			}
			if (flag) {
				if (len == 4) {
					time.insert (2, ":");
				} else {
					time.insert (1, ":");
				}
			}
		}
	}
	Time_String (time, time_format);
	return;
}

//---------------------------------------------------------
//	Time_Label - converts dtime to a time label
//---------------------------------------------------------

string Dtime::Time_Label (Units_Type time_format)
{
	int hour, minute, seconds, min_time, hour_time;
	double time;
	String text;

	if (time_format == NO_UNITS) time_format = Time_Format ();

	time = UnRound (DTOI (dtime));

	min_time = 60;
	hour_time = min_time * 60;

	if (time > 3 * MIDNIGHT) {
		time = 3 * MIDNIGHT;
	}
	if (time_format == HOURS) {
		text (time / hour_time);
	} else if (time_format == SECONDS) {
		text (time);
	} else {
		seconds = DTOI (time);
		hour = seconds / hour_time;
		seconds -= hour * hour_time;
		minute = seconds / min_time;
		seconds -= minute * min_time;

		if (seconds > 0) {
			text ("%d%02d%02d") % hour % minute % seconds;
		} else {
			text ("%d%02d") % hour % minute;
		}
	}
	return (text);
}

//---------------------------------------------------------
//	Time_Label - converts dtime to a padded time label
//---------------------------------------------------------

string Dtime::Time_Label (bool pad_flag)
{
	if (!pad_flag) return (Time_Label ());

	int hour, minute, seconds, min_time, hour_time;
	double time;
	String text;
	Units_Type time_format = Time_Format ();

	time = UnRound (DTOI (dtime));

	min_time = 60;
	hour_time = min_time * 60;

	if (time > 3 * MIDNIGHT) {
		time = 3 * MIDNIGHT;
	}
	if (time_format == HOURS) {
		text ("%09.6lf") % (time / hour_time);
	} else if (time_format == SECONDS) {
		text ("%05d") % time;
	} else {
		seconds = DTOI (time);
		hour = seconds / hour_time;
		seconds -= hour * hour_time;
		minute = seconds / min_time;
		seconds -= minute * min_time;

		text ("%02d%02d%02d") % hour % minute % seconds;
	}
	return (text);
}
