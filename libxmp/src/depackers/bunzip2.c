<<<<<<< HEAD
/* vi: set sw=4 ts=4: */
/*	Small bzip2 deflate implementation, by Rob Landley (rob@landley.net).

	Based on bzip2 decompression code by Julian R Seward (jseward@acm.org),
	which also acknowledges contributions by Mike Burrows, David Wheeler,
	Peter Fenwick, Alistair Moffat, Radford Neal, Ian H. Witten,
	Robert Sedgewick, and Jon L. Bentley.

	This code is licensed under the LGPLv2:
		LGPL (http://www.gnu.org/copyleft/lgpl.html
*/

/*
	Size and speed optimizations by Manuel Novoa III  (mjn3@codepoet.org).

	More efficient reading of huffman codes, a streamlined read_bunzip()
	function, and various other tweaks.  In (limited) tests, approximately
	20% faster than bzcat on x86 and about 10% faster on arm.

	Note that about 2/3 of the time is spent in read_unzip() reversing
	the Burrows-Wheeler transformation.  Much of that time is delay
	resulting from cache misses.

	I would ask that anyone benefiting from this work, especially those
	using it in commercial products, consider making a donation to my local
	non-profit hospice organization (see www.hospiceacadiana.com) in the
    name of the woman I loved, Toni W. Hagan, who passed away Feb. 12, 2003.

	Manuel
 */

/*
	Modified for xmp by Claudio Matsuoka, 20120809
=======
/* bzcat.c - bzip2 decompression
 *
 * Copyright 2003, 2007 Rob Landley <rob@landley.net>
 *
 * Based on a close reading (but not the actual code) of the original bzip2
 * decompression code by Julian R Seward (jseward@acm.org), which also
 * acknowledges contributions by Mike Burrows, David Wheeler, Peter Fenwick,
 * Alistair Moffat, Radford Neal, Ian H. Witten, Robert Sedgewick, and
 * Jon L. Bentley.
 *
 * This is 0BSD-licensed code from https://github.com/landley/toybox:
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
>>>>>>> db7344ebf (abc)
 */

#include <setjmp.h>
#include "../common.h"
#include "depacker.h"
#include "crc32.h"

<<<<<<< HEAD
/* Constants for huffman coding */
#define MAX_GROUPS			6
#define GROUP_SIZE   		50		/* 64 would have been more efficient */
#define MAX_HUFCODE_BITS 	20		/* Longest huffman code allowed */
#define MAX_SYMBOLS 		258		/* 256 literals + RUNA + RUNB */
#define SYMBOL_RUNA			0
#define SYMBOL_RUNB			1

/* Status return values */
#define RETVAL_OK						0
#define RETVAL_LAST_BLOCK				(-1)
#define RETVAL_NOT_BZIP_DATA			(-2)
#define RETVAL_UNEXPECTED_INPUT_EOF		(-3)
#define RETVAL_UNEXPECTED_OUTPUT_EOF	(-4)
#define RETVAL_DATA_ERROR				(-5)
#define RETVAL_OUT_OF_MEMORY			(-6)
#define RETVAL_OBSOLETE_INPUT			(-7)

/* Other housekeeping constants */
#define IOBUF_SIZE			4096

/* This is what we know about each huffman coding group */
struct group_data {
	/* limit and base are 1-indexed. index 0 is never used but increasing
	 * the length by 1 simplifies the code and isn't that much of a waste. */
	/* We have an extra slot at the end of limit[] for a sentinal value. */
	int limit[1+MAX_HUFCODE_BITS+1],base[1+MAX_HUFCODE_BITS],permute[MAX_SYMBOLS];
	int minLen, maxLen;
};

/* Structure holding all the housekeeping data, including IO buffers and
   memory that persists between calls to bunzip */
typedef struct {
	/* State for interrupting output loop */
	int writeCopies,writePos,writeRunCountdown,writeCount,writeCurrent;
	/* I/O tracking data (file handles, buffers, positions, etc.) */
	HIO_HANDLE *in;
	FILE *out;
	int inbufCount,inbufPos /*,outbufPos*/;
	unsigned char *inbuf /*,*outbuf*/;
	unsigned int inbufBitCount, inbufBits;
	/* The CRC values stored in the block header and calculated from the data */
	unsigned int headerCRC, totalCRC, writeCRC;
	/* Intermediate buffer and its size (in bytes) */
	unsigned int *dbuf, dbufSize;
	/* These things are a bit too big to go on the stack */
	unsigned char selectors[32768];			/* nSelectors=15 bits */
	struct group_data groups[MAX_GROUPS];	/* huffman coding tables */
	/* For I/O error handling */
	jmp_buf jmpbuf;
} bunzip_data;

/* Return the next nnn bits of input.  All reads from the compressed input
   are done through this function.  All reads are big endian */
static unsigned int get_bits(bunzip_data *bd, char bits_wanted)
{
	unsigned int bits=0;

	/* If we need to get more data from the byte buffer, do so.  (Loop getting
	   one byte at a time to enforce endianness and avoid unaligned access.) */
	while (bd->inbufBitCount<bits_wanted) {
		/* If we need to read more data from file into byte buffer, do so */
		if(bd->inbufPos==bd->inbufCount) {
			if((bd->inbufCount = hio_read(bd->inbuf, 1, IOBUF_SIZE, bd->in)) <= 0)
				longjmp(bd->jmpbuf,RETVAL_UNEXPECTED_INPUT_EOF);
			bd->inbufPos=0;
		}
		/* Avoid 32-bit overflow (dump bit buffer to top of output) */
		if(bd->inbufBitCount>=24) {
			bits=bd->inbufBits&((1<<bd->inbufBitCount)-1);
			bits_wanted-=bd->inbufBitCount;
			bits<<=bits_wanted;
			bd->inbufBitCount=0;
		}
		/* Grab next 8 bits of input from buffer. */
		bd->inbufBits=(bd->inbufBits<<8)|bd->inbuf[bd->inbufPos++];
		bd->inbufBitCount+=8;
	}
	/* Calculate result */
	bd->inbufBitCount-=bits_wanted;
	bits|=(bd->inbufBits>>bd->inbufBitCount)&((1<<bits_wanted)-1);

	return bits;
}

/* Unpacks the next block and sets up for the inverse burrows-wheeler step. */

