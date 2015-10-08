//*********************************************************
//	String.hpp - string functions
//*********************************************************

#ifndef STRING_HPP
#define STRING_HPP

#include "APIDefs.hpp"
#include "System_Defines.hpp"

#include <stdlib.h>

#include <vector>
#include <string>
using namespace std;

//---------------------------------------------------------
//	String
//---------------------------------------------------------

class SYSLIB_API String : public string 
{
	typedef vector <String> Strings;

public:
	String (void)                               {}
	String (string text) : string (text)        {}
	String (const char *text) : string (text)   {}

	String & Trim (const char *chars = SPACES);
	String & Trim_Left (const char *chars = SPACES);
	String & Trim_Right (const char *chars = SPACES);

	bool Equals (string test);
	bool Equals (const char *test);
	
	bool Less_Than (string test);
	bool Less_Than (const char *test);

	bool Greater_Than (string test);
	bool Greater_Than (const char *test);

	bool Starts_With (string test);
	bool Starts_With (const char *test);
	
	bool Ends_With (string test);
	bool Ends_With (const char *test);

	bool Split (string &result, const char *chars = CSV_DELIMITERS);
	bool Split_Last (string &result, const char *chars = CSV_DELIMITERS);

	int  Parse (Strings &result, const char *chars = CSV_DELIMITERS);
	bool Range (string &low, string &high);
	bool Range (double &low, double &high);
	bool Range (int &low, int &high);

	bool In_Range (String &range);

	String & Clean (void);

	String & Center (int size);
	String & To_Upper (void);
	String & To_Lower (void);
	String & To_Title (void);

	int    Integer (void)                       { return ((!empty ()) ? atoi (c_str ()) : 0); }
	double Double (void)                        { return ((!empty ()) ? atof (c_str ()) : 0.0); }
	bool   Bool (void);
	
	String & operator = (char right)            { assign (1, right); return (*this); }
	String & operator = (const char *right)     { assign (right); return (*this); }
	String & operator = (string right)          { assign (right); return (*this); }

	String & operator () (int right);
	String & operator () (short right)          { return ((*this) ((int) right)); }
	String & operator () (double right);
	String & operator () (double right, int decimal);
	String & operator () (bool right)           { return ((*this) ((int) right)); }
	String & operator () (const char *right)    { assign (right); return (*this); }
	String & operator () (string right)         { assign (right); return (*this); }
	String & operator () (size_t right);

	String & operator % (int right); 
	String & operator % (short right)           { *this % ((int) right); return (*this); }
	String & operator % (double right);
	String & operator % (bool right)            { *this % ((int) right); return (*this); }
	String & operator % (char right);
	String & operator % (const char *right);
	String & operator % (string right);
	String & operator % (size_t right);
	String & operator % (void *right);

private:
	bool Find_Field (int *start, string &field, const char *types);
	string * str_fmt (string *format, ...);
};

inline String * String_Ptr (string &text)  { return ((String *) &(text)); }

#endif
