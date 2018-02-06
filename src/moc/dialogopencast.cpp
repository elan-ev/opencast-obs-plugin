#include "dialogopencast.h"
#include "ui_dialogopencast.h"

#include <thread>
#include "QMessageBox"
#include "QDateTime"

#include <libOpencastIngest.hpp>
#include "../utilsAndConfig.hpp"

using std::string;
using namespace obsOpencastIngestPlugin;
using namespace utilsAndConfig;

DialogOpencast::DialogOpencast(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOpencast)
{
    ui->setupUi(this);
}

DialogOpencast::~DialogOpencast()
{
    delete ui;
}

// init methode
void DialogOpencast::prepareAndShow(QString videoPath)
{
	UploadRecordingDataConfig rD = readConfig();

	setVideoPath(videoPath.toStdString());

	setSettings(rD);

	show();

	this->setAttribute(Qt::WA_DeleteOnClose);
}


void DialogOpencast::setSettings(const UploadRecordingDataConfig & rD)
{
	// metadata
	setTitle(rD.title);
	setSubject(rD.subject);
	setDescription(rD.description);
	setLanguage(rD.language);
	setRights(rD.rights);
	setLicense(rD.license);
	setSeries(rD.series);
	setPresenters(rD.presenters);
	setContributors(rD.contributors);

	return;
}

UploadRecordingDataConfig DialogOpencast::getSettings(const UploadRecordingDataConfig & currentConfig)
{
	UploadRecordingDataConfig rD = currentConfig;

	// metadata
	rD.title = title();
	rD.subject = subject();
	rD.description = description();
	rD.language = language();
	rD.rights = rights();
	rD.license = license();
	rD.series = series();
	rD.presenters = presenters();
	rD.contributors = contributors();

	return rD;
}

// getter / setter

void DialogOpencast::setVideoPath(const std::string &s)
{
    QString t = QString::fromStdString(s);
    ui->lbl_videoPath->setText(t);
}

std::string DialogOpencast::videoPath()
{
    return ui->lbl_videoPath->text().toStdString();
}

// Track metadata getter/setter

std::string DialogOpencast::title()
{
    return ui->input_title->text().toStdString();
}

void DialogOpencast::setTitle(const std::string &s)
{
    QString t = QString::fromStdString(s);
    ui->input_title->setText(t);
}

std::string DialogOpencast::subject()
{
	return ui->input_subject->text().toStdString();
}

void DialogOpencast::setSubject(const std::string &s)
{
	QString t = QString::fromStdString(s);
	ui->input_subject->setText(t);

	return;
}

std::string DialogOpencast::description()
{
	return ui->input_description->text().toStdString();
}

void DialogOpencast::setDescription(const std::string &s)
{
	QString t = QString::fromStdString(s);
	ui->input_description->setText(t);

	return;
}

std::string DialogOpencast::language()
{
	return ui->input_language->text().toStdString();
}

void DialogOpencast::setLanguage(const std::string &s)
{
	QString t = QString::fromStdString(s);
	ui->input_language->setText(t);

	return;
}

std::string DialogOpencast::rights()
{
	return ui->input_rights->text().toStdString();
}

void DialogOpencast::setRights(const std::string &s)
{
	QString t = QString::fromStdString(s);
	ui->input_rights->setText(t);

	return;
}

std::string DialogOpencast::license()
{
	return ui->input_license->text().toStdString();
}

void DialogOpencast::setLicense(const std::string &s)
{
	QString t = QString::fromStdString(s);
	ui->input_license->setText(t);

	return;
}

std::string DialogOpencast::series()
{
	return ui->input_series->text().toStdString();
}

void DialogOpencast::setSeries(const std::string &s)
{
	QString t = QString::fromStdString(s);
	ui->input_series->setText(t);

	return;
}

std::string DialogOpencast::presenters()
{
	return ui->input_presenters->text().toStdString();
}

void DialogOpencast::setPresenters(const std::string &s)
{
	QString t = QString::fromStdString(s);
	ui->input_presenters->setText(t);

	return;
}

std::string DialogOpencast::contributors()
{
	return ui->input_contributors->text().toStdString();
}

void DialogOpencast::setContributors(const std::string &s)
{
	QString t = QString::fromStdString(s);
	ui->input_contributors->setText(t);

	return;
}


// events

void DialogOpencast::on_btn_upload_clicked()
{
	// Qt event handling seems to be single threaded, so we don't need to worry much about this stuff
	hide();

	UploadRecordingDataConfig rD = getSettings(readConfig());

	writeConfig(rD);

	QDateTime now{QDateTime::currentDateTimeUtc()};
	QString nowFormated = now.toString("yyyy-MM-ddTHH:mm:ssZ");
	UploadRecordingData uR = rD.toUploadRecordingData(nowFormated.toStdString());
	string vP = videoPath();

	std::thread uploadThread{uploadRecordingWithGuiNotification, uR, vP};
	uploadThread.detach();

	close();
}