static int get_next_block(bunzip_data *bd)
{
	struct group_data *hufGroup;
	int dbufCount,nextSym,dbufSize,groupCount,selector,
		i,j,k,t,runPos,symCount,symTotal,nSelectors,byteCount[256];
	unsigned char uc, symToByte[256], mtfSymbol[256], *selectors;
	unsigned int *dbuf,origPtr;

	/* Reset longjmp I/O error handling */
	i=setjmp(bd->jmpbuf);
	if(i) return i;

	hufGroup=NULL;
	dbuf=bd->dbuf;
	dbufSize=bd->dbufSize;
	selectors=bd->selectors;

	/* Read in header signature and CRC, then validate signature.
	   (last block signature means CRC is for whole file, return now) */
	i = get_bits(bd,24);
	j = get_bits(bd,24);
	bd->headerCRC=get_bits(bd,32);
	if ((i == 0x177245) && (j == 0x385090)) return RETVAL_LAST_BLOCK;
	if ((i != 0x314159) || (j != 0x265359)) return RETVAL_NOT_BZIP_DATA;
	/* We can add support for blockRandomised if anybody complains.  There was
	   some code for this in busybox 1.0.0-pre3, but nobody ever noticed that
	   it didn't actually work. */
	if(get_bits(bd,1)) return RETVAL_OBSOLETE_INPUT;
	if((origPtr=get_bits(bd,24)) > dbufSize) return RETVAL_DATA_ERROR;
	/* mapping table: if some byte values are never used (encoding things
	   like ascii text), the compression code removes the gaps to have fewer
	   symbols to deal with, and writes a sparse bitfield indicating which
	   values were present.  We make a translation table to convert the symbols
	   back to the corresponding bytes. */
	t=get_bits(bd, 16);
	symTotal=0;
	for (i=0;i<16;i++) {
		if(t&(1<<(15-i))) {
			k=get_bits(bd,16);
			for(j=0;j<16;j++)
				if(k&(1<<(15-j))) symToByte[symTotal++]=(16*i)+j;
		}
	}
	/* How many different huffman coding groups does this block use? */
	groupCount=get_bits(bd,3);
	if (groupCount<2 || groupCount>MAX_GROUPS) return RETVAL_DATA_ERROR;
	/* nSelectors: Every GROUP_SIZE many symbols we select a new huffman coding
	   group.  Read in the group selector list, which is stored as MTF encoded
	   bit runs.  (MTF=Move To Front, as each value is used it's moved to the
	   start of the list.) */
	if(!(nSelectors=get_bits(bd, 15))) return RETVAL_DATA_ERROR;
	for(i=0; i<groupCount; i++) mtfSymbol[i] = i;
	for(i=0; i<nSelectors; i++) {
		/* Get next value */
		for(j=0;get_bits(bd,1);) {
			j++;
			if (j>=groupCount) return RETVAL_DATA_ERROR;
		}
		/* Decode MTF to get the next selector */
		uc = mtfSymbol[j];
		for(;j;j--) mtfSymbol[j] = mtfSymbol[j-1];
		mtfSymbol[0]=selectors[i]=uc;
	}
	/* Read the huffman coding tables for each group, which code for symTotal
	   literal symbols, plus two run symbols (RUNA, RUNB) */
	symCount=symTotal+2;
	for (j=0; j<groupCount; j++) {
		unsigned char length[MAX_SYMBOLS],temp[MAX_HUFCODE_BITS+1];
		int	minLen,	maxLen, pp;
		/* Read huffman code lengths for each symbol.  They're stored in
		   a way similar to mtf; record a starting value for the first symbol,
		   and an offset from the previous value for everys symbol after that.
		   (Subtracting 1 before the loop and then adding it back at the end is
		   an optimization that makes the test inside the loop simpler: symbol
		   length 0 becomes negative, so an unsigned inequality catches it.) */
		t=get_bits(bd, 5)-1;
		for (i = 0; i < symCount; i++) {
			for(;;) {
				if (((unsigned)t) > (MAX_HUFCODE_BITS-1))
					return RETVAL_DATA_ERROR;
				/* If first bit is 0, stop.  Else second bit indicates whether
				   to increment or decrement the value.  Optimization: grab 2
				   bits and unget the second if the first was 0. */
				k = get_bits(bd,2);
				if (k < 2) {
					bd->inbufBitCount++;
					break;
				}
				/* Add one if second bit 1, else subtract 1.  Avoids if/else */
				t+=(((k+1)&2)-1);
			}
			/* Correct for the initial -1, to get the final symbol length */
			length[i]=t+1;
		}
		/* Find largest and smallest lengths in this group */
		minLen=maxLen=length[0];
		for(i = 1; i < symCount; i++) {
			if(length[i] > maxLen) maxLen = length[i];
			else if(length[i] < minLen) minLen = length[i];
		}
		/* Calculate permute[], base[], and limit[] tables from length[].
		 *
		 * permute[] is the lookup table for converting huffman coded symbols
		 * into decoded symbols.  base[] is the amount to subtract from the
		 * value of a huffman symbol of a given length when using permute[].
		 *
		 * limit[] indicates the largest numerical value a symbol with a given
		 * number of bits can have.  This is how the huffman codes can vary in
		 * length: each code with a value>limit[length] needs another bit.
		 */
		hufGroup=bd->groups+j;
		hufGroup->minLen = minLen;
		hufGroup->maxLen = maxLen;
		/* Calculate permute[].  Concurently, initialize temp[] and limit[]. */
		pp=0;
		for(i=minLen;i<=maxLen;i++) {
			temp[i]=hufGroup->limit[i]=0;
			for(t=0;t<symCount;t++)
				if(length[t]==i) hufGroup->permute[pp++] = t;
		}
		/* Count symbols coded for at each bit length */
		for (i=0;i<symCount;i++) temp[length[i]]++;
		/* Calculate limit[] (the largest symbol-coding value at each bit
		 * length, which is (previous limit<<1)+symbols at this level), and
		 * base[] (number of symbols to ignore at each bit length, which is
		 * limit minus the cumulative count of symbols coded for already). */
		pp=t=0;
		for (i=minLen; i<maxLen; i++) {
			pp+=temp[i];
			/* We read the largest possible symbol size and then unget bits
			   after determining how many we need, and those extra bits could
			   be set to anything.  (They're noise from future symbols.)  At
			   each level we're really only interested in the first few bits,
			   so here we set all the trailing to-be-ignored bits to 1 so they
			   don't affect the value>limit[length] comparison. */
			hufGroup->limit[i]= (pp << (maxLen - i)) - 1;
			pp<<=1;
			hufGroup->base[i+1]=pp-(t+=temp[i]);
		}
		hufGroup->limit[maxLen+1] = INT_MAX; /* Sentinal value for reading next sym. */
		hufGroup->limit[maxLen]=pp+temp[maxLen]-1;
		hufGroup->base[minLen]=0;
	}
	/* We've finished reading and digesting the block header.  Now read this
	   block's huffman coded symbols from the file and undo the huffman coding
	   and run length encoding, saving the result into dbuf[dbufCount++]=uc */

	/* Initialize symbol occurrence counters and symbol Move To Front table */
	for(i=0;i<256;i++) {
		byteCount[i] = 0;
		mtfSymbol[i]=(unsigned char)i;
	}
	/* Loop through compressed symbols. */
	runPos=dbufCount=symCount=selector=0;
	for(;;) {
		/* Determine which huffman coding group to use. */
		if(!(symCount--)) {
			symCount=GROUP_SIZE-1;
			if(selector>=nSelectors) return RETVAL_DATA_ERROR;
			hufGroup=bd->groups+selectors[selector++];
		}
		/* Read next huffman-coded symbol. */
		/* Note: It is far cheaper to read maxLen bits and back up than it is
		   to read minLen bits and then an additional bit at a time, testing
		   as we go.  Because there is a trailing last block (with file CRC),
		   there is no danger of the overread causing an unexpected EOF for a
		   valid compressed file.  As a further optimization, we do the read
		   inline (falling back to a call to get_bits if the buffer runs
		   dry).  The following (up to got_huff_bits:) is equivalent to
		   j=get_bits(bd,hufGroup->maxLen);
		 */
		while (bd->inbufBitCount<hufGroup->maxLen) {
			if(bd->inbufPos==bd->inbufCount) {
				j = get_bits(bd,hufGroup->maxLen);
				goto got_huff_bits;
			}
			bd->inbufBits=(bd->inbufBits<<8)|bd->inbuf[bd->inbufPos++];
			bd->inbufBitCount+=8;
		};
		bd->inbufBitCount-=hufGroup->maxLen;
		j = (bd->inbufBits>>bd->inbufBitCount)&((1<<hufGroup->maxLen)-1);
got_huff_bits:
		/* Figure how how many bits are in next symbol and unget extras */
		i=hufGroup->minLen;
		while(j>hufGroup->limit[i]) ++i;
		bd->inbufBitCount += (hufGroup->maxLen - i);
		/* Huffman decode value to get nextSym (with bounds checking) */
		if ((i > hufGroup->maxLen)
			|| (((unsigned)(j=(j>>(hufGroup->maxLen-i))-hufGroup->base[i]))
				>= MAX_SYMBOLS))
			return RETVAL_DATA_ERROR;
		nextSym = hufGroup->permute[j];
		/* We have now decoded the symbol, which indicates either a new literal
		   byte, or a repeated run of the most recent literal byte.  First,
		   check if nextSym indicates a repeated run, and if so loop collecting
		   how many times to repeat the last literal. */
		if (((unsigned)nextSym) <= SYMBOL_RUNB) { /* RUNA or RUNB */
			/* If this is the start of a new run, zero out counter */
			if(!runPos) {
				runPos = 1;
				t = 0;
			}
			/* Neat trick that saves 1 symbol: instead of or-ing 0 or 1 at
			   each bit position, add 1 or 2 instead.  For example,
			   1011 is 1<<0 + 1<<1 + 2<<2.  1010 is 2<<0 + 2<<1 + 1<<2.
			   You can make any bit pattern that way using 1 less symbol than
			   the basic or 0/1 method (except all bits 0, which would use no
			   symbols, but a run of length 0 doesn't mean anything in this
			   context).  Thus space is saved. */
			/* Note: use unsigned to avoid signed overflows. */
			t = (unsigned)t + ((unsigned)runPos << nextSym); /* +runPos if RUNA; +2*runPos if RUNB */
			runPos <<= 1;
			continue;
		}
		/* When we hit the first non-run symbol after a run, we now know
		   how many times to repeat the last literal, so append that many
		   copies to our buffer of decoded symbols (dbuf) now.  (The last
		   literal used is the one at the head of the mtfSymbol array.) */
		if(runPos) {
			runPos=0;
			if(t < 0 || t > dbufSize || dbufCount+t>=dbufSize)
				return RETVAL_DATA_ERROR;

			uc = symToByte[mtfSymbol[0]];
			byteCount[uc] += t;
			while(t--) dbuf[dbufCount++]=uc;
		}
		/* Is this the terminating symbol? */
		if(nextSym>symTotal) break;
		/* At this point, nextSym indicates a new literal character.  Subtract
		   one to get the position in the MTF array at which this literal is
		   currently to be found.  (Note that the result can't be -1 or 0,
		   because 0 and 1 are RUNA and RUNB.  But another instance of the
		   first symbol in the mtf array, position 0, would have been handled
		   as part of a run above.  Therefore 1 unused mtf position minus
		   2 non-literal nextSym values equals -1.) */
		if(dbufCount>=dbufSize) return RETVAL_DATA_ERROR;
		i = nextSym - 1;
		uc = mtfSymbol[i];
		/* Adjust the MTF array.  Since we typically expect to move only a
		 * small number of symbols, and are bound by 256 in any case, using
		 * memmove here would typically be bigger and slower due to function
		 * call overhead and other assorted setup costs. */
		do {
			mtfSymbol[i] = mtfSymbol[i-1];
		} while (--i);
		mtfSymbol[0] = uc;
		uc=symToByte[uc];
		/* We have our literal byte.  Save it into dbuf. */
		byteCount[uc]++;
		dbuf[dbufCount++] = (unsigned int)uc;
	}
	/* At this point, we've read all the huffman-coded symbols (and repeated
       runs) for this block from the input stream, and decoded them into the
	   intermediate buffer.  There are dbufCount many decoded bytes in dbuf[].
	   Now undo the Burrows-Wheeler transform on dbuf.
	   See http://dogma.net/markn/articles/bwt/bwt.htm
	 */
	/* Turn byteCount into cumulative occurrence counts of 0 to n-1. */
	j=0;
	for(i=0;i<256;i++) {
		k=j+byteCount[i];
		byteCount[i] = j;
		j=k;
	}
	/* Figure out what order dbuf would be in if we sorted it. */
	for (i=0;i<dbufCount;i++) {
		uc=(unsigned char)(dbuf[i] & 0xff);
		dbuf[byteCount[uc]] |= (i << 8);
		byteCount[uc]++;
	}
	/* Decode first byte by hand to initialize "previous" byte.  Note that it
	   doesn't get output, and if the first three characters are identical
	   it doesn't qualify as a run (hence writeRunCountdown=5). */
	if(dbufCount) {
		if(origPtr>=dbufCount) return RETVAL_DATA_ERROR;
		bd->writePos=dbuf[origPtr];
	    bd->writeCurrent=(unsigned char)(bd->writePos&0xff);
		bd->writePos>>=8;
		bd->writeRunCountdown=5;
	}
	bd->writeCount=dbufCount;

	return RETVAL_OK;
}

