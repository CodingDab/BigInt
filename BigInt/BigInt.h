#ifndef BIGINT_H
#define BIGINT_H

#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <random>
#include <cmath>

class BigInt;

std::ostream& operator<<(std::ostream&, BigInt);
void print(BigInt);

class BigInt
{
public:
	BigInt() = default;
	BigInt(const BigInt& bigint) : num(bigint.num), sign(bigint.sign) {}
	BigInt(BigInt&& bigint) : num(bigint.num), sign(bigint.sign) {} // nothing special but somone may wanna try to use move semantics
	BigInt(unsigned int digit_count, bool sign = false) : num(digit_count), sign(sign)
	{
		for(unsigned int i = 0; i < num.size(); i--)
		{
			num.at(i) = 0;
		}
	}
	BigInt(const char* x) : BigInt(std::string(x)) {}
	BigInt(const std::string& bignum)
	{
		std::vector<unsigned char> arr_digits;
		for(auto i = bignum.begin(); i < bignum.end(); i++)
		{
			if(i == bignum.begin())
			{
				sign = (*i == '-'); // if first element is '-' then sign is true
				if(!sign) arr_digits.push_back(char_to_8bit_int(*i));
			}
			else arr_digits.push_back(char_to_8bit_int(*i));
		}
		*this = BigInt(arr_digits, sign);
		this->num.shrink_to_fit();
	}
	const BigInt& operator=(const BigInt& bigint)
	{
		this->num = bigint.num;
		this->sign = bigint.sign;
		return *this;
	}
	const BigInt& operator=(const char* intString)
	{
		*this = BigInt(intString);
		return *this;
	}
	~BigInt() = default;
	static void Loop(const BigInt& xd) // delete this after testing
	{
		for(auto i : xd.num)
		{
			std::cout << "Element: " << static_cast<int>(i) << std::endl;
		}
		std::cout << std::endl << std::endl;
	}
	static BigInt GenerateRandom(int size)
	{
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> dist(48, 57);
		BigInt temp(size);

		for(int i = 0; i < size; i++)
		{
			temp.num.at(i) = char_to_8bit_int(static_cast<char>(dist(rng)));
		}
		temp.num.shrink_to_fit();
		return temp;
	}
	inline unsigned int NumberOfDigits()
	{
		return num.size();
	}
	inline unsigned char DigitAtPlace(int index) // allows loop operations on the BigInt from outside the class
	{											 // e.g. operator<< uses it.
		return num.at(index);
	}
	inline bool Sign()
	{
		return sign;
	}
	inline bool operator==(const BigInt& rhs) const
	{
		if(num.size() != rhs.num.size()) return false;
		else
		{
			for(unsigned int i = 0; i < num.size(); i++)
			{
				if(num.at(i) != rhs.num.at(i)) return false;
			}
			return true;
		}
	}
	inline bool operator!=(const BigInt& rhs) const
	{
		return !(*this == rhs);
	}
	inline bool operator<(const BigInt& rhs) const
	{
		if(num.size() < rhs.num.size()) return true;
		else if(num.size() > rhs.num.size()) return false;
		else
		{

			for(unsigned int i = num.size() - 1; i >= 0 && i < num.size(); i--)
			{
				if(num.at(i) < rhs.num.at(i)) return true;
				else if(num.at(i) > rhs.num.at(i)) return false;
			}

			return true; // again eclipse acts like it's possible to not return by this point. smh
		}
	}
	inline bool operator>(const BigInt& rhs) const
	{
		if(num.size() > rhs.num.size()) return true;
		else if(num.size() < rhs.num.size()) return false;
		else
		{
			for(unsigned int i = num.size() - 1; i >= 0; i--)
			{
				if(num.at(i) > rhs.num.at(i)) return true;
				else if(num.at(i) < rhs.num.at(i)) return false;
				else // num.at(i) == rhs.num.at(i)
					continue;
			}
			return true; // these extra returns are really a waste....
		}
	}
	inline bool operator<=(const BigInt& rhs) const
	{
		return *this < rhs + "1";
	}
	inline bool operator>=(const BigInt& rhs) const
	{
		return *this > rhs - "1";
	}
	inline BigInt operator*(const BigInt& rhs) const
	{
		// TODO: make it work for negative inputs too.
		if(rhs == "0" || *this == "0") return "0";
		else
		{
			BigInt temp("0");
			for(BigInt i = "1"; i < rhs; i = i + "1")
			{
				temp += *this;
			}
			return temp;
		}
	}
	inline BigInt operator+(const BigInt& rhs) const
	{
		const BigInt& x = *this;
		if(rhs.sign && x.sign) return -(-(x) + -rhs);
		else if(rhs.sign && !x.sign) return x - rhs;
		else if(!rhs.sign && x.sign) return rhs - x;
		else if(!rhs.sign && !x.sign) return add(x, rhs);
		// will never get returned, because all possible combinations are explored above
		// but eclipse thinks its SO smart -.-
		else return BigInt("0");
	}
	inline const BigInt& operator+=(const BigInt& rhs)
	{
		*this = *this + rhs;
		return *this;
	}
	inline BigInt operator-(const BigInt& rhs) const
	{
		// not as pretty as addition cuz addition is commutative
		if(rhs > *this) return -subtract(rhs, *this);
		else if(rhs.sign && !this->sign) return -(*this + rhs);
		else if(!rhs.sign && this->sign) return *this + rhs;
		else if(!rhs.sign && !this->sign) return subtract(*this, rhs);
		else if(rhs.sign && this->sign) return subtract(rhs, *this);
		// this code is unreachable (see add(const BigInt&, const BigInt&) for why that is if you care)
		else return BigInt("0");
	}
	inline const BigInt& operator-=(const BigInt& rhs)
	{
		*this = *this - rhs;
		return *this;
	}
	inline BigInt operator-() const // return opposite of *this
	{
		BigInt temp = *this;
		temp.sign = temp.sign ? false : true;
		return temp;
	}
private:
	static inline unsigned char char_to_8bit_int(char c)
	{
		// 48 is the start of the 0 - 9 char codes in ascii.
		return (c < 58 && c > 47) ? c - static_cast<unsigned char>(48) : static_cast<unsigned char>(10); // if the number is invalid, 10 is returned
	}
	inline BigInt add(const BigInt& lhs, const BigInt& rhs) const // expects 2 positive numbers
	{
		std::vector<unsigned char> temp_vec;
		int carry = 0;
		int flag = 0; // neutral state
		unsigned int smaller_size = 0;
		unsigned int larger_size = 0;
		if(lhs.num.size() < rhs.num.size())
		{
			smaller_size = lhs.num.size();
			larger_size = rhs.num.size();
			flag--; // the left hand side is the smaller int
		}
		else
		{
			smaller_size = rhs.num.size();
			larger_size = lhs.num.size();
			flag++; // the right hand side is the smaller int or they are the same size
		}
		int dif = larger_size - smaller_size;
		for(int j = larger_size - 1, i = smaller_size - 1; j >= -1; j--, i--)
		{
			int larger;
			int smaller;
			switch(flag)
			{
			case 1:
				larger = (j >= 0) ? lhs.num.at(j) : 0;
				smaller = (i >= 0) ? rhs.num.at(j - dif) : 0; // if there are no more digits in the smaller number add 0
				break;
			case -1:
				// rhs is greater
				larger = (j >= 0) ? rhs.num.at(j) : 0;
				smaller = (i >= 0) ? lhs.num.at(j - dif) : 0; // if there are no more digits in the smaller number add 0
				break;
			case 0:
				throw "Addition failed.";
				break;
			}
			unsigned char sum = (unsigned char)(larger + smaller + carry);
			if(sum > 9)
			{
				sum -= 10;
				carry = 1;
			}
			else
			{
				carry = 0;
			}
			temp_vec.push_back(sum);
		}
		if(temp_vec.at(temp_vec.size() - 1) == (unsigned char)0) temp_vec.pop_back();
		BigInt temp(temp_vec.size());
		std::reverse_copy(temp_vec.begin(), temp_vec.end(), temp.num.begin());
		return temp;
	}
	inline BigInt subtract(const BigInt& lhs, const BigInt& rhs) const // expects 2 positive numbers
	{
		std::vector<unsigned char> temp_vec;
		char borrow = 0;
		int flag = 0; // neutral state
		int smaller_size = 0;
		int larger_size = 0;
		if(lhs.num.size() < rhs.num.size())
		{
			smaller_size = lhs.num.size();
			larger_size = rhs.num.size();
			flag--; // the left hand side is the smaller int
		}
		else
		{
			smaller_size = rhs.num.size();
			larger_size = lhs.num.size();
			flag++; // the right hand side is the smaller int or they are the same size
		}
		int dif = larger_size - smaller_size;
		for(int j = larger_size - 1, i = smaller_size - 1; j >= 0; j--, i--)
		{
			int larger;
			int smaller;
			switch(flag)
			{
			case 1:
				larger = lhs.num.at(j);
				smaller = (i >= 0) ? rhs.num.at(j - dif) : 0; // if there are no more digits in the smaller number add 0
				break;
			case -1:
				// rhs is greater
				larger = rhs.num.at(j);
				smaller = (i >= 0) ? lhs.num.at(j - dif) : 0; // if there are no more digits in the smaller number add 0
				break;
			case 0:
				throw "Addition failed.";
				break;
			}
			char sum = (larger - borrow) - smaller;
			if(sum < 0)
			{
				sum += 10;
				borrow = 1;
			}
			else
			{
				borrow = 0;
			}
			temp_vec.push_back(sum);
		}
		if(temp_vec.at(temp_vec.size() - 1) == (unsigned char)0) temp_vec.pop_back();
		BigInt temp(temp_vec.size());
		std::reverse_copy(temp_vec.begin(), temp_vec.end(), temp.num.begin());
		return temp;
	}
	BigInt(const std::vector<unsigned char>& digits, bool sign) : sign(sign)
	{
		for(auto i : digits)
		{
			if(i == 10)
			{
				throw "A non-numeric value was passed!";
			}
			else
			{
				continue;
			}
		}
		num = digits;
	}
public:
private:
	std::vector<unsigned char> num;
	bool sign = false;
};

std::ostream& operator<<(std::ostream& out, BigInt num)
{
	if(num.Sign()) out << '-';
	for(unsigned int i = 0; i < num.NumberOfDigits(); i++)
	{
		out << static_cast<int>(num.DigitAtPlace(i));
	}
	return out;
}

void print(BigInt b)
{
	std::cout << b << std::endl;
}
#endif
