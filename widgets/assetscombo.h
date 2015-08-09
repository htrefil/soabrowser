#ifndef assetscombo_h
#define assetscombo_h

#include <QComboBox>



class SoaWg;
class SoaTb;
class SoaRw;

class AssetsCombo : public QComboBox
{
	Q_OBJECT

public:
	AssetsCombo(QWidget *, SoaWg *, SoaTb *);

	void Refresh(SoaRw *);

private:
	SoaWg *wg;
	SoaTb *tb;

private slots:
	void Slot();
};

#endif