/* Undo burrows-wheeler transform on intermediate buffer to produce output.
   If start_bunzip was initialized with out_fd=-1, then up to len bytes of
   data are written to outbuf.  Return value is number of bytes written or
   error (all errors are negative numbers).  If out_fd!=-1, outbuf and len
   are ignored, data is written to out_fd and return is RETVAL_OK or error.
*/

static int read_bunzip(bunzip_data *bd, char *outbuf, int len)
{
	const unsigned int *dbuf;
	int pos,current,previous,gotcount;

	/* If last read was short due to end of file, return last block now */
	if(bd->writeCount<0) return bd->writeCount;

	gotcount = 0;
	dbuf=bd->dbuf;
	pos=bd->writePos;
	current=bd->writeCurrent;

	/* We will always have pending decoded data to write into the output
	   buffer unless this is the very first call (in which case we haven't
	   huffman-decoded a block into the intermediate buffer yet). */

	if (bd->writeCopies) {
		/* Inside the loop, writeCopies means extra copies (beyond 1) */
		--bd->writeCopies;
		/* Loop outputting bytes */
		for(;;) {
			/* If the output buffer is full, snapshot state and return */
			if(gotcount >= len) {
				bd->writePos=pos;
				bd->writeCurrent=current;
				bd->writeCopies++;
				return len;
			}
			/* Write next byte into output buffer, updating CRC */
			outbuf[gotcount++] = current;
			bd->writeCRC=(((bd->writeCRC)<<8)
						  ^libxmp_crc32_table_B[((bd->writeCRC)>>24)^current]);
			/* Loop now if we're outputting multiple copies of this byte */
			if (bd->writeCopies) {
				--bd->writeCopies;
				continue;
			}
decode_next_byte:
			if (!bd->writeCount--) break;
			/* Follow sequence vector to undo Burrows-Wheeler transform */
			previous=current;
			pos=dbuf[pos];
			current=pos&0xff;
			pos>>=8;
			/* After 3 consecutive copies of the same byte, the 4th is a repeat
			   count.  We count down from 4 instead
			 * of counting up because testing for non-zero is faster */
			if(--bd->writeRunCountdown) {
				if(current!=previous) bd->writeRunCountdown=4;
			} else {
				/* We have a repeated run, this byte indicates the count */
				bd->writeCopies=current;
				current=previous;
				bd->writeRunCountdown=5;
				/* Sometimes there are just 3 bytes (run length 0) */
				if(!bd->writeCopies) goto decode_next_byte;
				/* Subtract the 1 copy we'd output anyway to get extras */
				--bd->writeCopies;
			}
		}
		/* Decompression of this block completed successfully */
		bd->writeCRC=~bd->writeCRC;
		bd->totalCRC=((bd->totalCRC<<1) | (bd->totalCRC>>31)) ^ bd->writeCRC;
		/* If this block had a CRC error, force file level CRC error. */
		if(bd->writeCRC!=bd->headerCRC) {
			bd->totalCRC=bd->headerCRC+1;
			return RETVAL_LAST_BLOCK;
		}
	}

	/* Refill the intermediate buffer by huffman-decoding next block of input */
	/* (previous is just a convenient unused temp variable here) */
	previous=get_next_block(bd);
	if(previous) {
		bd->writeCount=previous;
		return (previous!=RETVAL_LAST_BLOCK) ? previous : gotcount;
	}
	bd->writeCRC=0xffffffffUL;
	pos=bd->writePos;
	current=bd->writeCurrent;
	goto decode_next_byte;
}

