#include "helpers/recents.h"
#include <QFileInfo>



Recents::Recents() {}

// Searches for the file, pushes it to front
// if found, adds it to front if not.
bool Recents::Add(const QString &file) {

	bool exists = QFileInfo(file).exists();

	for (Recents::iterator recent = begin(); recent != end(); ++recent)
		if (*recent == file) {
			if (exists)
				splice(begin(), *this, recent);
			else
				erase(recent);

			return exists;
		}

	if (exists) {
		push_front(file);
		while (size() > REC_MAX_CAPACITY)
			pop_back();
	}

	return exists;
}

// Moves the string at index to front and
// returns its address (if index is valid).
const QString *Recents::Select(unsigned int index) {

	if (index < size()) {
		if (index) {
			Recents::iterator it = begin();
			std::advance(it, index);
			splice(begin(), *this, it);
		}

		return &front();
	}
	else
		return 0;
}

void Recents::Serialize(QSettings &settings) {

	settings.beginWriteArray("Recents");
	int i = 0;
	for (Recents::iterator r = begin(); r != end(); ++r) {
		settings.setArrayIndex(i++);
		settings.setValue("file", *r);
	}
	settings.endArray();
}

void Recents::Deserialize(QSettings &settings) {

	clear();

	int count = settings.beginReadArray("Recents");
	for (int i = 0; i < count; ++i) {
		settings.setArrayIndex(i);

		QString file = settings.value("file", "").toString();
		if (QFileInfo(file).exists())
			push_back(file);
	}
	settings.endArray();
}
