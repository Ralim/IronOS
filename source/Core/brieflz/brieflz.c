//
// BriefLZ - small fast Lempel-Ziv
//
// C packer
//
// Copyright (c) 2002-2020 Joergen Ibsen
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//   1. The origin of this software must not be misrepresented; you must
//      not claim that you wrote the original software. If you use this
//      software in a product, an acknowledgment in the product
//      documentation would be appreciated but is not required.
//
//   2. Altered source versions must be plainly marked as such, and must
//      not be misrepresented as being the original software.
//
//   3. This notice may not be removed or altered from any source
//      distribution.
//

#include "brieflz.h"

#include <assert.h>
#include <limits.h>
#include <stdint.h>

#if _MSC_VER >= 1400
#  include <intrin.h>
#  define BLZ_BUILTIN_MSVC
#elif defined(__clang__) && defined(__has_builtin)
#  if __has_builtin(__builtin_clz)
#    define BLZ_BUILTIN_GCC
#  endif
#elif __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#  define BLZ_BUILTIN_GCC
#endif

// Type used to store values in workmem.
//
// This is used to store positions and lengths, so src_size has to be within
// the range of this type.
//
typedef uint32_t blz_word;

#define BLZ_WORD_MAX UINT32_MAX

// Number of bits of hash to use for lookup.
//
// The size of the lookup table (and thus workmem) depends on this.
//
// Values between 10 and 18 work well. Lower values generally make compression
// speed faster but ratio worse. The default value 17 (128k entries) is a
// compromise.
//
#ifndef BLZ_HASH_BITS
#  define BLZ_HASH_BITS 17
#endif

#define LOOKUP_SIZE (1UL << BLZ_HASH_BITS)

#define NO_MATCH_POS ((blz_word) -1)

// Internal data structure
struct blz_state {
	unsigned char *next_out;
	unsigned char *tag_out;
	unsigned int tag;
	int bits_left;
};

