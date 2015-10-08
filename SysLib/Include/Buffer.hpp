//*********************************************************
//	Buffer.hpp - dynamically allocated memory buffer
//*********************************************************

#ifndef BUFFER_HPP
#define BUFFER_HPP

#include "APIDefs.hpp"

#include <string.h>
#include <stdlib.h>

#include <string>
using namespace std;

#define READ_SIZE	2

//---------------------------------------------------------
//	Buffer
//---------------------------------------------------------

class SYSLIB_API Buffer 
{
public:
	Buffer (void)                             { size = max_size = 0; buffer = 0; }
	Buffer (const Buffer &data)               { size = max_size = 0; buffer = 0; Data ((Buffer &) data); }
	Buffer (const char *text)                 { size = max_size = 0; buffer = 0; Data (text); }
	Buffer (string &text)                     { size = max_size = 0; buffer = 0; Data (text); }

	~Buffer (void)                            { Clear (); }

	bool OK (void)                            { return (buffer != 0); }

	int  Size (void)                          { return (size); }
	bool Size (int num);

	int  Max_Size (void)                      { return (max_size); }
	bool Max_Size (int num);

	int  Set_Size (bool flag = true);
	int  Max_Read (void)                      { return ((OK ()) ? max_size + READ_SIZE - 1 : 0); }

	void Clear (void)                         { if (OK ()) { free (buffer); buffer = 0; size = max_size = 0; } }
	void Fill (char fill = '\0')              { if (OK () && max_size > 0) memset (buffer, fill, max_size); }
	
	bool   Data (void *data, int size);
	bool   Data (Buffer &data);
	bool   Data (const char *text)            { return ((text) ? Data ((void *) text, (int) strlen (text)) : true); }
	bool   Data (string &text)                { return (Data ((void *) (text.c_str ()), (int) text.size ())); }
	char * Data (void)                        { return (buffer); }

	Buffer & Assign (Buffer &data, int offset = 0, int count = -1);
	Buffer & Assign (void *data, int size);

	Buffer & operator = (const Buffer &right) { return (Assign ((Buffer &) right)); }	
	Buffer & operator = (const char *right)   { return ((right) ? Assign ((void *) right, (int) strlen (right)) : *this); }

private:
	unsigned short size, max_size;
	char *buffer;
};
#endif
