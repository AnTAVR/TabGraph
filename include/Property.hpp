
#pragma once

#include <memory>

namespace TabGraph
{
	/*
	** Par ce que les templates c'est la Vie !
	** https://stackoverflow.com/questions/8368512/does-c11-have-c-style-properties
	*/

	template <typename T>
	class Property
	{
	public:
		virtual ~Property() {}
		virtual T& operator= (const T& f) { return value = f; }
		virtual T const & operator() (void) const { return value; }

	protected:
		T value;
	};

	/*template <typename Ptr>
	class HeapProperty
	{
	public:
		HeapProperty() : value(nullptr) {}
		virtual ~HeapProperty() { if (value) delete value; }
		virtual Ptr* const operator= (Ptr* const f) { return value = f; }
		virtual Ptr* const operator() (void) const { return value; }

	protected:
		Ptr* value;
	};*/

	/*
	** Les smart pointers aussi c'est la Vie !
	*/
	template <typename T>
	class HeapProperty
	{
	public:
		virtual ~HeapProperty() {}
		virtual std::unique_ptr<T>& operator= (T* const f)
		{
			value.reset(f);
			return value;
		}
		virtual T* const operator() (void) const
		{
			return value.get();
		}

	protected:
		std::unique_ptr<T> value;
	};
}