#if !defined(BLZ_NO_LUT)
static const unsigned short blz_gamma_lookup[512][2] = {
	{0, 0},
	{0, 0},

	{0x00, 2}, {0x02, 2},

	{0x04, 4}, {0x06, 4}, {0x0C, 4}, {0x0E, 4},

	{0x14, 6}, {0x16, 6}, {0x1C, 6}, {0x1E, 6},
	{0x34, 6}, {0x36, 6}, {0x3C, 6}, {0x3E, 6},

	{0x54, 8}, {0x56, 8}, {0x5C, 8}, {0x5E, 8},
	{0x74, 8}, {0x76, 8}, {0x7C, 8}, {0x7E, 8},
	{0xD4, 8}, {0xD6, 8}, {0xDC, 8}, {0xDE, 8},
	{0xF4, 8}, {0xF6, 8}, {0xFC, 8}, {0xFE, 8},

	{0x154, 10}, {0x156, 10}, {0x15C, 10}, {0x15E, 10},
	{0x174, 10}, {0x176, 10}, {0x17C, 10}, {0x17E, 10},
	{0x1D4, 10}, {0x1D6, 10}, {0x1DC, 10}, {0x1DE, 10},
	{0x1F4, 10}, {0x1F6, 10}, {0x1FC, 10}, {0x1FE, 10},
	{0x354, 10}, {0x356, 10}, {0x35C, 10}, {0x35E, 10},
	{0x374, 10}, {0x376, 10}, {0x37C, 10}, {0x37E, 10},
	{0x3D4, 10}, {0x3D6, 10}, {0x3DC, 10}, {0x3DE, 10},
	{0x3F4, 10}, {0x3F6, 10}, {0x3FC, 10}, {0x3FE, 10},

	{0x554, 12}, {0x556, 12}, {0x55C, 12}, {0x55E, 12},
	{0x574, 12}, {0x576, 12}, {0x57C, 12}, {0x57E, 12},
	{0x5D4, 12}, {0x5D6, 12}, {0x5DC, 12}, {0x5DE, 12},
	{0x5F4, 12}, {0x5F6, 12}, {0x5FC, 12}, {0x5FE, 12},
	{0x754, 12}, {0x756, 12}, {0x75C, 12}, {0x75E, 12},
	{0x774, 12}, {0x776, 12}, {0x77C, 12}, {0x77E, 12},
	{0x7D4, 12}, {0x7D6, 12}, {0x7DC, 12}, {0x7DE, 12},
	{0x7F4, 12}, {0x7F6, 12}, {0x7FC, 12}, {0x7FE, 12},
	{0xD54, 12}, {0xD56, 12}, {0xD5C, 12}, {0xD5E, 12},
	{0xD74, 12}, {0xD76, 12}, {0xD7C, 12}, {0xD7E, 12},
	{0xDD4, 12}, {0xDD6, 12}, {0xDDC, 12}, {0xDDE, 12},
	{0xDF4, 12}, {0xDF6, 12}, {0xDFC, 12}, {0xDFE, 12},
	{0xF54, 12}, {0xF56, 12}, {0xF5C, 12}, {0xF5E, 12},
	{0xF74, 12}, {0xF76, 12}, {0xF7C, 12}, {0xF7E, 12},
	{0xFD4, 12}, {0xFD6, 12}, {0xFDC, 12}, {0xFDE, 12},
	{0xFF4, 12}, {0xFF6, 12}, {0xFFC, 12}, {0xFFE, 12},

	{0x1554, 14}, {0x1556, 14}, {0x155C, 14}, {0x155E, 14},
	{0x1574, 14}, {0x1576, 14}, {0x157C, 14}, {0x157E, 14},
	{0x15D4, 14}, {0x15D6, 14}, {0x15DC, 14}, {0x15DE, 14},
	{0x15F4, 14}, {0x15F6, 14}, {0x15FC, 14}, {0x15FE, 14},
	{0x1754, 14}, {0x1756, 14}, {0x175C, 14}, {0x175E, 14},
	{0x1774, 14}, {0x1776, 14}, {0x177C, 14}, {0x177E, 14},
	{0x17D4, 14}, {0x17D6, 14}, {0x17DC, 14}, {0x17DE, 14},
	{0x17F4, 14}, {0x17F6, 14}, {0x17FC, 14}, {0x17FE, 14},
	{0x1D54, 14}, {0x1D56, 14}, {0x1D5C, 14}, {0x1D5E, 14},
	{0x1D74, 14}, {0x1D76, 14}, {0x1D7C, 14}, {0x1D7E, 14},
	{0x1DD4, 14}, {0x1DD6, 14}, {0x1DDC, 14}, {0x1DDE, 14},
	{0x1DF4, 14}, {0x1DF6, 14}, {0x1DFC, 14}, {0x1DFE, 14},
	{0x1F54, 14}, {0x1F56, 14}, {0x1F5C, 14}, {0x1F5E, 14},
	{0x1F74, 14}, {0x1F76, 14}, {0x1F7C, 14}, {0x1F7E, 14},
	{0x1FD4, 14}, {0x1FD6, 14}, {0x1FDC, 14}, {0x1FDE, 14},
	{0x1FF4, 14}, {0x1FF6, 14}, {0x1FFC, 14}, {0x1FFE, 14},
	{0x3554, 14}, {0x3556, 14}, {0x355C, 14}, {0x355E, 14},
	{0x3574, 14}, {0x3576, 14}, {0x357C, 14}, {0x357E, 14},
	{0x35D4, 14}, {0x35D6, 14}, {0x35DC, 14}, {0x35DE, 14},
	{0x35F4, 14}, {0x35F6, 14}, {0x35FC, 14}, {0x35FE, 14},
	{0x3754, 14}, {0x3756, 14}, {0x375C, 14}, {0x375E, 14},
	{0x3774, 14}, {0x3776, 14}, {0x377C, 14}, {0x377E, 14},
	{0x37D4, 14}, {0x37D6, 14}, {0x37DC, 14}, {0x37DE, 14},
	{0x37F4, 14}, {0x37F6, 14}, {0x37FC, 14}, {0x37FE, 14},
	{0x3D54, 14}, {0x3D56, 14}, {0x3D5C, 14}, {0x3D5E, 14},
	{0x3D74, 14}, {0x3D76, 14}, {0x3D7C, 14}, {0x3D7E, 14},
	{0x3DD4, 14}, {0x3DD6, 14}, {0x3DDC, 14}, {0x3DDE, 14},
	{0x3DF4, 14}, {0x3DF6, 14}, {0x3DFC, 14}, {0x3DFE, 14},
	{0x3F54, 14}, {0x3F56, 14}, {0x3F5C, 14}, {0x3F5E, 14},
	{0x3F74, 14}, {0x3F76, 14}, {0x3F7C, 14}, {0x3F7E, 14},
	{0x3FD4, 14}, {0x3FD6, 14}, {0x3FDC, 14}, {0x3FDE, 14},
	{0x3FF4, 14}, {0x3FF6, 14}, {0x3FFC, 14}, {0x3FFE, 14},

	{0x5554, 16}, {0x5556, 16}, {0x555C, 16}, {0x555E, 16},
	{0x5574, 16}, {0x5576, 16}, {0x557C, 16}, {0x557E, 16},
	{0x55D4, 16}, {0x55D6, 16}, {0x55DC, 16}, {0x55DE, 16},
	{0x55F4, 16}, {0x55F6, 16}, {0x55FC, 16}, {0x55FE, 16},
	{0x5754, 16}, {0x5756, 16}, {0x575C, 16}, {0x575E, 16},
	{0x5774, 16}, {0x5776, 16}, {0x577C, 16}, {0x577E, 16},
	{0x57D4, 16}, {0x57D6, 16}, {0x57DC, 16}, {0x57DE, 16},
	{0x57F4, 16}, {0x57F6, 16}, {0x57FC, 16}, {0x57FE, 16},
	{0x5D54, 16}, {0x5D56, 16}, {0x5D5C, 16}, {0x5D5E, 16},
	{0x5D74, 16}, {0x5D76, 16}, {0x5D7C, 16}, {0x5D7E, 16},
	{0x5DD4, 16}, {0x5DD6, 16}, {0x5DDC, 16}, {0x5DDE, 16},
	{0x5DF4, 16}, {0x5DF6, 16}, {0x5DFC, 16}, {0x5DFE, 16},
	{0x5F54, 16}, {0x5F56, 16}, {0x5F5C, 16}, {0x5F5E, 16},
	{0x5F74, 16}, {0x5F76, 16}, {0x5F7C, 16}, {0x5F7E, 16},
	{0x5FD4, 16}, {0x5FD6, 16}, {0x5FDC, 16}, {0x5FDE, 16},
	{0x5FF4, 16}, {0x5FF6, 16}, {0x5FFC, 16}, {0x5FFE, 16},
	{0x7554, 16}, {0x7556, 16}, {0x755C, 16}, {0x755E, 16},
	{0x7574, 16}, {0x7576, 16}, {0x757C, 16}, {0x757E, 16},
	{0x75D4, 16}, {0x75D6, 16}, {0x75DC, 16}, {0x75DE, 16},
	{0x75F4, 16}, {0x75F6, 16}, {0x75FC, 16}, {0x75FE, 16},
	{0x7754, 16}, {0x7756, 16}, {0x775C, 16}, {0x775E, 16},
	{0x7774, 16}, {0x7776, 16}, {0x777C, 16}, {0x777E, 16},
	{0x77D4, 16}, {0x77D6, 16}, {0x77DC, 16}, {0x77DE, 16},
	{0x77F4, 16}, {0x77F6, 16}, {0x77FC, 16}, {0x77FE, 16},
	{0x7D54, 16}, {0x7D56, 16}, {0x7D5C, 16}, {0x7D5E, 16},
	{0x7D74, 16}, {0x7D76, 16}, {0x7D7C, 16}, {0x7D7E, 16},
	{0x7DD4, 16}, {0x7DD6, 16}, {0x7DDC, 16}, {0x7DDE, 16},
	{0x7DF4, 16}, {0x7DF6, 16}, {0x7DFC, 16}, {0x7DFE, 16},
	{0x7F54, 16}, {0x7F56, 16}, {0x7F5C, 16}, {0x7F5E, 16},
	{0x7F74, 16}, {0x7F76, 16}, {0x7F7C, 16}, {0x7F7E, 16},
	{0x7FD4, 16}, {0x7FD6, 16}, {0x7FDC, 16}, {0x7FDE, 16},
	{0x7FF4, 16}, {0x7FF6, 16}, {0x7FFC, 16}, {0x7FFE, 16},
	{0xD554, 16}, {0xD556, 16}, {0xD55C, 16}, {0xD55E, 16},
	{0xD574, 16}, {0xD576, 16}, {0xD57C, 16}, {0xD57E, 16},
	{0xD5D4, 16}, {0xD5D6, 16}, {0xD5DC, 16}, {0xD5DE, 16},
	{0xD5F4, 16}, {0xD5F6, 16}, {0xD5FC, 16}, {0xD5FE, 16},
	{0xD754, 16}, {0xD756, 16}, {0xD75C, 16}, {0xD75E, 16},
	{0xD774, 16}, {0xD776, 16}, {0xD77C, 16}, {0xD77E, 16},
	{0xD7D4, 16}, {0xD7D6, 16}, {0xD7DC, 16}, {0xD7DE, 16},
	{0xD7F4, 16}, {0xD7F6, 16}, {0xD7FC, 16}, {0xD7FE, 16},
	{0xDD54, 16}, {0xDD56, 16}, {0xDD5C, 16}, {0xDD5E, 16},
	{0xDD74, 16}, {0xDD76, 16}, {0xDD7C, 16}, {0xDD7E, 16},
	{0xDDD4, 16}, {0xDDD6, 16}, {0xDDDC, 16}, {0xDDDE, 16},
	{0xDDF4, 16}, {0xDDF6, 16}, {0xDDFC, 16}, {0xDDFE, 16},
	{0xDF54, 16}, {0xDF56, 16}, {0xDF5C, 16}, {0xDF5E, 16},
	{0xDF74, 16}, {0xDF76, 16}, {0xDF7C, 16}, {0xDF7E, 16},
	{0xDFD4, 16}, {0xDFD6, 16}, {0xDFDC, 16}, {0xDFDE, 16},
	{0xDFF4, 16}, {0xDFF6, 16}, {0xDFFC, 16}, {0xDFFE, 16},
	{0xF554, 16}, {0xF556, 16}, {0xF55C, 16}, {0xF55E, 16},
	{0xF574, 16}, {0xF576, 16}, {0xF57C, 16}, {0xF57E, 16},
	{0xF5D4, 16}, {0xF5D6, 16}, {0xF5DC, 16}, {0xF5DE, 16},
	{0xF5F4, 16}, {0xF5F6, 16}, {0xF5FC, 16}, {0xF5FE, 16},
	{0xF754, 16}, {0xF756, 16}, {0xF75C, 16}, {0xF75E, 16},
	{0xF774, 16}, {0xF776, 16}, {0xF77C, 16}, {0xF77E, 16},
	{0xF7D4, 16}, {0xF7D6, 16}, {0xF7DC, 16}, {0xF7DE, 16},
	{0xF7F4, 16}, {0xF7F6, 16}, {0xF7FC, 16}, {0xF7FE, 16},
	{0xFD54, 16}, {0xFD56, 16}, {0xFD5C, 16}, {0xFD5E, 16},
	{0xFD74, 16}, {0xFD76, 16}, {0xFD7C, 16}, {0xFD7E, 16},
	{0xFDD4, 16}, {0xFDD6, 16}, {0xFDDC, 16}, {0xFDDE, 16},
	{0xFDF4, 16}, {0xFDF6, 16}, {0xFDFC, 16}, {0xFDFE, 16},
	{0xFF54, 16}, {0xFF56, 16}, {0xFF5C, 16}, {0xFF5E, 16},
	{0xFF74, 16}, {0xFF76, 16}, {0xFF7C, 16}, {0xFF7E, 16},
	{0xFFD4, 16}, {0xFFD6, 16}, {0xFFDC, 16}, {0xFFDE, 16},
	{0xFFF4, 16}, {0xFFF6, 16}, {0xFFFC, 16}, {0xFFFE, 16}
};
#endif

