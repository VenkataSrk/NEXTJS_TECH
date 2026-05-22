/* $Header: /xGate/variant.h 9     6/01/01 4:18p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * VARIANT DATA TYPE CLASS AND VARIANT VALIDATORS.
 *
 * $Log: /xGate/variant.h $
 * 
 * 9     6/01/01 4:18p Bennylp
 * Changed some members to inline.
 * 
 * 8     5/23/01 8:12p Bennylp
 * Changed SetValue to use const parameter.
 * 
 * 7     4/27/01 6:03a Bennylp
 * Add CVariantDynamicChoiceValidator.
 * 
 * 6     4/23/01 10:47p Bennylp
 * Fixed std namespace conflicts.
 * 
 * 5     4/17/01 10:06p Bennylp
 * Support user defined validator.
 * 
 * 4     4/12/01 8:09p Bennylp
 * Integration.
 *
 */

#ifndef __CVARIANT_H__
#define __CVARIANT_H__




#include "types.h"
#include "string.h"
#include "log.h"


typedef std::pair<String,String> v_string_pair;

//
// Define this to include implicit conversion operators in CVariant,
// which can be dangerous.
//
//#define __LEGACY_CTENGINE


/**
  * Variant data type
  */
struct CVariant {
    
    CVariant() 
	: m_Type(VT_VOID), m_Size(0), m_Value(0) {
    }
    
    virtual ~CVariant() {
	Free(); 
    }
    
    CVariant(const int ival) 
	: m_Type(VT_INT), m_Value((void*)ival), m_Size(0) {
	
    }
    
    CVariant(const char *sval)
	: m_Type(VT_STRING), m_Size(0) {
	
	NEW_RETURN_ (m_Value, char[strlen(sval)+1]); 
	if (m_Value)
	    strcpy((char*)m_Value, sval); 
	
    }
    
    CVariant(const float dval)
	: m_Type(VT_FLOAT), m_Size(0) { 
	
	memcpy(&m_Value, &dval, sizeof(float)); 
	
    }
    
    CVariant(const CVariant & v)
	: m_Type(VT_VOID), m_Value(0) {
	
	SetValue(v);
	
    }
    
    
    CVariant(const void *v, size_t len)
	: m_Type(VT_BINARY), m_Size(len) {
	
	NEW_RETURN_ (m_Value, char[len]);
	if (m_Value)
	    memcpy( m_Value, v, len);
	else
	    m_Size = 0;
    }
    
    
    CVariant & operator = (const CVariant& v)  { 
	SetValue(v); 
	return *this; 
    }
    
   bool operator ==(const CVariant& v) const {
	if (Type()!=v.Type() || Size()!=v.Size()) {
	    return false;
	}
	
	switch(m_Type) {
	case VT_STRING:
	    return !strcmp((char*)m_Value, (char*)v.m_Value);
	case VT_BINARY:
	    return !memcmp(m_Value, v.m_Value, Size());
	case VT_VOID:
	case VT_TABLE:
	    return true;
	    
	default:
	    return Value()==v.Value();
	}
    }
   
   /* Begin: Linux Porting */
   bool  operator !=(const CVariant&v) const { return !operator==(v); };
   int  operator < (const CVariant& ) const;
   bool  operator <=(const CVariant&v) const { return operator<(v) || operator==(v); };
   bool  operator > (const CVariant&v) const { return !operator<=(v); };
   bool  operator >=(const CVariant&v) const { return !operator<(v); };
   /* End: Linux Porting */
    
    /**
    Returns the integer value of VT_INT variant.
    Note that if the current value is not integer, this function does NOT make any
    conversion.
    */
    int AsInt() const {
	assert(Type()==VT_INT);
        //Linux Porting
        //int *x=(int *) Value();
	return (int)Value(); 
	//return *x;
    }
    
    /**
    Returns the String value of VT_STRING variant.
    Note that if the current value is not string, this function does NOT make any
    conversion.
    */
    const char* AsString() const {
	
	assert(Type()==VT_STRING);
	return static_cast<const char*>(m_Value); 
    }
    
