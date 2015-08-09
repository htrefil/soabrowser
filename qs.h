#ifndef qs_h
#define qs_h

#include <string>

#define QS(qs_arg) (QString::fromStdString(qs_arg))
#define SQ(sq_arg) ((sq_arg).toStdString())



int Minimum(int, int, int);
int LevenshteinDistance(std::string, std::string, bool);

#endif
