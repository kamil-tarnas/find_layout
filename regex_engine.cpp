#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <map>
#include <vector>
#include <array>

std::string info;

// Map of key = bitfield names and values = bitfield widths
std::map<std::string, unsigned> bitfieldSizes;

// Vector of pairs of bitfield name and bitfield width sorted by bitfield widths
std::vector<std::pair<std::string, unsigned>> bitfieldSizesVec;

// Map of key = bitfield variable name, value = string (containing all comments and appended things)
std::map<std::string, std::string> fullBitfieldNames;

// Vector of the global order of bitfields in globally processed struct
std::vector<std::pair<std::string, unsigned>> outputBitfield;

// The whole bitfield part of processed struct
std::string bitfieldOutput;

// Global output - for reposting - does not work right now...
std::string output;

// x-ambles
std::string preamble = "#ifdef debug\n";
std::string interamble = "#else\n";
std::string postamble = "#endif\n";

std::string padding = "u16";
std::string indent = "  ";


enum class InputParameterType
{
	BOOLEAN = 0,
	STRING,
	REGEX,
	INTEGER,
	NO_OF_PARAMETER_TYPES
};


// No support for std::variant on C++11 based platform...
// Cannot put non-POD, non-trivial types into union...
//union InputParameterValue
//{
//	bool boolean;
//	std::string string;
//	std::regex regex;
//	int integer;
//};


// TODO: Abstract class or interface? Every method would be abstract?
// TODO: Anyhow make the use of virtual constructor pattern here and make the value based
// on the actual InputParameterType:: static value? Delegate it to separate function?
// TODO: Pure abstract class that would generalize the input parameter objects?
class InputParameterValue
{
public:
	//virtual ~InputParameterValue(); // Doing nothing currently, might be needed later...
//	virtual std::string GetName();
//	virtual InputParameterType GetType();
	//static InputParameterType parameterType; -> would be implicit
	// TODO: Cannot create GetValue() - the return type is not known at this point?
	// We can have virtual setter (perhaps, the parameters would be different (???)
	// Yes, but we can do the following:
	/*
public:
  Variable (T v) : m_value (v) {}
  virtual void Callback (const char *values, ...) = 0;

private:
  T m_value;
};

class Derived : public Variable<int>
{
public:
  Derived (int v) : Variable<int> (v) {}
  virtual void Callback (const char *values, ...) {
	 */
	// We cannot have a virtual getter - the return values needs to be the same (or covariants)

	// Set via Input parameter value (argument would be InputParameterValue?) - pointer to void* value (?)
};


// TODO: Must create InputParameterValue hierarchy (with an pure abstract class?) that could call the right destructors
// when the times comes...


// TODO: This needs to be just the
// TODO: Ill formed because of the constructor? Need to implement the destructor for such cases?
// TODO: std::any or std::variant for value of the parameter?
struct InputParameterArrayElement  //TODO: Make this a hierarchy of parameters? This would make the vector to be a vector of pointers...
{
	std::string parameterName_m;
	InputParameterType parameterType_m;
	// TODO: Let's do the MVP now and make it a pointer, slow, but works (also,
	//       there are possibly other, more slow things in current implementation)
	InputParameterValue* parameterValue_mp; //Here
//
//	// TODO: Something of a dead end - need to have a virtual destructor here
//	~InputParameterArrayElement()
//	{
//		switch (parameterType_m)
//		{
//			// TODO: Have custom functions overloaded based on the InputParameterType value...
//			case InputParameterType::REGEX:
//			{
//				parameterValue_m.regex.~basic_regex();
//				break;
//			}
//			case InputParameterType::STRING:
//			{
//				parameterValue_m.string.~basic_string();
//				break;
//			}
//			default:
//			{
//				// Trivial types - no need to call any destructors...
//			}
//
//	}
};


#define NUMBER_OF_PARAMETERS 10

// TODO: an array of InputParameterValue pointers?
// TODO: ParamArray would be a set of values for a quicker search?
// TODO: Let's not do int in the MVP...
using ParamArray = std::array<InputParameterArrayElement, NUMBER_OF_PARAMETERS>;