static int
blz_log2(unsigned long n)
{
	assert(n > 0);

#if defined(BLZ_BUILTIN_MSVC)
	unsigned long msb_pos;
	_BitScanReverse(&msb_pos, n);
	return (int) msb_pos;
#elif defined(BLZ_BUILTIN_GCC)
	return (int) sizeof(n) * CHAR_BIT - 1 - __builtin_clzl(n);
#else
	int bits = 0;

	while (n >>= 1) {
		++bits;
	}

	return bits;
#endif
}

static unsigned long
blz_gamma_cost(unsigned long n)
{
	assert(n >= 2);

	return 2 * (unsigned long) blz_log2(n);
}

static unsigned long
blz_match_cost(unsigned long pos, unsigned long len)
{
	return 1 + blz_gamma_cost(len - 2) + blz_gamma_cost((pos >> 8) + 2) + 8;
}

// Heuristic to compare matches
static int
blz_match_better(unsigned long cur, unsigned long new_pos, unsigned long new_len,
                 unsigned long pos, unsigned long len)
{
	const unsigned long offs = cur - pos - 1;
	const unsigned long new_offs = cur - new_pos - 1;

	return (new_len > len + 1)
	    || (new_len >= len + 1 && new_offs / 8 <= offs);
}

// Heuristic to compare match with match at next position
static int
blz_next_match_better(unsigned long cur, unsigned long new_pos, unsigned long new_len,
                      unsigned long pos, unsigned long len)
{
	const unsigned long offs = cur - pos - 1;
	const unsigned long new_offs = cur + 1 - new_pos - 1;

	return (new_len > len + 1 && new_offs / 8 < offs)
	    || (new_len > len && new_offs < offs)
	    || (new_len >= len && new_offs < offs / 4);
}

