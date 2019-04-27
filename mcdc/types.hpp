#pragma once
#ifndef TYPES_HPP
#define TYPES_HPP

//
// General Type Definitions
//

#include <vector>
#include <cstdint>
#include <set>
#include <array>
#include <type_traits>


// MISRA: Never use build in types

using sint = int;
using uint = unsigned int;
using cchar = char;
using uchar = unsigned char;
using ulong = unsigned long;
using ull = unsigned long long;
using sll = signed long long;

using uint32 = uint_fast32_t;


// In contrast to people loving nullptr and auto. These are not typesafe at all. They fit to everything. This we do not want to have
// So this is a typesafe null for everything
template <typename T>
constexpr T null(void)
{
	return static_cast<T>(0);
}

// Narrow Cast From integer type to integer type for casts, where we know what we do
template <typename to, typename from>
constexpr typename std::enable_if<std::is_integral<from>::value && std::is_integral<to>::value, to>::type
narrow_cast(const from& value)
{
	return static_cast<to>(value & (static_cast<typename std::make_unsigned<from>::type>(-1)));
}


using NumberOfBitsCountType = uint;


// Depending on the maximum number of boolean variables we want to allow in our boolean
// expressions, we define types that fit and have a minimum width

#define MAX_NUMBER_OF_BOOLEAN_VARIABLES 16U

#if (MAX_NUMBER_OF_BOOLEAN_VARIABLES < 9U)

	constexpr sint MaxNumberOfBitsForEvaluation{ 8 };
	using MinTermNumber = uint_fast8_t;
	using MinTermType = uint_fast8_t;

	using MaxTermNumber = uint_fast8_t;
	using MaxTermType = uint_fast8_t;

	using BooleanVariable = uint_fast8_t;

	// Number of set bits in an unsigned integer type
	constexpr NumberOfBitsCountType numberOfSetBits(MinTermType x)
	{
		x = x - ((x >> 1) & 0x55);
		x = (x & 0x33) + ((x >> 2) & 0x33);
		x = (x + (x >> 4)) & 0x0F;
		return static_cast<NumberOfBitsCountType>(x & 0x3F);
}


#elif (MAX_NUMBER_OF_BOOLEAN_VARIABLES < 17U)

	constexpr sint MaxNumberOfBitsForEvaluation{ 16 };

	using MinTermNumber = uint_fast16_t;
	using MinTermType = uint_fast16_t;

	using MaxTermNumber = uint_fast16_t;
	using MaxTermType = uint_fast16_t;

	using BooleanVariable = uint_fast16_t;
	
	// Number of set bits in an unsigned integer type
	constexpr NumberOfBitsCountType numberOfSetBits(MinTermType x)
	{
		x = x - ((x >> 1) & 0x5555);
		x = (x & 0x3333) + ((x >> 2) & 0x3333);
		x = (x + (x >> 4)) & 0x0F0F;
		x = x + (x >> 8);
		return narrow_cast<NumberOfBitsCountType>(x & 0x003F);
	}


#elif (MAX_NUMBER_OF_BOOLEAN_VARIABLES < 33U)

	constexpr sint MaxNumberOfBitsForEvaluation{ 32 };

	using MinTermNumber = uint_fast32_t;
	using MinTermType = uint_fast32_t;

	using MaxTermNumber = uint_fast32_t;
	using MaxTermType = uint_fast32_t;

	using BooleanVariable = uint_fast32_t;

	// Number of set bits in an unsigned integer type
	constexpr NumberOfBitsCountType numberOfSetBits(MinTermType x)
	{
		x = x - ((x >> 1) & 0x55);
		x = (x & 0x33) + ((x >> 2) & 0x33);
		x = (x + (x >> 4)) & 0x0F;
		return static_cast<NumberOfBitsCountType>(x & 0x3F);
}

#else
	#error Too many Variables
#endif




using RowIndex = uint;
using ColumnIndex = uint;

using MintermSet = std::set<MinTermNumber>;
using Minterm = std::set< BooleanVariable>;
using MintermVector = std::vector<MinTermNumber>;

using MaxtermSet = std::set<MaxTermNumber>;
using ConjunctiveNormalForm = std::set<MaxtermSet>;

using ProductTerm = std::set<BooleanVariable>;
using ProductTermVector = std::vector<ProductTerm>;

// Disjunctive Normal Form
using DNF = std::set<ProductTerm>;
// Conjunctive Normal Form
using CNF = std::vector<DNF>;




constexpr uint numberOfThreads{ 8U };


// There are max 26 charachters in the latin alphabeth. So, 32 bits is always sufficient
constexpr sint MaxNumberOfBitmasksForMaskingBooleanInput{ 32 };

constexpr uint32 GetBitMask(uint bitPosition) { return (1U << bitPosition); }
constexpr std::array<uint32, MaxNumberOfBitmasksForMaskingBooleanInput> bitMask =
{
	GetBitMask(0U),GetBitMask(1U),GetBitMask(2u),GetBitMask(3U),GetBitMask(4u),GetBitMask(5U),GetBitMask(6u),GetBitMask(7U),
	GetBitMask(8U),GetBitMask(9U),GetBitMask(10u),GetBitMask(11u),GetBitMask(12u),GetBitMask(13u),GetBitMask(14u),GetBitMask(15u),
	GetBitMask(16U),GetBitMask(17U),GetBitMask(18u),GetBitMask(19U),GetBitMask(20U),GetBitMask(21u),GetBitMask(22u),GetBitMask(23u),
	GetBitMask(24U),GetBitMask(25U),GetBitMask(26u),GetBitMask(27U),GetBitMask(28u),GetBitMask(29u),GetBitMask(30u),GetBitMask(31u)
};

#endif
