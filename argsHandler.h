#pragma once
#include <vector>
#include <string>
#include <exception>
#include <memory>

//data size: if -1 will change to whatever length is passed
//if 0, must pass 0 arguments and must be bool
//other must pass <dataSize> aruments
namespace arguments{
	class argumentsError : public std::exception{
		const char* message;
	public:
		argumentsError(const char* inputMessage);
		const char* what() const throw();
	};

	enum dataType{
		intValue,
		floatValue,
		stringValue
	};

	class flag{
		bool called;
		bool defaulted;
		dataType dType;
		int requiredCount;
		std::vector<long long> intValue;
		std::vector<double> floatValue;
		std::vector<std::string> stringValue;
		std::vector<std::string> callNames;
		std::shared_ptr<flag> defaultState;
	public:
		flag(dataType inDataType, int expectedCount,
			std::vector<std::string> names);
		flag(std::vector<double> defaultValue,
			int expectedCount,
			std::vector<std::string> names);
		flag(std::vector<long long> defaultValue,
			int expectedCount,
			std::vector<std::string> names);
		flag(std::vector<std::string> defaultValue,
			int expectedCount,
			std::vector<std::string> names);
		bool isFlag(std::string value) const;
		void passArgs(std::vector<std::string> args);
		void reset();
		bool wasCalled() const;
		operator bool();
		operator std::vector<long long>();
		operator long long();
		operator std::vector<double>();
		operator double();
		operator std::vector<std::string>();
		operator std::string();
	};

	class argumentsHandler{
		std::vector<flag> flags;
		int getFlagIndex(std::string flagIdentifier);
		void setFlagValue(std::string flagIdentifier,
			std::vector<std::string> rawDatum);
	public:
		argumentsHandler& addFlag(std::vector<std::string> flagNames,
			dataType dataType, int dataSize);
		argumentsHandler& addFlag(std::vector<std::string> flagNames,
			std::vector<double> defaultValue, int dataSize);
		argumentsHandler& addFlag(std::vector<std::string> flagNames,
			std::vector<long long> defaultValue, int dataSize);
		argumentsHandler& addFlag(std::vector<std::string> flagNames,
			std::vector<std::string> defaultValue, int dataSize);
		flag getFlagValue(std::string);
		void passArgs(int argc, char** argv);
		void passArgs(std::vector<std::string> args);
		bool isFlagCalled(std::string flagIdentifier);
	};
}
