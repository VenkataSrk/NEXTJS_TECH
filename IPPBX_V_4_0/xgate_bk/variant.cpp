/* $Header: /xGate/variant.cpp 13    2/20/04 10:26a Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * VARIANT DATA TYPE CLASS AND VARIANT VALIDATORS.
 *
 * $Log: /xGate/variant.cpp $
 * 
 * 13    2/20/04 10:26a Bennylp
 * Added project configuration Hoard and Hoard Debug, to use STLPORT and
 * Hoard memory allocator.
 * 
 * 12    8/02/01 6:22p Bennylp
 * Fixed bug in choice validation because of strnicmp()
 * 
 * 11    6/01/01 4:18p Bennylp
 * Changed some members to inline.
 * 
 * 10    5/23/01 8:12p Bennylp
 * Changed SetValue to use const parameter.
 * 
 * 9     4/27/01 6:03a Bennylp
 * Add CVariantDynamicChoiceValidator.
 * 
 * 8     4/26/01 8:26a Bennylp
 * Fix bug in text/binary conversion.
 * 
 * 7     4/25/01 9:12a Bennylp
 * Change text/binary conversion to use dot as separator.
 * 
 * 6     24/04/01 14:10 Junanto
 * Corrected handling of hexadecimal values in configuration
 * 
 * 5     4/23/01 10:47p Bennylp
 * Fixed std namespace conflicts.
 * 
 * 4     4/12/01 8:09p Bennylp
 * Integration.
 *
 */

#include "stdafx.h"
#include "variant.h"
#include "sl.h"
#include "log.h"
#include <iostream>

#define stricmp strcasecmp
#define strnicmp strncasecmp
#define strcmpi strcasecmp

IdNamePair CVariant::m_TypeName[] = {
    { VT_VOID, "void" },
    { VT_INT, "integer" },
    { VT_FLOAT, "real" },
    { VT_STRING, "string" },
    { VT_TASK, "task" },
    { VT_RET, "ret" },
    { VT_BINARY, "binary" },
    { VT_TABLE, "table" },
    { VT_LONG,"long"} //changes made for compilation for 32 bit to 64 bit
    
};

IdNamePair CVariant::m_ShortTypeName[] = {
    { VT_VOID, "void" },
    { VT_INT, "int" },
    { VT_FLOAT, "real" },
    { VT_STRING, "str" },
    { VT_TASK, "task" },
    { VT_RET, "ret" },
    { VT_BINARY, "bin" },
    { VT_TABLE, "tbl" }
};

#if 0
inline int &CVariant::asInt()
{
	return *reinterpret_cast<int *>(castOrDetach(Int)); 
}
#endif;

CVariant::operator<(const CVariant& value) const
{
    if (Type()!=value.Type()) {
	assert(false);
	return true;
    };

    switch(Type()) {
    case VT_INT:
	return AsInt() < value.AsInt();
    case VT_FLOAT:
	return AsFloat() < value.AsFloat();
    case VT_STRING:
	return strcmp(AsString(), value.AsString()) < 0;
    case VT_BINARY:
	return memcmp(Value(), value.Value(), Size()<value.Size() ? Size() : value.Size()) < 0;
    //case VT_LONG:
      //   return ASLong()< value.AsLong();
    default:
	return false;
    }
}

const char *CVariant::GetTypeStr( CVariant::VType t )
{
    for (int i=0; i<sizeof(m_TypeName)/sizeof(IdNamePair); ++i)
	if (m_TypeName[i].Id==static_cast<int>(t))
	    return m_TypeName[i].Name;

    return "UNKNOWN";
}

const char *CVariant::GetShortTypeStr( CVariant::VType t )
{
    for (int i=0; i<sizeof(m_ShortTypeName)/sizeof(IdNamePair); ++i)
	if (m_ShortTypeName[i].Id==static_cast<int>(t))
	    return m_ShortTypeName[i].Name;

    return "???";
}

CVariant::VType CVariant::GetType(const char *type_str)
{
    for (int i=0; i<sizeof(m_TypeName)/sizeof(IdNamePair); ++i)
	if (!stricmp(type_str, m_TypeName[i].Name))
	    return static_cast<VType>(m_TypeName[i].Id);

    return VT_VOID;
}

