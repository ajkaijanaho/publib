#ifndef __publib_bitarr_h
#define __publib_bitarr_h

#include <stddef.h>	/* need size_t */
#include <limits.h>	/* need CHAR_BIT */
#include <publib/alloc.h>	/* need dynarr */

typedef struct {
	struct dynarr da;
	size_t rnglen;
} Bitarr;

/* How many bits are there in an unsigned int? */
#define ba_bits		(sizeof(unsigned)*CHAR_BIT)

/* Compute word a given bit is stored in */
#define ba_num2word(i)	((i) / ba_bits)

/* Create bitmask for one word, given index into bit array */
#define ba_num2mask(i)	(1 << ((i) % ba_bits))

Bitarr *ba_create(void);
void ba_destroy(Bitarr *__ba);
Bitarr *ba_copy(const Bitarr *__ba);
int ba_resize(Bitarr *__ba, size_t __max_number);

int ba_set(Bitarr *__ba, unsigned __number);		/* ba[number] = 1 */
void ba_clear(Bitarr *__ba, unsigned __number);		/* ba[number] = 0 */
void ba_clear_all(Bitarr *__ba);			/* ba[*] = 0 */
int ba_query(Bitarr *__ba, unsigned __number);		/* ba[number] */
void ba_and_ba(Bitarr *__ba1, const Bitarr *__ba2);	/* ba1 &= ba2 */
int ba_or_ba(Bitarr *__ba1, const Bitarr *__ba2);	/* ba1 |= ba2 */
void ba_xor_ba(Bitarr *__ba1, const Bitarr *__ba2);	/* ba1 ^= ba2 */
void ba_or_not_ba(Bitarr *__ba1, const Bitarr *__ba2);	/* ba1 |= ~ba2 */
void ba_not(Bitarr *__ba);				/* ba = ~ba */

#endif
