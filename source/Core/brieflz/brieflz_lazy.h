//
// BriefLZ - small fast Lempel-Ziv
//
// Lazy (non-greedy) parsing with one-byte-lookahead
//
// Copyright (c) 2016-2020 Joergen Ibsen
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

#ifndef BRIEFLZ_LAZY_H_INCLUDED
#define BRIEFLZ_LAZY_H_INCLUDED

static size_t
blz_lazy_workmem_size(size_t src_size)
{
	(void) src_size;

	return LOOKUP_SIZE * sizeof(blz_word);
}

// Lazy (non-greedy) parsing with one-byte-lookahead.
//
// Each time we find a match, we check if there is a better match at the next
// position, and if so encode a literal instead.
//
static unsigned long
blz_pack_lazy(const void *src, void *dst, unsigned long src_size, void *workmem)
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
		unsigned long pos = lookup[blz_hash4(&in[cur])];
		unsigned long len = 0;

		// Check match
		if (pos != NO_MATCH_POS) {
			const unsigned long len_limit = src_size - cur;

			while (len < len_limit
			    && in[pos + len] == in[cur + len]) {
				++len;
			}
		}

		// Check if match at next position is better
		if (len > 3 && cur < last_match_pos) {
			// Update lookup up to next position
			lookup[blz_hash4(&in[hash_pos])] = hash_pos;
			hash_pos++;

			// Look up match for next position
			const unsigned long next_pos = lookup[blz_hash4(&in[cur + 1])];
			unsigned long next_len = 0;

			// Check match
			if (next_pos != NO_MATCH_POS && next_pos != pos + 1) {
				const unsigned long next_len_limit = src_size - (cur + 1);

				// If last byte matches, so this has a chance to be a better match
				if (len - 1 < next_len_limit
				 && in[next_pos + len - 1] == in[cur + 1 + len - 1]) {
					while (next_len < next_len_limit
					    && in[next_pos + next_len] == in[cur + 1 + next_len]) {
						++next_len;
					}
				}
			}

			if (next_len >= len) {
				// Replace with next match if it extends backwards
				if (next_pos > 0 && in[next_pos - 1] == in[cur]) {
					if (blz_match_better(cur, next_pos - 1, next_len + 1, pos, len)) {
						pos = next_pos - 1;
						len = next_len + 1;
					}
				}
				else {
					// Drop current match if next match is better
					if (blz_next_match_better(cur, next_pos, next_len, pos, len)) {
						len = 0;
					}
				}

			}
		}

		// Output match or literal
		if (len > 4 || (len == 4 && cur - pos - 1 < 0x3FE00UL)) {
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

#endif /* BRIEFLZ_LAZY_H_INCLUDED */
