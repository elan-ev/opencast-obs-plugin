#include "guiLauncher.h"

#include "dialogopencast.h"
#include "infoBox.h"

GuiLauncher::GuiLauncher(QWidget * parent) :
	QObject(parent),
	parent{parent}
{
}

GuiLauncher::~GuiLauncher()
{
}

void GuiLauncher::createAndShowDialogOpencastWidget(QString videoPath)
{
	DialogOpencast * dia = new DialogOpencast{parent};

	dia->prepareAndShow(videoPath);

	return;
}

void GuiLauncher::createAndShowInfoBoxWidget(QString labelText)
{
	InfoBox * ib = new InfoBox{parent};

	ib->prepareAndShow(labelText);

	return;
}
