// arc4.cpp - written and placed in the public domain by Wei Dai

// The ARC4 algorithm was first revealed in an anonymous email to the
// cypherpunks mailing list. This file originally contained some
// code copied from this email. The code has since been rewritten in order
// to clarify the copyright status of this file. It should now be
// completely in the public domain.

#include "arc4.h"

unsigned int Parity(unsigned long value)
{
    for (unsigned int i=8*sizeof(value)/2; i>0; i/=2)
	value ^= value >> i;
    return (unsigned int)value&1;
}

unsigned int BytePrecision(unsigned long value)
{
    unsigned int i;
    for (i=sizeof(value); i; --i)
	if (value >> (i-1)*8)
	    break;

    return i;
}

unsigned int BitPrecision(unsigned long value)
{
    if (!value)
	return 0;

    unsigned int l=0, h=8*sizeof(value);

    while (h-l > 1)
    {
	unsigned int t = (l+h)/2;
	if (value >> t)
	    l = t;
 	else
	    h = t;
    }

    return h;
}

unsigned long Crop(unsigned long value, unsigned int size)
{
    if (size < 8*sizeof(value))
	return (value & ((1L << size) - 1));
    else
	return value;
}

unsigned int RandomNumberGenerator::GenerateBit()
{
    return Parity(GetByte());
}

void RandomNumberGenerator::GenerateBlock(byte *output, unsigned int size)
{
    while (size--)
	*output++ = GetByte();
}

word32 RandomNumberGenerator::GenerateWord32(word32 min, word32 max)
{
    word32 range = max-min;
    const int maxBytes = BytePrecision(range);
    const int maxBits = BitPrecision(range);

    word32 value;

    do
    {
	value = 0;
	for (int i=0; i<maxBytes; i++)
	    value = (value << 8) | GetByte();

	value = Crop(value, maxBits);
    } while (value > range);

    return value+min;
}

void StreamCipher::ProcessString(byte *outString, const byte *inString, unsigned int length)
{
    while(length--)
	*outString++ = ProcessByte(*inString++);
}

void StreamCipher::ProcessString(byte *inoutString, unsigned int length)
{
    while(length--)
	*inoutString++ = ProcessByte(*inoutString);
}


ARC4::ARC4(const byte *key, unsigned int keyLen)
	: m_state(256), m_x(0), m_y(0)
{
	unsigned int i;
	for (i=0; i<256; i++)
		m_state[i] = i;

	unsigned int keyIndex = 0, stateIndex = 0;
	for (i=0; i<256; i++)
	{
		unsigned int a = m_state[i];
		stateIndex += key[keyIndex] + a;
		stateIndex &= 0xff;
		m_state[i] = m_state[stateIndex];
		m_state[stateIndex] = a;
		if (++keyIndex >= keyLen)
			keyIndex = 0;
	}
}

ARC4::~ARC4()
{
	m_x=0;
	m_y=0;
}

byte ARC4::GenerateByte()
{
	m_x = (m_x+1) & 0xff;
	unsigned int a = m_state[m_x];
	m_y = (m_y+a) & 0xff;
	unsigned int b = m_state[m_y];
	m_state[m_x] = b;
	m_state[m_y] = a;
	return m_state[(a+b) & 0xff];
}

byte ARC4::ProcessByte(byte input)
{
	return input ^ ARC4::GenerateByte();
}

void ARC4::ProcessString(byte *outString, const byte *inString, unsigned int length)
{
	byte *const s=m_state;
	unsigned int x = m_x;
	unsigned int y = m_y;

	while(length--)
	{
		x = (x+1) & 0xff;
		unsigned int a = s[x];
		y = (y+a) & 0xff;
		unsigned int b = s[y];
		s[x] = b;
		s[y] = a;
		*outString++ = *inString++ ^ s[(a+b) & 0xff];
	}

	m_x = x;
	m_y = y;
}

void ARC4::ProcessString(byte *inoutString, unsigned int length)
{
	byte *const s=m_state;
	unsigned int x = m_x;
	unsigned int y = m_y;

	while(length--)
	{
		x = (x+1) & 0xff;
		unsigned int a = s[x];
		y = (y+a) & 0xff;
		unsigned int b = s[y];
		s[x] = b;
		s[y] = a;
		*inoutString++ ^= s[(a+b) & 0xff];
	}

	m_x = x;
	m_y = y;
}

MARC4::MARC4(const byte *userKey, unsigned int keyLength, unsigned int discardBytes)
	: ARC4(userKey, keyLength)
{
	while (discardBytes--)
		MARC4::GenerateByte();
}

/*
}

*/
