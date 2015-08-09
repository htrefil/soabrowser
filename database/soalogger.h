#ifndef soalogger_h
#define soalogger_h

#include <string>



struct sleol {
	bool flush;
};

class sl
{
public:
	static sl log;
	static sleol end;
	std::string text;

	sl &operator<<(const char *);
	sl &operator<<(const std::string &);
	sl &operator<<(int);
	sl &operator<<(unsigned int);
	sl &operator<<(float);
	sl &operator<<(sleol);
};

#endif
