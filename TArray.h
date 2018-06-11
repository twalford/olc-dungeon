// * * TArray2D.h
// * * Author: @Timkin  (thank you Tim!)
// * * Created: 05/03/11

#ifndef TARRAY2D_H
#define TARRAY2D_H

#include <memory>
#include <new>
#include <exception>

#include <string.h>

const size_t BUFSIZE = 256;

// Visual Studio accepts but does not implement exception specifications... so stop it from complaining
#pragma warning( disable : 4290)

// define a new exception class for invalid matrix (2D) indices
class invalid_index : public std::exception
{
private:
	size_t r, c;
	char szMsg[BUFSIZE];

protected:
	invalid_index& operator=(const invalid_index &bi) {}

public:
	invalid_index(size_t const &_r, size_t const &_c)
		: r(_r), c(_c) {}

	invalid_index(const invalid_index &bi)
	{
		r = bi.r;
		c = bi.c;
#ifdef _WIN32      
		strcpy_s(szMsg, BUFSIZE * sizeof(char), bi.szMsg);
#else
		strncpy(szMsg, bi.szMsg, BUFSIZE * sizeof(char));
#endif
	}

	const char * what()
	{
		// construct error msg only when needed
#ifdef _WIN32
		sprintf_s(szMsg, BUFSIZE * sizeof(char), "Bad index value (%u,%u)", r, c);
#else
		snprintf(szMsg, BUFSIZE * sizeof(char), "Bad index value (%u,%u)", r, c);
#endif
		return szMsg;
	}
};

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct _coord2
	{
		short row, col;

		_coord2(short r = 0, short c = 0) : row(r), col(c) {}
		_coord2(_coord2 const& c) : row(c.row), col(c.col) {}

#ifdef _WINDOWS
		// if _WINDOWS is defined the COORD is defined as a struct{ SHORT X,Y; } 
		// and is used as an argument for Windows Console API functions, so
		// we support casting to a COORD
		operator COORD()
		{
			COORD c = { col,row };
			return c;
		}
#endif

	} COORD2, *PCOORD2;

#ifdef __cplusplus
}
#endif


template <typename ValueType>
class TArray2D
{

public:
	typedef ValueType	value_type;
	typedef ValueType	*pointer;
	typedef ValueType	&reference;
	typedef ValueType	const *const_pointer;
	typedef ValueType	const &const_reference;
	typedef pointer			iterator;
	typedef const_pointer	const_iterator;

private:
	size_t			m_uRows,
		m_uCols;
	value_type		*m_pBuffer;

	void DeleteBuffer();
	void AllocateBuffer(size_t r, size_t c);
	size_t index_of(size_t r, size_t c);

protected:
	TArray2D(const TArray2D&) {}
	TArray2D& operator=(const TArray2D&) {}

public:
	TArray2D();
	TArray2D(size_t _rows, size_t _cols);
	~TArray2D();

	void resize(size_t _rows, size_t _cols);

	reference   operator()(size_t row, size_t col) throw (std::bad_alloc);
	value_type  operator()(size_t row, size_t col) const throw (std::bad_alloc);

	reference   operator()(COORD2 const & p) throw (std::bad_alloc);
	value_type  operator()(COORD2 const & p) const throw (std::bad_alloc);

	iterator begin();
	const_iterator begin() const;

	iterator end();
	const_iterator end() const;

	size_t const& rows() const;
	size_t const& cols() const;

	COORD2 size() const;
	size_t const length() const;

	pointer buffer();
	void clear();

};

template <typename T>
TArray2D<T>::TArray2D()
	: m_pBuffer(NULL), m_uRows(0), m_uCols(0)
{
}


template <typename T>
TArray2D<T>::TArray2D(size_t _rows, size_t _cols)
	: m_pBuffer(NULL), m_uRows(_rows), m_uCols(_cols)
{
	AllocateBuffer(m_uRows, m_uCols);
}


template <typename T>
TArray2D<T>::~TArray2D()
{
	DeleteBuffer();
}


