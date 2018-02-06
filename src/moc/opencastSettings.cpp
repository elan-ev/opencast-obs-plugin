#include "opencastSettings.h"
#include "ui_opencastSettings.h"

using namespace obsOpencastIngestPlugin::utilsAndConfig;
using namespace std;

OpencastSettings::OpencastSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpencastSettings)
{
    ui->setupUi(this);

    ui->input_authType->addItem("DIGESTAUTH");
    ui->input_authType->addItem("BASICAUTH");
    ui->input_authType->addItem("COOKIEAUTH");

    // Update window title with version information
    QString windowTitle = QString::fromStdString(
    		this->windowTitle().toStdString() +
			" v" + OBSOPENCASTINGESTPLUGIN_BUILD_VERSION);
    this->setWindowTitle(windowTitle);
}

OpencastSettings::~OpencastSettings()
{
    delete ui;
}

// init methode
void OpencastSettings::prepareAndShow()
{
	UploadRecordingDataConfig rD = readConfig();

	setSettings(rD);

	show();

	return;
}

void OpencastSettings::setSettings(const UploadRecordingDataConfig & rD)
{
	setHost(rD.serverInfo.host);
	setAuthType(authTypeToString(rD.serverInfo.authType));
	setUsername(rD.serverInfo.username);
	setPassword(rD.serverInfo.password);
	setAuthCookie(rD.serverInfo.authCookie);

	setFlavorDCCatalog(rD.flavorDCCatalog);
	setFlavorTrack(rD.flavorTrack);
	setWorkflowId(rD.workflowId);

	return;
}

UploadRecordingDataConfig OpencastSettings::getSettings(const UploadRecordingDataConfig & currentConfig)
{
	UploadRecordingDataConfig rD = currentConfig;

	rD.serverInfo.host = host();
	rD.serverInfo.username = username();
	rD.serverInfo.password = password();
	rD.serverInfo.authCookie = authCookie();

	// authType
	// we assume that the gui is adjusted according to the libOpencastIngest::AuthType enum
	string tmpAuthType = authType();
	if (tmpAuthType == "DIGESTAUTH")
	{
		rD.serverInfo.authType = LIBOPENCASTINGEST_DIGESTAUTH;
	}
	else if (tmpAuthType == "BASICAUTH")
	{
		rD.serverInfo.authType = LIBOPENCASTINGEST_BASICAUTH;
	}
	else if (tmpAuthType == "COOKIEAUTH")
	{
		rD.serverInfo.authType = LIBOPENCASTINGEST_COOKIEAUTH;
	}
	// else shouldn't be possible

	rD.flavorDCCatalog = flavorDCCatalog();
	rD.flavorTrack = flavorTrack();
	rD.workflowId = workflowId();

	return rD;
}

// Server info getter/setter

void OpencastSettings::setHost(const std::string &s)
{
    QString t = QString::fromStdString(s);
    ui->input_host->setText(t);
}

std::string OpencastSettings::host()
{
    return ui->input_host->text().toStdString();
}

void OpencastSettings::setAuthType(const std::string &s)
{
    for(int i = 0; i<ui->input_authType->count(); i++){
        if(s == "DIGESTAUTH"){
            ui->input_authType->setCurrentIndex(0);
        } else if(s == "BASICAUTH"){
            ui->input_authType->setCurrentIndex(1);
        } else if(s == "COOKIEAUTH"){
            ui->input_authType->setCurrentIndex(2);
        } else {
            ui->input_authType->setCurrentIndex(0);
        }
    }
}

std::string OpencastSettings::authType()
{
    //return ui->input_authType->text().toStdString();
    return ui->input_authType->currentText().toStdString();
}

void OpencastSettings::setUsername(const std::string &s)
{
    QString t = QString::fromStdString(s);
    ui->input_username->setText(t);
}

std::string OpencastSettings::username()
{
    return ui->input_username->text().toStdString();
}

void OpencastSettings::setPassword(const std::string &s)
{
    QString t = QString::fromStdString(s);
    ui->input_password->setText(t);
}

std::string OpencastSettings::password()
{
    return ui->input_password->text().toStdString();
}

void OpencastSettings::setAuthCookie(const std::string &s)
{
    QString t = QString::fromStdString(s);
    ui->input_authCookie->setText(t);
}

std::string OpencastSettings::authCookie()
{
    return ui->input_authCookie->text().toStdString();
}

// Additional Track Info getter/setter

void OpencastSettings::setFlavorDCCatalog(const std::string &s)
{
    QString t = QString::fromStdString(s);
    ui->input_flavorDCCatalog->setText(t);
}

std::string OpencastSettings::flavorDCCatalog()
{
    return ui->input_flavorDCCatalog->text().toStdString();
}

void OpencastSettings::setFlavorTrack(const std::string &s)
{
    QString t = QString::fromStdString(s);
    ui->input_flavorTrack->setText(t);
}

std::string OpencastSettings::flavorTrack()
{
    return ui->input_flavorTrack->text().toStdString();
}

void OpencastSettings::setWorkflowId(const std::string &s)
{
    QString t = QString::fromStdString(s);
    ui->input_workflowId->setText(t);
}

std::string OpencastSettings::workflowId()
{
    return ui->input_workflowId->text().toStdString();
}

// events

void OpencastSettings::on_btn_save_clicked()
{
	hide();

	UploadRecordingDataConfig rD = getSettings(readConfig());

	writeConfig(rD);

	return;
}
