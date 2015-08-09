#ifndef soacell_h
#define soacell_h

#include <string>

#define CE_DEF_ROW	0
#define CE_DEF_TXT	""
#define CE_DEF_NUM	-13021980



class SoaRw;

class SoaCe
{
public:
	enum CeType {
		cetInv,
		cetTxt,
		cetNum,
		cetRow,
		cetRowAlias
	};

	SoaCe();
	SoaCe(SoaRw *);
	SoaCe(const char *);
	SoaCe(const std::string &);
	SoaCe(const std::string &, CeType);
	SoaCe(int);

	bool operator!=(const SoaCe &) const;

	void Reset();

	SoaRw *Row() const;
	const std::string &Txt() const;
	std::string SqlTxt() const;
	int Num() const;
	bool IsNull() const;

	std::string ToString() const;

private:
	SoaRw *row;
	std::string txt;
	int num;
	unsigned char type;

	void CleanText();
};

#endif
