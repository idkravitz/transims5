//*********************************************************
//	String.hpp - string functions
//*********************************************************

#include "String.hpp"

#include "Static_Service.hpp"

#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;
//---------------------------------------------------------
//	Trim -- remove specified charaters
//---------------------------------------------------------

String & String::Trim (const char *chars) 
{
	Trim_Left (chars);
	Trim_Right (chars);
	return (*this);
}

String & String::Trim_Left (const char *chars) 
{
	int index;
	const char *ptr;

	string::iterator itr;
	index = 0;

	for (itr = begin (); itr < end (); itr++) {
		for (ptr = chars; *ptr != '\0'; ptr++) {
			if (*itr == *ptr) break;
		}
		if (*ptr == '\0') break;
		index++;
	}
	if (index) {
		erase (0, index);
	}
	return (*this);
}

String & String::Trim_Right (const char *chars) 
{
	int index;
	const char *ptr;

	string::reverse_iterator itr;
	index = 0;

	for (itr = rbegin (); itr != rend (); itr++) {
		for (ptr = chars; *ptr != '\0'; ptr++) {
			if (*itr == *ptr) break;
		}
		if (*ptr == '\0') break;
		index++;
	}
	if (index) {
		erase (length () - index);
	}
	return (*this);
}

//---------------------------------------------------------
//	Equals -- case insensitive compare strings
//---------------------------------------------------------

bool String::Equals (string test) 
{
	if (length () != test.length ()) return (false);

	char ch1, ch2;
	string::iterator itr1, itr2;

	for (itr1 = begin (), itr2 = test.begin (); itr1 < end (); itr1++, itr2++) {
		ch1 = *itr1;
		ch2 = *itr2;

		if (ch1 >= 'a' && ch1 <= 'z') ch1 = ch1 - 'a' + 'A';
		if (ch2 >= 'a' && ch2 <= 'z') ch2 = ch2 - 'a' + 'A';

		if (ch1 != ch2) return (false);
	}
	return (true);
}

bool String::Equals (const char *test) 
{
	if (length () != strlen (test)) return (false);

	char ch1, ch2, *ptr;
	string::iterator itr;

	for (itr = begin (), ptr = (char *) test; itr < end (); itr++, ptr++) {
		ch1 = *itr;
		ch2 = *ptr;

		if (ch1 >= 'a' && ch1 <= 'z') ch1 = ch1 - 'a' + 'A';
		if (ch2 >= 'a' && ch2 <= 'z') ch2 = ch2 - 'a' + 'A';

		if (ch1 != ch2) return (false);
	}
	return (true);
}

//---------------------------------------------------------
//	Less_Than -- case insensitive compare strings
//---------------------------------------------------------

bool String::Less_Than (string test) 
{
	char ch1, ch2;
	string::iterator itr1, itr2;

	for (itr1 = begin (), itr2 = test.begin (); itr1 < end () && itr2 < test.end (); itr1++, itr2++) {
		ch1 = *itr1;
		ch2 = *itr2;

		if (ch1 >= 'a' && ch1 <= 'z') ch1 = ch1 - 'a' + 'A';
		if (ch2 >= 'a' && ch2 <= 'z') ch2 = ch2 - 'a' + 'A';

		if (ch1 < ch2) return (true);
		if (ch1 > ch2) return (false);
	}
	if (itr2 < test.end ()) return (true);
	return (false);
}

bool String::Less_Than (const char *test) 
{
	char ch1, ch2, *ptr;
	string::iterator itr;

	for (itr = begin (), ptr = (char *) test; itr < end () && *ptr != '\0'; itr++, ptr++) {
		ch1 = *itr;
		ch2 = *ptr;

		if (ch1 >= 'a' && ch1 <= 'z') ch1 = ch1 - 'a' + 'A';
		if (ch2 >= 'a' && ch2 <= 'z') ch2 = ch2 - 'a' + 'A';

		if (ch1 < ch2) return (true);
		if (ch1 > ch2) return (false);
	}
	if (*ptr != '\0') return (true);
	return (false);
}


