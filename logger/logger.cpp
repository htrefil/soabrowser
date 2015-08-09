#include "logger/logger.h"
#include <QDir>
#include <QDateTime>
#include <QMessageBox>



Logger Logger::ins;

Logger::Logger() : QTextStream(),
	timestamp(QString::number(QDateTime::currentMSecsSinceEpoch())) {}

const QString &Logger::Location() const {
	return location;
}

void Logger::SetLocation(const QString &_location) {

	location = _location;

	// close previously opened file

	if (file.isOpen())
		file.close();

	// create new file

	QDir dir(location);
	QFileInfo info(dir, "soabrowser_log" + timestamp + ".txt");

	file.setFileName(info.absoluteFilePath());
	file.open(QIODevice::WriteOnly | QIODevice::Truncate);

	if (!file.isOpen())
		QMessageBox::warning(0, "Logger", "Could not create the log file in: " + info.absoluteFilePath());

	setDevice(&file);

	*this << "Log opened: " << QDateTime::currentDateTime().toString() << "\n\n";
}

Logger &Logger::Header() {

	*this << "I> " << QDateTime::currentDateTime().toString() << "> ";

	return *this;
}

Logger &Logger::Warning() {

	*this << "W> ";

	return *this;
}

Logger &Logger::Error() {

	*this << "E> ";

	return *this;
}

Logger &Logger::Info() {

	*this << "I> ";

	return *this;
}
