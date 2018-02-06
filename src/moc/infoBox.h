#ifndef INFOBOX_H
#define INFOBOX_H

#include <QDialog>

namespace Ui {
class InfoBox;
}

class InfoBox : public QDialog
{
    Q_OBJECT

public:
    explicit InfoBox(QWidget *parent = 0);
    ~InfoBox();

    // init Methode
    void prepareAndShow(QString labelText);

    // event
    // pushButton(ok) -> close()

private:
    Ui::InfoBox *ui;
};

#endif // INFOBOX_H