//---------------------------------------------------------
//	Greater_Than -- case insensitive compare strings
//---------------------------------------------------------

bool String::Greater_Than (string test) 
{
	char ch1, ch2;
	string::iterator itr1, itr2;

	for (itr1 = begin (), itr2 = test.begin (); itr1 < end () && itr2 < test.end (); itr1++, itr2++) {
		ch1 = *itr1;
		ch2 = *itr2;

		if (ch1 >= 'a' && ch1 <= 'z') ch1 = ch1 - 'a' + 'A';
		if (ch2 >= 'a' && ch2 <= 'z') ch2 = ch2 - 'a' + 'A';

		if (ch1 > ch2) return (true);
		if (ch1 < ch2) return (false);
	}
	if (itr1 < end ()) return (true);
	return (false);
}

bool String::Greater_Than (const char *test) 
{
	char ch1, ch2, *ptr;
	string::iterator itr;

	for (itr = begin (), ptr = (char *) test; itr < end () && *ptr != '\0'; itr++, ptr++) {
		ch1 = *itr;
		ch2 = *ptr;

		if (ch1 >= 'a' && ch1 <= 'z') ch1 = ch1 - 'a' + 'A';
		if (ch2 >= 'a' && ch2 <= 'z') ch2 = ch2 - 'a' + 'A';

		if (ch1 > ch2) return (true);
		if (ch1 < ch2) return (false);
	}
	if (itr < end ()) return (true);
	return (false);
}

//---------------------------------------------------------
//	Starts_With -- case insensitive compare range
//---------------------------------------------------------

bool String::Starts_With (string test) 
{
	if (length () < test.length ()) return (false);

	char ch1, ch2;
	string::iterator itr1, itr2;

	for (itr1 = begin (), itr2 = test.begin (); itr2 < test.end (); itr1++, itr2++) {
		ch1 = *itr1;
		ch2 = *itr2;

		if (ch1 >= 'a' && ch1 <= 'z') ch1 = ch1 - 'a' + 'A';
		if (ch2 >= 'a' && ch2 <= 'z') ch2 = ch2 - 'a' + 'A';

		if (ch1 != ch2) return (false);
	}
	return (true);
}

bool String::Starts_With (const char *test) 
{
	if (length () < strlen (test)) return (false);

	char ch1, ch2, *ptr;
	string::iterator itr;

	for (itr = begin (), ptr = (char *) test; *ptr != '\0'; itr++, ptr++) {
		ch1 = *itr;
		ch2 = *ptr;

		if (ch1 >= 'a' && ch1 <= 'z') ch1 = ch1 - 'a' + 'A';
		if (ch2 >= 'a' && ch2 <= 'z') ch2 = ch2 - 'a' + 'A';

		if (ch1 != ch2) return (false);
	}
	return (true);
}

//---------------------------------------------------------
//	Ends_With -- case insensitive compare range
//---------------------------------------------------------

bool String::Ends_With (string test) 
{
	if (length () < test.length ()) return (false);

	char ch1, ch2;
	string::reverse_iterator itr1, itr2;

	for (itr1 = rbegin (), itr2 = test.rbegin (); itr2 < test.rend (); itr1++, itr2++) {
		ch1 = *itr1;
		ch2 = *itr2;

		if (ch1 >= 'a' && ch1 <= 'z') ch1 = ch1 - 'a' + 'A';
		if (ch2 >= 'a' && ch2 <= 'z') ch2 = ch2 - 'a' + 'A';

		if (ch1 != ch2) return (false);
	}
	return (true);
}

bool String::Ends_With (const char *test) 
{
	size_t len = strlen (test);
	if (length () < len) return (false);

	char ch1, ch2, *ptr, *p0;
	string::reverse_iterator itr;

	p0 = (char *) test;

	for (itr = rbegin (), ptr = p0 + len - 1; ptr >= p0; itr++, ptr--) {
		ch1 = *itr;
		ch2 = *ptr;

		if (ch1 >= 'a' && ch1 <= 'z') ch1 = ch1 - 'a' + 'A';
		if (ch2 >= 'a' && ch2 <= 'z') ch2 = ch2 - 'a' + 'A';

		if (ch1 != ch2) return (false);
	}
	return (true);
}

