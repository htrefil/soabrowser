#include "qs.h"
#include <QString>



int Minimum(int a, int b, int c) {
	if (a < b && a < c)
		return a;
	else if (b < c)
		return b;
	else
		return c;
}

std::string UrlWithoutHost(const std::string &s) {

	size_t pos = 0;
	for (unsigned count = 0; count < 3; ++count) {

		pos = s.find('/', pos);
		if (pos != std::string::npos)
			++pos;
		else
			return s;
	}

	std::string sub = s.substr(pos);
	return (sub.length() < 2) ? s : sub;
}

int LevenshteinDistance(std::string s, std::string t, bool removeHost) {

	if (removeHost) {
		s = UrlWithoutHost(s);
		t = UrlWithoutHost(t);
	}

	if (s == t) return 0;
	if (s.length() == 0) return t.length();
	if (t.length() == 0) return s.length();

	int vl = t.length() + 1;
	int *v0 = new int[vl], *v1 = new int[vl];

	for (int i = 0; i < vl; i++)
		v0[i] = i;

	for (unsigned int i = 0; i < s.length(); i++) {

		v1[0] = i + 1;

		for (unsigned int j = 0; j < t.length(); j++) {
			int cost = (s[i] == t[j]) ? 0 : 1;
			v1[j + 1] = Minimum(v1[j] + 1, v0[j + 1] + 1, v0[j] + cost);
		}

		for (int j = 0; j < vl; j++)
			v0[j] = v1[j];
	}

	int res = v1[t.length()];
	delete[] v0;
	delete[] v1;

	return res;
}
