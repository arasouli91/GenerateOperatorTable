/*
Generates code for hard coded operator tables
One table for single character operators
One smaller table for multiple character operators
Outputs all the lines of code needed after the first line of a declaration.
For example: 
OperatorData operatorTable[SIZE]{
.....};	//All code represented here is in output file
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <sstream>
#include <algorithm>
#include <map>
#include "main.h"

void
SetBooleanProperty(OperatorData* aOperatorData, string aName)
{
	if (aName.empty())
		return;

	if (!aName.compare("stretchy") && (1 == aOperatorData->mStr.length()))
		aOperatorData->mFlags |= NS_MATHML_OPERATOR_STRETCHY;
	else if (!aName.compare("fence"))
		aOperatorData->mFlags |= NS_MATHML_OPERATOR_FENCE;
	else if (!aName.compare("accent"))
		aOperatorData->mFlags |= NS_MATHML_OPERATOR_ACCENT;
	else if (!aName.compare("largeop"))
		aOperatorData->mFlags |= NS_MATHML_OPERATOR_LARGEOP;
	else if (!aName.compare("separator"))
		aOperatorData->mFlags |= NS_MATHML_OPERATOR_SEPARATOR;
	else if (!aName.compare("movablelimits"))
		aOperatorData->mFlags |= NS_MATHML_OPERATOR_MOVABLELIMITS;
	else if (!aName.compare("symmetric"))
		aOperatorData->mFlags |= NS_MATHML_OPERATOR_SYMMETRIC;
	else if (!aName.compare("integral"))
		aOperatorData->mFlags |= NS_MATHML_OPERATOR_INTEGRAL;
	else if (!aName.compare("mirrorable"))
		aOperatorData->mFlags |= NS_MATHML_OPERATOR_MIRRORABLE;
}

static void
SetProperty(OperatorData* aOperatorData, string aName, string aValue)
{
	if (aName.empty() || aValue.empty())
		return;

	if (!aName.compare("direction")) {
		if (!aValue.compare("vertical"))
			aOperatorData->mFlags |= NS_MATHML_OPERATOR_DIRECTION_VERTICAL;
		else if (!aValue.compare("horizontal"))
			aOperatorData->mFlags |= NS_MATHML_OPERATOR_DIRECTION_HORIZONTAL;
		else return; // invalid value
	}
	else {
		bool isLeadingSpace;
		if (!aName.compare("lspace"))
			isLeadingSpace = true;
		else if (!aName.compare("rspace"))
			isLeadingSpace = false;
		else return;  // input is not applicable

		// aValue is assumed to be a digit from 0 to 7
		int space = stoi(aValue);	

		if (isLeadingSpace)
			aOperatorData->mLeadingSpace = space;
		else
			aOperatorData->mTrailingSpace = space;
	}
}

static bool
SetOperator(OperatorData*   aOperatorData,
	nsOperatorFlags			aForm,
	string&					aOperator,
	string&					aAttributes,
	string&					comment)

{
	// aOperator is in the expanded format \uNNNN\uNNNN ...
	int32_t i = 0;
	int32_t len = aOperator.length();
	char16_t c = aOperator[i++];
	uint32_t state = 0;
	char16_t uchar = 0;
	u16string operatorVal;
	while (i <= len) {
		if (0 == state) {
			if (c != '\\')
				return false;
			if (i < len)
				c = aOperator[i];
			++i;
			if (('u' != c) && ('U' != c))
				return false;
			if (i < len)
				c = aOperator[i];
			++i;
			++state;
		}
		else {
			if (('0' <= c) && (c <= '9'))
				uchar = (uchar << 4) | (c - '0');
			else if (('a' <= c) && (c <= 'f'))
				uchar = (uchar << 4) | (c - 'a' + 0x0a);
			else if (('A' <= c) && (c <= 'F'))
				uchar = (uchar << 4) | (c - 'A' + 0x0a);
			else return false;
			if (i < len)
				c = aOperator[i];
			++i;
			++state;
			if (5 == state) {		// collected 4 chars i.e. the hex number for one unicode char
				operatorVal += uchar;		// append hex number to value
				uchar = 0;
				state = 0;			// reset for possibly another unicode point
			}
		}
	}
	//if (0 != state) return false;

	// Store operator flags and value		
	aOperatorData->mFlags |= aForm;
	aOperatorData->mStr = (operatorVal);

#ifdef DEBUG
	NS_LossyConvertUTF16toASCII str(aAttributes);
#endif
	// Loop over the space-delimited list of attributes to get the name:value pairs
	auto it = aAttributes.begin();
	while ((it != aAttributes.end()) && (kDashCh != *it)) {
		// name of property. Value of property. Name is value for boolean properties
		string name; string value = "";	
		while ((it != aAttributes.end()) && (kDashCh != *it) && *it == ' ') {
			++it;	// skip space
		}
		// Take all characters before ':'
		while ((it != aAttributes.end()) && (kDashCh != *it)
					&& *it != ' ' && (kColonCh != *it)) 
		{
			name += *it;
			++it;
		}
		// If ':' is not found, then it's a boolean property
		bool IsBooleanProperty = (kColonCh != *it);
		if (IsBooleanProperty) {
			SetBooleanProperty(aOperatorData, name);
		}
		else {	// not boolean property
			++it;	// skip ':'
			// look for space or end of line, to extract property value
			while ((it != aAttributes.end()) && (kDashCh != *it) && *it != ' ') {
				value += *it;
				++it;
			}
			SetProperty(aOperatorData, name, value);
		}
		++it;
	}
	// If reached '#', extract comment
	if ((kDashCh == *it))
	{
		++it;	// skip dash
		for (; it != aAttributes.end(); ++it)
			comment += *it;
	}

	return true;
}


int main()
{
	ifstream inFile("mathfont.properties");
	ofstream outFile("operatorTableCode.txt");
	vector<pair<OperatorData, vector<string>>> compoundRows, rows;
	int compoundCount = 0, count = 0;
	while (inFile) {
		string line;
		getline(inFile, line);
		if (line[0] == '#' || line[0] == ' ') continue;
		// Get operator and attributes separated
		regex reg("operator.\\S*\\ ");
		smatch match;
		regex_search(line, match, reg);
		string name = match.str(0);
		// remove match length chars from beginning of line
		string attributes = line.substr(name.length(), line.length() - name.length());
		///cout << " " << name.size() << " " << name << " " << attributes << endl;
		// expected key: operator.\uNNNN.{infix,postfix,prefix}
		if ((21 <= name.length()) && (0 == name.find("operator.\\u"))) {
			name.erase(0, 9); // 9 is the length of "operator.";
			name.pop_back();  // remove ' '
			int32_t len = name.length();
			nsOperatorFlags form = 0;
			if (name.find(".infix")!=string::npos) {
				form = NS_MATHML_OPERATOR_FORM_INFIX;
				len -= 6;  // 6 is the length of ".infix";
			}
			else if (name.find(".postfix")!=string::npos) {
				form = NS_MATHML_OPERATOR_FORM_POSTFIX;
				len -= 8; // 8 is the length of ".postfix";
			}
			else if (name.find(".prefix")!=string::npos) {
				form = NS_MATHML_OPERATOR_FORM_PREFIX;
				len -= 7; // 7 is the length of ".prefix";
			}
			else continue; // input is not applicable
			// remove form
			name = name.substr(0, len );

			string comment = "";
			OperatorData dummyData;
			OperatorData* operatorData = &dummyData;

			// If the operator should be retained
			//	construct row for table
			if (SetOperator(operatorData, form, name, attributes, comment)) {
				bool prev = 0;
				stringstream ss;
				ss << "{ NS_LITERAL_STRING(\""<< name << "\"), " << int(operatorData->mTrailingSpace) 
					<< ", " << int(operatorData->mLeadingSpace) << ", ";
				if (form == NS_MATHML_OPERATOR_FORM_INFIX)
					ss << "NS_MATHML_OPERATOR_FORM_INFIX ";
				else if (form == NS_MATHML_OPERATOR_FORM_PREFIX)
					ss << "NS_MATHML_OPERATOR_FORM_PREFIX ";
				else if (form == NS_MATHML_OPERATOR_FORM_POSTFIX)
					ss << "NS_MATHML_OPERATOR_FORM_POSTFIX ";
				if (operatorData->mFlags & NS_MATHML_OPERATOR_STRETCHY) {
					ss << "| NS_MATHML_OPERATOR_STRETCHY ";
					prev = 1;
				}
				if (operatorData->mFlags & NS_MATHML_OPERATOR_FENCE) {
					ss << "| NS_MATHML_OPERATOR_FENCE ";
				}
				if(operatorData->mFlags & NS_MATHML_OPERATOR_ACCENT) {
					ss << "| NS_MATHML_OPERATOR_ACCENT ";
				}
				if(operatorData->mFlags & NS_MATHML_OPERATOR_LARGEOP) {
					ss << "| NS_MATHML_OPERATOR_LARGEOP ";
				}
				if(operatorData->mFlags & NS_MATHML_OPERATOR_SEPARATOR) {
					ss << "| NS_MATHML_OPERATOR_SEPARATOR ";
				}
				if(operatorData->mFlags & NS_MATHML_OPERATOR_MOVABLELIMITS) {
					ss << "| NS_MATHML_OPERATOR_MOVABLELIMITS ";
				}
				if(operatorData->mFlags & NS_MATHML_OPERATOR_SYMMETRIC) {
					ss << "| NS_MATHML_OPERATOR_SYMMETRIC ";
				}
				if(operatorData->mFlags & NS_MATHML_OPERATOR_INTEGRAL) {
					ss << "| NS_MATHML_OPERATOR_INTEGRAL ";
				}
				if(operatorData->mFlags & NS_MATHML_OPERATOR_MIRRORABLE) {
					ss << "| NS_MATHML_OPERATOR_MIRRORABLE ";
				}
				vector<string> s{ ss.str(), comment };
				// If compound operator: save row for compound operator table
				if (name.length() > 6) {
					++compoundCount;
					compoundRows.push_back(make_pair(*operatorData, s ));
				}
				else {
					++count;
					rows.push_back(make_pair(*operatorData, s ));
				}
			}
		}
	}

	// Sort tables
	sort(begin(rows), end(rows));
	sort(begin(compoundRows), end(compoundRows));

	// Output tables to file
	for (int i = 0; i < rows.size(); ++i)
	{
		outFile << rows[i].second[0];
		if (i < rows.size() - 1) outFile << "}, //";
		else outFile << "} //";
		outFile << rows[i].second[1] << endl;
	}
	outFile << "};" << endl << endl;
	for (int i = 0; i < compoundRows.size(); ++i)
	{
		outFile << compoundRows[i].second[0];
		if (i < compoundRows.size() - 1) outFile << "}, //";
		else outFile << "} //";
		outFile << compoundRows[i].second[1] << endl;
	}
	outFile << "};" << endl << endl;

	cout << dec << compoundCount << " " << count;

	int wait;
	cin >> wait;
	return 0;
}

//TODO: format comments: remove ampersand, add spaces(detect camel case), capitalize all
//OR NOT TODO?
