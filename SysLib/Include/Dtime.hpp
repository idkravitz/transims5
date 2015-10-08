//*********************************************************
//	Dtime.hpp - day-time data type
//*********************************************************

#ifndef DTIME_HPP
#define DTIME_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"

#ifdef BOOST_MPI
#include <boost/serialization/serialization.hpp>
#endif

#include <string>
using namespace std;

//---------------------------------------------------------
//	Dtime
//---------------------------------------------------------

class SYSLIB_API Dtime : public Static_Service
{
public:
	Dtime (void)                              { dtime = 0; }
	Dtime (int time)                          { dtime = time; }
	Dtime (double time)                       { dtime = DTOI (time); }
	Dtime (string text, Units_Type units)     { Time_String (text, false, units); }
	Dtime (string text, bool duration = false, Units_Type units = NO_UNITS)    
	                                          { Time_String (text, duration, units); }
	Dtime (const Dtime &time)                 { dtime = ((Dtime &) time).dtime; }
	Dtime (int time, Units_Type units);
	Dtime (double time, Units_Type units);

	operator int ()                           { return (dtime); }
	int    Time (void)                        { return (dtime); }

	double Seconds (void)                     { return (UnRound (dtime)); }
	double Minutes (void)                     { return (UnRound (dtime / 60.0)); }
	double Hours (void)                       { return (UnRound (dtime / 3600.0)); }
	
	Dtime & Seconds (double value)            { dtime = Round (value); return (*this); }
	Dtime & Minutes (double value)            { dtime = Round (value * 60.0); return (*this); }
	Dtime & Hours (double value)              { dtime = Round (value * 3600.0); return (*this); }
	
	Dtime & Seconds (int value)               { dtime = Round (value); return (*this); }
	Dtime & Minutes (int value)               { dtime = Round (value * 60); return (*this); }
	Dtime & Hours (int value)                 { dtime = Round (value * 3600); return (*this); }

	Dtime   Round_Seconds (void)              { Dtime temp = *this; return (Round (Resolve (temp))); }

	Dtime & operator = (int right)            { dtime = right; return (*this); }
	Dtime & operator = (double right)         { dtime = DTOI (right); return (*this); }
	Dtime & operator = (string &right)        { Time_String (right); return (*this); }
	Dtime & operator = (const Dtime &right)   { dtime = right.dtime; return (*this); }

	Dtime & operator () (int right)           { dtime = right; return (*this); }
	Dtime & operator () (double right)        { dtime = DTOI (right); return (*this); }
	Dtime & operator () (string right)        { Time_String (right); return (*this); }
	Dtime & operator () (Dtime &right)        { dtime = right.dtime; return (*this); }

	Dtime & operator ++ ()                    { dtime++; return (*this); }
	Dtime operator ++ (int)                   { Dtime temp = *this; dtime++; return (temp); }
	Dtime & operator -- ()                    { dtime--; return (*this); }
	Dtime operator -- (int)                   { Dtime temp = *this; dtime--; return (temp); }

	Dtime & operator += (int right)           { dtime += right; return (*this); }
	Dtime & operator += (double right)        { dtime += DTOI (right); return (*this); }
	Dtime & operator += (const Dtime& right)  { dtime += right.dtime; return (*this); }
	
	Dtime & operator -= (int right)           { dtime -= right; return (*this); }
	Dtime & operator -= (double right)        { dtime -= DTOI (right); return (*this); }
	Dtime & operator -= (const Dtime& right)  { dtime -= right.dtime; return (*this); }

	Dtime operator + (int right)              { return (Dtime (dtime) += right); }
	Dtime operator + (double right)           { return (Dtime (dtime) += DTOI (right)); }
	Dtime operator + (const Dtime& right)     { return (Dtime (dtime) += right.dtime); }

	Dtime operator - (int right)              { return (Dtime (dtime) -= right); }
	Dtime operator - (double right)           { return (Dtime (dtime) -= DTOI (right)); }
	Dtime operator - (const Dtime& right)     { return (Dtime (dtime) -= right.dtime); }

	void  Time_String (string text, bool duration = false, Units_Type format = NO_UNITS);
	void  Time_String (string text, Units_Type units) { Time_String (text, false, units); }

	string Time_String (Units_Type format = NO_UNITS);

	void   Time_Label (string text, Units_Type format = NO_UNITS);
	string Time_Label (Units_Type format = NO_UNITS);
	string Time_Label (bool pad_flag);

private:
	int dtime;
	static bool warning_flag;

#ifdef BOOST_MPI
	friend class boost::serialization::access;

	template <class Archive>
	void serialize (Archive &ar, const unsigned int version)
	{
		ar & dtime;
	}
#endif
};

//---- array of Dtime ----

typedef vector <Dtime>               Dtimes;
typedef Dtimes::iterator             Dtime_Itr;
typedef Dtimes::reverse_iterator     Dtime_RItr;
typedef Dtimes::pointer              Dtime_Ptr;

//---- Dtime operators ----

inline Dtime operator + (const Dtime &left, const Dtime &right) { return (Dtime (left) + right); }
inline Dtime operator + (const int left, const Dtime &right)    { return (Dtime (left) + right); }
inline Dtime operator + (const Dtime &left, const int right)    { return (Dtime (left) + right); }

inline Dtime operator - (const Dtime &left, const Dtime &right) { return (Dtime (left) - right); }
inline Dtime operator - (const int left, const Dtime &right)    { return (Dtime (left) - right); }
inline Dtime operator - (const Dtime &left, const int right)    { return (Dtime (left) - right); }

inline bool operator == (Dtime left, Dtime right)               { return ((int) left == (int) right); }
inline bool operator == (int left, Dtime right)                 { return (left == (int) right); }
inline bool operator == (Dtime left, int right)                 { return ((int) left == right); }

inline bool operator != (Dtime left, Dtime right)               { return ((int) left != (int) right); }
inline bool operator != (int left, Dtime right)                 { return (left != (int) right); }
inline bool operator != (Dtime left, int right)                 { return ((int) left != right); }

inline bool operator < (Dtime left, Dtime right)                { return ((int) left < (int) right); }
inline bool operator < (int left, Dtime right)                  { return (left < (int) right); }
inline bool operator < (Dtime left, int right)                  { return ((int) left < right); }

inline bool operator > (Dtime left, Dtime right)                { return ((int) left > (int) right); }
inline bool operator > (int left, Dtime right)                  { return (left > (int) right); }
inline bool operator > (Dtime left, int right)                  { return ((int) left > right); }

inline bool operator <= (Dtime left, Dtime right)               { return ((int) left <= (int) right); }
inline bool operator <= (int left, Dtime right)                 { return (left <= (int) right); }
inline bool operator <= (Dtime left, int right)                 { return ((int) left <= right); }

inline bool operator >= (Dtime left, Dtime right)               { return ((int) left >= (int) right); }
inline bool operator >= (int left, Dtime right)                 { return (left >= (int) right); }
inline bool operator >= (Dtime left, int right)                 { return ((int) left >= right); }

#endif