int CVariant::Factory(CVariant &the_value, VType type, const char* str_value)
{
    assert(str_value);
    if (str_value==NULL)
	return -1;

    char *err;

    switch (type) {
    case VT_FLOAT:
	the_value.SetValue((float)strtod(str_value, &err));
	if (*err) return -1;
	break;
    case CVariant::VT_INT:
	if (str_value[0] == '0' && tolower(str_value[1]) == 'x')
	    the_value.SetValue((int)strtoul(str_value, &err, 16));
	else
	    the_value.SetValue((int)strtol(str_value, &err, 10));
	if (*err) return -1;
	break;
    case CVariant::VT_STRING:
	the_value.SetValue(str_value);
	break;
    case CVariant::VT_BINARY:
	{
	    char *binary;
	    size_t len;
	    if (TextToBinary(str_value, binary, len)) {
		the_value.SetValue(binary, len);
		delete [] binary;
	    }
	}
	break;
    case CVariant::VT_VOID:
	the_value.Clear();
	break;
    case CVariant::VT_TABLE:
	the_value.CreateTable();
	break;
    default:
	return -1;
    };
    return 0;
}

bool CVariant::TextToBinary(const char *str_input, char*& binary_output, size_t& binary_len)
{
    assert(str_input);	// str_input must not be NULL

    int input_len=str_input ? strlen(str_input) : 0;
    if ((input_len==0) || ((input_len-2) % 3 != 0)) {
	binary_len=0;
	binary_output=NULL;
	return false;
    }

    // duplicate input for strtok
    char *dup_input;
    NEW_RETURN_ (dup_input, char[input_len+1]);
    if (dup_input == NULL) {
	binary_output = NULL;
	binary_len = 0;
	return false;
    }
    strcpy(dup_input, str_input);

    binary_len=(input_len-2)/3+1;
    NEW_RETURN_ (binary_output, char[binary_len]);
    if (binary_output == NULL) {
	delete [] dup_input;
	binary_len = 0;
	return false;
    }
    char *w_ptr=binary_output;

    char *token=strtok(dup_input, ".");
    while(token) {
	if (strlen(token)!=2)
	    goto on_error;

	char *err;
	*w_ptr++=(char)strtol(token, &err, 16);
	if (*err)
	    goto on_error;
	token=strtok(NULL, ".");
    }

    delete [] dup_input;
    return true;

on_error:
    delete [] dup_input;
    binary_len=0;
    delete [] binary_output;
    binary_output=NULL;
    return false;
}

bool CVariant::BinaryToText(const char *binary_input, size_t binary_len, String& text_out)
{
    text_out.erase();

    if (binary_len==0)
	return true;

    char s[8];
    for (unsigned int i=0; i<binary_len-1; ++i) {
	sprintf( s, "%.2x.", binary_input[i] & 0xFF);
	text_out += s;
    }
    
    sprintf( s, "%.2x", binary_input[i] & 0xFF);
    text_out += s;
    return true;
}

String CVariant::ToString() const
{
    char s[64];
    String S;

    switch( Type() ) {
    case VT_INT:
	sprintf( s, "%d", AsInt());
	S = s;
	break;
    case VT_FLOAT:
	sprintf( s, "%f", AsInt());
	S = s;
	break;
    case VT_STRING:
	S = AsString();
	break;
    case VT_BINARY:
	BinaryToText((const char*)Value(), Size(), S);
	break;
    default:
	assert(false);  // type is unsupported for printing
	sprintf( s, "%p", Value());
	S = s;
	break;
    }

    return S;
}

void CVariant::Print(std::ostream& os) const
{
    switch( Type() ) {
    case VT_INT:
	os << AsInt();
	break;
    case VT_FLOAT:
	os << AsFloat();
	break;
    case VT_STRING:
	os << "\"" << AsString() << "\"";
	break;
    case VT_BINARY:
	{
	    os << "[" << Size() << "]";
	    for (unsigned int i=0; i<Size(); ++i) {
		char s[6];
		sprintf( s, " %.2x", static_cast<char*>(Value())[i] & 0xFF);
		os << s;
	    }
	}
	break;
    default:
	os << Value();
	break;
    }
}

CVariant CVariant::CreateTable()
{
    CVariant v;

    v.m_Value = NULL;
    v.m_Size = 0;
    v.m_Type = VT_TABLE;

    return v;
}


