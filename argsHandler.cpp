#include "argsHandler.h"

#include <tuple>
#include <iostream>

using namespace std;
using namespace arguments;


#define print(input) cout << "Arguments Handler: " << input << endl

argumentsError::argumentsError(const char* inputMessage){
	message = inputMessage;
}

const char* argumentsError::what() const throw(){
	return message;
}

flag::flag(dataType inDataType, int expectedCount,
		vector<string> names){
	called = false;
	defaulted = false;
	dType = inDataType;
	requiredCount = expectedCount;
	callNames = names;
	defaultState = shared_ptr<flag>(new flag(*this));
}

flag::flag(vector<double> defaultValue, int expectedCount,
		vector<string> names) : flag(dataType::floatValue,
		expectedCount, names){
	defaulted = true;
	floatValue = defaultValue;
	if (expectedCount != -1 && expectedCount != defaultValue.size()){
		throw argumentsError("default value arguments count mismatch");
	}
	defaultState = shared_ptr<flag>(new flag(*this));
}

flag::flag(vector<long long> defaultValue, int expectedCount,
		vector<string> names) : flag(dataType::intValue,
		expectedCount, names){
	defaulted = true;
	intValue = defaultValue;
	if (expectedCount != -1 && expectedCount != defaultValue.size()){
		throw argumentsError("default value arguments count mismatch");
	}
	defaultState = shared_ptr<flag>(new flag(*this));
}

flag::flag(vector<string> defaultValue, int expectedCount,
		vector<string> names) : flag(dataType::stringValue,
		expectedCount, names){
	defaulted = true;
	stringValue = defaultValue;
	if (expectedCount != -1 && expectedCount != defaultValue.size()){
		throw argumentsError("default value arguments count mismatch");
	}
	defaultState = shared_ptr<flag>(new flag(*this));
}

void flag::passArgs(vector<string> args){
	if (called){
		throw argumentsError("flag was already called");
	}
	if (requiredCount != -1 && args.size() != requiredCount){
		throw argumentsError("invalid number of arguments passed");
	}
	if (defaulted){
		intValue = vector<long long>();
		floatValue = vector<double>();
		stringValue = vector<string>();
	}
	switch(dType){
		case dataType::intValue:{
			for (string item : args){
				try{
					intValue.push_back(stoll(item));
				}
				catch(...){
					throw argumentsError("invalid argument passed");
				}
			}
			break;
		}
		case dataType::floatValue:{
			for (string item : args){
				try{
					floatValue.push_back(stod(item));
				}
				catch(...){
					throw argumentsError("invalid argument passed");
				}
			}
			break;
		}
		case dataType::stringValue:{
			stringValue = args;
			break;
		}
	}
	called = true;
}

void flag::reset(){
	*this = *defaultState;
}

bool flag::isFlag(string value) const{
	for (string name : callNames){
		if (name == value){
			return true;
		}
	}
	return false;
}

bool flag::wasCalled() const{
	return called;
}

flag::operator bool(){
	return called || defaulted;
}

flag::operator vector<long long>(){
	if (!(defaulted || called)){
		throw argumentsError("flag not called yet and does not have\
			default value");
	}
	if (dType == dataType::intValue){
		return intValue;
	}
	else{
		throw argumentsError("flag is not of int type");
	}
}

flag::operator long long(){
	if (!(defaulted || called)){
		throw argumentsError("flag not called yet and does not have\
			default value");
	}
	if (dType == dataType::intValue){
		if (requiredCount == 1 || requiredCount == -1){
			return intValue[0];
		}
		else{
			throw argumentsError("cannot cast multi-value flag\
				to single value");
		}
	}
	else{
		throw argumentsError("flag is not of int type");
	}
}

flag::operator vector<double>(){
	if (!(defaulted || called)){
		throw argumentsError("flag not called yet and does not have\
			default value");
	}
	if (dType == dataType::floatValue){
		return floatValue;
	}
	else{
		throw argumentsError("flag is not of float type");
	}
}

flag::operator double(){
	if (!(defaulted || called)){
		throw argumentsError("flag not called yet and does not have\
			default value");
	}
	if (dType == dataType::floatValue){
		if (requiredCount == 1 || requiredCount == -1){
			return floatValue[0];
		}
		else{
			throw argumentsError("cannot cast multi-value flag\
				to single value");
		}
	}
	else{
		throw argumentsError("flag is not of int type");
	}
}