//---------------------------------------------------------
//	Split - split a string into two strings
//---------------------------------------------------------

bool String::Split (string &result, const char *chars) 
{
	int i, i1, i2, num;
	const char *ptr;
	char ch;
	bool quote, tick, found;

	result.clear ();

	if (empty ()) return (false);

	num = (int) length ();
	i = 0;

	ch = at (i);

	quote = (ch == '"');
	tick = (ch == '\'');
	if (quote || tick) i++;

	i1 = i2 = i;
	found = false;

	for (; i < num; i++) {
		ch = at (i);

		if (quote && ch == '"') {
			quote = false;
		} else if (tick && ch == '\'') {
			tick = false;
		} else if (quote || tick) {
			i2 = i + 1;
		} else if (ch == '\n' || ch == '\r' || ch == '\f') {
			i = num;
			found = true;
			break;
		} else {
			for (ptr = chars; *ptr != '\0'; ptr++) {
				if (ch == *ptr) break;
			}
			if (*ptr == '\0') {
				if (ch != ' ') i2 = i + 1;
			} else {
				i++;
				while (i < num && at (i) == ' ') i++;
				found = true;
				break;
			}
		}
	}
	if (found) {
		result = substr (i1, i2 - i1);
		erase (0, i);
	} else {
		result = *this;
		clear ();
	}
	return (true);
}

//---------------------------------------------------------
//	Split_Last - split a string into two strings
//---------------------------------------------------------

bool String::Split_Last (string &result, const char *chars) 
{
	int i, i1, i2, num;
	const char *ptr;
	char ch;
	bool quote, tick, found;

	result.clear ();
	Trim_Right (" \t\n\r\f");

	if (empty ()) return (false);

	num = (int) length ();
	i = num - 1;

	ch = at (i);

	quote = (ch == '"');
	tick = (ch == '\'');
	if (quote || tick) i--;

	i1 = i;
	i2 = i + 1;
	found = false;

	for (; i >= 0; i--) {
		ch = at (i);

		if (quote && ch == '"') {
			quote = false;
		} else if (tick && ch == '\'') {
			tick = false;
		} else if (quote || tick) {
			i1 = i;
		} else {
			for (ptr = chars; *ptr != '\0'; ptr++) {
				if (ch == *ptr) break;
			}
			if (*ptr == '\0') {
				if (ch != ' ') i1 = i;
			} else {
				while (i > 0 && at (i-1) == ' ') i--;
				found = true;
				break;
			}
		}
	}
	if (found) {
		result = substr (i1, i2 - i1);
		erase (i);
	} else {
		result = *this;
		clear ();
	}
	return (true);
}

//---------------------------------------------------------
//	Parse - parse a string into a vector of substrings
//---------------------------------------------------------

int  String::Parse (Strings &result, const char *chars) 
{
	String element;

	result.clear ();

	while (Split (element, chars)) {
		result.push_back (element);
	}
	return ((int) result.size ());
}

//---------------------------------------------------------
//	Range - unpack a range string
//---------------------------------------------------------

bool String::Range (string &low, string &high) 
{
	size_t i, num;

	Trim ();
	if (empty () || at (0) == '\n') return (false);

	i = find ("..");

	if (i == npos) {
		i = find ("-");

		if (i == npos || i == 0) {
			low = high = *this;
		} else {
			low = substr (0, i);

			num = length ();

			while (i < num && at (i) == '-') i++;

			high = substr (i);
		}
	} else {
		low = substr (0, i);

		num = length ();

		while (i < num && at (i) == '.') i++;

		high = substr (i);
	}
	return (!high.empty ());
}

//---------------------------------------------------------
//	Range - unpack a range string
//---------------------------------------------------------

