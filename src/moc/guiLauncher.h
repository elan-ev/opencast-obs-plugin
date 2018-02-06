#ifndef GUILAUNCHER_H
#define GUILAUNCHER_H

#include <QObject>
#include <QtWidgets>

class GuiLauncher : public QObject
{
    Q_OBJECT

public:
    explicit GuiLauncher(QWidget * parent = 0);
    ~GuiLauncher();

private:
    QWidget * parent;

private slots:
    void createAndShowDialogOpencastWidget(QString videoPath);
    void createAndShowInfoBoxWidget(QString labelText);

};

#endif // GUILAUNCHER_H