static void
blz_putbit(struct blz_state *bs, unsigned int bit)
{
	// Check if tag is full
	if (!bs->bits_left--) {
		// Store tag
		bs->tag_out[0] = bs->tag & 0x00FF;
		bs->tag_out[1] = (bs->tag >> 8) & 0x00FF;

		// Init next tag
		bs->tag_out = bs->next_out;
		bs->next_out += 2;
		bs->bits_left = 15;
	}

	// Shift bit into tag
	bs->tag = (bs->tag << 1) + bit;
}

static void
blz_putbits(struct blz_state *bs, unsigned long bits, int num)
{
	assert(num >= 0 && num <= 16);
	assert((bits & (~0UL << num)) == 0);

	// Shift num bits into tag
	unsigned long tag = ((unsigned long) bs->tag << num) | bits;
	bs->tag = (unsigned int) tag;

	// Check if tag is full
	if (bs->bits_left < num) {
		const unsigned int top16 = (unsigned int) (tag >> (num - bs->bits_left));

		// Store tag
		bs->tag_out[0] = top16 & 0x00FF;
		bs->tag_out[1] = (top16 >> 8) & 0x00FF;

		// Init next tag
		bs->tag_out = bs->next_out;
		bs->next_out += 2;

		bs->bits_left += 16;
	}

	bs->bits_left -= num;
}