//=============================================================================
CVariantValidator::CVariantValidator(const CVariant& value, const String & desc, ValidatorType type)
: m_ValidatorType(type), m_Type(value.Type()), m_Desc(desc)
{
}

CVariantValidator::~CVariantValidator()
{
}

CVariantValidator::CVariantValidator(const CVariantValidator& rhs)
: m_Desc(rhs.m_Desc), m_Type(rhs.m_Type)
{
}

const CVariantValidator& CVariantValidator::operator =(const CVariantValidator& rhs)
{
    assert(m_ValidatorType==rhs.m_ValidatorType);
    m_Type=rhs.m_Type;
    return *this;
}

CVariantValidator* CVariantValidator::Clone() const
{
    CVariantValidator *ret;
    NEW_RETURN_ (ret, CVariantValidator(*this));
    return ret;
}

bool CVariantValidator::Validate(const CVariant& value) const
{
    return value.Type()==m_Type;
}

void CVariantValidator::GetPrintableValues(List<v_string_pair>& values) const
{
    const char* type_str=CVariant::GetTypeStr(m_Type);

    char val_str[20];
    snprintf( val_str, 20, "<%s>", type_str);
    val_str[19]='\0';

    char *p=&val_str[0];
    while(*p)
	*p++=(char)toupper(*p);

    //v_string_pair vpair((const char*)val_str, String("A ")+type_str+" value");
    v_string_pair vpair((const char*)val_str, m_Desc);
    values.push_back(vpair);
}

//=============================================================================
CVariantSingleValueValidator::CVariantSingleValueValidator(const CVariant& the_value, const String & desc)
: CVariantValidator(the_value, desc, SingleValue), m_TheValue(the_value)
{
}

CVariantSingleValueValidator::~CVariantSingleValueValidator()
{
}

CVariantSingleValueValidator::CVariantSingleValueValidator(const CVariantSingleValueValidator& V)
: CVariantValidator(V.m_TheValue,V.m_Desc,SingleValue), m_TheValue(V.m_TheValue)
{
}

const CVariantSingleValueValidator & 
CVariantSingleValueValidator::operator=(const CVariantSingleValueValidator& rhs)
{
    CVariantValidator::operator=(rhs);
    m_TheValue=rhs.m_TheValue;
    return *this;
}

CVariantValidator *CVariantSingleValueValidator::Clone() const
{
    CVariantValidator *ret;
    NEW_RETURN_ (ret, CVariantSingleValueValidator(*this));
    return ret;
}

bool CVariantSingleValueValidator::Validate(const CVariant& value) const
{
    return CVariantValidator::Validate(value) && m_TheValue==value;
}

bool CVariantSingleValueValidator::Suggest(CVariant& value) const
{
    String s1=m_TheValue.ToString();
    String s2=value.ToString();

    if (!strnicmp(s2.c_str(), s1.c_str(), s2.size())) {
	value = m_TheValue;
	return true;
    } else
	return false;
}

void CVariantSingleValueValidator::GetPrintableValues(List<v_string_pair>& values) const
{
    values.push_back(v_string_pair(m_TheValue.ToString(), m_Desc) );
}


//=============================================================================
CVariantDynamicChoiceValidator::CVariantDynamicChoiceValidator()
: CVariantValidator(CVariant(0), "", CVariantValidator::Choice)
{
}

CVariantDynamicChoiceValidator::~CVariantDynamicChoiceValidator()
{
}

CVariantDynamicChoiceValidator::CVariantDynamicChoiceValidator(const CVariantDynamicChoiceValidator & rhs)
: CVariantValidator(CVariant(0), "", CVariantValidator::Choice)
{
}

const CVariantDynamicChoiceValidator & 
CVariantDynamicChoiceValidator::operator = (const CVariantDynamicChoiceValidator & rhs)
{
    return *this;
}


bool CVariantDynamicChoiceValidator::Validate(const CVariant & value) const
{
    List<ValidatorChoice>::const_iterator it, end;
    bool found = false;

    AcquireLock();
    const List<ValidatorChoice> & choice_list = GetChoiceList();
    end = choice_list.end();
    for( it=choice_list.begin(); it!=end; ++it) {
	if (it->first==value) {
	    found = true;
	    break;
	}
    }

    ReleaseLock();
    return found;
}

