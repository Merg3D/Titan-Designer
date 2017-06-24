#include "TextFile.h"

#include <string>
#include <iostream>
#include <fstream>

TextFile::TextFile(const File& p_file)
{
	load(p_file);
}

String TextFile::load(const File & p_file)
{
	std::string line;
	String src;
	std::ifstream myfile(p_file.get_absolute_path().c_str());

	if (!myfile.is_open())
	{
		T_ERROR("Error loading file: " + p_file);
		return "";
	}

	while (std::getline(myfile, line))
		src.append(line + String("\n"));

	myfile.close();

	file = p_file;
	source = src;

	return source;
}

void TextFile::write(const String& p_source)
{
	std::ofstream myfile(file.get_absolute_path().c_str());

	if (!myfile.is_open())
	{
		T_ERROR(String("Error loading file: ") + file);
		return;
	}

	myfile << p_source.c_str();
	myfile.close();
}

String TextFile::get_source() const
{
	return source;
}
