#ifndef INPUT_PARAMETER_HPP
#define INPUT_PARAMETER_HPP


enum class InputParameterType
{
	BOOLEAN = 0,
	STRING,
	REGEX,
	INTEGER,
	NO_OF_PARAMETER_TYPES
};


bool ReadParams();


#endif //#ifndef INPUT_PARAMETER_HPP
