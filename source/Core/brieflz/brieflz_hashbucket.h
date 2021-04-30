//
// BriefLZ - small fast Lempel-Ziv
//
// Lazy parsing with multiple previous positions per hash
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

#ifndef BRIEFLZ_HASHBUCKET_H_INCLUDED
#define BRIEFLZ_HASHBUCKET_H_INCLUDED

static size_t
blz_hashbucket_workmem_size(size_t src_size, unsigned int bucket_size)
{
	(void) src_size;

	assert(bucket_size > 0);
	assert(sizeof(bucket_size) < sizeof(size_t)
	    || bucket_size < SIZE_MAX / (LOOKUP_SIZE * sizeof(blz_word)));

	return (LOOKUP_SIZE * bucket_size) * sizeof(blz_word);
}

// Lazy parsing with multiple previous positions per hash.
//
// Instead of storing only the previous position a given hash occured at,
// this stores the last bucket_size such positions in lookup. This means we
// can check each of these and choose the "best".
//
// There are multiple options for maintaining the entries of the buckets, we
// simply insert at the front to maintain the order of matches and avoid extra
// variables. This gives some overhead for moving elements, but as long as
// bucket_size is small and everything fits in a cache line it is pretty fast.
//
// If we find a match that is accept_len or longer, we stop searching.
//
static unsigned long
blz_pack_hashbucket(const void *src, void *dst, unsigned long src_size, void *workmem,
                    const unsigned int bucket_size, const unsigned long accept_len)
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

	assert(bucket_size > 0);
	assert(sizeof(bucket_size) < sizeof(unsigned long)
	    || bucket_size < ULONG_MAX / LOOKUP_SIZE);

	// Initialize lookup
	for (unsigned long i = 0; i < LOOKUP_SIZE * bucket_size; ++i) {
		lookup[i] = NO_MATCH_POS;
	}

	// Main compression loop
	for (cur = 1; cur <= last_match_pos; ) {
		// Update lookup up to current position
		while (hash_pos < cur) {
			blz_word *const bucket = &lookup[blz_hash4(&in[hash_pos]) * bucket_size];
			unsigned long next = hash_pos;

			// Insert hash_pos at start of bucket
			for (unsigned int i = 0; i < bucket_size; ++i) {
				unsigned long tmp = bucket[i];
				bucket[i] = next;
				next = tmp;
			}

			hash_pos++;
		}

		unsigned long best_pos = NO_MATCH_POS;
		unsigned long best_len = 0;

		// Look up first match for current position
		const blz_word *const bucket = &lookup[blz_hash4(&in[cur]) * bucket_size];
		unsigned long pos = bucket[0];
		unsigned int bucket_idx = 0;

		const unsigned long len_limit = src_size - cur;

		// Check matches
		while (pos != NO_MATCH_POS) {
			unsigned long len = 0;

			// Check match
			if (best_len < len_limit
			 && in[pos + best_len] == in[cur + best_len]) {
				while (len < len_limit && in[pos + len] == in[cur + len]) {
					++len;
				}
			}

			// Update best match
			if (blz_match_better(cur, pos, len, best_pos, best_len)) {
				best_pos = pos;
				best_len = len;
				if (best_len >= accept_len) {
					break;
				}
			}

			// Go to previous match
			if (++bucket_idx == bucket_size) {
				break;
			}
			pos = bucket[bucket_idx];
		}

		// Check if match at next position is better
		if (best_len > 3 && best_len < accept_len && cur < last_match_pos) {
			// Update lookup up to next position
			{
				blz_word *const next_bucket = &lookup[blz_hash4(&in[hash_pos]) * bucket_size];
				unsigned long next = hash_pos;

				// Insert hash_pos at start of bucket
				for (unsigned int i = 0; i < bucket_size; ++i) {
					unsigned long tmp = next_bucket[i];
					next_bucket[i] = next;
					next = tmp;
				}

				hash_pos++;
			}

			// Look up first match for next position
			const blz_word *const next_bucket = &lookup[blz_hash4(&in[cur + 1]) * bucket_size];
			unsigned long next_pos = next_bucket[0];
			unsigned int next_bucket_idx = 0;

			const unsigned long next_len_limit = src_size - (cur + 1);

			// Check matches
			while (next_pos != NO_MATCH_POS) {
				unsigned long next_len = 0;

				// Check match
				if (best_len - 1 < next_len_limit
				 && in[next_pos + best_len - 1] == in[cur + 1 + best_len - 1]) {
					while (next_len < next_len_limit
					    && in[next_pos + next_len] == in[cur + 1 + next_len]) {
						++next_len;
					}
				}

				if (next_len >= best_len) {
					// Replace with next match if it extends backwards
					if (next_pos > 0 && in[next_pos - 1] == in[cur]) {
						if (blz_match_better(cur, next_pos - 1, next_len + 1, best_pos, best_len)) {
							best_pos = next_pos - 1;
							best_len = next_len + 1;
						}
					}
					else {
						// Drop current match if next match is better
						if (blz_next_match_better(cur, next_pos, next_len, best_pos, best_len)) {
							best_len = 0;
							break;
						}
					}
				}

				// Go to previous match
				if (++next_bucket_idx == bucket_size) {
					break;
				}
				next_pos = next_bucket[next_bucket_idx];
			}
		}

		// Output match or literal
		if (best_len > 4 || (best_len == 4 && cur - best_pos - 1 < 0x3FE00UL)) {
			const unsigned long offs = cur - best_pos - 1;

			// Output match tag
			blz_putbit(&bs, 1);

			// Output match length
			blz_putgamma(&bs, best_len - 2);

			// Output match offset
			blz_putgamma(&bs, (offs >> 8) + 2);
			*bs.next_out++ = offs & 0x00FF;

			cur += best_len;
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

#endif /* BRIEFLZ_HASHBUCKET_H_INCLUDED */
