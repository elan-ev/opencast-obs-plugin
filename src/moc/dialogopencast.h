#ifndef DIALOGOPENCAST_H
#define DIALOGOPENCAST_H

#include <QDialog>
#include "../obsOpencastIngestPlugin.hpp"
#include "../utilsAndConfig.hpp"

namespace Ui {
class DialogOpencast;
}

class DialogOpencast : public QDialog
{
    Q_OBJECT

public:
    explicit DialogOpencast(QWidget *parent = 0);
    ~DialogOpencast();

    // init methode
    void prepareAndShow(QString videoPath);

private:
    void setSettings(const obsOpencastIngestPlugin::utilsAndConfig::UploadRecordingDataConfig & rD);
    obsOpencastIngestPlugin::utilsAndConfig::UploadRecordingDataConfig getSettings(
    		const obsOpencastIngestPlugin::utilsAndConfig::UploadRecordingDataConfig & currentConfig);

    // getter / setter
    std::string videoPath();
    void setVideoPath(const std::string &s);

    // meta data getter/setter
    std::string title();
    void setTitle(const std::string &s);
    std::string subject();
    void setSubject(const std::string &s);
    std::string description();
    void setDescription(const std::string &s);
    std::string language();
    void setLanguage(const std::string &s);
    std::string rights();
    void setRights(const std::string &s);
    std::string license();
    void setLicense(const std::string &s);
    std::string series();
    void setSeries(const std::string &s);
    std::string presenters();
    void setPresenters(const std::string &s);
    std::string contributors();
    void setContributors(const std::string &s);

private slots: // events
	// btn_close (push_Button) -> close()
    void on_btn_upload_clicked();

private:
    Ui::DialogOpencast *ui;
};

#endif // DIALOGOPENCAST_H
