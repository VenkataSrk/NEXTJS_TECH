#ifndef CRYPTOPP_ARC4_H
#define CRYPTOPP_ARC4_H

#include <algorithm>	//linux porting
#include <assert.h>	//linux porting

typedef unsigned char byte;
typedef unsigned short word16;
typedef word16 word;
typedef unsigned long word32;

#define SecAlloc(T,sz)	new T[sz]
#define SecFree(P,sz)	delete [] P

//! a block of memory allocated using SecAlloc
template <class T> struct SecBlock
{
    explicit SecBlock(unsigned int size=0)
	: size(size) {ptr = SecAlloc(T, size);}
    SecBlock(const SecBlock<T> &t)
	: size(t.size) {ptr = SecAlloc(T, size); memcpy(ptr, t.ptr, size*sizeof(T));}
    SecBlock(const T *t, unsigned int len)
	: size(len) {ptr = SecAlloc(T, len); memcpy(ptr, t, len*sizeof(T));}
    ~SecBlock()
	{SecFree(ptr, size);}

#if defined(__GNUC__) || defined(__BCPLUSPLUS__)
    operator const void *() const
	{return ptr;}
    operator void *()
	{return ptr;}
#endif
#if defined(__GNUC__)	// reduce warnings
    operator const void *()
	{return ptr;}
#endif

    operator const T *() const
	{return ptr;}
    operator T *()
	{return ptr;}
#if defined(__GNUC__)	// reduce warnings
    operator const T *()
	{return ptr;}
#endif

// CodeWarrior defines _MSC_VER
#if !defined(_MSC_VER) || defined(__MWERKS__)
    template <typename I>
    T *operator +(I offset)
	{return ptr+offset;}

    template <typename I>
    const T *operator +(I offset) const
	{return ptr+offset;}

    template <typename I>
    T& operator[](I index)
	{assert(index<size); return ptr[index];}

    template <typename I>
    const T& operator[](I index) const
	{assert(index<size); return ptr[index];}
#endif

    const T* Begin() const
	{return ptr;}
    T* Begin()
	{return ptr;}
    const T* End() const
	{return ptr+size;}
    T* End()
	{return ptr+size;}

    unsigned int Size() const {return size;}

    void Assign(const T *t, unsigned int len)
    {
	New(len);
	memcpy(ptr, t, len*sizeof(T));
    }

    void Assign(const SecBlock<T> &t)
    {
	New(t.size);
	memcpy(ptr, t.ptr, size*sizeof(T));
    }

    SecBlock& operator=(const SecBlock<T> &t)
    {
	Assign(t);
	return *this;
    }

    bool operator==(const SecBlock<T> &t) const
    {
	return size == t.size && memcmp(ptr, t.ptr, size*sizeof(T)) == 0;
    }

    bool operator!=(const SecBlock<T> &t) const
    {
	return !operator==(t);
    }

    void New(unsigned int newSize)
    {
	if (newSize != size)
	{
	    T *newPtr = SecAlloc(T, newSize);
	    SecFree(ptr, size);
	    ptr = newPtr;
	    size = newSize;
	}
    }

    void CleanNew(unsigned int newSize)
    {
	if (newSize != size)
	{
	    T *newPtr = SecAlloc(T, newSize);
	    SecFree(ptr, size);
	    ptr = newPtr;
	    size = newSize;
	}
	memset(ptr, 0, size*sizeof(T));
    }

    void Grow(unsigned int newSize)
    {
	if (newSize > size)
	{
	    T *newPtr = SecAlloc(T, newSize);
	    memcpy(newPtr, ptr, size*sizeof(T));
	    SecFree(ptr, size);
	    ptr = newPtr;
	    size = newSize;
	}
    }

    void CleanGrow(unsigned int newSize)
    {
	if (newSize > size)
	{
	    T *newPtr = SecAlloc(T, newSize);
	    memcpy(newPtr, ptr, size*sizeof(T));
	    memset(newPtr+size, 0, (newSize-size)*sizeof(T));
	    SecFree(ptr, size);
	    ptr = newPtr;
	    size = newSize;
	}
    }

    void Resize(unsigned int newSize)
    {
	if (newSize != size)
	{
	    T *newPtr = SecAlloc(T, newSize);
	    memcpy(newPtr, ptr, STDMIN(newSize, size)*sizeof(T));
	    SecFree(ptr, size);
	    ptr = newPtr;
	    size = newSize;
	}
    }

    void swap(SecBlock<T> &b);

    unsigned int size;
    T *ptr;
};

