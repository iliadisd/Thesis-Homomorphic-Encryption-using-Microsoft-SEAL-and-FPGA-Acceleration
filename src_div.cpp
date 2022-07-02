#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <array>
#include <cstddef>
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>
#include <memory>
#include <sstream>
#include <cstring>
#include <functional>

#define times 393485

// We expect 128-bit input
constexpr size_t uint64_count = 2;
constexpr int bits_per_uint64 = 64;
const std::size_t bits_per_uint64_sz = static_cast<std::size_t>(bits_per_uint64);
// Determine significant bits in numerator and denominator.
static const unsigned long deBruijnTable64[64] = { 63, 0,  58, 1,  59, 47, 53, 2,  60, 39, 48, 27, 54,
														33, 42, 3,  61, 51, 37, 40, 49, 18, 28, 20, 55, 30,
														34, 11, 43, 14, 22, 4,  62, 57, 46, 52, 38, 26, 32,
														41, 50, 36, 17, 19, 29, 10, 13, 21, 56, 45, 25, 31,
														35, 16, 9,  12, 44, 24, 15, 8,  23, 7,  6,  5 };

int ptr_bits(uint64_t *ptr){
	size_t c = uint64_count;
	ptr += c - 1;

	if (*ptr == 0 && c > 1){
		c--;
		ptr--;
	}

	int bit_count = 0;
	uint64_t V = *ptr;
	if (V == 0)
	{
		bit_count = static_cast<int>(c - 1) * bits_per_uint64;
	}
	else
	{
		unsigned long r = 0;
		unsigned long *result = &r;
		unsigned long *result_bits = result;
		V |= V >> 1;
		V |= V >> 2;
		V |= V >> 4;
		V |= V >> 8;
		V |= V >> 16;
		V |= V >> 32;

		*result_bits = deBruijnTable64[((V - (V >> 1)) * uint64_t(0x07EDD5E59A4E28C2)) >> 58];

		bit_count = static_cast<int>(c - 1) * bits_per_uint64 + static_cast<int>(r + 1);
	}
	return bit_count;
}

int arr_bits(uint64_t arr){
	int bit_count = 0;
	//Don't mess with the denominator' value in memory.
	if (arr != 0)
	{
		size_t c = uint64_count;

		unsigned long r = 0;
		unsigned long *result = &r;
		arr |= arr >> 1;
		arr |= arr >> 2;
		arr |= arr >> 4;
		arr |= arr >> 8;
		arr |= arr >> 16;
		arr |= arr >> 32;

		*result = deBruijnTable64[((arr - (arr >> 1)) * uint64_t(0x07EDD5E59A4E28C2)) >> 58];

		//return static_cast<int>(result + 1);

		bit_count = static_cast<int>(r + 1);
	}
	return bit_count;
}

uint64_t cor_ret(uint64_t *numerator, uint64_t *quotient, int numerator_bits, int denominator_shift){
	// Correct numerator (which is also the remainder) for shifting of
	// denominator, unless it is just zero.

	if (numerator_bits > 0)
	{
		const std::size_t shift_amount_sz3 = static_cast<std::size_t>(denominator_shift);

		if (shift_amount_sz3 & bits_per_uint64_sz)
		{
			numerator[0] = numerator[1];
			numerator[1] = 0;
		}
		// How many bits to shift in addition to word shift
		std::size_t bit_shift_amount = shift_amount_sz3 & (bits_per_uint64_sz - 1);

		if (bit_shift_amount)
		{
			std::size_t neg_bit_shift_amount = bits_per_uint64_sz - bit_shift_amount;

			// Warning: if bit_shift_amount == 0 this is incorrect
			numerator[0] = (numerator[0] >> bit_shift_amount) | (numerator[1] << neg_bit_shift_amount);
			numerator[1] = numerator[1] >> bit_shift_amount;
		}

	}
	return quotient[0];
}


