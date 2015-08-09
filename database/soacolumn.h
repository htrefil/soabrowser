#ifndef soacolumn_h
#define soacolumn_h

#include "soaenums.h"
#include <string>



struct SoaClSc
{
	bool editable, critical;
	std::string merge, schema, mangledSchema;
	bool valid;	// applicable only when critical

	SoaClSc() :
		editable(true), critical(false), valid(false) {}

	SoaClSc(const std::string &_merge, bool _editable = true, bool _critical = true) :
		editable(_editable), critical(_critical), merge(_merge), schema(_merge), mangledSchema(_merge), valid(false) {}
};



class SoaTb;

class SoaCl
{
	friend class SoaTb;

public:
	static std::string TypeToString(SoaClType);
	static SoaClType StringToType(const std::string &);

	SoaCl();
	SoaCl(SoaTb *_fk, const SoaClSc &_schema = SoaClSc());
	SoaCl(const std::string &_name, SoaClType _type, const SoaClSc &_schema = SoaClSc());
	SoaCl(const std::string &_name, int _capacity, const SoaClSc &_schema = SoaClSc());
	SoaCl(const std::string &_name, const SoaClSc &_schema = SoaClSc());
	SoaCl(const std::string &_name, const std::string &_displayName, int _capacity);
	SoaCl(int, const SoaCl &);

	SoaTb *Fk() const;
	const std::string &Name() const;
	const std::string &DisplayName() const;
	SoaClType Type() const;
	int Index() const;
	const SoaClSc &Schema() const;
	bool IsMandatory() const;

	void ToSQL(std::string &, std::string &, SoaPlType);

private:
	SoaTb *fk;
	std::string name, displayName;
	SoaClType type;
	SoaClSc schema;
	int capacity, index;
};

#endif
