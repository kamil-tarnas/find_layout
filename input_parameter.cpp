#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <map>
#include <vector>
#include <array>

#include "input_parameter.hpp"
#include "utils.hpp"
#include "input_parameter_value.hpp"


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

	//[[left]] Constructor taking initializer_list would be needed for this...
	//public:
//	InputParameterArrayElement(InputParameterValue* inputValue_p, std::string parameterName, InputParameterType parameterType) :
//		parameterValue_mp(inputValue_p),
//        parameterName_m(parameterName),
//		parameterType_m(parameterType)
//	{
//
//	}
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
using ParamArray = std::array<InputParameterArrayElement, NUMBER_OF_PARAMETERS>; //Array of InputParameter?


class InputParameter // TODO: Virtual or abstract class... depending on that - call a proper function to extract value
{
public:

	static const InputParameter dummy;
	// TODO: This could be made 'private' //TODO: A type which contains itself - that is also not possible?
	static ParamArray parameters;

	bool AssignParameterValue(std::string parameterValue, std::string parameterName, InputParameterType parameterType)
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
		{ //Why not just the InputParameter...?
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

			//[[left]] Need to create just the InputParameter element instead of this workaround
//			InputParameterArrayElement element =
//					InputParameterArrayElement(parameterToBeAssigned, parameterName, parameterType);
			// Push to the array?

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

			AssignParameterValue(parameterValue, parameterName, parameterType);

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


bool ReadParams() // A static function of InputParameter class?
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
				InputParameter parameter(parameterName, parameterValue); //Push to the array?
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
