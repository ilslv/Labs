#pragma once
#include <iostream>

namespace is
{
	namespace
	{
		void strcat_m_internal(char*, int) {}

		template<typename First, typename... Param>
		void strcat_m_internal(char* output, const int buffer, First first, Param... rest)
		{
			strcat_s(output, buffer, first);
			strcat_m_internal(output, buffer, rest...);
		}
	}

	template<typename... Param>
	void strcat_m(char* output, const int buffer, Param... strings)
	{
		strcat_m_internal(output, buffer, strings...);
	}

	void dtoa(const double value, const int precision, char* output, const int buffer)
	{
		auto a = int(value);
		char i[10], f[10];
		_itoa_s(a, i, 10);
		_itoa_s(int(double(value - a) * pow(10, precision)), f, 10);
		strcpy_s(output, buffer, i);
		strcat_m(output, buffer, ".", f);
	}

	template<typename T>
	class array
	{
		T* array_ = nullptr;
		int size_ = 0, filled_ = 0;

		static int comparator(const void* left, const void* right)
		{
			if (*(T*)(left) > *(T*)(right)) { return 1; }
			if (*(T*)(left) == *(T*)(right)) { return 0; }
			return  -1;
		}

		static int comparator_reverse(const void* left, const void* right)
		{
			return comparator(right, left);
		}
	public:
		array() = default;

		array(const array& copy)
		{
			size_ = copy.size_;
			filled_ = copy.filled_;
			array_ = new T[copy.filled_];
			for (auto i = 0; i < filled_; ++i)
			{
				array_[i] = copy.array_[i];
			}
		}

		array(array&& move) noexcept
		{
			size_ = move.size_;
			filled_ = move.filled_;
			array_ = move.array_;
			move.array_ = nullptr;
		}

		explicit array(const int n)
		{
			filled_ = 0;
			size_ = n;
			array_ = new T[size_];
		}

		virtual ~array()
		{
			delete[] array_;
		}

		int get_size() const
		{
			return filled_;
		}

		T& operator[](const int id) const
		{
			if (id < filled_ && id >= 0)
			{
				return array_[id];
			}
			throw std::invalid_argument("No element at such index");
		}

		void push_back(const T value)
		{
			if (filled_ >= size_)
			{
				size_ = (size_ + 1) * 2;
				auto tmp = new T[size_];
				for (auto i = 0; i < filled_; ++i)
				{
					tmp[i] = array_[i];
				}
				array_ = tmp;
			}
			array_[filled_] = value;
			filled_++;
		}

		void sort()
		{
			qsort(array_, filled_, sizeof(array_[0]), comparator);
		}

		void sort_reverse()
		{
			qsort(array_, filled_, sizeof(array_[0]), comparator_reverse);
		}

		T& find(const T& key) const
		{
			auto result = static_cast<T*>(bsearch(&key, array_, filled_, sizeof(array_[0]), comparator));
			if (result != nullptr)
			{
				return *result;
			}
			throw std::invalid_argument("No element found!");
		}

		T* begin()
		{
			return array_;
		}

		T* end()
		{
			return array_ + filled_;
		}
	};
}