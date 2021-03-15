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


bool InputParameter::AssignParameterValue(std::string parameterValue)
{
	// Polimorphic type that would get the dynamicly-typed object in following cases
	InputParameterValue* parameterToBeAssigned = nullptr;

	// TODO: Have a return value instead of multiple returns...
	// Useful in many places - extract to a separate entity...
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

	/// We are inside the constructor of the object here...
	if (parameterToBeAssigned) // if the object was created...
	{
		unsigned parameterIndexInArray = 0;

		for (auto& parameter: parameters)
		{
			// Check both parameter name and parameters type
			// TODO: Probably is redundand and guaranteed to match already...
			if (this->parameterName_m == parameter.parameterName_m &&
					this->parameterType_m == parameter.parameterType_m)
			{
				// Assign just the pointer - don't modify another fields
				this->parameterValue_mp = parameterToBeAssigned;
				break;
			}
			parameterIndexInArray++;
		}
		// Everything is successful at this point, push the constructed parameter into the array
		// TODO: Move? Can it be moved? "this" pointer is const? Needs move assignment operator?
		// Can "this" pointer be moved, can it be an r-value?
		parameters[parameterIndexInArray] = std::move(*this);

		return true;
	}
	else
	{
		//thorw
		// or return false?
		return false;
	}
}


InputParameter::InputParameter(std::string parameterName, std::string parameterValue)
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
	}
	else
	{
		// throw
	}
}


// This ctor would be intially called when constucting the array of values
// Should take it by the moves? Constexpr string might be a improvement...
InputParameter::InputParameter(std::string&& parameterName, InputParameterType parameterType) :
				parameterName_m(std::move(parameterName)),
				parameterType_m(parameterType),
				parameterValue_mp(nullptr)
{

}


InputParameter& InputParameter::operator=(InputParameter&& rhs) //TODO: We could have run-time errors in case of dismatches...
{
	// We could not even bother with assigning the parameters to
	// the object that is being created with InputParameter default constructor
	// (the one called from ReadParams), but lets copy the content anyway
	parameterName_m = std::move(rhs.parameterName_m); //TODO: [[left]] Why copy if I can move? Why does it crash?
	parameterType_m = rhs.parameterType_m;

	// The parameterValue_mp is guaranteed to be a nullptr at the point
	// of calling the move assignment operator, but swap the pointers
	// anyway, it would be at maximum one move (value assignment) more
	// and could prevent from memory leaks in the future
	InputParameterValue* tmp = parameterValue_mp;
	parameterValue_mp = rhs.parameterValue_mp;
	rhs.parameterValue_mp = tmp;
	return *this;
}


InputParameter::InputParameter(const InputParameter& rhs) :
				parameterName_m(rhs.parameterName_m),
				parameterType_m(rhs.parameterType_m),
				parameterValue_mp(rhs.parameterValue_mp)
{
	// TODO: Seems it is not even called - no std::cout output
	//       no calls to this funcition in dbg build...
	// https://stackoverflow.com/questions/32938907/error-using-defaulted-copy-constructor-deleted-function
	// https://stackoverflow.com/questions/26749505/error-use-of-deleted-function-why
	// https://stackoverflow.com/questions/33319731/use-of-deleted-function-error-occurs-when-the-constructor-of-a-class-calls-its-m
	// TODO: [[left]] This is needed for InputParameter::parameters initialization, but why?
	// Only the input string r-valued constructor seems to be called...
	std::cout << "InputParameter's copy ctor called";
	//= default; // What for then if move constructor is called?
}


InputParameter::~InputParameter()
{
	// Not having the virtual destructor is UB in the case of InputParameterValue, but...
	// The dtor is not even needed for the mem-leak sake
	// All of heap-allocated and stack allocated InputParameter objects have parameterValue_mp
	// set to nulltpr, so the delete is noop
	// In case of static allocations - the heap would be teared down with the program exit

	delete parameterValue_mp;
}


// TODO: Hold the values sorted - it will be quicker for accessing
// TODO: Map in which InputParameter is a key - it should be comparable with the other things?
// only == needs to be defined?
// TODO: How to make it look for the dyamic hierarchy? Could this hold somehow the type of
// the parameter as the second value (typeof (GCC specific extension (do not use), typeid (okay?), decltype and so on...)?
// TODO: Have values or default values...
// TODO: Currently cannot be constexpr - is having string typed member variables...
// TODO: Yes, use a map for easier search...
// TODO: Make it a map and use find()
InputParameter::ParamArray InputParameter::parameters = //TODO: Remove const, maybe make mutable in the future...
{
	InputParameter(std::string("preamble"), InputParameterType::STRING), // static reflections to compare types?
	InputParameter(std::string("interamble"), InputParameterType::STRING),
	InputParameter(std::string("postamble"), InputParameterType::STRING),
	InputParameter(std::string("alignment"), InputParameterType::INTEGER),
	InputParameter(std::string("ignore"), InputParameterType::REGEX),
	InputParameter(std::string("indent"), InputParameterType::STRING),
	InputParameter(std::string("padding"), InputParameterType::STRING),
	InputParameter(std::string("files"), InputParameterType::REGEX), //???
	InputParameter(std::string("prefix"), InputParameterType::STRING),
	InputParameter(std::string("mirror"), InputParameterType::BOOLEAN)
};


InputParameterName::operator unsigned() const
{
	return elemIndex_m;
}


InputParameterType InputParameterName::GetType() const
{
	return InputParameter::parameters[elemIndex_m].parameterType_m;
}


InputParameterName::InputParameterName(std::string name) :
		name_m(name)
{
	unsigned parameterIndexInArray = 0;

	for (auto const& parameter: InputParameter::parameters)
	{
		if (parameter.parameterName_m == name_m)
		{
			break;
		}
		parameterIndexInArray++;
	}

	elemIndex_m = parameterIndexInArray;
}


// Free function that is befriended with the InputParameter?
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

			// Read params would just check the syntax of parameter
			// The c-tor would be responsible for actually checking the value...
			try
			{
				//[[left]] Any side effect of that? Is the destructor called? Is the object unusable? XD
				//[[left]] succesfully creating an object means also succesflully pushed element to the array?
				InputParameter parameter(parameterName, parameterValue); //Push to the array?
			}
			catch (...) // Bad name or bad value of parameter... Catch and continue? Do not process the parameter
			// print some error meassage to stderr?
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