/* Allocate the structure, read file header.  If in_fd==-1, inbuf must contain
   a complete bunzip file (len bytes long).  If in_fd!=-1, inbuf and len are
   ignored, and data is read from file handle into temporary buffer. */
static int start_bunzip(bunzip_data **bdp, HIO_HANDLE *in, char *inbuf, int len)
{
	bunzip_data *bd;
	unsigned int i;
	const unsigned int BZh0=(((unsigned int)'B')<<24)+(((unsigned int)'Z')<<16)
							+(((unsigned int)'h')<<8)+(unsigned int)'0';

	/* Figure out how much data to allocate */
	i=sizeof(bunzip_data);
	if(in!=NULL) i+=IOBUF_SIZE;
	/* Allocate bunzip_data.  Most fields initialize to zero. */
	bd=*bdp=(bunzip_data *) calloc(1,i);
	if(!bd) return RETVAL_OUT_OF_MEMORY;
	/* Setup input buffer */
	if(NULL==(bd->in=in)) {
		bd->inbuf=(unsigned char *)inbuf;
		bd->inbufCount=len;
	} else bd->inbuf=(unsigned char *)(bd+1);

#if 0
	/* Init the CRC32 table (big endian) */
	for(i=0;i<256;i++) {
		c=i<<24;
		for(j=8;j;j--)
			c=c&0x80000000 ? (c<<1)^0x04c11db7 : (c<<1);
		bd->crc32Table[i]=c;
	}
#endif

	/* Setup for I/O error handling via longjmp */
	i=setjmp(bd->jmpbuf);
	if(i) return i;

	/* Ensure that file starts with "BZh['1'-'9']." */
	i = get_bits(bd,32);
	if (((unsigned int)(i-BZh0-1)) >= 9) return RETVAL_NOT_BZIP_DATA;

	/* Fourth byte (ascii '1'-'9'), indicates block size in units of 100k of
	   uncompressed data.  Allocate intermediate buffer for block. */
	bd->dbufSize=100000*(i-BZh0);

	bd->dbuf=(unsigned int *) malloc(bd->dbufSize * sizeof(unsigned int));
	if (!bd->dbuf)
		return RETVAL_OUT_OF_MEMORY;
	return RETVAL_OK;
=======
#define THREADS 1

// Constants for huffman coding
#define MAX_GROUPS               6
#define GROUP_SIZE               50     /* 64 would have been more efficient */
#define MAX_HUFCODE_BITS         20     /* Longest huffman code allowed */
#define MAX_SYMBOLS              258    /* 256 literals + RUNA + RUNB */
#define SYMBOL_RUNA              0
#define SYMBOL_RUNB              1

// Other housekeeping constants
#define IOBUF_SIZE               4096

// Status return values
#define RETVAL_LAST_BLOCK        (-100)
#define RETVAL_NOT_BZIP_DATA     (-1)
#define RETVAL_DATA_ERROR        (-2)
#define RETVAL_OBSOLETE_INPUT    (-3)
#define RETVAL_EOF_IN            (-4)
#define RETVAL_EOF_OUT           (-5)
#define RETVAL_OUT_OF_MEMORY     (-6)   /* libxmp addition */

// This is what we know about each huffman coding group
struct group_data {
  // limit and base are 1-indexed. index 0 is never used but increasing
  // the length by 1 simplifies the code and isn't that much of a waste.
  int limit[1+MAX_HUFCODE_BITS+1], base[1+MAX_HUFCODE_BITS+1], permute[MAX_SYMBOLS];
  char minLen, maxLen;
};

// Data for burrows wheeler transform

struct bwdata {
  unsigned int origPtr;
  int byteCount[256];
  // State saved when interrupting output
  int writePos, writeRun, writeCount, writeCurrent;
  unsigned int dataCRC, headerCRC;
  unsigned int *dbuf;
};

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4324) /* structure was padded due to alignment specifier */
#endif
// Structure holding all the housekeeping data, including IO buffers and
// memory that persists between calls to bunzip
struct bunzip_data {
  // Input stream, input buffer, input bit buffer
  HIO_HANDLE *in_fd; /* libxmp: int -> HIO_HANDLE * */
  int inbufCount, inbufPos;
  unsigned char *inbuf; /* libxmp: char -> unsigned char */
  unsigned int inbufBitCount, inbufBits;

