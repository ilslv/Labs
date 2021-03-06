#pragma once
#include <iostream>

namespace is
{
	template<typename... T>
	void strcat_m(char* output, const rsize_t size_in_bytes, T... strings)
	{
		(strcat_s(output, size_in_bytes, strings), ...);
	}

	void dtoa(char* output, const rsize_t size_in_bytes, const double value, const unsigned precision)
	{
		const auto a = int(value);
		char i[10], f[10];
		_itoa_s(a, i, 10);
		_itoa_s(int(abs(double(value - a)) * pow(10, precision)), f, 10);
		strcpy_s(output, size_in_bytes, i);
		strcat_m(output, size_in_bytes, ".", f);
	}

	template<typename T>
	class array
	{
		T* array_ = nullptr;
		size_t size_ = 0, filled_ = 0;

		template<typename K>
		static int comparator(const void* left, const void* right)
		{
			if (*static_cast<const K*>(left) > *static_cast<const T*>(right)) { return 1; }
			if (*static_cast<const K*>(left) == *static_cast<const T*>(right)) { return 0; }
			return  -1;
		}

		static int comparator_reverse(const void* left, const void* right)
		{
			return comparator<T>(right, left);
		}
	public:
		array() = default;

		explicit array(const unsigned n)
		{
			size_ = n;
			array_ = new T[size_];
		}

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

		virtual ~array()
		{
			delete[] array_;
		}

		int size() const
		{
			return filled_;
		}

		T& operator[](const unsigned id) const
		{
			if(id < filled_)
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
				for (unsigned i = 0; i < filled_; ++i)
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
			qsort(array_, filled_, sizeof(array_[0]), comparator<T>);
		}

		void sort_reverse()
		{
			qsort(array_, filled_, sizeof(array_[0]), comparator_reverse);
		}

		template<typename K>
		T& find(const K& key) const
		{
			auto result = static_cast<T*>(bsearch(&key, array_, filled_, sizeof(array_[0]), comparator<K>));
			if(result != nullptr)
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