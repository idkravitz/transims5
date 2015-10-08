//*********************************************************
//	Skim_Data.cpp - skim data processing
//*********************************************************

#include "Skim_Data.hpp"

//---------------------------------------------------------
//	Add_Skim
//---------------------------------------------------------

void Skim_Data::Add_Skim (int wk, int dr, int tr, int wt, int ot, int len, int ct, unsigned imp)
{
	if (count == 0) {
		count = 1;
		walk = (unsigned short) wk;
		drive = dr;
		transit = tr;
		wait = (unsigned short) wt;
		other = (unsigned short) ot;
		length = len;
		cost = (unsigned short) ct;
		impedance = imp;
	} else {
		double cnt = count;
		count++;
		walk = (unsigned short) ((walk * cnt + wk) / count + 0.5);
		drive = (int) ((drive * cnt + dr) / count + 0.5);
		transit = (int) ((transit * cnt + tr) / count + 0.5);
		wait = (unsigned short) ((wait * cnt + wt) / count + 0.5);
		other = (unsigned short) ((other * cnt + ot) / count + 0.5);
		length = (int) ((length * cnt + len) / count + 0.5);
		cost = (unsigned short) ((cost * cnt + ct) / count + 0.5);
		impedance = (unsigned) ((impedance * cnt + imp) / count + 0.5);
	}
}

void Skim_Data::Add_Skim (int time, int len, int ct, unsigned imp)
{
	if (count == 0) {
		count = 1;
		drive = time;
		length = len;
		cost = (unsigned short) ct;
		impedance = imp;
	} else {
		double cnt = count;
		count++;
		drive = (int) ((drive * cnt + time) / count + 0.5);
		length = (int) ((length * cnt + len) / count + 0.5);
		cost = (unsigned short) ((cost * cnt + ct) / count + 0.5);
		impedance = (unsigned) ((impedance * cnt + imp) / count + 0.5);
	}
}

void Skim_Data::Add_Skim (Skim_Data &skim)
{
	if (count == 0) {
		count = 1;
		drive = skim.Time ();
		length = skim.Length ();
		cost = (unsigned short) skim.Cost ();
		impedance = skim.Impedance ();
	} else {
		double cnt = count;
		count++;
		drive = (int) ((drive * cnt + skim.Time ()) / count + 0.5);
		length = (int) ((length * cnt + skim.Length ()) / count + 0.5);
		cost = (unsigned short) ((cost * cnt + skim.Cost ()) / count + 0.5);
		impedance = (unsigned) ((impedance * cnt + skim.Impedance ()) / count + 0.5);
	}
}
