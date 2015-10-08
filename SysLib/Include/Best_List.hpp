//*********************************************************
//	Best_List.hpp - vector of integers/doubles/bools
//*********************************************************

#ifndef BEST_LIST_HPP
#define BEST_LIST_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Integer_List Class definition
//---------------------------------------------------------

class SYSLIB_API Integer_List : public Integers
{
public:
	Integer_List (void) {}

	int  Best (int num);  
	bool Combine (Integer_List &list, int index = 1, bool round_flag = false);
};

//---------------------------------------------------------
//	Double_List Class definition
//---------------------------------------------------------

class SYSLIB_API Double_List : public Doubles
{
public:
	Double_List (void) {}

	double Best (int num);
	bool   Combine (Double_List &list, int index = 1, bool round_flag = false);
};

//---------------------------------------------------------
//	Dtime_List Class definition
//---------------------------------------------------------

class SYSLIB_API Dtime_List : public Dtimes
{
public:
	Dtime_List (void) {}

	Dtime Best (int num);  
	bool  Combine (Dtime_List &list, int index = 1, bool round_flag = false);
};

//---------------------------------------------------------
//	String_List Class definition
//---------------------------------------------------------

class SYSLIB_API String_List : public Strings
{
public:
	String_List (void) {}

	String Best (int num);
	bool  Combine (String_List &list, int index = 1);
};

//---------------------------------------------------------
//	Bool_List Class definition
//---------------------------------------------------------

class SYSLIB_API Bool_List : public Bools
{
public:
	Bool_List (void) {}

	bool Best (int num);
};

#endif
