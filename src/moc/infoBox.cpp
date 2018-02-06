#include "infoBox.h"
#include "ui_infoBox.h"

InfoBox::InfoBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoBox)
{
    ui->setupUi(this);
}

InfoBox::~InfoBox()
{
    delete ui;
}

// init methode
void InfoBox::prepareAndShow(QString labelText)
{
	ui->label->setText(labelText);

	show();

	this->setAttribute(Qt::WA_DeleteOnClose);
}