bool String::Range (double &low, double &high) 
{
	size_t i;
	String start, end, temp;

	low = high = 0.0;

	if (!Range (start, end)) return (false);

	i = start.find (":");
	if (i == start.npos) {
		low = start.Double ();
	} else {
		temp = start.substr (0, i);
		low = temp.Double ();
		temp = start.substr (i+1);
		low += temp.Double () / 60.0;
	}

	i = end.find (":");

	if (i == end.npos) {
		high = end.Double ();
	} else {
		temp = end.substr (0, i);
		high = temp.Double ();
		temp = end.substr (i+1);
		high += temp.Double () / 60.0;
	}
	return (true);
}

//---------------------------------------------------------
//	Range - unpack a range string
//---------------------------------------------------------

bool String::Range (int &low, int &high) 
{
	String start, end;

	low = high = 0;
	if (!Range (start, end)) return (false);

	low = start.Integer ();
	high = end.Integer ();
	return (true);
}

//---------------------------------------------------------
//	In_Range - check range string
//---------------------------------------------------------

bool String::In_Range (String &range) 
{
	string low, high;

	range.Range (low, high);
	if (Less_Than (low) || Greater_Than (high)) return (false);
	return (true);
}

//---------------------------------------------------------
//	Clean -- remove white space and comments
//---------------------------------------------------------

String & String::Clean (void)
{
	if (!empty ()) {
		int i1, i2;
		bool quote, tick, clean;
		string::iterator itr, itr0;

		quote = tick = clean = false;
		
		//--- remove leading blanks ----

		for (i1 = 0, itr = begin (); itr != end (); i1++, itr++) {
			if (*itr != ' ' && *itr != '\t') break;
			clean = true;
		}

		//---- remove comment statements ----

		for (i2 = i1, itr0 = itr; itr != end (); itr++) {
			if (!tick && *itr == '"') {
				quote = !quote;
			} else if (!quote && *itr == '\'') {
				tick = !tick;
			} else if (!quote && !tick && (itr+1) != end () &&
				((*itr == '#' && (*(itr+1) == '#' || *(itr+1) == '-' || *(itr+1) == '*')) ||
				(*itr == '/' && (*(itr+1) == '/' || *(itr+1) == '-' || *(itr+1) == '*')) ||
				(*itr == ';' && (*(itr+1) == ';' || *(itr+1) == '-' || *(itr+1) == '*')))) {
				clean = true;
				break;
			}
			i2++;
		}

		//---- remove white space ----

		if (itr > itr0 && itr != end ()) {
			while (itr-- > itr0) {
				if (*itr != ' ' && *itr != '\t' && *itr != '\n' && *itr != '\r' && *itr != '\f') break;
				i2--;
				clean = true;
			}
		}
		if (clean) {
			if (i1 > 0) erase (0, i1);
			erase (i2 - i1);
		}
	}
	return (*this);
}

//---------------------------------------------------------
//	Center -- center text on a string 
//---------------------------------------------------------

String & String::Center (int size)
{
	int len = (int) length ();

	if (len > size) {
		erase (size);
	} else if (len < size) {
		len = (size - len) / 2;
		insert (0, len, ' ');
		len = (int) length ();

		if (len < size) {
			size -= len;
			insert (len, size, ' ');
		}
	}
	return (*this);
}

//---------------------------------------------------------
//	To_Upper -- convert string to upper case 
//---------------------------------------------------------

String & String::To_Upper (void)
{
	string::iterator itr;

	for (itr = begin (); itr < end (); itr++) {
		if (*itr >= 'a' && *itr <= 'z') {
			*itr = *itr - 'a' + 'A';
		}
	}
	return (*this);
}

//---------------------------------------------------------
//	To_Lower -- convert string to lower case 
//---------------------------------------------------------

String & String::To_Lower (void)
{
	string::iterator itr;

	for (itr = begin (); itr < end (); itr++) {
		if (*itr >= 'A' && *itr <= 'Z') {
			*itr = *itr - 'A' + 'a';
		}
	}
	return (*this);
}