//InputParameterType::BOOLEAN // TODO: Static member variable of InputParameterType? //-> would be implicit
// TODO: Wouldn't it be useless - we could not call get or set methods anyway?
class InputParameterBoolean : public InputParameterValue
{
public:
	InputParameterBoolean(std::string parameterValue)
	{
		// This could be moved to the parameter constuctor of InputParameterBoolean and could throw from there...
		if (parameterValue == "true")
		{
			value_m = true;
		}
		else if (parameterValue == "false")
		{
			value_m = false;
		}
		else
		{
			// throw
		}
	}


private:
	bool value_m;
};

// Basically a wrapper to check if the regex is valid
// If throws - then regex syntax isn't valid
bool isRegexValid(const std::string &regexString)
{
    try
    {
        std::regex re(regexString);
    }
    catch (const std::regex_error&)
    {
        return false;
    }
    return true;
}

//InputParameterType::REGEX
class InputParameterRegex : public InputParameterValue
{
public:
	InputParameterRegex(std::string parameterValue)
	{
		// Just accept everything passed as a regex string?
		// TODO: What to do with the escape characters?
		if (isRegexValid(parameterValue))
		{
			value_m = std::regex(parameterValue);
		}
		else
		{
			// throw
		}
	}
	std::regex value_m;

};

class InputParameterInteger : public InputParameterValue
{
public:
	InputParameterInteger(std::string parameterValue)
	{
		std::regex regexParameterString("[0-9]+");

		if (std::regex_match(parameterValue, regexParameterString))
		{
			// TODO: Cases of overflow, other exceptions...
			value_m = std::stoi(parameterValue);
		}
		else
		{
			//throw
		}
	}


private:
	int value_m;
};

class InputParameterString : public InputParameterValue
{
public:
	InputParameterString(std::string parameterValue)
	{
		if (parameterValue.at(0) == '\"')
		{
			// Write the parameter value
			// TODO: Remove the quotes? ""?
			value_m = parameterValue;
		}
		else
		{
			// throw
		}
	}


	// TODO: Should be private!
	std::string value_m;
};


class InputParameter // TODO: Virtual or abstract class... depending on that - call a proper function to extract value
{
public:

	static const InputParameter dummy;
	// TODO: This could be made 'private' //TODO: A type which contains itself - that is also not possible?
	static ParamArray parameters;

	bool AssignParameterValue(std::string parameterValue)
	{
		// Polimorphic type that would get the dynamicly-typed object in following cases
		InputParameterValue* parameterToBeAssigned = nullptr;

		// TODO: Have a return value instead of multiple returns...
		switch (parameterType_m)
		{
			// TODO: Have custom functions overloaded based on the InputParameterType value... (possible?)
			case InputParameterType::BOOLEAN:
			{
				parameterToBeAssigned = new InputParameterBoolean(parameterValue);
				break;
			}
			case InputParameterType::INTEGER:
			{
				//Assign the value...
				parameterToBeAssigned = new InputParameterInteger(parameterValue);
				break;
			}
			case InputParameterType::REGEX:
			{
				// Just accept everything passed as a regex string?
				// TODO: What to do with the escape characters?

				parameterToBeAssigned = new InputParameterRegex(parameterValue);

				break;
			}
			case InputParameterType::STRING:
			{
				// Write the parameter value
				parameterToBeAssigned = new InputParameterString(parameterValue);
				break;
			}
			default:
			{
				//throw
			}
		}

		if (parameterToBeAssigned) // if the object was created... /// We are inside the constructor of the object here...
		{
			// Just assign the value - object constuction is self-contained...

			// BELOW MISSLEADING!!!!!!!!
//			//using ParamArray = std::array<InputParameterArrayElement, NUMBER_OF_PARAMETERS>;
//			for (auto& parameter: parameters)
//			{
//				//TODO: Get type or something - identify it by the things we already have...
//				//parameterToBeAssigned->InputParameterValue
//				// Okay, having separate operator== for that
//				if (*parameterToBeAssigned == parameter) // TODO: A bit dangerous overall, with pointers juggling
//				{
//					// Assign just the pointer - don't modify another fields
//					// TODO: Make it mutable - make only the pointer assignable
//					parameter.parameterValue_mp = parameterToBeAssigned;
//				}
//			}
//			// TODO: Here we need to write it to the array (static array of values?)
			return true;
		}
		else
		{
			//thorw
			// or return false?
			return false;
		}
	}