// Encode val using a universal code based on Elias gamma.
//
// This outputs each bit of val (after the leading one bit) as a pair where
// the first bit is the value, and the second is zero if this was the last
// pair, and one otherwise.
//
//     2 =  10 ->    00
//     3 =  11 ->    10
//     4 = 100 -> 01 00
//     5 = 101 -> 01 10
//     6 = 110 -> 11 00
//     ...
//
// On modern hardware this variant is slower to decode because we cannot count
// the leading zeroes to get the number of value bits and then read them
// directly. However on constrained hardware, it has the advantage of being
// decodable using only one variable (register) and a tiny loop:
//
//     result = 1;
//     do { result = (result << 1) + getbit(); } while (getbit());
//
// Strictly speaking, this is order-1 exp-Golomb, where we interleave the
// value bits with the bits of the unary coding of the length, but I've always
// known it as the gamma2 code. I am not sure where it originated from, but I
// can see I used it in aPLib around 1998.
//
static void
blz_putgamma(struct blz_state *bs, unsigned long val)
{
	assert(val >= 2);

#if !defined(BLZ_NO_LUT)
	// Output small values using lookup
	if (val < 512) {
		const unsigned int bits = blz_gamma_lookup[val][0];
		const unsigned int shift = blz_gamma_lookup[val][1];

		blz_putbits(bs, bits, (int) shift);

		return;
	}
#endif

	// Create a mask for the second-highest bit of val
#if defined(BLZ_BUILTIN_MSVC)
	unsigned long msb_pos;
	_BitScanReverse(&msb_pos, val);
	unsigned long mask = 1UL << (msb_pos - 1);
#elif defined(BLZ_BUILTIN_GCC)
	unsigned long mask = 1UL << ((int) sizeof(val) * CHAR_BIT - 2 - __builtin_clzl(val));
#else
	unsigned long mask = val >> 1;

	// Clear bits except highest
	while (mask & (mask - 1)) {
		mask &= mask - 1;
	}
#endif

	// Output gamma2-encoded bits
	blz_putbit(bs, (val & mask) ? 1 : 0);

	while (mask >>= 1) {
		blz_putbit(bs, 1);
		blz_putbit(bs, (val & mask) ? 1 : 0);
	}

	blz_putbit(bs, 0);
}

