//*********************{***********************************
//	Data_Buffer.hpp - dynamically allocated memory buffer
//*********************************************************

#include "Data_Buffer.hpp"

//---------------------------------------------------------
//	Max_Size -- allocate memory buffer
//---------------------------------------------------------

bool Data_Buffer::Max_Size (size_t num) 
{
	if (num == 0) return (false);

	size_t old_size, new_size;

	old_size = max_size;
	max_size = 0;

	new_size = num;

	buffer = (char *) realloc (buffer, new_size);
	if (!buffer) {
		size = 0;
		return (false);
	}
	max_size = new_size;

	char *start = buffer;
	
	if (new_size > old_size) {
		start += old_size;
		new_size -= old_size;
	} else {
		if (size > new_size) size = new_size;
		new_size = 0;
	}
	if (new_size > 0) memset (start, '\0', new_size);
	get_ptr = buffer;

	return (true);
}

//---------------------------------------------------------
//	Size -- set the data size
//---------------------------------------------------------

bool Data_Buffer::Size (size_t num) 
{
	if (num > 0 && (num > max_size || !OK ())) {
		if (!Max_Size (num)) return (false);
	}
	size = num; 
	get_ptr = buffer;
	return (true);
}

//---------------------------------------------------------
//	Data -- copy data to the buffer
//---------------------------------------------------------

bool Data_Buffer::Data (void *data, size_t num) 
{
	if (!Size (num)) return (false);
		
	if (num && buffer != data) {
		memcpy (buffer, data, num);
	}
	size = num;
	return (true);
}

//---------------------------------------------------------
//	Add_Data -- copy data to the end of the buffer
//---------------------------------------------------------

bool Data_Buffer::Add_Data (void *data, size_t num) 
{
	size_t old_size = Size ();
	char *start;

	if (!Size (old_size + num)) return (false);
	
	start = buffer + old_size;

	if (num && start != data) {
		memcpy (start, data, num);
	}
	size = old_size + num;
	return (true);
}

//---------------------------------------------------------
//	Get_Data -- copy data to the get offset
//---------------------------------------------------------

bool Data_Buffer::Get_Data (void *data, size_t num) 
{
	if ((get_ptr + num) > (buffer + size)) return (false);

	memcpy (data, get_ptr, num);
	get_ptr += num;
	return (true);
}