  // Output buffer
  char outbuf[IOBUF_SIZE];
  int outbufPos;

  unsigned int totalCRC;

  // First pass decompression data (Huffman and MTF decoding)
  char selectors[32768];                  // nSelectors=15 bits
  struct group_data groups[MAX_GROUPS];   // huffman coding tables
  int symTotal, groupCount, nSelectors;
  unsigned char symToByte[256], mtfSymbol[256];

  // The CRC values stored in the block header and calculated from the data
  unsigned int crc32Table[256];

  // Second pass decompression data (burrows-wheeler transform)
  unsigned int dbufSize;
  struct bwdata bwdata[THREADS];

  /* libxmp: For I/O error handling */
  jmp_buf jmpbuf;
};
#if defined(_MSC_VER)
#pragma warning (pop)
#endif

/* libxmp addition */
struct bunzip_output {
  unsigned char *buf;
  size_t buf_size;
  size_t buf_alloc;
};

static void crc_init(unsigned *crc_table, int little_endian)
{
  unsigned int i;

  // Init the CRC32 table (big endian)
  for (i=0; i<256; i++) {
    unsigned int j, c = little_endian ? i : i<<24;
    for (j=8; j; j--)
      if (little_endian) c = (c&1) ? (c>>1)^0xEDB88320 : c>>1;
      else c=c&0x80000000 ? (c<<1)^0x04c11db7 : (c<<1);
    crc_table[i] = c;
  }
}

// Return the next nnn bits of input.  All reads from the compressed input
// are done through this function.  All reads are big endian.
static unsigned int get_bits(struct bunzip_data *bd, char bits_wanted)
{
  unsigned int bits = 0;

  // If we need to get more data from the byte buffer, do so.  (Loop getting
  // one byte at a time to enforce endianness and avoid unaligned access.)
  while (bd->inbufBitCount < bits_wanted) {

    // If we need to read more data from file into byte buffer, do so
    if (bd->inbufPos == bd->inbufCount) {
      if (0 >= (bd->inbufCount = hio_read(bd->inbuf, 1, IOBUF_SIZE, bd->in_fd))) /* libxmp: read -> hio_read */
        longjmp(bd->jmpbuf, RETVAL_EOF_IN);
      bd->inbufPos = 0;
    }

    // Avoid 32-bit overflow (dump bit buffer to top of output)
    if (bd->inbufBitCount>=24) {
      bits = bd->inbufBits&((1u<<bd->inbufBitCount)-1);
      bits_wanted -= bd->inbufBitCount;
      bits <<= bits_wanted;
      bd->inbufBitCount = 0;
    }

    // Grab next 8 bits of input from buffer.
    bd->inbufBits = (bd->inbufBits<<8) | bd->inbuf[bd->inbufPos++];
    bd->inbufBitCount += 8;
  }

  // Calculate result
  bd->inbufBitCount -= bits_wanted;
  bits |= (bd->inbufBits>>bd->inbufBitCount) & ((1<<bits_wanted)-1);

  return bits;
}

/* Read block header at start of a new compressed data block.  Consists of:
 *
 * 48 bits : Block signature, either pi (data block) or e (EOF block).
 * 32 bits : bw->headerCRC
 * 1  bit  : obsolete feature flag.
 * 24 bits : origPtr (Burrows-wheeler unwind index, only 20 bits ever used)
 * 16 bits : Mapping table index.
 *[16 bits]: symToByte[symTotal] (Mapping table.  For each bit set in mapping
 *           table index above, read another 16 bits of mapping table data.
 *           If correspondig bit is unset, all bits in that mapping table
 *           section are 0.)
 *  3 bits : groupCount (how many huffman tables used to encode, anywhere
 *           from 2 to MAX_GROUPS)
 * variable: hufGroup[groupCount] (MTF encoded huffman table data.)
 */