static unsigned char*
blz_finalize(struct blz_state *bs)
{
	// Trailing one bit to delimit any literal tags
	blz_putbit(bs, 1);

	// Shift last tag into position and store
	bs->tag <<= bs->bits_left;
	bs->tag_out[0] = bs->tag & 0x00FF;
	bs->tag_out[1] = (bs->tag >> 8) & 0x00FF;

	// Return pointer one past end of output
	return bs->next_out;
}

// Hash four bytes starting a p.
//
// This is Fibonacci hashing, also known as Knuth's multiplicative hash. The
// constant is a prime close to 2^32/phi.
//
static unsigned long
blz_hash4_bits(const unsigned char *p, int bits)
{
	assert(bits > 0 && bits <= 32);

	uint32_t val = (uint32_t) p[0]
	             | ((uint32_t) p[1] << 8)
	             | ((uint32_t) p[2] << 16)
	             | ((uint32_t) p[3] << 24);

	return (val * UINT32_C(2654435761)) >> (32 - bits);
}

static unsigned long
blz_hash4(const unsigned char *p)
{
	return blz_hash4_bits(p, BLZ_HASH_BITS);
}

size_t
blz_max_packed_size(size_t src_size)
{
	return src_size + src_size / 8 + 64;
}

size_t
blz_workmem_size(size_t src_size)
{
	(void) src_size;

	return LOOKUP_SIZE * sizeof(blz_word);
}

// Simple LZSS using hashing.
//
// The lookup table stores the previous position in the input that had a given
// hash value, or NO_MATCH_POS if none.
//
unsigned long
blz_pack(const void *src, void *dst, unsigned long src_size, void *workmem)
{
	struct blz_state bs;
	blz_word *const lookup = (blz_word *) workmem;
	const unsigned char *const in = (const unsigned char *) src;
	const unsigned long last_match_pos = src_size > 4 ? src_size - 4 : 0;
	unsigned long hash_pos = 0;
	unsigned long cur = 0;

	assert(src_size < BLZ_WORD_MAX);

	// Check for empty input
	if (src_size == 0) {
		return 0;
	}

	bs.next_out = (unsigned char *) dst;

	// First byte verbatim
	*bs.next_out++ = in[0];

	// Check for 1 byte input
	if (src_size == 1) {
		return 1;
	}

	// Initialize first tag
	bs.tag_out = bs.next_out;
	bs.next_out += 2;
	bs.tag = 0;
	bs.bits_left = 16;

	// Initialize lookup
	for (unsigned long i = 0; i < LOOKUP_SIZE; ++i) {
		lookup[i] = NO_MATCH_POS;
	}

	// Main compression loop
	for (cur = 1; cur <= last_match_pos; ) {
		// Update lookup up to current position
		while (hash_pos < cur) {
			lookup[blz_hash4(&in[hash_pos])] = hash_pos;
			hash_pos++;
		}

		// Look up match for current position
		const unsigned long pos = lookup[blz_hash4(&in[cur])];
		unsigned long len = 0;

		// Check match
		if (pos != NO_MATCH_POS) {
			const unsigned long len_limit = src_size - cur;

			while (len < len_limit
			    && in[pos + len] == in[cur + len]) {
				++len;
			}
		}

		// Output match or literal
		//
		// When offs >= 0x1FFE00, encoding a match of length 4
		// (37 bits) is longer than encoding 4 literals (36 bits).
		//
		// The value 0x7E00 is a heuristic that sacrifices some
		// length 4 matches in the hope that there will be a better
		// match at the next position.
		if (len > 4 || (len == 4 && cur - pos - 1 < 0x7E00UL)) {
			const unsigned long offs = cur - pos - 1;

			// Output match tag
			blz_putbit(&bs, 1);

			// Output match length
			blz_putgamma(&bs, len - 2);

			// Output match offset
			blz_putgamma(&bs, (offs >> 8) + 2);
			*bs.next_out++ = offs & 0x00FF;

			cur += len;
		}
		else {
			// Output literal tag
			blz_putbit(&bs, 0);

			// Copy literal
			*bs.next_out++ = in[cur++];
		}
	}

	// Output any remaining literals
	while (cur < src_size) {
		// Output literal tag
		blz_putbit(&bs, 0);

		// Copy literal
		*bs.next_out++ = in[cur++];
	}

	// Trailing one bit to delimit any literal tags
	blz_putbit(&bs, 1);

	// Shift last tag into position and store
	bs.tag <<= bs.bits_left;
	bs.tag_out[0] = bs.tag & 0x00FF;
	bs.tag_out[1] = (bs.tag >> 8) & 0x00FF;

	// Return compressed size
	return (unsigned long) (bs.next_out - (unsigned char *) dst);
}