    /**
    Returns the float value of VT_FLOAT variant.
    Note that if the current value is not float, this function does NOT make any
    conversion.
    */
    float AsFloat() const {
	assert(Type()==VT_FLOAT);
	float f;
	
	memcpy( &f, &m_Value, sizeof(float));
	return f;
    }
    
#ifdef __LEGACY_CTENGINE
    operator int() const {
	return AsInt();
    }
    operator float() const {
	return AsFloat();
    }
    operator const char *() const {
	return AsString();
    }
#endif
    
    /**
    Return the value of the variant as it is (pointer to void)
    */
    void* Value() const {
	return m_Value;
    }

    /**
      Assign binary value
      */
    void SetValue( const void *p, size_t len ) {
	// handle self assignment
	if (Value()==p)
	    return;
	
	Free();
	
	if (p) {
	    NEW_RETURN_ (m_Value, char[len]);
	    if (m_Value) {
		memcpy( m_Value, p, len);
		m_Size = len;
	    } else
		m_Size = 0;
	} else {
	    m_Value = 0;
	    m_Size = 0;
	}
	
	m_Type = VT_BINARY;
    }

    /**
      Variant types
      */
    // Don't change the sequence !!
    enum VType {
	VT_VOID,    // NULL
	VT_INT,	    // integer
	VT_FLOAT,   // float
	VT_STRING,  // String
	VT_TASK,    // current task will be passed as argument (for API)
	VT_RET,	    // return value (for API)
	VT_BINARY,  // binary (for configurator)
	VT_TABLE,   // table (for configurator)
	VT_USRTYPE,	// user defined type
	VT_BOOLEAN,	// boolean
	VT_LONG    //long
    };

    /**
      Return the variant type
      */
    VType Type() const {
	return m_Type;
    }

    /**
      Free resources allocated by the value, and clear the value (make it zero).
      The variant will be set as NULL.
      */
    void Clear() {
	Free();
	m_Type=VT_VOID;
	m_Value=(void*)0;
    }

    /**
      Check whether the variant has been assigned a value (is not NULL)
      */
    bool IsEmpty() const {
	return m_Type==VT_VOID;
    }

    /**
      Get the size/length of the value
      */
    size_t Size() const {
	if (Type()==VT_INT || Type()==VT_FLOAT)
	    return sizeof(int);
	else if (Type()==VT_STRING)
	    return strlen( (char*)m_Value);
	else if (Type()==VT_BINARY)
	    return m_Size;
	else
	    return 0;
    }
    
    /**
      Change the meaning of the value
      */
    void ChangeType(VType t) {
	m_Type = t;
    }

    /**
      Get String description of this type
      */
    const char *GetTypeStr() const {
	return GetTypeStr( Type() );
    }

    /**
      Get the short String description of this type
      */
    const char *GetShortTypeStr() const {
	return GetShortTypeStr( Type() );
    }

    /**
      Get String description of the variant type
      */
    static const char *GetTypeStr( VType );

    /**
      Get String description of the variant type
      */
    static const char *GetShortTypeStr( VType );

    /**
      Get the VType according to a String

      @return
      VT_VOID if the conversion failed
      */
    static VType GetType(const char *type_str);

    /**
      Create variant object based on a printed String value. For a binary variant,
      the string argument must be in format "HH HH HH", where H is a hex digit.

      @return
      zero if the conversion was success (the result argument will also be set)
      nonzero if the conversion failed (the result is undefined)
      */
    static int Factory(
	CVariant &result,	    // the resulted variant [output]
	VType type,		    // type of the variant to be created [input]
	const char* str_value	    // text [input]
	);

    /**
      Convert the value to string
      */
    String ToString() const;

    /**
      Print the variant value in a readable format
      */
    void Print(std::ostream& os) const;

    /**
      Create a VT_TABLE variant
      */
    static CVariant CreateTable();

protected:
    void*   m_Value;
    VType   m_Type;
    size_t  m_Size;

    void SetValue( int i) {
	Free();
	m_Value = (void *)i;
	m_Type = VT_INT;
    }
    
