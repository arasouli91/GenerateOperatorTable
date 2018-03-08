#pragma once
#include <stdint.h>
#include <string>
using namespace std;

static const char16_t kDashCh = char16_t('#');
static const char16_t kColonCh = char16_t(':');

enum nsStretchDirection {
	NS_STRETCH_DIRECTION_UNSUPPORTED = -1,
	NS_STRETCH_DIRECTION_DEFAULT = 0,
	NS_STRETCH_DIRECTION_HORIZONTAL = 1,
	NS_STRETCH_DIRECTION_VERTICAL = 2
};

typedef uint32_t nsOperatorFlags;
enum {
	// define the bits used to handle the operator
	NS_MATHML_OPERATOR_MUTABLE = 1 << 30,
	NS_MATHML_OPERATOR_EMBELLISH_ANCESTOR = 1 << 29,
	NS_MATHML_OPERATOR_EMBELLISH_ISOLATED = 1 << 28,
	NS_MATHML_OPERATOR_CENTERED = 1 << 27,
	NS_MATHML_OPERATOR_INVISIBLE = 1 << 26,

	// define the bits used in the Operator Dictionary

	// the very last two bits tell us the form
	NS_MATHML_OPERATOR_FORM = 0x3,
	NS_MATHML_OPERATOR_FORM_INFIX = 1,
	NS_MATHML_OPERATOR_FORM_PREFIX = 2,
	NS_MATHML_OPERATOR_FORM_POSTFIX = 3,

	// the next 2 bits tell us the direction
	NS_MATHML_OPERATOR_DIRECTION = 0x3 << 2,
	NS_MATHML_OPERATOR_DIRECTION_HORIZONTAL = 1 << 2,
	NS_MATHML_OPERATOR_DIRECTION_VERTICAL = 2 << 2,

	// other bits used in the Operator Dictionary
	NS_MATHML_OPERATOR_STRETCHY = 1 << 4,
	NS_MATHML_OPERATOR_FENCE = 1 << 5,
	NS_MATHML_OPERATOR_ACCENT = 1 << 6,
	NS_MATHML_OPERATOR_LARGEOP = 1 << 7,
	NS_MATHML_OPERATOR_SEPARATOR = 1 << 8,
	NS_MATHML_OPERATOR_MOVABLELIMITS = 1 << 9,
	NS_MATHML_OPERATOR_SYMMETRIC = 1 << 10,
	NS_MATHML_OPERATOR_INTEGRAL = 1 << 11,
	NS_MATHML_OPERATOR_MIRRORABLE = 1 << 12,

	// Additional bits not stored in the dictionary
	NS_MATHML_OPERATOR_MINSIZE_ABSOLUTE = 1 << 13,
	NS_MATHML_OPERATOR_MAXSIZE_ABSOLUTE = 1 << 14,
	NS_MATHML_OPERATOR_LSPACE_ATTR = 1 << 15,
	NS_MATHML_OPERATOR_RSPACE_ATTR = 1 << 16
};

// operator dictionary entry
struct OperatorData {
	OperatorData(void)
		: mFlags(0),
		mLeadingSpace(0),
		mTrailingSpace(0)
	{}
	bool compoundCompare(const OperatorData& A, const OperatorData& B)
	{
		// get values of first place operator
		int a = static_cast<int>(static_cast<char16_t>(A.mStr[0]));
		int b = static_cast<int>(static_cast<char16_t>(B.mStr[0]));
		// Compare values
		if (a > b) return true;
		else if (a < b) return false;
		// Values tied
		// if either value is compound
		else if ( A.mStr.size()>1 || B.mStr.size()>1 ) {
			// Consider next values
			for(int i = 1; i <= 2; ++i){
				// if either lacks a successor value
				// set value to 0, so that the other value takes precendence
				if (A.mStr.size() < i+1) a = 0;
				else a = static_cast<int>(static_cast<char16_t>(A.mStr[i]));
				if (B.mStr.size() < i+1) b = 0;
				else b = static_cast<int>(static_cast<char16_t>(B.mStr[i]));
				// Compare values
				if (a > b) return true;
				else if (a < b) return false;
			}
		}
		// Values tied, compare by form
		//TODO: how to handle form? need to wait for search results to see if we can change the enum
	}

	// member data
	u16string	mStr;
	uint32_t	mFlags;
	uint8_t		mLeadingSpace : 4;
	uint8_t		mTrailingSpace : 4;
};


#define NS_MATHML_OPERATOR_SIZE_INFINITY NS_IEEEPositiveInfinity()

////////////////////////////////////////////////////////////////////////////
// Macros that retrieve the bits used to handle operators

#define NS_MATHML_OPERATOR_IS_MUTABLE(_flags) \
  (NS_MATHML_OPERATOR_MUTABLE == ((_flags) & NS_MATHML_OPERATOR_MUTABLE))

#define NS_MATHML_OPERATOR_HAS_EMBELLISH_ANCESTOR(_flags) \
  (NS_MATHML_OPERATOR_EMBELLISH_ANCESTOR == ((_flags) & NS_MATHML_OPERATOR_EMBELLISH_ANCESTOR))

