/*              secure hash algorithm 1 (sha1)
**
**              THIS APPLICATION NOTE IS SUBJECT TO THE
**              'CONDITIONS FOR USE OF APPLICATION NOTES'
**
**              joachim velten  11-nov-1999
**
**              (c)  1999  infineon technologies cc at sw
**
**              11-nov-1999: initial creation  jv
*/

/*              derived from 
**              SHA-1 in C by Steve Reid <steve@edmweb.com>, 100% Public Domain
**              and the cc application note sha1  V1.03: 13.01.1998
*/

/*              Test Vectors (from FIPS PUB 180-1)
**              "abc"
**              A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
**
**              "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
**              84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
**
**              A million repetitions of "a"
**              34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/


#include <string.h>
#include <intrins.h>

#include <sha1.h>

//SHA1结构体全局变量
SHA1_CTX	xdata		Sha1_Context;
	
static void SHA1Transform( unsigned long * state /*[5]*/, unsigned char * buffer /*[64]*/ )  {
	
	/*      Hash a single 512-bit block. This is the core of the algorithm
	*/
	
	xdata  unsigned  long a;
	xdata  unsigned  long b;
	xdata  unsigned  long c;
	xdata unsigned   long d;
	xdata unsigned   long e;
	
	typedef union {
		unsigned  char c[64];
		unsigned  long l[16];
	} CHAR64LONG16;
	
	xdata CHAR64LONG16 * block;
	
	xdata unsigned   char t;
	xdata unsigned   char s;
	xdata unsigned   long kt;
	xdata unsigned   long temp;
	
	block = ( CHAR64LONG16 * ) buffer;
	
	/*      Copy context->state[] to working vars 
	*/
	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];
	e = state[4];
	
	for ( t = 0; t < 80; t ++ ) 
	{
        
		/* set constant kt 
		*/
		switch( t ) 
		{  
			
		case 0:
			kt = 0x5A827999;
			break;
		case 20:
			kt = 0x6ED9EBA1;
			break;
		case 40:
			kt = 0x8F1BBCDC;
			break;
		case 60:
			kt = 0xCA62C1D6;
			break;
		}
		
		s = t & 15;
		
		if( t >= 16 ) 
		{
			
			( ( unsigned long * ) buffer ) [s] = _lrol_(
				( ( unsigned long * ) buffer ) [(s+13) & 15] ^
				( ( unsigned long * ) buffer ) [(s+8) & 15] ^
				( ( unsigned long * ) buffer ) [(s+2) & 15] ^
				( ( unsigned long * ) buffer ) [s],1
				);
		}
		
		/*      calculate function ft(B,C,D)
		*/
		if( t < 20 ) 
		{
			
			temp = ( b & c ) | ( ( ~b ) & d );
		}
		else if( t < 40 ) 
		{
			
			temp = b ^ c ^ d;
		}
		else if( t < 60 ) 
		{
			
			temp = ( b & c ) | ( b & d ) | ( c & d );
		}
		else {
			
			temp = b ^ c ^ d;
		}
		
		temp += _lrol_( a, 5 ) + e + ( ( unsigned long * ) buffer )[s] + kt;
		e = d;
		d = c;
		c = _lrol_( b, 30 );
		b = a;
		a = temp;
	}
	
	/*      Add the working vars back into context.state[] 
	*/
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;
	
	/*      Wipe variables 
	*/
	a = b = c = d = e = 0;
 }
 
 
 void SHA1Init( SHA1_CTX * context )  
 {
	 
	/*      SHA1 initialization
	 */
	 
	 context->state[0] = 0x67452301;
	 context->state[1] = 0xEFCDAB89;
	 context->state[2] = 0x98BADCFE;
	 context->state[3] = 0x10325476;
	 context->state[4] = 0xC3D2E1F0;
	 context->count[0] = context->count[1] = 0;
 }
 
 
 void SHA1Update( SHA1_CTX * context, unsigned char * const dat, unsigned int len )  
 {
	 
	/*      Run your data through this
	 */
	 
	 xdata unsigned  int i;
	 xdata unsigned char j;
	 
	 j = ( unsigned int ) context->count[0] >> 3 & 63; // need only lowest 11 bits
	 if( ( context->count[0] += ( unsigned long ) len << 3 ) < ( unsigned long ) len << 3 ) context->count[1] ++; 
	 
	 // context->count[1] += len >> 29; // always 0 because we only support 16 bits len
	 if( j + len > 63 ) 
	 {
		 memcpy( context->buffer + j, dat, i = 64 - j );
		 SHA1Transform( context->state, context->buffer );
		 for ( ; i + 63 < len; i += 64 ) {
			 memcpy( context->buffer + 0, dat + i, 64 ); // must copy dat, may be in e2 or rom space !
			 SHA1Transform( context->state, context->buffer );
		 }
		 j = 0;
	 }
	 else 
	 {
		 i = 0;
	 }
	 
	 memcpy( context->buffer + j, dat + i, len - i );
 }
 
 
 void SHA1Final( unsigned char * digest /*[20]*/, SHA1_CTX * context)  
 {
	 
	/*      Add padding and return the message digest
	 */
	 
	 xdata unsigned char i;
	 xdata unsigned char finalcount[8];
	 
	 for( i = 0; i < 8; i++ ) 
	 {
		 
		 finalcount[i] = ( unsigned char ) ( (context->count[(i >= 4 ? 0 : 1)] >> ( ( 3 - ( i & 3 ) ) * 8 ) ) & 255 ); 
	 }
	 
	 SHA1Update( context, ( unsigned char * ) "\200", 1 );
	 
	 while( (context->count[0] & 504) != 448 ) 
	 {
		 
		 SHA1Update( context, (unsigned char *)"\0", 1 );
	 }
	 
	 
	 SHA1Update( context, finalcount, 8 );
	 
	 /* Wipe variables 
	 */
	 memset( context->buffer, 0, 64 );
	 memset( context->count, 0, 8 );
	 memset( & finalcount, 0, 8 );
	 
	 for( i = 0; i < 20; i++ ) 
	 {
		 
		 context->buffer[i] = (unsigned char) ( (context->state[i>>2] >> ((3-(i & 3)) * 8) ) & 255 );
	 }
	 
	 memset( context->state, 0, 20 );
	 memcpy( digest, context->buffer, 20 );
}
 
