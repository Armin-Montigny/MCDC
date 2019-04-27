#pragma once

#ifndef PREPROCESSOR_HPP
#define PREPROCESSOR_HPP

#include <string>

class SymbolTable
{

};

class Preprocessor
{
public:
	explicit Preprocessor(std::string &raw) : rawSource(raw) {};
	int run(std::string &preprocessedOutput);

protected:
	std::string rawSource;

};


#endif