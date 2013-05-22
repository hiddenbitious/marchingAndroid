#ifndef _C_ARRAY_H_
#define _C_ARRAY_H_

#include <cstddef>
#include <assert.h>
#include <cstring>

template<class T> class C_Array {
public:
	size_t length;
	size_t count;
	size_t element;

	C_Array();
	C_Array(int s);
	void push_back(const T & val);
	T pop_back();
	T& operator[](int index);
	void compactify();
	unsigned int size();
	void erase(int index);

private:
	T* data;
};

template <class T>
C_Array<T>::C_Array()
: count(0), length(4), element(sizeof(T))
{
	data = new T[length];
}

template <class T>
C_Array<T>::C_Array(int s)
: count(0), element(sizeof(T))
{
	length = s ? s : 4;
	data = new T[length];
	assert(data);
}

template <class T>
void C_Array<T>::push_back(const T & val)
{
	assert(element);
	assert(length);

	/// Vector is full. Double the space.
	if(count == length) {
		T *new_data = new T[2 * length];
		assert(new_data);
		memcpy((void *)new_data, (void *)data, length * element);
		length *= 2;
		delete[] data;
		data = new_data;
	}

	data[count++] = val;
}

template <class T>
T C_Array<T>::pop_back()
{
	assert(element);
	assert(data);

	if(count > 0) {
		--count;
		return data[count + 1];
	}
}

template <class T>
T& C_Array<T>::operator[](int index)
{
	assert(element);
	assert(data);
	assert(index <= count);

	return data[index];
};

template <class T>
void C_Array<T>::compactify()
{
	assert(element);
	assert(length);
	assert(data);
	assert(count <= length);

	if(length == count) {
		return;
	}

	/// Dont let it be zero
	length = count ? count : 4;
	T *new_data = new T[length];
	assert(new_data);
	memcpy((void *)new_data, (void *)data, length * element);
	delete[] data;
	data = new_data;
};

template <class T>
unsigned int C_Array<T>::size()
{
	return count;
};

template <class T>
void C_Array<T>::erase(int index)
{
	assert(element);
	assert(data);

	if(!count || index < 0 || index >= count) {
		return;
	}

	/// If it is the last object
	if(index == count - 1) {
		--count;
		return;
	}

	/// If it is the first object
	if(!index && count > 1) {
		--count;
		data += 1;
		return;
	}

	/// Else we have to do the memmove...
	/// Find the smallest memory transfer
	if(index < count / 2) {
		memmove(data + 1, data, index * element);
		data += 1;
	} else {
		memmove(&data[index], &data[index + 1], (count - index - 1) * element);
	}

	--count;
}
#endif