bool CVariantDynamicChoiceValidator::Suggest(CVariant & value) const
{
    const CVariant * match_value=NULL;
    String s2=value.ToString();

    AcquireLock();
    const List <ValidatorChoice> & choice_list = GetChoiceList();
    List <ValidatorChoice>::const_iterator it, end;
    end = choice_list.end();
    
    bool has_duplicate = false;
    bool found_exact = false;

    for(it=choice_list.begin(); it!=end; ++it) {
	String s1=it->first.ToString();
	if (!strnicmp(s2.c_str(), s1.c_str(), s2.size())) {
	    if (s1.size() == s2.size()) {
		// exact match
		found_exact = true;
		match_value=&it->first;
		break;
	    } else if (!match_value)
		match_value=&it->first;
	    else {
		has_duplicate = true;
	    }
	}
    }

    if (found_exact) {
	value = *match_value;
    } else if (has_duplicate) {
	goto on_duplicate_match;
    } if (match_value) {
	value = *match_value;
    } 

    ReleaseLock();
    return match_value != NULL;

on_duplicate_match:
    ReleaseLock();
    return false;
}

void CVariantDynamicChoiceValidator::GetPrintableValues(List<v_string_pair>& values) const
{
    AcquireLock();
    const List <ValidatorChoice> & choice_list = GetChoiceList();
    List<ValidatorChoice>::const_iterator it, end;
    end = choice_list.end();

    for(it=choice_list.begin(); it!=end; ++it) {
	values.push_back( std::make_pair(it->first.ToString(), 
					 it->second));
    }
    ReleaseLock();
}

//=============================================================================
CVariantChoiceValidator::CVariantChoiceValidator(const CVariant& value, const String& desc)
{
    m_Choices.push_back(std::make_pair(value, desc));
}

CVariantChoiceValidator::~CVariantChoiceValidator()
{
}

CVariantChoiceValidator::CVariantChoiceValidator(const CVariantChoiceValidator& rhs)
: m_Choices(rhs.m_Choices)
{
}

void CVariantChoiceValidator::Add(const CVariant& value, const String& desc)
{
    m_Choices.push_back(std::make_pair(value, desc));
}

const CVariantChoiceValidator & 
CVariantChoiceValidator::operator =(const CVariantChoiceValidator & rhs)
{
    CVariantDynamicChoiceValidator::operator=(rhs);
    m_Choices=rhs.m_Choices;
    return *this;
}

CVariantValidator *CVariantChoiceValidator::Clone() const
{
    CVariantValidator *ret;
    NEW_RETURN_ (ret, CVariantChoiceValidator(*this));
    return ret;
}


//=============================================================================
CVariantRangeValidator::CVariantRangeValidator(const CVariant& min_val, const CVariant& max_val, const String & desc)
: CVariantValidator(min_val,desc, Range), m_MinValue(min_val), m_MaxValue(max_val)
{
}

CVariantRangeValidator::~CVariantRangeValidator()
{
}

CVariantRangeValidator::CVariantRangeValidator(const CVariantRangeValidator& rhs)
: CVariantValidator(rhs.m_MinValue,rhs.m_Desc, Range), m_MinValue(rhs.m_MinValue), m_MaxValue(rhs.m_MaxValue)
{
}

const CVariantRangeValidator & 
CVariantRangeValidator::operator =(const CVariantRangeValidator & rhs)
{
    CVariantValidator::operator=(rhs);
    m_MinValue=rhs.m_MinValue;
    m_MaxValue=rhs.m_MaxValue;
    return *this;
}

CVariantValidator *CVariantRangeValidator::Clone() const
{
    CVariantValidator *ret;
    NEW_RETURN_ (ret, CVariantRangeValidator(*this));
    return ret;
}

bool CVariantRangeValidator::Validate(const CVariant& value) const
{
    return CVariantValidator::Validate(value) &&
	   (value >= m_MinValue) && 
	   (value <= m_MaxValue);
}

void CVariantRangeValidator::GetPrintableValues(List<v_string_pair>& values) const
{
    values.push_back(v_string_pair(m_MinValue.ToString()+String("-")+m_MaxValue.ToString(), m_Desc));
}