	InputParameter(std::string parameterName, std::string parameterValue)
	{
		bool isParameterFound = false;
		InputParameterType parameterType = InputParameterType::NO_OF_PARAMETER_TYPES;

		for (auto& parameter: parameters)
		{
			// If the name was found - then proceed
			if (parameter.parameterName_m == parameterName)
			{
				isParameterFound = true;
				parameterType = parameter.parameterType_m;
				break;
			}
		}

		if (isParameterFound)
		{
			// Name matches - rewrite
			parameterName_m = parameterName;

			// Inherit the type from parameters array
			parameterType_m = parameterType;

			AssignParameterValue(parameterValue);

			// If was found then read the value
			// Based on the value std::string parameterValue
			// Dispatch proper function and save the value...
			// Here add it...


		}
		else
		{
			// throw
		}
	}

	std::string GetName()
	{
		return parameterName_m;
	}

	InputParameterType GetType()
	{
		return parameterType_m;
	}

	// Name and type must match to be the same value-semantics wise
	// This allows to have parameters of the same name and differents types
	// TODO: Check if there are any limitations in the rest of code
	//       that could restrict that
	// TODO: This could be implemented in other way
	//       Have reflections - if the type is so and so
	// NEEDS SOME REWORKING
//	bool operator==(const InputParameterValue& lhs, const InputParameterArrayElement& rhs)
//	{
//		rhs.parameterName_m
//	    //return lhs.GetName() == rhs.parameterName_m && lhs.GetType() == rhs.parameterType_m;
//	}

private:
	std::string parameterName_m;

	// TODO: If we are about to develop a class hierarchy then it should not contain the parameterType variable,
	// the parameter information type needs to be embedded in the dynamic type of the object
	InputParameterType parameterType_m;
	// Also value here??
	// yup

};

// TODO: Hold the values sorted - it will be quicker for accessing
// TODO: Map in which InputParameter is a key - it should be comparable with the other things?
// only == needs to be defined?
// TODO: How to make it look for the dyamic hierarchy? Could this hold somehow the type of
// the parameter as the second value (typeof (GCC specific extension (do not use), typeid (okay?), decltype and so on...)?
// TODO: Have values or default values...
// TODO: Currently cannot be constexpr - is having string typed member variables...
ParamArray InputParameter::parameters = //TODO: Remove const, maybe make mutable in the future...
{{
	{"preamble", InputParameterType::STRING}, // static reflections to compare types?
	{"interamble", InputParameterType::STRING},
	{"postamble", InputParameterType::STRING},
	{"alignment", InputParameterType::INTEGER},
	{"ignore", InputParameterType::REGEX},
	{"indent", InputParameterType::STRING},
	{"padding", InputParameterType::STRING},
	{"files", InputParameterType::REGEX}, //???
	{"prefix", InputParameterType::STRING},
	{"mirror", InputParameterType::BOOLEAN}
}};

const InputParameter InputParameter::dummy("cos", "cos");

// TODO: Default values of the parameters?
//

// This should be wrappen in InputParameter class with static variables
void MatchParameterName();
void ProcessParameter();

bool OpenFile(std::ifstream& logFile, char* logFilename)
{
  logFile.open(logFilename, std::ios::in);

  if(logFile.is_open())
  {
    std::cout << logFilename << " file opened successfully\n";
    return true;
  }
  else
  {
    std::cout << "Cannot open "<< logFilename << "\n";
    return false;
  }
}