    void SetValue( float f) {
	Free();
	memcpy( &m_Value, &f, sizeof(float));
	m_Type = VT_FLOAT;
    }
    
    void SetValue( const char *s ) {
	// handle self assignment
	if ((const char*)Value()==s)
	    return;
	
	Free();
	
	if (s) {
	    NEW_RETURN_ (m_Value, char[strlen(s)+1]);
	    if (m_Value)
		strcpy( (char*)m_Value, s);
	} else
	    m_Value = 0;	
	
	m_Type = VT_STRING;
    }
    
    void SetValue( const CVariant & v) {
	if (v.Type()==VT_STRING) {
	    SetValue( (const char*) v.Value() );
	} else if (v.Type()==VT_BINARY) {
	    SetValue( v.Value(), v.Size());
	} else {
	    Free();
	    
	    m_Type = v.m_Type;
	    m_Value = v.m_Value;
	}
    }
    
    /** Free internal buffer allocated for this variant, if any */
    void    Free() {
	if (Type()==VT_STRING || Type()==VT_BINARY || m_Size) {
	    delete [] m_Value;
	    m_Size=0;
	}
    }
    
private:
    static IdNamePair	m_TypeName[], m_ShortTypeName[];
    
    /**
    Convert printed binary string to a binary
    The caller is responsible to delete the returned buffer.
    */
    static bool TextToBinary(
	const char *str_input,	// printed binary [input]
	char*& binary_output,	// binary buffer [output]
	size_t& binary_len	// binary buffer length [output]
	);
    
	/**
	Convert binary buffer to a printed text.
	The caller is responsible to delete the returned string.
    */
    static bool BinaryToText(
	const char *binary_input,   // binary buffer [input]
	size_t binary_len,	    // size of binary buffer [input]
	String& text_out		    // text [output]
	);
};


/**
 Variant validator can be used to test whether a variant value can be accepted
 according to various criteria. CVariantValidator is the base class for all
 kinds of validation criteria. This class will only check for the type of the 
 value to be validated.
 */
class CVariantValidator {

protected:
    /**
      Type of descendants. This enumeration will be used internally as
      typechecking for copy constructor and assignment
      */
    enum ValidatorType {
	Base,
	SingleValue,
	Choice,
	Range,
	User
    } m_ValidatorType;

public:
    /**
      Create the validator. The <code>value</code> argument will be used
      as the type criteria. The <code>Validate</code> method will check
      the variant argument whether it is matched with this value's type.
      */
    CVariantValidator(
	const CVariant& value,	// type of the variant
	const String & desc,	// description
	ValidatorType type=Base	// internal use
    );

    /** Destructor */
    virtual ~CVariantValidator();

    /** Create a duplicate */
    virtual CVariantValidator *Clone() const;

    /**
      Validate the variant value. For this class, this will only check
      the variant type, but descendant might do more checks.

      @return
      true if the value can be accepted
      false if the value can NOT be accepted
      */
    virtual bool Validate(const CVariant&) const;

    /**
      Give suggestion of the possible values, given the 'clue' as parameter.
      For example, when the validator is a CVariantChoiceValidator with 
      choices: "one", "two", "three", then if given "o" as the clue, this
      function will return "one" as the correct value.

      The default behaviour is to call Validate().

      @return
      true if a suggestion has been made
      false if a suggestion can not be created.
      */
    virtual bool Suggest(CVariant& v) const { return Validate(v); }

    /**
      Create a printable list of values (and their description) that
      can be accepted by this validator
      */
    virtual void GetPrintableValues(List<v_string_pair>& values) const;

protected:
    String m_Desc;

    /** Copy constructor */
    CVariantValidator(const CVariantValidator&);

    /** Assignment operator */
    const CVariantValidator& operator =(const CVariantValidator&);

private:
    CVariant::VType m_Type;
};

/**
  Validator that only accept a single value.
 */
