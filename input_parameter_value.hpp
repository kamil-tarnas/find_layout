#ifndef INPUT_PARAMETER_VALUE_HPP
#define INPUT_PARAMETER_VALUE_HPP

#include <typeinfo>
#include <cassert>


//[[left]] Make this FRICKEN THING PURE ABSTRACT!!!!!!!!!
class InputParameterValue // Only an (pure) abstract class, defining interface...
{
public:
	virtual ~InputParameterValue() = default; // Doing nothing currently, might be needed later...

	// An union of all of the classes that inherit from this base class?
	// Would that be a good design? Maybe? Lets try it?
	// so: std::sting InputParameter
	// Make the funtion for getting arguments a template function template with specialization?

	virtual std::string GetParameterValue(std::string name);
	// Does not have the member variable - Variable template and specialize in
	// the derived types?
//protected:
//	template<typename T>
//	T GetParameterValue()
//	{
//		return value_m;
//	}

	// TODO: Type erasure? How to get the value?
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
	virtual ~InputParameterBoolean() = default;

private:
	bool value_m;
};


class InputParameterRegex : public InputParameterValue
{
public:
	InputParameterRegex(std::string parameterValue);
	virtual ~InputParameterRegex() = default;

	std::regex GetParameterValue();

private:
	std::regex value_m;

};


class InputParameterInteger : public InputParameterValue
{
public:
	InputParameterInteger(std::string parameterValue);
	virtual ~InputParameterInteger() = default;

	int GetParameterValue();

private:
	int value_m;
};


class InputParameterString : public InputParameterValue
{
public:
	InputParameterString(std::string parameterValue);
	virtual ~InputParameterString() = default;

	// Get parameter value cannot be override BUT we can return a generic union type of bool, regex and so on...
	// That would get converted to the "real" value...
	// But we are relying on the functionality of getting the right pointer...
	virtual std::string GetParameterValue(std::string name) override;
	//bool GetParameterValue(); //[[left]]

private:
	std::string value_m;
};


//https://cpp-polska.pl/post/czym-jest-stdvariantij
class InputParameterUnion
{
	//typedef decltype int_type;
	// References to the typeid thing?
	//enum class InputParameterValueType { NUM, STR, VEC };
	// Does the type need to be known internally? Probably not.

	// Ony copy constructor and a 'normal' ctor is needed?
private:
	union
	{
		bool boolean_m;
		std::regex regex_m;
		std::string string_m;
		int integer_m;
	};

};

struct A {
  enum class Type { NUM, STR, VEC };
  Type type;
  A(int i_) : type(Type::NUM), i(i_) {}

  A(const std::string& s_) : type(Type::STR) {
    new ((void*)&s) std::string(s_);
  }

  A(const std::vector<A>& v_) : type(Type::VEC) {
    new ((void*)&v) std::vector<A>(v_);
  }

  A(const A& a) : type(a.type) {
    switch (type) {
      case Type::NUM:
        i = a.i;
        break;
      case Type::STR:
        new ((void*)&s) std::string(a.s);
        break;
      case Type::VEC:
        new ((void*)&v) std::vector<A>(a.v);
        break;
    }
  }

  A& operator=(const A& a) {
    switch (type) {
      case Type::STR:
        s.~basic_string<char>();
        break;
      case Type::VEC:
        v.~vector<A>();
        break;
      case Type::NUM:
        break;
    }
    type = a.type;
    switch (type) {
      case Type::NUM:
        i = a.i;
        break;
      case Type::STR:
        new ((void*)&s) std::string(a.s);
        break;
      case Type::VEC:
        new ((void*)&v) std::vector<A>(a.v);
        break;
    }
    return *this;
  }

  ~A() {
    switch (type) {
      case Type::STR:
        s.~basic_string<char>();
        break;
      case Type::VEC:
        v.~vector<A>();
        break;
      case Type::NUM:
        break;
    }
  }

  int& getInt() {
    assert(Type::NUM == type);
    return i;
  };

  std::string& getString() {
    assert(Type::STR == type);
    return s;
  };

  std::vector<A>& getVec() {
    assert(Type::VEC == type);
    return v;
  };

 private:
  union {
    int i;
    std::string s;
    std::vector<A> v;
  };
};


#endif //#ifndef INPUT_PARAMETER_VALUE_HPP
