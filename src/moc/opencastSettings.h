#ifndef OPENCASTSETTINGS_H
#define OPENCASTSETTINGS_H

#include <QDialog>

#include "../utilsAndConfig.hpp"

namespace Ui {
class OpencastSettings;
}

class OpencastSettings : public QDialog
{
    Q_OBJECT

public:
    explicit OpencastSettings(QWidget *parent = 0);
    ~OpencastSettings();

    // init methode
    void prepareAndShow();

private:
    void setSettings(const obsOpencastIngestPlugin::utilsAndConfig::UploadRecordingDataConfig & rD);
    obsOpencastIngestPlugin::utilsAndConfig::UploadRecordingDataConfig getSettings(
    		const obsOpencastIngestPlugin::utilsAndConfig::UploadRecordingDataConfig & currentConfig);

    // server info getter/setter
    std::string host();
    void setHost(const std::string &s);
    std::string authType();
    void setAuthType(const std::string &s);
    std::string username();
    void setUsername(const std::string &s);
    std::string password();
    void setPassword(const std::string &s);
    std::string authCookie();
    void setAuthCookie(const std::string &s);

    // Additional Track Info getter/setter
    std::string flavorDCCatalog();
    void setFlavorDCCatalog(const std::string &s);
    std::string flavorTrack();
    void setFlavorTrack(const std::string &s);
    std::string workflowId();
    void setWorkflowId(const std::string &s);

private slots: // events
	// btn_cancel -> hide()
	void on_btn_save_clicked();

private:
    Ui::OpencastSettings *ui;
};

#endif // OPENCASTSETTINGS_H
