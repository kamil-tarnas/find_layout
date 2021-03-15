#ifndef INPUT_PARAMETER_HPP
#define INPUT_PARAMETER_HPP

#include "input_parameter_value.hpp"


enum class InputParameterType
{
	BOOLEAN = 0,
	STRING,
	REGEX,
	INTEGER,
	NO_OF_PARAMETER_TYPES
};

class InputParameterName
{
public:
	// Implicit, for switch..case conversions and returning index
	// in the ParamArray
	operator unsigned() const;

	InputParameterType GetType() const;

	// Implicit, for implicitly conversion from std::string
	InputParameterName(std::string);

private:
	const std::string name_m;
	mutable unsigned elemIndex_m; // Index in std::map instead of std::array?
};


class InputParameter
{
public:
	static constexpr unsigned numOfParameters = 10;

	using ParamArray = std::array<InputParameter, numOfParameters>;

	static ParamArray parameters;

	bool AssignParameterValue(std::string parameterValue);

	template<typename T>
	static T GetParameterValue(InputParameterName name)
	{
		InputParameterValue* parameterValue_p = (InputParameter::parameters[name].parameterValue_mp);

		switch (name.GetType())
		{
			// TODO: So much redundancy going on here...
			case InputParameterType::BOOLEAN:
			{
				return static_cast<InputParameterBoolean*>(parameterValue_p)->GetParameterValue();
			}
			case InputParameterType::INTEGER:
			{
				return static_cast<InputParameterInteger*>(parameterValue_p)->GetParameterValue();
			}
			case InputParameterType::REGEX:
			{
				return static_cast<InputParameterRegex*>(parameterValue_p)->GetParameterValue();
			}
			case InputParameterType::STRING:
			{
				return static_cast<InputParameterString*>(parameterValue_p)->GetParameterValue();
			}
			default:
			{
				//throw
			}
		}
	}

	InputParameter(std::string parameterName, std::string parameterValue);

	InputParameter(std::string&& parameterName, InputParameterType parameterType);

	InputParameter& operator=(InputParameter&& rhs);

	InputParameter(const InputParameter& rhs);

	~InputParameter();

private:

	// Not defined as type InputParameterName for avoiding
	// throwing move...
	std::string parameterName_m;

	InputParameterType parameterType_m;

	InputParameterValue* parameterValue_mp;

	//TODO: Can be defined as inner class instead of firend
	friend class InputParameterName;
};


bool ReadParams();


#endif //#ifndef INPUT_PARAMETER_HPP