bool ReadParams()
{
	std::ifstream settings;
	std::string settingsLine;

	OpenFile(settings, const_cast<char*>(std::string("settings").c_str())); // XD

	// 2st position is parameter name, 6th position is value parameter
	// https://regex101.com/r/rHZUC5/2
	std::regex settingsParamRegex("(^\040*)(\\w*)(\040*)(=)(\040*)(\\w*$|\".*)");

	const unsigned nameParamPos = 2;
	const unsigned valueParamPos = 6;

	// TODO: Maybe it be taken from std::regex member function - if all groups were found...
	const unsigned validParameterMatchesSize = 7;

	std::string info;

	while (std::getline(settings, settingsLine))
	{
		std::smatch matchesSettingsLine;
		std::regex_search(settingsLine, matchesSettingsLine, settingsParamRegex);

		// If name of the parameter and its value was found
		if (matchesSettingsLine.size() == validParameterMatchesSize)
		{
			// Put value or search for name

			std::string parameterName = matchesSettingsLine[nameParamPos].str();
			std::string parameterValue = matchesSettingsLine[valueParamPos].str();

			// TODO: OBJECT ORIIENTED DISPACTH OF THE PROPER VALUE EXTRACION - OR JUST USE FUNCTION POINTERS

			try
			{
				//[[left]] succesfully creating an object means also succesflully pushed element to the array?
				InputParameter parameter(parameterName, parameterValue);
			}
			catch (...) // Bad name or bad value of parameter...
			{

			}

//			// That was delegated to the constructor...
//			// Search for the parameter name
//			for (auto& parameterElem: InputParameter::parameters)
//			{
//				// If the parameter was found in parameters array
//				if (parameterElem.parameterName_m == parameterName)
//				{
//					try
//					{
//						//InputParameter(parameterName);
//					}
//					catch (...) // TODO: Develop specific parameter...
//					{
//
//					}
//					// Dispatch the check of correctness for parameter value with parameter character...
//					// Have a table of function pointers or how to dispatch the constuctor?
//					// Just a function with if..else ladder based on the parameter type? Maybe wrap the array
//					// together with this function and create a singleton class?
//
//					//Check of paremater, then create an object (using a virtual constructor pattern?)
//				}
//			}


		}


		// DEBUG
		if (true)
		{
			info += "size of matches " + std::to_string(matchesSettingsLine.size()) + "\n";
			for (unsigned i = 0; i < matchesSettingsLine.size(); ++i)
			{
				info += matchesSettingsLine.str(i) + "\n";
			}
			info += "\n";
		}
	}

	std::cout << "\n\n" << info << "\n\n";

	settings.close();
	return true;
}

template <class firstType, class secondType>
bool SortBySecondElem(const std::pair<firstType, secondType> &a, const std::pair<firstType, secondType> &b)
{
    return (a.second > b.second);
}

unsigned CountUsedGroups(std::map<std::string, unsigned> bitfieldSizes, unsigned alignment)
{
	unsigned sumOfBitfieldSizes = 0;

	for (const auto& elem: bitfieldSizes)
	{
		sumOfBitfieldSizes += elem.second;
	}

	unsigned modulo = sumOfBitfieldSizes % alignment;
	if (modulo)
	{
		//throw - or don't - there is no reason for throwing
		std::cout << "All bitfields sizes summed are NOT DIVISIBLE by the alignment!\n";
		std::cout << "Modulo is " << modulo << std::endl;
	}

	return sumOfBitfieldSizes / alignment;
}

void AddToGlobalBitfieldOutputString(const std::vector<std::pair<std::string, unsigned>>& groupedBitfieldSizesVec)
{
	//TODO: Adding to global bitfieldOutput

	bitfieldOutput += preamble; // Preamble contains \n currently...
	for (const auto& elem: groupedBitfieldSizesVec)
	{
		bitfieldOutput += elem.first + ":";
		bitfieldOutput += std::to_string(elem.second)+ "\n";
	}
	bitfieldOutput += interamble;
	for (std::vector<std::pair<std::string, unsigned>>::const_reverse_iterator iter = groupedBitfieldSizesVec.rbegin();
		iter != groupedBitfieldSizesVec.rend();
		iter++)
	{
		bitfieldOutput += iter->first + ":";
		bitfieldOutput += std::to_string(iter->second) + "\n";
	}
	bitfieldOutput += postamble;
	bitfieldOutput += "\n";
}