//---------------------------------------------------------
//	To_Title -- convert string to title case 
//---------------------------------------------------------

String & String::To_Title (void)
{
	size_t i;
	string::iterator itr;
	bool flag = true;

	for (itr = begin (); itr < end (); itr++) {
		if (*itr == '_') {
			*itr = ' ';
		}
		if (*itr == ' ') {
			flag = true;
		} else if (flag) {
			if (*itr >= 'a' && *itr <= 'z') {
				*itr = *itr - 'a' + 'A';
			}
			flag = false;
		} else if (*itr >= 'A' && *itr <= 'Z') {
			*itr = *itr - 'A' + 'a';
		}
	}

	//---- look for special words ----

	i = find ("Of ");
	if (i != npos) at (i) = 'o';

	i = find ("To ");
	if (i != npos) at (i) = 't';
	
	i = find ("Od ");
	if (i != npos) at (i+1) = 'D';

	i = find ("Id ");
	if (i != npos) at (i+1) = 'D';
	
	i = find ("Id");
	if (i != npos && i == (size () - 2)) at (i+1) = 'D';

	i = find ("Ab ");
	if (i != npos) at (i+1) = 'B';

	i = find ("Ba ");
	if (i != npos) at (i+1) = 'A';

	i = find (" Ab");
	if (i != npos) at (i+2) = 'B';

	i = find (" Ba");
	if (i != npos && (i + 3) == (int) size ()) {
		at (i+2) = 'A';
	}
	i = find ("Vc ");
	if (i != npos) at (i+1) = 'C';

	i = find (" Xy");
	if (i != npos) at (i+2) = 'Y';

	i = find (" XYz");
	if (i != npos) {
		at (i+3) = 'Z';
	}
	i = find (" XYm");
	if (i != npos) {
		at (i+3) = 'M';
	}
	i = find ("Cbd ");
	if (i != npos) {
		at (i+1) = 'B';
		at (i+2) = 'D';
	}
	i = find ("Pnr ");
	if (i != npos) {
		at (i+1) = 'N';
		at (i+2) = 'R';
	}
	i = find ("Knr ");
	if (i != npos) {
		at (i+1) = 'N';
		at (i+2) = 'R';
	}
	i = find ("On Off ");
	if (i != npos) {
		at (i+2) = '-';
	}
	return (*this);
}

//---------------------------------------------------------
//	Bool -- convert string to bool 
//---------------------------------------------------------

bool String::Bool (void)
{
	if (empty ()) return (false);

	char ch = at (0);

	return (ch == 'Y' || ch == 'y' || ch == 't' || ch == 'T' || ch == '1');
}

//---------------------------------------------------------
//	str_fmt - safe string formating
//---------------------------------------------------------

string * String::str_fmt (string *format, ...)
{
	if (!format->empty ()) {
		char text [4096];
		va_list args;
		va_start (args, format);

#ifdef _MSC_EXTENSIONS
		vsprintf_s (text, sizeof (text), format->c_str (), args);
#else
		vsprintf (text, format->c_str (), args);
#endif
		va_end (args);
		*format = text;
	}
	return (format);
}

//---------------------------------------------------------
//	Find_Field -- extract the format string 
//---------------------------------------------------------