class CVariantSingleValueValidator : public CVariantValidator {
public:
    /**
     The variant value specified in the argument will be the 
     only value accepted by the validator.
     */
    CVariantSingleValueValidator(const CVariant& the_value, const String & desc);
    ~CVariantSingleValueValidator();
    CVariantSingleValueValidator(const CVariantSingleValueValidator&);

    const CVariantSingleValueValidator& operator = 
	    (const CVariantSingleValueValidator & rhs);
    
    virtual CVariantValidator *Clone() const;
    virtual bool Validate(const CVariant&) const;
    virtual bool Suggest(CVariant&) const;
    virtual void GetPrintableValues(List<v_string_pair>& values) const;

private:
    CVariant m_TheValue;
};


/** A choice entry in the Variant choice validator.
    The entry consist of a value (CVariant) and a string description.
 */
typedef std::pair <CVariant, String> ValidatorChoice;

/**
 Choice validator that performs validation/suggestion from value choices
 that are constructed dynamically upon execution.
 */
class CVariantDynamicChoiceValidator : public CVariantValidator {
public:
    CVariantDynamicChoiceValidator();
    ~CVariantDynamicChoiceValidator();
    
    virtual bool Validate(const CVariant&) const;
    virtual bool Suggest(CVariant&) const;
    virtual void GetPrintableValues(List<v_string_pair>& values) const;

protected:
    CVariantDynamicChoiceValidator(const CVariantDynamicChoiceValidator&);

    const CVariantDynamicChoiceValidator& operator = 
	    (const CVariantDynamicChoiceValidator & rhs);

    // Implementation:

    /** Acquire mutex lock.
	This function will be called by the framework BEFORE calling
	GetChoiceList(). The lock will be kept until this class is finished
	processing all the choices in the choice list.
     */
    virtual void AcquireLock() const = 0;

    /** Release mutex lock.
	This function will be called by the framework AFTER calling
	GetChoiceList().
     */
    virtual void ReleaseLock() const = 0;

    /** Get list of choices available.
     */
    virtual const List<ValidatorChoice> & GetChoiceList() const = 0;
};


/**
 Entry of CVariantChoiceValidator. Actually we should put this structure 
 inside private area of CVariantChoiceValidator, but then it will 
 generate a lot of warnings with MSVC.
 */

/**
 Validator that is able to validate a choices of values
 */
class CVariantChoiceValidator : public CVariantDynamicChoiceValidator {
public:
    /**
     The variant value specified in the argument will be the
     first value of the choices.
     */
    CVariantChoiceValidator(const CVariant& value, const String& desc);
    ~CVariantChoiceValidator();
    CVariantChoiceValidator(const CVariantChoiceValidator & rhs);

    /**
      Add a value to the choices.
      */
    void Add(const CVariant&, const String& description);

    virtual CVariantValidator *Clone() const;

protected:
    const CVariantChoiceValidator& operator =
	    (const CVariantChoiceValidator & rhs);

private:
    List<ValidatorChoice> m_Choices;

    // Implementation:

    /** Acquire mutex lock.
	This function will be called by the framework BEFORE calling
	GetChoiceList(). The lock will be kept until this class is finished
	processing all the choices in the choice list.
     */
    virtual void	  AcquireLock() const {}

    /** Release mutex lock.
	This function will be called by the framework AFTER calling
	GetChoiceList().
     */
    virtual void	  ReleaseLock() const {}

    /** Get list of choices available.
     */
    virtual const List<ValidatorChoice> & GetChoiceList() const {
	return m_Choices;
    }
};

/**
 Validator that is able to validate a range of values (of the
 same types)
 */
class CVariantRangeValidator : public CVariantValidator {
public:
    CVariantRangeValidator(const CVariant& min_val, const CVariant& max_val, const String & desc);
    ~CVariantRangeValidator();
    CVariantRangeValidator(const CVariantRangeValidator&);

    virtual CVariantValidator *Clone() const;
    virtual bool Validate(const CVariant&) const;
    virtual void GetPrintableValues(List<v_string_pair>& values) const;

protected:
    const CVariantRangeValidator& operator =
	    (const CVariantRangeValidator &);

private:
    CVariant m_MinValue, m_MaxValue;
};


#endif