void AddToGlobalBitfieldOutputStringFull() // Not called currently
{
	// Needs to contain the unsigned also...
	std::vector<std::string> reverseNamesVec;

	unsigned alignmentCounter = 0;

	std:: cout << "\n\n\n in order FULL \n\n\n";

	for (auto& mapElem: outputBitfield)
	{
		// Print the preamble if we are at the begining of new group
		if (alignmentCounter == 0)
		{
			std::cout << preamble;
		}

		// Add the size of the bitfield to the counter
		alignmentCounter += mapElem.second;

		// This is printing with the comments...
		std::cout << (fullBitfieldNames.find(mapElem.first))->second;

		// Regex for removing comments...
		const std::regex bitFieldGrouped("^\040*\\w+\040*\\w+\040*:\040*[0-9]+");
		std::cmatch matchesGrouped;

		// If new padding was added it will not be found in the fullBitfieldNames...
		if (fullBitfieldNames.find(mapElem.first) == fullBitfieldNames.end())
		{
			// The entry wasn't found, let's do something else...
			// entry is pobably padding - construct it...
			std::string paddingEntry =
					indent + padding + " " + mapElem.first + ":" + std::to_string(mapElem.second) + ";" + "\n";

			reverseNamesVec.push_back(paddingEntry);

			// cout before the interamble...
			std::cout << paddingEntry;
		}
		else // the expected path
		{
			std::regex_search((fullBitfieldNames.find(mapElem.first))->second.c_str(), matchesGrouped, bitFieldGrouped);

			// Remove comments before pushing back...
			reverseNamesVec.push_back(std::string(matchesGrouped[0]) + ";" + "\n");
		}
		// Remove comments before pushing back...
		//reverseNamesVec.push_back(std::string(matchesGrouped[0]) + "\n");
		//reverseNamesVec.push_back((fullBitfieldNames.find(mapElem.first))->second);

		if (alignmentCounter == 16)
		{
			alignmentCounter = 0;

			std::cout << interamble;

			// Print elements in reverse order
			for (std::vector<std::string>::reverse_iterator iter = reverseNamesVec.rbegin();
				iter != reverseNamesVec.rend();
				iter++)
			{
				std::cout << *iter;
			}
			std::cout << postamble << "\n";
			reverseNamesVec.clear();
		}
	}
	std::cout << "\n\n\n";

	std::cout << "PRINTING THE bitfieldOutput: \n\n";
	std::cout << bitfieldOutput;
}

void AddToGlobalBitfieldOutputStringFull_printingCommentsInElse()
{
	// Needs to contain the unsigned also...
	std::vector<std::string> reverseNamesVec;

	unsigned alignmentCounter = 0;

	std::cout << "\nConstructing bitfield output...\n";

	for (auto& mapElem: outputBitfield)
	{
		// Print the preamble if we are at the begining of new group
		if (alignmentCounter == 0)
		{
			//std::cout << preamble;
			bitfieldOutput += preamble;
		}

		// Add the size of the bitfield to the counter
		alignmentCounter += mapElem.second;

		// This is printing with the comments...
		reverseNamesVec.push_back((fullBitfieldNames.find(mapElem.first))->second);
		//std::cout << (fullBitfieldNames.find(mapElem.first))->second;

		// Regex for removing comments...
		const std::regex bitFieldGrouped("^\040*\\w+\040*\\w+\040*:\040*[0-9]+");
		std::cmatch matchesGrouped;

		// If new padding was added it will not be found in the fullBitfieldNames...
		if (fullBitfieldNames.find(mapElem.first) == fullBitfieldNames.end())
		{
			// The entry wasn't found, let's do something else...
			// entry is pobably padding - construct it...
			std::string paddingEntry =
					indent + padding + " " + mapElem.first + ":" + std::to_string(mapElem.second) + ";" + "\n";

			reverseNamesVec.push_back(paddingEntry);

			// cout before the interamble...
			//std::cout << paddingEntry;
			bitfieldOutput += paddingEntry;
		}
		else // the expected path
		{
			std::regex_search((fullBitfieldNames.find(mapElem.first))->second.c_str(), matchesGrouped, bitFieldGrouped);

			// Remove comments before pushing back...
			//std::cout << (std::string(matchesGrouped[0]) + ";" + "\n");
			bitfieldOutput += (std::string(matchesGrouped[0]) + ";" + "\n");
			//reverseNamesVec.push_back(std::string(matchesGrouped[0]) + ";" + "\n");
		}
		// Remove comments before pushing back...
		//reverseNamesVec.push_back(std::string(matchesGrouped[0]) + "\n");
		//reverseNamesVec.push_back((fullBitfieldNames.find(mapElem.first))->second);

		if (alignmentCounter == 16)
		{
			alignmentCounter = 0;

			//std::cout << interamble;
			bitfieldOutput += interamble;

			// Print elements in reverse order
			for (std::vector<std::string>::reverse_iterator iter = reverseNamesVec.rbegin();
				iter != reverseNamesVec.rend();
				iter++)
			{
				//std::cout << *iter;
				bitfieldOutput += *iter;
			}
			//std::cout << postamble << "\n";
			bitfieldOutput += postamble +	 "\n";
			reverseNamesVec.clear();
		}
	}
}