// Include compression algorithms used by blz_pack_level
#include "brieflz_btparse.h"
#include "brieflz_hashbucket.h"
#include "brieflz_lazy.h"
#include "brieflz_leparse.h"

size_t
blz_workmem_size_level(size_t src_size, int level)
{
	switch (level) {
	case 1:
		return blz_workmem_size(src_size);
	case 2:
		return blz_lazy_workmem_size(src_size);
	case 3:
		return blz_hashbucket_workmem_size(src_size, 2);
	case 4:
		return blz_hashbucket_workmem_size(src_size, 4);
	case 5:
	case 6:
	case 7:
		return blz_leparse_workmem_size(src_size);
	case 8:
	case 9:
	case 10:
		return blz_btparse_workmem_size(src_size);
	default:
		return (size_t) -1;
	}
}

unsigned long
blz_pack_level(const void *src, void *dst, unsigned long src_size,
               void *workmem, int level)
{
	switch (level) {
	case 1:
		return blz_pack(src, dst, src_size, workmem);
	case 2:
		return blz_pack_lazy(src, dst, src_size, workmem);
	case 3:
		return blz_pack_hashbucket(src, dst, src_size, workmem, 2, 16);
	case 4:
		return blz_pack_hashbucket(src, dst, src_size, workmem, 4, 16);
	case 5:
		return blz_pack_leparse(src, dst, src_size, workmem, 1, 16);
	case 6:
		return blz_pack_leparse(src, dst, src_size, workmem, 8, 32);
	case 7:
		return blz_pack_leparse(src, dst, src_size, workmem, 64, 64);
	case 8:
		return blz_pack_btparse(src, dst, src_size, workmem, 16, 96);
	case 9:
		return blz_pack_btparse(src, dst, src_size, workmem, 32, 224);
	case 10:
		return blz_pack_btparse(src, dst, src_size, workmem, ULONG_MAX, ULONG_MAX);
	default:
		return BLZ_ERROR;
	}
}

// clang -g -O1 -fsanitize=fuzzer,address -DBLZ_FUZZING brieflz.c depack.c
#if defined(BLZ_FUZZING)
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef BLZ_FUZZ_LEVEL
#  define BLZ_FUZZ_LEVEL 1
#endif

extern int
LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	if (size > ULONG_MAX / 2) { return 0; }
	void *workmem = malloc(blz_workmem_size_level(size, BLZ_FUZZ_LEVEL));
	void *packed = malloc(blz_max_packed_size(size));
	void *depacked = malloc(size);
	if (!workmem || !packed || !depacked) { abort(); }
	unsigned long packed_size = blz_pack_level(data, packed, size, workmem, BLZ_FUZZ_LEVEL);
	blz_depack(packed, depacked, size);
	if (memcmp(data, depacked, size)) { abort(); }
	free(depacked);
	free(packed);
	free(workmem);
	return 0;
}
#endif
