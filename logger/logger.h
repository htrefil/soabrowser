#ifndef logger_h
#define logger_h

#include <QFile>
#include <QTextStream>



class Logger : public QTextStream
{
public:
	static Logger ins;

	Logger();

	void SetLocation(const QString &);
	const QString &Location() const;

	Logger &Header();
	Logger &Warning();
	Logger &Error();
	Logger &Info();

private:
	QString location, timestamp;
	QFile file;
};

#endif
