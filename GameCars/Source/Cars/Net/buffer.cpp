//---------------------------------------------------------------------------
// buffer.h
//---------------------------------------------------------------------------

/**
 * @file buffer.h
 *
 * Implementa un buffer din�mico que los objetos utilizan para serializarse o deserializarse
 *
 *
 * @author Juan A. Recio-Garc�a, David Llans�
 * @date Octubre, 2006
 */

#include "buffer.h"
#include "memory.h"

#define DEBUG_BUFFER 1

namespace Net {

	CBuffer::CBuffer(size_t initsize, size_t delta)
	{
		_current = _begin = new byte[initsize];
		_maxsize = initsize;
		_size = 0;
		_delta = delta;
	}

	CBuffer::~CBuffer()
	{
		delete[] _begin;
	}

	byte* CBuffer::getData()
	{
		return _begin;
	}

	size_t CBuffer::getSize()
	{
		return _size;
	}

	void CBuffer::reset()
	{
		_current = _begin;
	}

	void CBuffer::write(const void* data, size_t datalength)
	{
		while (_size + datalength >= _maxsize)
			realloc();
		memcpy(_current, data, datalength);
		_current += datalength;
		_size += datalength;
	}

	void CBuffer::write(const char* data)
	{
		size_t iSize = strlen(data);
		write(iSize);
		write(data, iSize);
	}

	void CBuffer::read(void* data, size_t datalength)
	{
		memcpy(data, _current, datalength);
		_current += datalength;
	}

	void CBuffer::read(char* data)
	{
		size_t iSize;
		read(iSize);
		read(data, iSize);
		data[iSize] = '\0';
	}

	void CBuffer::realloc()
	{
		byte* newbuffer = new byte[_maxsize + _delta];
		memcpy(newbuffer, _begin, _size);
		delete[] _begin;
		_begin = newbuffer;
		_current = _begin + _size;
		_maxsize += _delta;
	}

} // namespace Net