#include <string>
#include <regex>

#include "input_parameter_value.hpp"
#include "input_parameter.hpp"
#include "utils.hpp"



std::string InputParameterValue::GetParameterValue(std::string name)
{
	return ""; //[[left]] Remove, make InputParameterValue a pure abstract class...
}
//InputParameterType::BOOLEAN // TODO: Static member variable of InputParameterType? //-> would be implicit
// TODO: Wouldn't it be useless - we could not call get or set methods anyway?
InputParameterBoolean::InputParameterBoolean(std::string parameterValue)
{
	// This could be moved to the parameter constructor of InputParameterBoolean and could throw from there...
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




std::string InputParameterString::GetParameterValue(std::string name)
{
	// InputParameterValue* GetParameterValue(InputParameterName name)
	return value_m;
}


//InputParameterBoolean::~InputParameterBoolean() {}


//InputParameterType::REGEX
InputParameterRegex::InputParameterRegex(std::string parameterValue)
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


std::regex InputParameterRegex::GetParameterValue()
{
	return value_m;
}


//InputParameterRegex::~InputParameterRegex() {}


InputParameterInteger::InputParameterInteger(std::string parameterValue)
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


int InputParameterInteger::GetParameterValue()
{
	return value_m;
}


//InputParameterInteger::~InputParameterInteger() {}


InputParameterString::InputParameterString(std::string parameterValue)
{
	if (parameterValue.at(0) == '\"') // TODO: That is a very weak evaluation
	{
		// TODO: Need to have at least two characters...
		value_m = parameterValue.substr(1, parameterValue.size() - 2);
		// Write the parameter value
		// TODO: Remove the quotes? ""?
		//value_m = parameterValue;
	}
	else
	{
		// throw
	}
}




//InputParameterString::~InputParameterString() {}
