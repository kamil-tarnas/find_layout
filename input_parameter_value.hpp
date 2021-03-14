#ifndef INPUT_PARAMETER_VALUE_HPP
#define INPUT_PARAMETER_VALUE_HPP


class InputParameterValue // Only an (pure) abstract class, defining interface...
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


//InputParameterType::BOOLEAN // TODO: Static member variable of InputParameterType? //-> would be implicit
// TODO: Wouldn't it be useless - we could not call get or set methods anyway?
class InputParameterBoolean : public InputParameterValue
{
public:
	InputParameterBoolean(std::string parameterValue);

private:
	bool value_m;
};


class InputParameterRegex : public InputParameterValue
{
public:
	InputParameterRegex(std::string parameterValue);

private:
	std::regex value_m;

};


class InputParameterInteger : public InputParameterValue
{
public:
	InputParameterInteger(std::string parameterValue);

private:
	int value_m;
};


class InputParameterString : public InputParameterValue
{
public:
	InputParameterString(std::string parameterValue);

private:
	std::string value_m;
};


#endif //#ifndef INPUT_PARAMETER_VALUE_HPP
