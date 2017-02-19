#include "Helpers.h"
#include <fstream>
#include <iostream>
#include <exception>

std::string fw::loadFile(const std::string& filename)
{
	std::string data;
	std::ifstream file(filename.c_str());
	if (file) {
		data.append((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();
	} else {
		throw std::runtime_error("Could not open file: " + filename);
	}
	return data;
}