void div(uint64_t num[times], uint64_t den[times], uint64_t quo[times])
{

	for (int i = 0; i < 393485; i++) {
#pragma HLS PIPELINE II=1
		//Initialize the variables.
		//uint64_t *numerator = (uint64_t *) malloc(sizeof(uint64_t) * 2);
		uint64_t _numerator[2];
		uint64_t *numerator = &_numerator[0];
		numerator[0] = 0;
		numerator[1] = num[i];
		uint64_t denominator = den[i];
		// Clear quotient. Set it to zero.
		//uint64_t *quotient = (uint64_t *) malloc(sizeof(uint64_t) * 2);
		uint64_t _quotient[2];
		uint64_t *quotient = &_quotient[0];
		quotient[0] = 0;
		quotient[1] = 0;
		int numerator_bits = ptr_bits(numerator);
		int denominator_bits = arr_bits(denominator);
		// If numerator has fewer bits than denominator, then done.
		if (!(numerator_bits < denominator_bits))
		{

			// Create temporary space to store mutable copy of denominator.
			uint64_t shifted_denominator[uint64_count]{ denominator, 0 };
			// Create temporary space to store difference calculation.
			uint64_t difference[uint64_count]{ 0, 0 };
			// Shift denominator to bring MSB in alignment with MSB of numerator.
			int denominator_shift = numerator_bits - denominator_bits;
			//left_shift_uint128(shifted_denominator, denominator_shift, shifted_denominator);
			const std::size_t bits_per_uint64_sz = static_cast<std::size_t>(bits_per_uint64);
			const std::size_t shift_amount_sz = static_cast<std::size_t>(denominator_shift);
			// Early return
			if (shift_amount_sz & bits_per_uint64_sz)
			{
				shifted_denominator[1] = shifted_denominator[0];
				shifted_denominator[0] = 0;
			}
			// How many bits to shift in addition to word shift
			std::size_t bit_shift_amount = shift_amount_sz & (bits_per_uint64_sz - 1);
			// Do we have a word shift
			if (bit_shift_amount)
			{
				std::size_t neg_bit_shift_amount = bits_per_uint64_sz - bit_shift_amount;

				// Warning: if bit_shift_amount == 0 this is incorrect
				shifted_denominator[1] = (shifted_denominator[1] << bit_shift_amount) | (shifted_denominator[0] >> neg_bit_shift_amount);
				shifted_denominator[0] = shifted_denominator[0] << bit_shift_amount;
			}
			denominator_bits += denominator_shift;
			// Perform bit-wise division algorithm.
			int remaining_shifts = denominator_shift;


			for (int i=0; i<50; i++){
				if(numerator_bits==denominator_bits)
				{
					const uint64_t *op1 = numerator;
					const uint64_t *op2 = shifted_denominator;
					size_t c3 = uint64_count;
					uint64_t *res = difference;
					uint64_t op1a = *op1++;
					uint64_t op2a = *op2++;
					uint64_t *resa = res++;
					*resa = op1a - op2a;
					unsigned char borrow = static_cast<unsigned char>(op2a > op1a);
					// Do the rest
					c3--;
					if (c3!=0)
					{
						unsigned long long tmp;
						unsigned long long *restmp = &tmp;
						unsigned long long *restmpb = restmp;
						unsigned long op1b = *op1;
						unsigned long op2b = *op2;
						auto diff = op1b - op2b;
						*restmpb = diff - (borrow != 0);
						unsigned char btemp = (diff > op1b) || (diff < borrow);
						borrow = btemp;
						*res = tmp;
						op1++;
						op2++;
						res++;
					}
					if (remaining_shifts != 0)
					{
						// No shifts remain and numerator < denominator so done.

						if (borrow){
							// numerator < shifted_denominator and MSBs are aligned,
							// so current quotient bit is zero and next one is definitely one.

							// Effectively shift numerator left by 1 by instead adding
							// numerator to difference (to prevent overflow in numerator).
							//add_uint(difference, numerator, uint64_count, difference); returns carry.
							const uint64_t *oper1 = difference;
							const uint64_t *oper2 = numerator;
							size_t c4 = uint64_count;
							uint64_t *re = difference;
							uint64_t oper1a = *oper1++;
							uint64_t oper2a = *oper2++;
							uint64_t *rea = re++;

							*rea = oper1a + oper2a;
							unsigned char carry =  static_cast<unsigned char>(*rea < oper1a);
							// Do the rest
							c4--;
							if (c4!=0)
							{
								unsigned long long temp;
								unsigned long long *retemp = &temp;
								unsigned long long *retempb = retemp;
								unsigned long oper1b = *oper1;
								unsigned long oper2b = *oper2;

								oper1b += oper2b;
								*retempb = oper1b + carry;
								unsigned char ctemp = (oper1b < oper2b) || (~oper1b < carry);
								carry = ctemp;
								*re = temp;
								oper1++;
								oper2++;
								re++;
							}


							// Adjust quotient and remaining shifts as a result of shifting numerator.
							quotient[1] = (quotient[1] << 1) | (quotient[0] >> (bits_per_uint64 - 1));
							quotient[0] <<= 1;
							remaining_shifts--;
						}
						// Difference is the new numerator with denominator subtracted.
						// Determine amount to shift numerator to bring MSB in alignment
						// with denominator.
						numerator_bits = ptr_bits(difference);

						// Clip the maximum shift to determine only the integer
						// (as opposed to fractional) bits.
						int numerator_shift = 0;
						if (denominator_bits - numerator_bits > remaining_shifts)
						{
							numerator_shift = remaining_shifts;
						}
						else
						{
							numerator_shift = denominator_bits - numerator_bits;
						}
						// Shift and update numerator.
						// This may be faster; first set to zero and then update if needed
						// Difference is zero so no need to shift, just set to zero.
						numerator[0] = 0;
						numerator[1] = 0;
						const std::size_t shift_amount_sz2 = static_cast<std::size_t>(numerator_shift);
						if (numerator_bits > 0)
						{
							// Early return
							if (shift_amount_sz2 & bits_per_uint64_sz)
							{
								numerator[1] = difference[0];
								numerator[0] = 0;
							}
							else
							{
								numerator[1] = difference[1];
								numerator[0] = difference[0];
							}

							// How many bits to shift in addition to word shift
							std::size_t bit_shift_amount = shift_amount_sz2 & (bits_per_uint64_sz - 1);

							// Do we have a word shift
							if (bit_shift_amount)
							{
								std::size_t neg_bit_shift_amount = bits_per_uint64_sz - bit_shift_amount;

								// Warning: if bit_shift_amount == 0 this is incorrect
								numerator[1] = (numerator[1] << bit_shift_amount) | (numerator[0] >> neg_bit_shift_amount);
								numerator[0] = numerator[0] << bit_shift_amount;
							}
							numerator_bits += numerator_shift;
						}
						// Update quotient to reflect subtraction.
						quotient[0] |= 1;
						// Adjust quotient and remaining shifts as a result of shifting numerator.
						if (shift_amount_sz2 & bits_per_uint64_sz)
							{
								quotient[1] = quotient[0];
								quotient[0] = 0;
							}
							// How many bits to shift in addition to word shift
							std::size_t bit_shift_amount = shift_amount_sz2 & (bits_per_uint64_sz - 1);

							// Do we have a word shift
							if (bit_shift_amount)
							{
								std::size_t neg_bit_shift_amount = bits_per_uint64_sz - bit_shift_amount;

								// Warning: if bit_shift_amount == 0 this is incorrect
								quotient[1] = (quotient[1] << bit_shift_amount) | (quotient[0] >> neg_bit_shift_amount);
								quotient[0] = quotient[0] << bit_shift_amount;
							}
						remaining_shifts -= numerator_shift;
					}
				}
			}
			quo[i] = cor_ret(numerator, quotient, numerator_bits, denominator_shift);
		}
	}
}