bool String::Find_Field (int *start, string &field, const char *types) 
{
	int len = 0;
	bool type_flag = false;
	bool star_flag = (*types == 'd' || *types == 'i' || *types == 'u');
	*start = 0;
	field.clear ();

	string::iterator itr, itr1;

	for (itr = begin (); itr < end (); itr++) {
		if (*itr == '%') {
			itr1 = itr + 1;
			if (itr1 < end () && *itr1 == '%') {
				itr++;
				continue;
			}
			break;
		}
	}
	if (itr == end ()) return (false);

	itr1 = itr++;
	*start = (int) (itr1 - begin ());

	if (itr == end ()) {
		len = 1;
		field = substr (*start, len);
	} else {
		if (*itr == '-') itr++;

		if (*itr == '*') {
			if (star_flag) {
				*start = (int) (itr - begin ());
				field = "%";
				len = 1;
			} else {
				len = (int) (itr - itr1);
				field = substr (*start, len);
			}
		} else {
			while (itr != end () && *itr >= '0' && *itr <= '9') itr++;

			if (*itr == '.') itr++;

			if (*itr == '*') {
				if (star_flag) {

					*start = (int) (itr - begin ());
					field = "%";
					len = 1;
				} else {
					len = (int) (itr - itr1);
					field = substr (*start, len);
				}
			} else {
				while (itr != end () && *itr >= '0' && *itr <= '9') itr++;

				if (*itr == 'l') itr++;

				if (itr != end () &&
					(*itr == 'd' || *itr == 'f' || *itr == 'p' || *itr == 's' || 
					*itr == 'i' || *itr == 'g' || *itr == 'u' || *itr == 'c')) {
					len = (int) (itr - itr1 + 1);
					type_flag = true;
				} else {
					len = (int) (itr - itr1);
				}
				field = substr (*start, len);
			}
		}
	}
	erase (*start, len);

	if (type_flag) {
		char ch, *ch_ptr;

		len = (int) (field.length () - 1);
		ch = field [len];

		for (ch_ptr = (char *) types; *ch_ptr != '\0'; ch_ptr++) {
			if (ch == *ch_ptr) break;
		}
		if (*ch_ptr == '\0') {
			field [len] = *types;
		}
	} else if (*types == 'f') {
		field += "lf";
	} else {
		field += *types;
	}
	return (true);
}

//---------------------------------------------------------
//	operator () -- set operators
//---------------------------------------------------------

String & String::operator () (int right)
{
	char text [24];

#ifdef _MSC_EXTENSIONS
	_itoa_s (right, text, sizeof (text), 10);
#else
	sprintf (text, "%d", right);
#endif
	*this = text;
	return (*this);
}

String & String::operator () (size_t right)
{
	*this = "%u";
	str_fmt (this, right);
	return (*this);
}

String & String::operator () (double right, int decimal)
{
	*this = "%.*lf";
	str_fmt (this, decimal, right);
	return (*this);
}

String & String::operator () (double right)
{
	*this = "%g";
	str_fmt (this, right);
	return (*this);
}

//---------------------------------------------------------
//	operator % -- type safe field replacement operators
//---------------------------------------------------------

String & String::operator % (int right)
{
	int start;
	string field;

	if (Find_Field (&start, field, "diu")) {
		insert (start, *str_fmt (&field, right));
	}
	return (*this);
}

String & String::operator % (double right)
{
	int start;
	string field;

	if (Find_Field (&start, field, "fg")) {
		insert (start, *str_fmt (&field, right));
	}
	return (*this);
}

String & String::operator % (char right)
{
	if (right == FINISH) {
		string::iterator itr, itr1;

		for (itr = begin (); itr < end (); itr++) {
			if (*itr == '%') {
				itr1 = itr + 1;
				if (itr1 < end () && *itr1 == '%') {
					erase (itr1 - begin (), 1);
				}
			}
		}
	} else {
		int start;
		string field;

		if (Find_Field (&start, field, "c")) {
			insert (start, *str_fmt (&field, right));
		}
	}
	return (*this);
}

String & String::operator % (const char * right)
{
	int start;
	string field;

	if (Find_Field (&start, field, "s")) {
		insert (start, *str_fmt (&field, right));
	}
	return (*this);
}

String & String::operator % (string right)
{
	int start;
	string field;

	if (Find_Field (&start, field, "s")) {
		insert (start, *str_fmt (&field, right.c_str ()));
	}
	return (*this);
}

String & String::operator % (size_t right)
{
	int start;
	string field;

	if (Find_Field (&start, field, "udi")) {
		insert (start, *str_fmt (&field, right));
	}
	return (*this);
}

String & String::operator % (void *right)
{
	int start;
	string field;

	if (Find_Field (&start, field, "p")) {
		insert (start, *str_fmt (&field, right));
	}
	return (*this);
}