template <class T> void SecBlock<T>::swap(SecBlock<T> &b)
{
    std::swap(size, b.size);
    std::swap(ptr, b.ptr);
}

typedef SecBlock<byte> SecByteBlock;
typedef SecBlock<word> SecWordBlock;

class StreamCipher
{
public:
    //!
    virtual ~StreamCipher() {}

    //! encrypt or decrypt one byte
    virtual byte ProcessByte(byte input) =0;

    //! encrypt or decrypt an array of bytes of specified length in place
    virtual void ProcessString(byte *inoutString, unsigned int length);
    //! encrypt or decrypt an array of bytes of specified length, may assume inString != outString
    virtual void ProcessString(byte *outString, const byte *inString, unsigned int length);
};


class RandomNumberGenerator
{
public:
    //!
    virtual ~RandomNumberGenerator() {}

    //! generate new random byte and return it
    virtual byte GenerateByte() =0;

    //! generate new random bit and return it
    /*! Default implementation is to call GenerateByte() and return its parity. */
    virtual unsigned int GenerateBit();

    //! generate a random 32 bit word in the range min to max, inclusive
    virtual word32 GenerateWord32(word32 a=0, word32 b=0xffffffffL);

    //! generate random array of bytes
    /*! Default implementation is to call GenerateByte() size times. */
    virtual void GenerateBlock(byte *output, unsigned int size);

    //! randomly shuffle the specified array, resulting permutation is uniformly distributed
    template <class IT> void Shuffle(IT begin, IT end)
    {
	for (; begin != end; ++begin)
	    std::iter_swap(begin, begin + GenerateWord32(0, end-begin-1));
    }

    // for backwards compatibility, maybe be remove later
    byte GetByte() {return GenerateByte();}
    unsigned int GetBit() {return GenerateBit();}
    word32 GetLong(word32 a=0, word32 b=0xffffffffL) {return GenerateWord32(a, b);}
    word16 GetShort(word16 a=0, word16 b=0xffff) {return (word16)GenerateWord32(a, b);}
    void GetBlock(byte *output, unsigned int size) {GenerateBlock(output, size);}
};


/// support query of variable key length, template parameters are default, min, max, multiple (default multiple 1)
template <unsigned int D, unsigned int N, unsigned int M, unsigned int Q=1>
class VariableKeyLength
{
public:
    enum {MIN_KEYLENGTH=N, MAX_KEYLENGTH=M, DEFAULT_KEYLENGTH=D, KEYLENGTH_MULTIPLE=Q};
    /// returns the smallest valid key length in bytes that is >= min(n, MAX_KEYLENGTH)
    static unsigned int KeyLength(unsigned int n)
    {
	assert(KEYLENGTH_MULTIPLE > 0 && MIN_KEYLENGTH % KEYLENGTH_MULTIPLE == 0 && MAX_KEYLENGTH % KEYLENGTH_MULTIPLE == 0);
	if (n < MIN_KEYLENGTH)
	    return MIN_KEYLENGTH;
	else if (n > MAX_KEYLENGTH)
	    return MAX_KEYLENGTH;
	else
	    return RoundUpToMultipleOf(n, KEYLENGTH_MULTIPLE);
    }
};

//! <a href="http://www.weidai.com/scan-mirror/cs.html#RC4">Alleged RC4</a>
class ARC4 : public RandomNumberGenerator, 
	     public StreamCipher, 
	     public VariableKeyLength<16, 1, 256>
{
public:
    ARC4(const byte *userKey, unsigned int keyLength=DEFAULT_KEYLENGTH);
    ~ARC4();

    byte GenerateByte();

    byte ProcessByte(byte input);
    void ProcessString(byte *outString, const byte *inString, unsigned int length);
    void ProcessString(byte *inoutString, unsigned int length);

private:
    SecByteBlock m_state;
    byte m_x, m_y;
};

//! Modified ARC4: it discards the first 256 bytes of keystream which may be weaker than the rest
class MARC4 : public ARC4
{
public:
    MARC4(const byte *userKey, unsigned int keyLength=DEFAULT_KEYLENGTH, unsigned int discardBytes=256);
};


#endif
