//*********************{***********************************
//	Buffer.hpp - dynamically allocated memory buffer
//*********************************************************

#include "Buffer.hpp"

//---------------------------------------------------------
//	Max_Size -- allocate memory buffer
//---------------------------------------------------------

bool Buffer::Max_Size (int num) 
{
	if (num < 0) return (false);

	int old_size, new_size;

	old_size = max_size + READ_SIZE;
	max_size = 0;

	new_size = num + READ_SIZE;

	buffer = (char *) realloc (buffer, new_size);
	if (!buffer) {
		size = 0;
		return (false);
	}
	max_size = (unsigned short) num;

	char *start = buffer;
	
	if (old_size > READ_SIZE) {
		if (new_size > old_size) {
			start += old_size;
			new_size -= old_size;
		} else {
			if (size > num) size = (unsigned short) num;
			new_size = 0;
		}
	}
	if (new_size > 0) memset (start, '\0', new_size);

	return (true);
}

//---------------------------------------------------------
//	Size -- set the data size
//---------------------------------------------------------

bool Buffer::Size (int num) 
{
	if (num > 0 && (num > max_size || !OK ())) {
		if (!Max_Size (num)) return (false);
	}
	size = (unsigned short) num; 
	return (num == size);
}

//---------------------------------------------------------
//	Set_Size -- set the data size
//---------------------------------------------------------

int Buffer::Set_Size (bool flag) 
{
	if (!buffer) return (0);

	if (flag) {
		size = (unsigned short) strlen (buffer);
		if (size > max_size) {
			size = max_size;
			buffer [size] = '\0';
		}
	} else {
		size = max_size;
	}
	return (size);
}

//---------------------------------------------------------
//	Data -- copy data to the buffer
//---------------------------------------------------------

bool Buffer::Data (void *data, int _size) 
{
	if (!Size (_size)) return (false);
		
	if (_size && buffer != data) {
		memcpy (buffer, data, _size);
	}
	if (OK ()) buffer [_size] = '\0';
	size = (unsigned short) _size;
	return (true);
}

bool Buffer::Data (Buffer &data)
{
	if (!Size (data.size)) return (false);
	size = data.size;
	if (size > 0) {
		memcpy (buffer, data.buffer, size);
	}
	if (OK ()) buffer [size] = '\0';
	return (true);
}

//---------------------------------------------------------
//	Assign -- copy data to the buffer
//---------------------------------------------------------

Buffer & Buffer::Assign (void *data, int _size) 
{
	if (Size (_size)) {
		if (_size && buffer != data) {
			memcpy (buffer, data, _size);
		}
		if (OK ()) buffer [_size] = '\0';
		size = (unsigned short) _size;
	}
	return (*this);
}

Buffer & Buffer::Assign (Buffer &data, int offset, int count) 
{
	if (count < 0) {
		count = data.size - offset;
	}
	if (offset >= 0 && offset + count <= data.size) {
		if (Size (count)) {
			if (count) {
				memcpy (buffer, data.buffer + offset, count);
			}
			if (OK ()) buffer [count] = '\0';
			size = (unsigned short) count;
		}
	}
	return (*this);
}