#define NS_MATHML_OPERATOR_EMBELLISH_IS_ISOLATED(_flags) \
  (NS_MATHML_OPERATOR_EMBELLISH_ISOLATED == ((_flags) & NS_MATHML_OPERATOR_EMBELLISH_ISOLATED))

#define NS_MATHML_OPERATOR_IS_CENTERED(_flags) \
  (NS_MATHML_OPERATOR_CENTERED == ((_flags) & NS_MATHML_OPERATOR_CENTERED))

#define NS_MATHML_OPERATOR_IS_INVISIBLE(_flags) \
  (NS_MATHML_OPERATOR_INVISIBLE == ((_flags) & NS_MATHML_OPERATOR_INVISIBLE))

#define NS_MATHML_OPERATOR_GET_FORM(_flags) \
  ((_flags) & NS_MATHML_OPERATOR_FORM)

#define NS_MATHML_OPERATOR_GET_DIRECTION(_flags) \
  ((_flags) & NS_MATHML_OPERATOR_DIRECTION)

#define NS_MATHML_OPERATOR_FORM_IS_INFIX(_flags) \
  (NS_MATHML_OPERATOR_FORM_INFIX == ((_flags) & NS_MATHML_OPERATOR_FORM))

#define NS_MATHML_OPERATOR_FORM_IS_PREFIX(_flags) \
  (NS_MATHML_OPERATOR_FORM_PREFIX == ((_flags) & NS_MATHML_OPERATOR_FORM))

#define NS_MATHML_OPERATOR_FORM_IS_POSTFIX(_flags) \
  (NS_MATHML_OPERATOR_FORM_POSTFIX == ((_flags) & NS_MATHML_OPERATOR_FORM))

#define NS_MATHML_OPERATOR_IS_DIRECTION_VERTICAL(_flags) \
  (NS_MATHML_OPERATOR_DIRECTION_VERTICAL == ((_flags) & NS_MATHML_OPERATOR_DIRECTION))

#define NS_MATHML_OPERATOR_IS_DIRECTION_HORIZONTAL(_flags) \
  (NS_MATHML_OPERATOR_DIRECTION_HORIZONTAL == ((_flags) & NS_MATHML_OPERATOR_DIRECTION))

#define NS_MATHML_OPERATOR_IS_STRETCHY(_flags) \
  (NS_MATHML_OPERATOR_STRETCHY == ((_flags) & NS_MATHML_OPERATOR_STRETCHY))

#define NS_MATHML_OPERATOR_IS_FENCE(_flags) \
  (NS_MATHML_OPERATOR_FENCE == ((_flags) & NS_MATHML_OPERATOR_FENCE))

#define NS_MATHML_OPERATOR_IS_ACCENT(_flags) \
  (NS_MATHML_OPERATOR_ACCENT == ((_flags) & NS_MATHML_OPERATOR_ACCENT))

#define NS_MATHML_OPERATOR_IS_LARGEOP(_flags) \
  (NS_MATHML_OPERATOR_LARGEOP == ((_flags) & NS_MATHML_OPERATOR_LARGEOP))

#define NS_MATHML_OPERATOR_IS_SEPARATOR(_flags) \
  (NS_MATHML_OPERATOR_SEPARATOR == ((_flags) & NS_MATHML_OPERATOR_SEPARATOR))

#define NS_MATHML_OPERATOR_IS_MOVABLELIMITS(_flags) \
  (NS_MATHML_OPERATOR_MOVABLELIMITS == ((_flags) & NS_MATHML_OPERATOR_MOVABLELIMITS))

#define NS_MATHML_OPERATOR_IS_SYMMETRIC(_flags) \
  (NS_MATHML_OPERATOR_SYMMETRIC == ((_flags) & NS_MATHML_OPERATOR_SYMMETRIC))

#define NS_MATHML_OPERATOR_IS_INTEGRAL(_flags) \
  (NS_MATHML_OPERATOR_INTEGRAL == ((_flags) & NS_MATHML_OPERATOR_INTEGRAL))

#define NS_MATHML_OPERATOR_IS_MIRRORABLE(_flags) \
  (NS_MATHML_OPERATOR_MIRRORABLE == ((_flags) & NS_MATHML_OPERATOR_MIRRORABLE))

#define NS_MATHML_OPERATOR_MINSIZE_IS_ABSOLUTE(_flags) \
  (NS_MATHML_OPERATOR_MINSIZE_ABSOLUTE == ((_flags) & NS_MATHML_OPERATOR_MINSIZE_ABSOLUTE))

#define NS_MATHML_OPERATOR_MAXSIZE_IS_ABSOLUTE(_flags) \
  (NS_MATHML_OPERATOR_MAXSIZE_ABSOLUTE == ((_flags) & NS_MATHML_OPERATOR_MAXSIZE_ABSOLUTE))

#define NS_MATHML_OPERATOR_HAS_LSPACE_ATTR(_flags) \
  (NS_MATHML_OPERATOR_LSPACE_ATTR == ((_flags) & NS_MATHML_OPERATOR_LSPACE_ATTR))

#define NS_MATHML_OPERATOR_HAS_RSPACE_ATTR(_flags) \
  (NS_MATHML_OPERATOR_RSPACE_ATTR == ((_flags) & NS_MATHML_OPERATOR_RSPACE_ATTR))