static int read_block_header(struct bunzip_data *bd, struct bwdata *bw)
{
  struct group_data *hufGroup;
  int hh, ii, jj, kk, symCount;
  unsigned char uc;

  // Read in header signature and CRC (which is stored big endian)
  ii = get_bits(bd, 24);
  jj = get_bits(bd, 24);
  bw->headerCRC = get_bits(bd,32);

  // Is this the EOF block with CRC for whole file?  (Constant is "e")
  if (ii==0x177245 && jj==0x385090) return RETVAL_LAST_BLOCK;

  // Is this a valid data block?  (Constant is "pi".)
  if (ii!=0x314159 || jj!=0x265359) return RETVAL_NOT_BZIP_DATA;

  // We can add support for blockRandomised if anybody complains.
  if (get_bits(bd,1)) return RETVAL_OBSOLETE_INPUT;
  if ((bw->origPtr = get_bits(bd,24)) > bd->dbufSize) return RETVAL_DATA_ERROR;

  // mapping table: if some byte values are never used (encoding things
  // like ascii text), the compression code removes the gaps to have fewer
  // symbols to deal with, and writes a sparse bitfield indicating which
  // values were present.  We make a translation table to convert the symbols
  // back to the corresponding bytes.
  hh = get_bits(bd, 16);
  bd->symTotal = 0;
  for (ii=0; ii<16; ii++) {
    if (hh & (1 << (15 - ii))) {
      kk = get_bits(bd, 16);
      for (jj=0; jj<16; jj++)
        if (kk & (1 << (15 - jj)))
          bd->symToByte[bd->symTotal++] = (16 * ii) + jj;
    }
  }

  // How many different huffman coding groups does this block use?
  bd->groupCount = get_bits(bd,3);
  if (bd->groupCount<2 || bd->groupCount>MAX_GROUPS) return RETVAL_DATA_ERROR;

  // nSelectors: Every GROUP_SIZE many symbols we switch huffman coding
  // tables.  Each group has a selector, which is an index into the huffman
  // coding table arrays.
  //
  // Read in the group selector array, which is stored as MTF encoded
  // bit runs.  (MTF = Move To Front.  Every time a symbol occurs it's moved
  // to the front of the table, so it has a shorter encoding next time.)
  bd->nSelectors = get_bits(bd, 15);
  if (!bd->nSelectors) return RETVAL_DATA_ERROR;
  for (ii=0; ii<bd->groupCount; ii++) bd->mtfSymbol[ii] = ii;
  for (ii=0; ii<bd->nSelectors; ii++) {

    // Get next value
    for(jj=0;get_bits(bd,1);jj++)
      if (jj>=bd->groupCount) return RETVAL_DATA_ERROR;

    // Decode MTF to get the next selector, and move it to the front.
    uc = bd->mtfSymbol[jj];
    memmove(bd->mtfSymbol+1, bd->mtfSymbol, jj);
    bd->mtfSymbol[0] = bd->selectors[ii] = uc;
  }

  // Read the huffman coding tables for each group, which code for symTotal
  // literal symbols, plus two run symbols (RUNA, RUNB)
  symCount = bd->symTotal+2;
  if (symCount < 1) return RETVAL_DATA_ERROR; /* libxmp: fix broken warning */
  for (jj=0; jj<bd->groupCount; jj++) {
    unsigned char length[MAX_SYMBOLS];
    unsigned temp[MAX_HUFCODE_BITS+1];
    int minLen, maxLen, pp;

    // Read lengths
    hh = get_bits(bd, 5);
    for (ii = 0; ii < symCount; ii++) {
      for(;;) {
        // !hh || hh > MAX_HUFCODE_BITS in one test.
        if (MAX_HUFCODE_BITS-1 < (unsigned)hh-1) return RETVAL_DATA_ERROR;
        // Grab 2 bits instead of 1 (slightly smaller/faster).  Stop if
        // first bit is 0, otherwise second bit says whether to
        // increment or decrement.
        kk = get_bits(bd, 2);
        if (kk & 2) hh += 1 - ((kk&1)<<1);
        else {
          bd->inbufBitCount++;
          break;
        }
      }
      length[ii] = hh;
    }

    // Find largest and smallest lengths in this group
    minLen = maxLen = length[0];
    for (ii = 1; ii < symCount; ii++) {
      if(length[ii] > maxLen) maxLen = length[ii];
      else if(length[ii] < minLen) minLen = length[ii];
    }

    /* Calculate permute[], base[], and limit[] tables from length[].
     *
     * permute[] is the lookup table for converting huffman coded symbols
     * into decoded symbols.  It contains symbol values sorted by length.
     *
     * base[] is the amount to subtract from the value of a huffman symbol
     * of a given length when using permute[].
     *
     * limit[] indicates the largest numerical value a symbol with a given
     * number of bits can have.  It lets us know when to stop reading.
     *
     * To use these, keep reading bits until value <= limit[bitcount] or
     * you've read over 20 bits (error).  Then the decoded symbol
     * equals permute[hufcode_value - base[hufcode_bitcount]].
     */
    hufGroup = bd->groups+jj;
    hufGroup->minLen = minLen;
    hufGroup->maxLen = maxLen;

    // zero temp[] and limit[], and calculate permute[]
    pp = 0;
    for (ii = minLen; ii <= maxLen; ii++) {
      temp[ii] = hufGroup->limit[ii] = 0;
      for (hh = 0; hh < symCount; hh++)
        if (length[hh] == ii) hufGroup->permute[pp++] = hh;
    }

    // Count symbols coded for at each bit length
    for (ii = 0; ii < symCount; ii++) temp[length[ii]]++;

    /* Calculate limit[] (the largest symbol-coding value at each bit
     * length, which is (previous limit<<1)+symbols at this level), and
     * base[] (number of symbols to ignore at each bit length, which is
     * limit minus the cumulative count of symbols coded for already). */
    pp = hh = 0;
    for (ii = minLen; ii < maxLen; ii++) {
      pp += temp[ii];
      hufGroup->limit[ii] = pp-1;
      pp <<= 1;
      hufGroup->base[ii+1] = pp-(hh+=temp[ii]);
    }
    hufGroup->limit[maxLen] = pp+temp[maxLen]-1;
    hufGroup->limit[maxLen+1] = INT_MAX;
    hufGroup->base[minLen] = 0;
  }

  return 0;
}

/* First pass, read block's symbols into dbuf[dbufCount].
 *
 * This undoes three types of compression: huffman coding, run length encoding,
 * and move to front encoding.  We have to undo all those to know when we've
 * read enough input.
 */

static int read_huffman_data(struct bunzip_data *bd, struct bwdata *bw)
{
  struct group_data *hufGroup;
  int ii, jj, kk, runPos, dbufCount, symCount, selector, nextSym,
    *byteCount;
  unsigned hh, *dbuf = bw->dbuf;
  unsigned char uc;

  // We've finished reading and digesting the block header.  Now read this
  // block's huffman coded symbols from the file and undo the huffman coding
  // and run length encoding, saving the result into dbuf[dbufCount++] = uc

  // Initialize symbol occurrence counters and symbol mtf table
  byteCount = bw->byteCount;
  for(ii=0; ii<256; ii++) {
    byteCount[ii] = 0;
    bd->mtfSymbol[ii] = ii;
  }

  // Loop through compressed symbols.  This is the first "tight inner loop"
  // that needs to be micro-optimized for speed.  (This one fills out dbuf[]
  // linearly, staying in cache more, so isn't as limited by DRAM access.)
  runPos = dbufCount = symCount = selector = 0;
  // Some unnecessary initializations to shut gcc up.
  hufGroup = NULL;
  hh = 0;

  for (;;) {
    // Have we reached the end of this huffman group?
    if (!(symCount--)) {
      // Determine which huffman coding group to use.
      symCount = GROUP_SIZE-1;
      if (selector >= bd->nSelectors) return RETVAL_DATA_ERROR;
      hufGroup = bd->groups + bd->selectors[selector++];
    }

    // Read next huffman-coded symbol (into jj).
    ii = hufGroup->minLen;
    jj = get_bits(bd, ii);
    while (jj > hufGroup->limit[ii]) {
      // if (ii > hufGroup->maxLen) return RETVAL_DATA_ERROR;
      ii++;

      // Unroll get_bits() to avoid a function call when the data's in
      // the buffer already.
      kk = bd->inbufBitCount
        ? (bd->inbufBits >> --(bd->inbufBitCount)) & 1 : get_bits(bd, 1);
      jj = (jj << 1) | kk;
    }
    // Huffman decode jj into nextSym (with bounds checking)
    jj-=hufGroup->base[ii];

    if (ii > hufGroup->maxLen || (unsigned)jj >= MAX_SYMBOLS)
      return RETVAL_DATA_ERROR;
    nextSym = hufGroup->permute[jj];

    // If this is a repeated run, loop collecting data
    if ((unsigned)nextSym <= SYMBOL_RUNB) {
      // If this is the start of a new run, zero out counter
      if(!runPos) {
        runPos = 1;
        hh = 0;
      }

      /* Neat trick that saves 1 symbol: instead of or-ing 0 or 1 at
         each bit position, add 1 or 2 instead. For example,
         1011 is 1<<0 + 1<<1 + 2<<2. 1010 is 2<<0 + 2<<1 + 1<<2.
         You can make any bit pattern that way using 1 less symbol than
         the basic or 0/1 method (except all bits 0, which would use no
         symbols, but a run of length 0 doesn't mean anything in this
         context). Thus space is saved. */
      hh += (runPos << nextSym); // +runPos if RUNA; +2*runPos if RUNB
      runPos <<= 1;
      continue;
    }

    /* When we hit the first non-run symbol after a run, we now know
       how many times to repeat the last literal, so append that many
       copies to our buffer of decoded symbols (dbuf) now. (The last
       literal used is the one at the head of the mtfSymbol array.) */
    if (runPos) {
      runPos = 0;
      // Check for integer overflow
      if (hh>bd->dbufSize || dbufCount+hh>bd->dbufSize)
        return RETVAL_DATA_ERROR;

      uc = bd->symToByte[bd->mtfSymbol[0]];
      byteCount[uc] += hh;
      while (hh--) dbuf[dbufCount++] = uc;
    }

    // Is this the terminating symbol?
    if (nextSym>bd->symTotal) break;

    /* At this point, the symbol we just decoded indicates a new literal
       character. Subtract one to get the position in the MTF array
       at which this literal is currently to be found. (Note that the
       result can't be -1 or 0, because 0 and 1 are RUNA and RUNB.
       Another instance of the first symbol in the mtf array, position 0,
       would have been handled as part of a run.) */
    if (dbufCount>=bd->dbufSize) return RETVAL_DATA_ERROR;
    ii = nextSym - 1;
    uc = bd->mtfSymbol[ii];
    // On my laptop, unrolling this memmove() into a loop shaves 3.5% off
    // the total running time.
    while(ii--) bd->mtfSymbol[ii+1] = bd->mtfSymbol[ii];
    bd->mtfSymbol[0] = uc;
    uc = bd->symToByte[uc];

    // We have our literal byte.  Save it into dbuf.
    byteCount[uc]++;
    dbuf[dbufCount++] = (unsigned int)uc;
  }

  // Now we know what dbufCount is, do a better sanity check on origPtr.
  if (bw->origPtr >= (bw->writeCount = dbufCount)) return RETVAL_DATA_ERROR;

  return 0;
}

