#include <string.h>
#include <sha1.h>

SHA1_CTX			Sha1_Context;

unsigned long _lrol2_(unsigned long c, unsigned char b)
{
    unsigned long left;
    unsigned long right;
    unsigned long temp;
    b = b%32;
    left=c<<b;
    right=c>>( 32-b);
    temp=left|right;
    return temp;
}

unsigned long _lrol_(unsigned long c, unsigned char b)
{
    unsigned long left;
    unsigned long right;
    unsigned long temp;
    unsigned char temp_c[4];
    temp_c[0] = (unsigned char)c;
    temp_c[1] = (unsigned char)(c>>8);			//小端数据转换
    temp_c[2] = (unsigned char)(c>>16);
    temp_c[3] = (unsigned char)(c>>24);
    c = temp_c[0];
    c = c<<8;
    c += temp_c[1];
    c = c<<8;
    c += temp_c[2];
    c = c<<8;
    c += temp_c[3];
    b = b%32;
    left=c<<b;
    right=c>>( 32-b);
    temp=left|right;
    temp_c[0] = (unsigned char)temp;
    temp_c[1] = (unsigned char)(temp>>8);
    temp_c[2] = (unsigned char)(temp>>16);
    temp_c[3] = (unsigned char)(temp>>24);
    temp = temp_c[0];
    temp = temp<<8;
    temp += temp_c[1];
    temp = temp<<8;
    temp += temp_c[2];
    temp = temp<<8;
    temp += temp_c[3];
    return temp;
}

void SHA1Init( SHA1_CTX * context )
{
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = context->count[1] = 0;
}

void SHA1Update( SHA1_CTX * context, unsigned char * const dat, unsigned long len )
{
    unsigned  short i;
    unsigned char j;
    j = ( unsigned short ) context->count[0] >> 3 & 63; // need only lowest 11 bits
    if( ( context->count[0] += ( unsigned long ) len << 3 ) < ( unsigned long ) len << 3 ) context->count[1] ++;

    if( (j + len) > 63 )
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
    unsigned char i;
    unsigned char finalcount[8];

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

void SHA1Transform( unsigned long * state /*[5]*/, unsigned char * buffer /*[64]*/ )
{
    unsigned  long buf_temp;
    unsigned char a_temp[4];
    unsigned  long a;
    unsigned  long b;
    unsigned  long c;
    unsigned   long d;
    unsigned   long e;

    unsigned   char t;
    unsigned   char s;
    unsigned   long kt;
    unsigned   long temp;
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    for ( t = 0; t < 80; t ++ )
    {
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
        s =(unsigned char) (t & 15);
        if( t >= 16 )
        {
            ( ( unsigned long * ) buffer ) [s] = _lrol_(
                    ( ( unsigned long * ) buffer ) [(s+13) & 15] ^
                    ( ( unsigned long * ) buffer ) [(s+8) & 15] ^
                    ( ( unsigned long * ) buffer ) [(s+2) & 15] ^
                    ( ( unsigned long * ) buffer ) [s],1
                                                 );
        }

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

        buf_temp = ( ( unsigned long * ) buffer )[s];		//小端数据转换
        a_temp[0] = (unsigned char)buf_temp;
        a_temp[1] = (unsigned char)(buf_temp>>8);
        a_temp[2] = (unsigned char)(buf_temp>>16);
        a_temp[3] = (unsigned char)(buf_temp>>24);

        buf_temp = a_temp[0];
        buf_temp = buf_temp<<8;
        buf_temp += a_temp[1];
        buf_temp = buf_temp<<8;
        buf_temp += a_temp[2];
        buf_temp = buf_temp<<8;
        buf_temp += a_temp[3];

        temp += _lrol2_( a, 5 ) + e + buf_temp + kt;

        e = d;
        d = c;
        c = _lrol2_( b, 30 );
        b = a;
        a = temp;
    }
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    a = b = c = d = e = 0;
}