flag::operator vector<string>(){
	if (!(defaulted || called)){
		throw argumentsError("flag not called yet and does not have\
			default value");
	}
	if (dType == dataType::stringValue){
		return stringValue;
	}
	else{
		throw argumentsError("flag is not of string type");
	}
}

flag::operator string(){
	if (!(defaulted || called)){
		throw argumentsError("flag not called yet and does not have\
			default value");
	}
	if (dType == dataType::stringValue){
		if(stringValue.size() == 0){
			return "";
		}
		string output = stringValue[0];
		for (int i = 1; i < stringValue.size(); i++){
			output += " ";
			output += stringValue[i];
		}
		return output;
	}
	else{
		throw argumentsError("flag is not of string type");
	}
}

//generic arguments handler. flags are added by specifying a list of names
//(vector<string>), data type (see #define's above), and data size (int).
//data size is how many arguments should be passed to the given flag, -1
//means any number is valid. Also setHelp(string) will setup a help
//display if the user runs -help or -h. passArgs is where the args
//go. It takes the exact set sent to main. isFlagCalled returns bool
//for if the given flag was called in args. getFlagValue returns
//a void* pointer. The programmer must cast it to vector<data type>.
//if no argument was passed for the flag, getFlagValue will return NULL.

vector<tuple<int, string>> locateFlags(vector<string> args){
	vector<tuple<int, string>> output = vector<tuple<int, string>>();
	for (int i = 0; i < args.size(); i++){
		if (args[i][0] == '-'){
			try{
				stod(args[i]);
				continue;
			}
			catch(...){}
			string temp = args[i];
			temp.erase(0, 1);
			output.push_back(make_tuple(i, temp));
		}
	}
	return output;
}

argumentsHandler& argumentsHandler::addFlag(
		vector<string> flagNames,
		dataType dType, int dataSize){
	flags.push_back(flag(dType, dataSize, flagNames));
	return *this;
}
argumentsHandler& argumentsHandler::addFlag(
	vector<string> flagNames,
	vector<double> defaultValue, int dataSize){
	flags.push_back(flag(defaultValue, dataSize, flagNames));
	return *this;
}
argumentsHandler& argumentsHandler::addFlag(
	vector<string> flagNames,
	vector<long long> defaultValue, int dataSize){
	flags.push_back(flag(defaultValue, dataSize, flagNames));
	return *this;
}
argumentsHandler& argumentsHandler::addFlag(
	vector<string> flagNames,
	vector<string> defaultValue, int dataSize){
	flags.push_back(flag(defaultValue, dataSize, flagNames));
	return *this;
}

int argumentsHandler::getFlagIndex(string flagIdentifier){
	for (int i = 0; i < flags.size(); i++){
		if (flags[i].isFlag(flagIdentifier)){
			return i;
		}
	}
	throw argumentsError("flag not found");
}

void argumentsHandler::setFlagValue(string flagIdentifier,
		vector<string> rawDatum){
	int index = getFlagIndex(flagIdentifier);
	flags[index].passArgs(rawDatum);
}

void argumentsHandler::passArgs(int argc, char** argv){
	vector<string> args = vector<string>();
	for (int i = 1; i < argc; i++){
		args.push_back(string(argv[i]));
	}
	passArgs(args);
}

void argumentsHandler::passArgs(vector<string> args){
	vector<tuple<int, string>> flagLocations = locateFlags(args);
	for (int i = 0; i < flagLocations.size(); i++){
		int index = get<0>(flagLocations[i]);
		string flagIdentifier = get<1>(flagLocations[i]);
		int nextIndex;
		if (i != flagLocations.size() - 1){
			nextIndex = get<0>(flagLocations[i + 1]);
		}
		else{
			nextIndex = args.size();
		}
		vector<string> rawData;
		for (int j = index + 1; j < nextIndex; j++){
			rawData.push_back(args[j]);
		}
		try{
			setFlagValue(flagIdentifier, rawData);
		}
		catch(argumentsError e){
			print(e.what());
		}
	}
}

flag argumentsHandler::getFlagValue(string flagIdentifier){
	int index = getFlagIndex(flagIdentifier);
	return flags[index];
}

bool argumentsHandler::isFlagCalled(string flag){
	int index = getFlagIndex(flag);
	return flags[index];
}
