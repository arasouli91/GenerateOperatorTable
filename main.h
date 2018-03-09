#pragma once
#include <stdint.h>
#include <string>
using namespace std;

#define NS_MATHML_OPERATOR_GET_FORM(_flags) \
  ((_flags) & NS_MATHML_OPERATOR_FORM)

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

static const int rearrange[]{ 0,1,3,2 };

// operator dictionary entry
struct OperatorData {
	// member data
	u16string	mStr;
	uint32_t	mFlags;
	uint8_t		mLeadingSpace : 4;
	uint8_t		mTrailingSpace : 4;

	OperatorData(void)
		: mFlags(0),
		mLeadingSpace(0),
		mTrailingSpace(0)
	{}

	bool operator<(const OperatorData& B) const
	{
		// get values of first place operator
		int a = static_cast<int>(static_cast<char16_t>(this->mStr[0]));
		int b = static_cast<int>(static_cast<char16_t>(B.mStr[0]));
		// Compare values
		if (a < b) return true;
		if (a > b) return false;
		// Values tied
		// if either value is compound
		if (this->mStr.size()>1 || B.mStr.size()>1) {
			// Consider next values
			for (int i = 1; i <= 2; ++i) {
				// if either lacks a successor value
				// set value to 0, so that the other value takes precendence
				if (this->mStr.size() < i + 1) a = 0;
				else a = static_cast<int>(static_cast<char16_t>(this->mStr[i]));
				if (B.mStr.size() < i + 1) b = 0;
				else b = static_cast<int>(static_cast<char16_t>(B.mStr[i]));
				// Compare values
				if (a < b) return true;
				if (a > b) return false;
			}
		}
		// Values tied, compare by form
		if (rearrange[NS_MATHML_OPERATOR_GET_FORM(this->mFlags)] 
			< rearrange[NS_MATHML_OPERATOR_GET_FORM(B.mFlags)])
			return true;
		else
			return false;
	}
};
