#include "soalogger.h"
#include <iostream>



sl sl::log;
sleol sl::end;

sl &sl::operator<<(const char *_text) {
	text += _text;
	std::cerr << _text;

	return *this;
}

sl &sl::operator<<(const std::string &_text) {
	text += _text;
	std::cerr << _text;

	return *this;
}

sl &sl::operator<<(int _int) {
	std::cerr << _int;

	return *this;
}

sl &sl::operator<<(unsigned int _int) {
	std::cerr << _int;

	return *this;
}

sl &sl::operator<<(float _float) {
	std::cerr << _float;

	return *this;
}

sl &sl::operator<<(sleol) {
	text += "\n";
	std::cerr << std::endl;

	return *this;
}