void RemovePaddingEntries()
{
	// Map bitfieldSizes is ready at this point, create a vector of pairs and copy the pairs
	std::map<std::string, unsigned>::iterator iter;

	// During the insertion to vector padding and dummies varaibles will be removed
	for (iter = bitfieldSizes.begin(); iter != bitfieldSizes.end(); iter++)
	{
		// Only push if name isn't matching the padding regex...
		std::regex paddingRegex("padding|dummy|doNotUse");

		if (!std::regex_search(iter->first, paddingRegex))
		{
			bitfieldSizesVec.push_back(std::make_pair(iter->first, iter->second));
		}
	}
}

// Finding new layout for one group!!!!!! Only one group is processed here!!!
void FindNewLayoutForOneGroup(unsigned alignment, unsigned bitfieldGroup, unsigned& paddingInstance)
{
	// Pick the highest width bitfield from the bitfieldSizesVec vector
	unsigned counter = 0;
	std::vector<std::pair<std::string, unsigned>>::iterator iter = bitfieldSizesVec.begin();

	// Vector of bitfields that will be grouped together
	std::vector<std::pair<std::string, unsigned>> groupedBitfieldSizesVec;

	while (counter < alignment && iter != bitfieldSizesVec.end()) // or the vector is empty
	{
		if (iter->second + counter <= alignment)
		{
			// Add the chosen bitfield size to total size
			counter += iter->second;

			// Push the element to local vector
			groupedBitfieldSizesVec.push_back(*iter);

			// Erase the used element for available elements, make the iter point to next valid element
			iter = bitfieldSizesVec.erase(iter);
		}
		else
		{
			// Element does not fit - try the next one
			iter++;
		}
	}

	// Iteration has ended - check if the bitfields are perfectly filled to the alignment boundary or if
	// padding needs to be used
	if (counter != alignment)
	{
		// Add padding
		groupedBitfieldSizesVec.push_back(std::make_pair("padding" + std::to_string(paddingInstance), alignment - counter));
		paddingInstance++;
	}

	// Add to a global order vector...
	// Appending the elements of groupedBitfieldSizesVec to the end of outputBitfield
	outputBitfield.insert(outputBitfield.end(), groupedBitfieldSizesVec.begin(), groupedBitfieldSizesVec.end());
}

