//*********************************************************
//	Data_Buffer.hpp - dynamically allocated memory buffer
//*********************************************************

#ifndef DATA_BUFFER_HPP
#define DATA_BUFFER_HPP

#include "APIDefs.hpp"
#include "String.hpp"
#include "Buffer.hpp"

#include <string.h>
#include <stdlib.h>

//---------------------------------------------------------
//	Data_Buffer
//---------------------------------------------------------

class SYSLIB_API Data_Buffer 
{
public:
	Data_Buffer (void)                             { Set (); }
	Data_Buffer (void *data, int num)              { Set (); Data (data, num); }
	Data_Buffer (void *data, size_t num)           { Set (); Data (data, num); }
	Data_Buffer (const Data_Buffer &data)          { Set (); Data ((Data_Buffer &) data); }
	Data_Buffer (const Buffer &data)               { Set (); Data ((Buffer &) data); }

	~Data_Buffer (void)                            { Clear (); }
	
	bool   OK (void)                               { return (buffer != 0); }
	void   Set (void)                              { size = max_size = 0; buffer = get_ptr = 0; }

	size_t Size (void)                             { return (size); }
	bool   Size (int num)                          { return (Size ((size_t) num)); }
	bool   Size (size_t num);

	size_t Max_Size (void)                         { return (max_size); }
	bool   Max_Size (int num)                      { return (Max_Size ((size_t) num)); }
	bool   Max_Size (size_t num);

	void Clear (void)                              { if (OK ()) { free (buffer); buffer = get_ptr = 0; size = max_size = 0; } }
	void Fill (char fill = '\0')                   { if (OK () && max_size > 0) memset (buffer, fill, max_size); }

	char * Data (void)                             { return (buffer); }	

	bool Data (void *data, size_t num);
	bool Data (void *data, int num)                { return (Data (data, (size_t) num)); }
	bool Data (Data_Buffer &data)                  { return (Data ((char *) data.Data (), data.Size ())); }
	bool Data (Buffer &data)                       { return (Data (data.Data (), data.Size ())); }

	bool Add_Data (void *data, size_t num);
	bool Add_Data (void *data, int num)            { return (Add_Data (data, (size_t) num)); }
	bool Add_Data (Data_Buffer &data)              { return (Add_Data ((char *) data.Data (), data.Size ())); }
	bool Add_Data (Buffer &data)                   { return (Add_Data (data.Data (), data.Size ())); }

	Data_Buffer & Assign (void *data, size_t num)  { Data (data, num); return (*this); }
	Data_Buffer & Assign (void *data, int num)     { return (Assign (data, (size_t) num)); }
	Data_Buffer & Assign (Data_Buffer &data)       { return (Assign ((char *) data.Data (), data.Size ())); }
	Data_Buffer & Assign (Buffer &data)            { return (Assign (data.Data (), data.Size ())); }

	Data_Buffer & Append (void *data, size_t num)  { Add_Data (data, num); return (*this); }
	Data_Buffer & Append (void *data, int num)     { return (Append (data, (size_t) num)); }
	Data_Buffer & Append (Data_Buffer &data)       { return (Append ((char *) data.Data (), data.Size ())); }
	Data_Buffer & Append (Buffer &data)            { return (Append (data.Data (), data.Size ())); }

	Data_Buffer & operator = (const Data_Buffer &right) { return (Assign ((Data_Buffer &) right)); }	
	Data_Buffer & operator + (const Data_Buffer &right) { return (Append ((Data_Buffer &) right)); }

	bool Message (String message)                  { return (Data ((void *) message.c_str (), message.size () + 1)); }
	
	bool Get_Data (void *data, size_t num);
	bool Get_Data (void *data, int num)            { return (Get_Data (data, (size_t) num)); }

	void Set_First (void)                          { get_ptr = buffer; }
	bool Get_First (void *data, size_t num)        { Set_First (); return (Get_Data (data, num)); }

private:
	size_t size, max_size;
	char * buffer;
	char * get_ptr;
};
#endif