static size_t next_power_of_two_32(size_t i)
{
  i |= i >> 16UL;
  i |= i >> 8UL;
  i |= i >> 4UL;
  i |= i >> 2UL;
  i |= i >> 1UL;
  return i + 1;
}

// Flush output buffer to disk
// libxmp: changed to output to memory instead of a file
static int flush_bunzip_outbuf(struct bunzip_data *bd, struct bunzip_output *out_fd)
{
  if (bd->outbufPos) {
    unsigned char *buf = out_fd->buf;

    if (bd->outbufPos > out_fd->buf_alloc - out_fd->buf_size) {
      size_t new_size = next_power_of_two_32(out_fd->buf_size + bd->outbufPos);
      if (new_size <= out_fd->buf_alloc || new_size > LIBXMP_DEPACK_LIMIT)
        return RETVAL_EOF_OUT;

      buf = (unsigned char *)realloc(buf, new_size);
      if (!buf)
        return RETVAL_EOF_OUT;

      out_fd->buf = buf;
      out_fd->buf_alloc = new_size;
    }
    memcpy(buf + out_fd->buf_size, bd->outbuf, bd->outbufPos);
    out_fd->buf_size += bd->outbufPos;
    bd->outbufPos = 0;
  }
  return 0;
}

static void burrows_wheeler_prep(struct bwdata *bw)
{
  int ii, jj;
  unsigned int *dbuf = bw->dbuf;
  int *byteCount = bw->byteCount;

  // Turn byteCount into cumulative occurrence counts of 0 to n-1.
  jj = 0;
  for (ii=0; ii<256; ii++) {
    int kk = jj + byteCount[ii];
    byteCount[ii] = jj;
    jj = kk;
  }

  // Use occurrence counts to quickly figure out what order dbuf would be in
  // if we sorted it.
  for (ii=0; ii < bw->writeCount; ii++) {
    unsigned char uc = dbuf[ii];
    dbuf[byteCount[uc]] |= (ii << 8);
    byteCount[uc]++;
  }

  // blockRandomised support would go here.

  // Using ii as position, jj as previous character, hh as current character,
  // and uc as run count.
  bw->dataCRC = 0xffffffffL;

  /* Decode first byte by hand to initialize "previous" byte. Note that it
     doesn't get output, and if the first three characters are identical
     it doesn't qualify as a run (hence uc=255, which will either wrap
     to 1 or get reset). */
  if (bw->writeCount) {
    bw->writePos = dbuf[bw->origPtr];
    bw->writeCurrent = (unsigned char)bw->writePos;
    bw->writePos >>= 8;
    bw->writeRun = -1;
  }
}

// Decompress a block of text to intermediate buffer
static int read_bunzip_data(struct bunzip_data *bd)
{
  int rc = read_block_header(bd, bd->bwdata);
  if (!rc) rc=read_huffman_data(bd, bd->bwdata);

  // First thing that can be done by a background thread.
  burrows_wheeler_prep(bd->bwdata);

  return rc;
}

// Undo burrows-wheeler transform on intermediate buffer to produce output.
// If !len, write up to len bytes of data to buf.  Otherwise write to out_fd.
// Returns len ? bytes written : 0.  Notice all errors are negative #'s.
//
// Burrows-wheeler transform is described at:
// http://dogma.net/markn/articles/bwt/bwt.htm
// http://marknelson.us/1996/09/01/bwt/

/* libxmp: int -> struct bunzip_output * */
static int write_bunzip_data(struct bunzip_data *bd, struct bwdata *bw,
  struct bunzip_output *out_fd, char *outbuf, const int len_)
{
  unsigned int *dbuf = bw->dbuf;
  int count, pos, current, run, copies, outbyte, previous, gotcount, len;

  /* libxmp: Reset longjmp I/O error handling */
  int ret = setjmp(bd->jmpbuf);
  if (ret) return ret;

  gotcount = 0;
  len = len_;