void ProcessExtractedLines(std::ifstream& logFile)
{
	std::cout << "Finding new layout...\n";

	std::string line;

	std::string cache;

	// Will be the pair.second for fullBitfieldNames map
	std::string fullBitfieldName;

	std::string fullEntry;

	std::cmatch matchesBitFieldGrouped;

	//const std::regex bitField(".*");
	//const std::regex bitField("^\\s*\\w+\\s*\\w+\\s*:\\s*[0-9]+");
	//const std::regex bitField("^\040*\\w+\040*\\w+\040*:\040*[0-9]+");
	const std::regex bitFieldGrouped("(^\040*\\w+)\040*(\\w+\040*)(:)(\040*)([0-9]+)");

	// TODO: Separate comments after bitfield regex?
	const std::regex bitFieldCommentGrouped("(^\040*\\w+\040*\\w+\040*:\040*[0-9]+\040*;\040*)(\\/\\/.*)");

	//const std::regex bitField("^\040*\\w+");
	const std::regex nonBitFieldVariable("non");


	const unsigned bitfieldNamePos = 2;
	const unsigned bitfieldWidthPos = 5;

	// Regex for bitfields variables
	// tested on https://regex101.com/
	//^\040*\w+\040*\w+\040*:\040*[0-9]+

	// Regex for NON-bitfield variables
	// tested on https://regex101.com/
	// has problem with int something[MACRO/2];
	//^\040*\w+\040*\w+\040*[^:]\040*[^0-9\s]+[;]

	// Regex for comments
	//^\040*\/\/|^\040*\*\/|^\040*\*
	// TODO: Use if the comments immediately follows the bitfield

	bool immediatelyFollowedComment = false;

	bool print = true;

	std::string lastBitfieldName;
	bool isBitfieldEntryActive = false;
	bool isHypotheticallyCommentAbove = false;

	bool isDuplicateProcessed = false;

	while (std::getline(logFile, line)) //TODO: Have two lines - oldLine and line
										// - if line matches regex - then start new processing
										// - if line does not match regex - then append the string to given entry
	{


		// If line starts with comments - add to cached
		const std::regex comment("^\040*\\/\\/|^\040*\\*\\/|^\040*\\*");

		if (std::regex_search(line.c_str(), comment))
		{
			cache += line + "\n";
			isHypotheticallyCommentAbove = true;
		}

		std::regex_search(line.c_str(), matchesBitFieldGrouped, bitFieldGrouped);

		// TODO: If has been found - do not repost
		// Add only if it has been found
		if (matchesBitFieldGrouped.size() > 2) // Why 2? How the submatches work? //Check this, but should be robust
		{
			if (print)
			{
				std::cmatch matchesComments;
				std::regex_search(line.c_str(), matchesComments, bitFieldCommentGrouped);

				print = false;
				info += std::to_string(matchesBitFieldGrouped.size()) + "\n";
				for (unsigned i = 0; i < matchesBitFieldGrouped.size(); ++i)
				{
					info += matchesBitFieldGrouped.str(i) + "\n";
				}

				for (unsigned i = 0; i < matchesComments.size(); ++i)
				{
					info += matchesComments.str(i) + "\n";
				}

			}
			// Construct the element to be inserted into the map
			std::pair<std::string, unsigned>
				bitfieldSizesElem(matchesBitFieldGrouped.str(bitfieldNamePos), std::stoul(matchesBitFieldGrouped.str(bitfieldWidthPos)));

			std::cmatch matchesComments;
			std::regex_search(line.c_str(), matchesComments, bitFieldCommentGrouped);

//			// TODO: Edit this to add spaces in between comments and bitfields
//			std::pair<std::string, std::string>
//				fullBitfielElem(matchesBitFieldGrouped.str(bitfieldNamePos), matchesBitFieldGrouped.str(1) + matchesBitFieldGrouped.str(2) + " " + matchesComments.str(2));
//


			//std::cout << "\n\n\n\n\n";
			//std::cout << fullBitfielElem.first;
			//std::cout << fullBitfielElem.second;

			std::map<std::string, unsigned>::iterator iter = bitfieldSizes.find(matchesBitFieldGrouped.str(bitfieldNamePos));

			// Check if it is not a duplicate of already seen bitfield
			if (iter == bitfieldSizes.end())
			{
				bitfieldSizes.insert(bitfieldSizesElem);
				isDuplicateProcessed = false;
//				bitfieldWholeEntryMap.insert(fullBitfielElem);
			}
			else // Bitfield already inserted, duplicate is found...
			{
				isDuplicateProcessed = true;

				// Need to check if bitfields have the same width...
				if (iter->second != bitfieldSizesElem.second)
				{
					// The bitfield duplicate has been found and has a different width!
					// throw
				}

//				//TODO: Need to check if there is a comment is the #else...
//				std::map<std::string, std::string>::iterator commentIter = bitfieldWholeEntryMap.find(iter->first);
//
//				// If the newly found line containing duplicate bitfield has a comment or the comment is more verbose
//				if (commentIter->second.length() < fullBitfielElem.second.length())
//				{
//
//					// Assign the longer line as the full entry
//					commentIter->second = fullBitfielElem.second;
//
//				}
			}

			// TODO: Throw away the unnecessary things - #defs


			std::string fullEntry;

			if (isHypotheticallyCommentAbove == true)
			{
				// Bitfield found - it has to be connected to cached comment that is above it
				fullEntry = cache;
			}
			// Full entry construction
			fullEntry +=
					matchesBitFieldGrouped.str(1) + " " +
					matchesBitFieldGrouped.str(2) + ":" + matchesBitFieldGrouped.str(bitfieldWidthPos) + ";" +
					" " + matchesComments.str(2) + "\n";

			// Bitfield has been found

			std::pair<std::string, std::string>
							fullBitfieldElem(matchesBitFieldGrouped.str(bitfieldNamePos), fullEntry);


			if (isDuplicateProcessed)
			{
				if ((fullBitfieldNames.find(fullBitfieldElem.first))->second.length() < fullBitfieldElem.second.length())
				{
					fullBitfieldNames.find(fullBitfieldElem.first)->second = fullBitfieldElem.second;
				}
			}
//
//			std::pair<std::string, std::string>
//				fullBitfieldElem(matchesBitFieldGrouped.str(bitfieldNamePos), matchesBitFieldGrouped.str(0) + " " + matchesComments.str(2) + "\n");

			// add the previous to the map
			//std::map<std::string, std::string>::iterator iter = fullBitfieldNames.find(lastBitfieldName);
			fullBitfieldNames.insert(fullBitfieldElem);

			isBitfieldEntryActive = true; // instead of that - just empty string (??)
			lastBitfieldName = fullBitfieldElem.first;

		} // BITFIELD FOUND...
		else
		{
			if (isBitfieldEntryActive && cache.empty() == false)
			{
				//std::cout << "here";
				// Immediately followed comment!
				// Apend to previously found bitfield and clear the cache

				// If it was processes then would be inserted two times...
				// Check if this found entry has more information
				/// Check only if the bitfield was seen already...!!!!!!!!!!!!!!

				// If processes duplicate has the comment information already
				if (isDuplicateProcessed && (fullBitfieldNames.find(lastBitfieldName)->second.find(cache) != std::string::npos))
				{
					// Just clear the cached, we have this info already
					cache.clear();
				}
				else
				{
					// Unique information, add to bitfield entry and clear the cache
					fullBitfieldNames.find(lastBitfieldName)->second += cache;
					cache.clear();
				}

				isBitfieldEntryActive = true;

			}
			else //if (!isHypotheticallyCommentAbove) //?????????
			{

				std::regex preprocessorDirective("#");

				if (!std::regex_search(line, preprocessorDirective))
				{
					output += cache;

					// To not have duplicates if we are processing the same line as it was cached...
					if (cache != (line + "\n"))
					{
						output += line + "\n";
					}
				}
				cache.clear();
				// repost and empty cache if no bitfield (the scope) AND no preprocessor directive
				isBitfieldEntryActive = false;

				// Hypothesy falsivied
				isHypotheticallyCommentAbove = false;
			}
		}



		// Find duplicates and throw if two the same bitfields have different sizes!

	}

    unsigned usedGroup = CountUsedGroups(bitfieldSizes, 16);

    std::cout << "We have " << usedGroup
			  << " groups of bitfields given the alignment of " << 16 << "\n";

	// Remove inserted padding entries
	RemovePaddingEntries();

	// Sort the vector by decreasing order of its pair's second value
	std::sort(bitfieldSizesVec.begin(), bitfieldSizesVec.end(), SortBySecondElem<std::string, unsigned>);

	// Only increase padding when padding is used...
	unsigned paddingInstance = 0;
	unsigned bitfieldGroup = 0;

	// bitfieldSizesVec is ready and without padding entries, find new layout and group bitfields
	while (!bitfieldSizesVec.empty())
	{
		FindNewLayoutForOneGroup(16, bitfieldGroup, paddingInstance);
		bitfieldGroup++;
	}

	std::cout << "\n\nNew number of bitfield group is : " << bitfieldGroup << "\n\n";

	//AddToGlobalBitfieldOutputStringFull();
	AddToGlobalBitfieldOutputStringFull_printingCommentsInElse();

	// Print the reposted part plus the bitfieldOutput...
	//std::cout << output + bitfieldOutput;

	std::cout << "\n\n\n\n\n";
	std::cout << bitfieldOutput;
}


int main(int argc, char* argv[])
{
	ReadParams();

	std::ifstream logFile;

	if (OpenFile(logFile, argv[1]))
	{
		ProcessExtractedLines(logFile);
	}
}