template <typename T>
void TArray2D<T>::DeleteBuffer()
{
	if (m_pBuffer)
	{
		delete[] m_pBuffer;
		m_pBuffer = NULL;
	}
}

template <typename T>
void TArray2D<T>::AllocateBuffer(size_t r, size_t c)
{
	if (m_pBuffer)
		DeleteBuffer();

	if (r * c == 0)
		throw invalid_index(r, c);

	m_pBuffer = new value_type[r*c];

	if (!m_pBuffer)
#ifdef _WIN32
		//    throw std::exception("Invalid array access: memory not allocated");
		throw std::bad_alloc();
#else
		throw std::bad_alloc();
#endif

	memset(m_pBuffer, 0, r*c * sizeof(value_type));
}

template <typename T>
void TArray2D<T>::resize(size_t _rows, size_t _cols)
{
	DeleteBuffer();
	m_uRows = _rows;
	m_uCols = _cols;

	AllocateBuffer(m_uRows, m_uCols);
}


// row,col hash function (simple array index, column major form
template <typename T>
size_t TArray2D<T>::index_of(size_t r, size_t c)
{
	if (r >= m_uRows || c >= m_uCols)
		throw invalid_index(r, c);
	return  c + r * m_uCols;
}

// Array dimension accessors
template <typename T>
const size_t& TArray2D<T>::rows() const
{
	return m_uRows;
}

template <typename T>
const size_t& TArray2D<T>::cols() const
{
	return m_uCols;
}

template <typename T>
COORD2 TArray2D<T>::size() const
{
	COORD2 c(m_uRows, m_uCols);
	return c;
}

// Element access methods
template <typename T>
T& TArray2D<T>::operator()(size_t row, size_t col) throw (std::bad_alloc)
{
	if (!m_pBuffer)
#ifdef _WIN32
		//    throw std::exception("Invalid array access: memory not allocated");
		throw std::bad_array_new_length();
#else
		throw std::bad_alloc();
#endif

	return m_pBuffer[index_of(row, col)];
}

template <typename T>
T  TArray2D<T>::operator()(size_t row, size_t col) const throw (std::bad_alloc)
{
	if (!m_pBuffer)
#ifdef _WIN32
		//    throw std::exception("Invalid array access: memory not allocated");
		throw std::bad_array_new_length();
#else
		throw std::bad_alloc();
#endif

	return m_pBuffer[index_of(row, col)];
}

template <typename T>
T& TArray2D<T>::operator()(COORD2 const & p) throw (std::bad_alloc)
{
	if (!m_pBuffer)
#ifdef _WIN32    
		//    throw std::exception("Invalid array access: memory not allocated");
		throw std::bad_array_new_length();
#else    
		throw std::bad_alloc();
#endif    

	return m_pBuffer[index_of(p.row, p.col)];
}

template <typename T>
T  TArray2D<T>::operator()(COORD2 const & p) const throw (std::bad_alloc)
{
	if (!m_pBuffer)
#ifdef _WIN32
		//    throw std::exception("Invalid array access: memory not allocated");
		throw std::bad_array_new_length();
#else
		throw std::bad_alloc();
#endif

	return m_pBuffer[index_of(p.row, p.col)];
}

template <typename T>
T* TArray2D<T>::buffer()
{
	return m_pBuffer;
}

template <typename T>
void TArray2D<T>::clear()
{
	memset(m_pBuffer, 0, m_uRows*m_uCols * sizeof(T));
}

template <typename T>
size_t const TArray2D<T>::length() const
{
	return m_uRows*m_uCols;
}

template <typename T>
typename TArray2D<T>::iterator TArray2D<T>::begin()
{
	return m_pBuffer;
}

template <typename T>
typename TArray2D<T>::const_iterator TArray2D<T>::begin() const
{
	return m_pBuffer;
}

template <typename T>
typename TArray2D<T>::iterator TArray2D<T>::end()
{
	return m_pBuffer + length();
}

template <typename T>
typename TArray2D<T>::const_iterator TArray2D<T>::end() const
{
	return m_pBuffer + length();
}

#endif

// ************************************************************************** //