  for (;;) {
    // If last read was short due to end of file, return last block now
    if (bw->writeCount < 0) return bw->writeCount;

    // If we need to refill dbuf, do it.
    if (!bw->writeCount) {
      int i = read_bunzip_data(bd);
      if (i) {
        if (i == RETVAL_LAST_BLOCK) {
          bw->writeCount = i;
          return gotcount;
        } else return i;
      }
    }

    // loop generating output
    count = bw->writeCount;
    pos = bw->writePos;
    current = bw->writeCurrent;
    run = bw->writeRun;
    while (count) {

      // If somebody (like tar) wants a certain number of bytes of
      // data from memory instead of written to a file, humor them.
      if (len && bd->outbufPos >= len) goto dataus_interruptus;
      count--;

      // Follow sequence vector to undo Burrows-Wheeler transform.
      previous = current;
      pos = dbuf[pos];
      current = pos&0xff;
      pos >>= 8;

      // Whenever we see 3 consecutive copies of the same byte,
      // the 4th is a repeat count
      if (run++ == 3) {
        copies = current;
        outbyte = previous;
        current = -1;
      } else {
        copies = 1;
        outbyte = current;
      }

      // Output bytes to buffer, flushing to file if necessary
      while (copies--) {
        if (bd->outbufPos == IOBUF_SIZE) {
            int i = flush_bunzip_outbuf(bd, out_fd); /* libxmp: error checking */
            if (i) return i;
        }
        bd->outbuf[bd->outbufPos++] = outbyte;
        bw->dataCRC = (bw->dataCRC << 8)
                ^ bd->crc32Table[(bw->dataCRC >> 24) ^ outbyte];
      }
      if (current != previous) run=0;
    }

    // decompression of this block completed successfully
    bw->dataCRC = ~(bw->dataCRC);
    bd->totalCRC = ((bd->totalCRC << 1) | (bd->totalCRC >> 31)) ^ bw->dataCRC;

    // if this block had a crc error, force file level crc error.
    if (bw->dataCRC != bw->headerCRC) {
      bd->totalCRC = bw->headerCRC+1;

      return RETVAL_LAST_BLOCK;
    }
dataus_interruptus:
    bw->writeCount = count;
    if (len) {
      gotcount += bd->outbufPos;
      memcpy(outbuf, bd->outbuf, len);

      // If we got enough data, checkpoint loop state and return
      if ((len -= bd->outbufPos)<1) {
        bd->outbufPos -= len;
        if (bd->outbufPos) memmove(bd->outbuf, bd->outbuf+len, bd->outbufPos);
        bw->writePos = pos;
        bw->writeCurrent = current;
        bw->writeRun = run;

        return gotcount;
      }
    }
  }
}

// Allocate the structure, read file header. If !len, src_fd contains
// filehandle to read from. Else inbuf contains data.
/* libxmp: int -> HIO_HANDLE *, char -> unsigned char */
static int start_bunzip(struct bunzip_data **bdp, HIO_HANDLE *src_fd, unsigned char *inbuf,
  int len)
{
  struct bunzip_data *bd;
  unsigned int i;

  // Figure out how much data to allocate.
  i = sizeof(struct bunzip_data);
  if (!len) i += IOBUF_SIZE;

  // Allocate bunzip_data. Most fields initialize to zero.
  bd = *bdp = (struct bunzip_data *)calloc(1, i); /* libxmp: xzalloc -> calloc + error checking */
  if (!bd) return RETVAL_OUT_OF_MEMORY;
  if (len) {
    bd->inbuf = inbuf;
    bd->inbufCount = len;
    bd->in_fd = NULL;
  } else {
    bd->inbuf = (unsigned char *)(bd+1); /* libxmp: char -> unsigned char */
    bd->in_fd = src_fd;
  }

  crc_init(bd->crc32Table, 0);

  /* libxmp: Setup for I/O error handling via longjmp */
  i = setjmp(bd->jmpbuf);
  if (i) return i;

  // Ensure that file starts with "BZh".
  for (i=0;i<3;i++) if (get_bits(bd,8)!="BZh"[i]) return RETVAL_NOT_BZIP_DATA;

  // Next byte ascii '1'-'9', indicates block size in units of 100k of
  // uncompressed data. Allocate intermediate buffer for block.
  i = get_bits(bd, 8);
  if (i<'1' || i>'9') return RETVAL_NOT_BZIP_DATA;
  bd->dbufSize = 100000*(i-'0')*THREADS;
  for (i=0; i<THREADS; i++) {
    bd->bwdata[i].dbuf = (unsigned int *)malloc(bd->dbufSize * sizeof(int)); /* libxmp: xmalloc -> malloc + error checking */
    if (!bd->bwdata[i].dbuf) return RETVAL_OUT_OF_MEMORY;
  }

  return 0;
>>>>>>> db7344ebf (abc)
}

static int test_bzip2(unsigned char *b)
{
	return b[0] == 'B' && b[1] == 'Z' && b[2] == 'h';
}

<<<<<<< HEAD
/* Example usage: decompress src_fd to dst_fd.  (Stops at end of bzip data,
   not end of file.) */
static int decrunch_bzip2(HIO_HANDLE *src, FILE *dst, long inlen)
{
	char *outbuf;
	bunzip_data *bd;
	int i;

	libxmp_crc32_init_B();

	outbuf=(char *)malloc(IOBUF_SIZE);
	if(!outbuf) return RETVAL_OUT_OF_MEMORY;
	if(!(i=start_bunzip(&bd,src,0,0))) {
		for(;;) {
			if((i=read_bunzip(bd,outbuf,IOBUF_SIZE)) <= 0) break;
			if(i!=fwrite(outbuf,1,i,dst)) {
				i=RETVAL_UNEXPECTED_OUTPUT_EOF;
				break;
			}
		}
	}
	/* Check CRC and release memory */
	if(i==RETVAL_LAST_BLOCK && bd->headerCRC==bd->totalCRC) i=RETVAL_OK;
	if(bd->dbuf) free(bd->dbuf);
	free(bd);
	free(outbuf);
	return i == 0 ? 0 : -1;
}

struct depacker libxmp_depacker_bzip2 = {
	test_bzip2,
	decrunch_bzip2,
	NULL
=======
static int decrunch_bzip2(HIO_HANDLE *in, void **out, long *outlen)
{
  struct bunzip_data *bd;
  struct bunzip_output output;
  int i, j;

  output.buf = NULL;
  output.buf_size = 0;
  output.buf_alloc = 0;

  i = start_bunzip(&bd, in, NULL, 0);
  if (!i) {
    i = write_bunzip_data(bd, bd->bwdata, &output, NULL, 0);
    if (i==RETVAL_LAST_BLOCK) {
      if (bd->bwdata[0].headerCRC==bd->totalCRC) i = 0;
      else i = RETVAL_DATA_ERROR;
    }
  }
  if (!i) i = flush_bunzip_outbuf(bd, &output);

  for (j=0; j<THREADS; j++) free(bd->bwdata[j].dbuf);
  free(bd);

  if (i != 0) {
    free(output.buf);
    return -1;
  }

  /* Shrink allocation */
  if (output.buf_size < output.buf_alloc) {
    unsigned char *tmp = (unsigned char *)realloc(output.buf, output.buf_size);
    if (tmp)
      output.buf = tmp;
  }

  *out = output.buf;
  *outlen = output.buf_size;

  return 0;
}

const struct depacker libxmp_depacker_bzip2 = {
	test_bzip2,
	NULL,
	decrunch_bzip2
>>>>>>> db7344ebf (abc)
};
