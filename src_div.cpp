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
/*
#include <chrono>
using namespace std::chrono;
*/
void div(uint64_t *numerator, uint64_t denominator, uint64_t *quotient)
{
//static int times = 0;
//auto start = high_resolution_clock::now();


    // We expect 128-bit input
    constexpr size_t uint64_count = 2;
    constexpr int bits_per_uint64 = 64;

    // Clear quotient. Set it to zero.
    quotient[0] = 0;
    quotient[1] = 0;

    // Determine significant bits in numerator and denominator.
    static const unsigned long deBruijnTable64[64] = { 63, 0,  58, 1,  59, 47, 53, 2,  60, 39, 48, 27, 54,
                                                            33, 42, 3,  61, 51, 37, 40, 49, 18, 28, 20, 55, 30,
                                                            34, 11, 43, 14, 22, 4,  62, 57, 46, 52, 38, 26, 32,
                                                            41, 50, 36, 17, 19, 29, 10, 13, 21, 56, 45, 25, 31,
                                                            35, 16, 9,  12, 44, 24, 15, 8,  23, 7,  6,  5 };

    //int numerator_bits = get_significant_bit_count_uint(numerator, uint64_count);
    const uint64_t *v = numerator;
    size_t c1 = uint64_count;
    v += c1 - 1;

    for (; *v == 0 && c1 > 1; c1--)
    {
        v--;
    }

    //return static_cast<int>(uint64_count - 1) * bits_per_uint64 + get_significant_bit_count(num);
    int numerator_bits = 0;
    uint64_t V = *v;
    if (V == 0)
    {
        numerator_bits = static_cast<int>(c1 - 1) * bits_per_uint64;
    }
    else
    {
        unsigned long r1 = 0;
        unsigned long *result = &r1;
        unsigned long *result1 = result;
        //SEAL_MSB_INDEX_UINT64(&result, numerator); -> get_msb_index_generic(result, numerator);
        V |= V >> 1;
        V |= V >> 2;
        V |= V >> 4;
        V |= V >> 8;
        V |= V >> 16;
        V |= V >> 32;

        *result1 = deBruijnTable64[((V - (V >> 1)) * uint64_t(0x07EDD5E59A4E28C2)) >> 58];
        //return static_cast<int>(result + 1);

        numerator_bits = static_cast<int>(c1 - 1) * bits_per_uint64 + static_cast<int>(r1 + 1);
    }

    //int denominator_bits = get_significant_bit_count(denominator);
    //return static_cast<int>(uint64_count - 1) * bits_per_uint64 + get_significant_bit_count(*denominator);
    int denominator_bits = 0;
    //Don't mess with the denominator' value in memory.
    uint64_t d = denominator;
    if (d != 0)
    {
        size_t c2 = uint64_count;

        unsigned long r2 = 0;
        unsigned long *result2 = &r2;
        //SEAL_MSB_INDEX_UINT64(&result, numerator); -> get_msb_index_generic(result, numerator);
        d |= d >> 1;
        d |= d >> 2;
        d |= d >> 4;
        d |= d >> 8;
        d |= d >> 16;
        d |= d >> 32;

        *result2 = deBruijnTable64[((d - (d >> 1)) * uint64_t(0x07EDD5E59A4E28C2)) >> 58];

        //return static_cast<int>(result + 1);

        denominator_bits = static_cast<int>(r2 + 1);
    }

    // If numerator has fewer bits than denominator, then done.

    if (numerator_bits < denominator_bits)
    {

        return;
    }

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
    else
    {
        shifted_denominator[1] = shifted_denominator[1];
        shifted_denominator[0] = shifted_denominator[0];
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
    while (numerator_bits == denominator_bits)
    {
        // NOTE: MSBs of numerator and denominator are aligned.

        // Even though MSB of numerator and denominator are aligned,
        // still possible numerator < shifted_denominator.
        
        
        //if (sub_uint(numerator, shifted_denominator, uint64_count, difference))
        const uint64_t *op1 = numerator;
        const uint64_t *op2 = shifted_denominator;
        size_t c3 = uint64_count;
        uint64_t *res = difference;

        //unsigned char borrow = sub_uint64(*numerator++, *shifted_denominator++, difference++);
        uint64_t op1a = *op1++;
        uint64_t op2a = *op2++;
        uint64_t *resa = res++;
        *resa = op1a - op2a;
        unsigned char borrow = static_cast<unsigned char>(op2a > op1a);
        // Do the rest
        for (; --c3; op1++, op2++, res++)
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
        }

        if (borrow){

            // numerator < shifted_denominator and MSBs are aligned,
            // so current quotient bit is zero and next one is definitely one.
            if (remaining_shifts == 0)
            {
                // No shifts remain and numerator < denominator so done.
                break;
            }

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
            for (; --c4; oper1++, oper2++, re++)
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
            }

            // Adjust quotient and remaining shifts as a result of shifting numerator.
            quotient[1] = (quotient[1] << 1) | (quotient[0] >> (bits_per_uint64 - 1));
            quotient[0] <<= 1;
            remaining_shifts--;


        }



        // Difference is the new numerator with denominator subtracted.

        // Determine amount to shift numerator to bring MSB in alignment
        // with denominator.

        //numerator_bits = get_significant_bit_count_uint(difference, uint64_count);

        const uint64_t *value = difference;
        size_t c5 = uint64_count;
        value += c5 - 1;
        for (; *value == 0 && c5 > 1; c5--)
        {
            value--;

        }

        uint64_t val = *value;

        //return static_cast<int>(uint64_count - 1) * bits_per_uint64 + get_significant_bit_count(*difference);
        if (val == 0)
        {
            numerator_bits = static_cast<int>(c5 - 1) * bits_per_uint64;
        }
        else 
        {
            unsigned long r=0;
            unsigned long *rtemp = &r;

            //SEAL_MSB_INDEX_UINT64(&result, difference); -> get_msb_index_generic(result, difference);
            val |= val >> 1;
            val |= val>> 2;
            val |= val >> 4;
            val |= val >> 8;
            val |= val >> 16;
            val |= val >> 32;

            *rtemp = deBruijnTable64[((val - (val >> 1)) * std::uint64_t(0x07EDD5E59A4E28C2)) >> 58];

            //return static_cast<int>(result + 1);

            numerator_bits = static_cast<int>(c5 - 1) * bits_per_uint64 + static_cast<int>(r + 1);


        }


        // Clip the maximum shift to determine only the integer
        // (as opposed to fractional) bits.

        //int numerator_shift = min(denominator_bits - numerator_bits, remaining_shifts);
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
            //left_shift_uint128(difference, numerator_shift, numerator);
            //const std::size_t bits_per_uint64_sz = static_cast<std::size_t>(bits_per_uint64);
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
        //left_shift_uint128(quotient, numerator_shift, quotient);

        if (shift_amount_sz2 & bits_per_uint64_sz)
            {
                quotient[1] = quotient[0];
                quotient[0] = 0;
            }
            else
            {
                quotient[1] = quotient[1];
                quotient[0] = quotient[0];
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


    // Correct numerator (which is also the remainder) for shifting of
    // denominator, unless it is just zero.

    if (numerator_bits > 0)
    {
        //right_shift_uint128(numerator, denominator_shift, numerator);
        const std::size_t shift_amount_sz3 = static_cast<std::size_t>(denominator_shift);

        if (shift_amount_sz3 & bits_per_uint64_sz)
        {
            numerator[0] = numerator[1];
            numerator[1] = 0;
        }
        else
        {
            numerator[1] = numerator[1];
            numerator[0] = numerator[0];
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
/*
auto stop = high_resolution_clock::now();
auto duration = duration_cast<microseconds>(stop - start);
cout << endl;
cout << duration.count() << endl;
cout << endl;
//times++;
//cout<<times<<endl;
*/
}
