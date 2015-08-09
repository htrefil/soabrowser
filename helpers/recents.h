#ifndef recents_h
#define recents_h

#include <QSettings>
#include <QString>
#include <list>

#define REC_MAX_CAPACITY	20



class Recents : public std::list<QString>
{
public:
	Recents();

	bool Add(const QString &);
	const QString *Select(unsigned int);

	void Serialize(QSettings &);
	void Deserialize(QSettings &);
};

#endif
