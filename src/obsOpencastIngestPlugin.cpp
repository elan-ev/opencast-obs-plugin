#include "obsOpencastIngestPlugin.hpp"

#include <thread>
#include <atomic>
#include <vector>
#include <stdexcept>
#include <sstream>

#include <obs-module.h>

#include <QMainWindow>
#include <QAction>

#include "utilsAndConfig.hpp"
#include "moc/guiLauncher.h"
#include "moc/opencastSettings.h"

using namespace obsOpencastIngestPlugin;
using namespace utilsAndConfig;
using namespace std;
using namespace libOpencastIngest;


// forward declaration of obs-frontend-api
// used because obs-frontend-api and libconfig clash on struct config_t
extern "C"
{
EXPORT void *obs_frontend_get_main_window(void);
EXPORT void *obs_frontend_add_tools_menu_qaction(const char *name);
}


// Defines
// const int obsOpencastIngestPlugin::DEF_LOGLEVEL = LOG_DEBUG;
const int obsOpencastIngestPlugin::DEF_LOGLEVEL = LOG_INFO;
const static long curlOkCode = 0;
const static long httpOkCode = 200;



// Vars
static thread workerThread{};
static atomic_bool shouldWorkerRun{true};
static GuiLauncher * guiLauncher{nullptr};

// OBS Plugin Basic Defines

OBS_DECLARE_MODULE()

OBS_MODULE_AUTHOR("see Contributors.md on https://github.com/elan-ev/opencast-obs-plugin")

const char *obs_module_name(void)
{
	return "OpencastObsPlugin" " v" OBSOPENCASTINGESTPLUGIN_BUILD_VERSION;
}

const char *obs_module_description(void)
{
	return "see README.md on https://github.com/elan-ev/opencast-obs-plugin";
}


static tuple<long, long> uploadRecordingWithoutLog(UploadRecordingData & rD, const string & filePath)
{
	string host = rD.serverInfo.host;
	libOpencastIngest_AuthType authType = rD.serverInfo.authType;
	string username = rD.serverInfo.username;
	string password = rD.serverInfo.password;
	string authCookie = rD.serverInfo.authCookie;

	string useragent = string{obs_module_name()};

	HostInformation hI{host, authType, username, password, authCookie, useragent};

	string flavorDCCatalog = rD.flavorDCCatalog;
	string DCCatalog = rD.DCCatalog;
	string flavorTrack = rD.flavorTrack;
	string workflowId = rD.workflowId;

	string pathToTrack = filePath;


	long curlReturnCode = -1;
	long httpReturnCode = -1;
	string mediaPackage{};

	tie(curlReturnCode, httpReturnCode, mediaPackage) = createMediaPackage(hI);
	// return on error
	if (curlReturnCode != curlOkCode || httpReturnCode != httpOkCode)
	{
		return make_tuple(curlReturnCode, httpReturnCode);
	}

	tie(curlReturnCode, httpReturnCode, mediaPackage) = addDCCatalog(hI, flavorDCCatalog, mediaPackage, DCCatalog);
	if (curlReturnCode != curlOkCode || httpReturnCode != httpOkCode)
	{
		return make_tuple(curlReturnCode, httpReturnCode);
	}

	tie(curlReturnCode, httpReturnCode, mediaPackage) = addTrack(hI, flavorTrack, mediaPackage, pathToTrack);
	if (curlReturnCode != curlOkCode || httpReturnCode != httpOkCode)
	{
		return make_tuple(curlReturnCode, httpReturnCode);
	}

	tie(curlReturnCode, httpReturnCode, mediaPackage) = ingest(hI, mediaPackage, workflowId);

	return make_tuple(curlReturnCode, httpReturnCode);
}


void obsOpencastIngestPlugin::uploadRecordingWithGuiNotification(UploadRecordingData rD, const string filePath)
{
	long curlReturnCode = -1;
	long httpReturnCode = -1;

	// upload started notification
	QMetaObject::invokeMethod(guiLauncher, "createAndShowInfoBoxWidget",
			Qt::QueuedConnection,
			Q_ARG(QString, QString::fromStdString(string{"Upload started ..."})));

	tie(curlReturnCode, httpReturnCode) = uploadRecordingWithoutLog(rD, filePath);

	if(curlReturnCode == curlOkCode && httpReturnCode == httpOkCode)
	{
		blog(DEF_LOGLEVEL, "[%s] Upload of file '%s' finished", obs_module_name(), filePath.c_str());
		QMetaObject::invokeMethod(guiLauncher, "createAndShowInfoBoxWidget",
				Qt::QueuedConnection,
				Q_ARG(QString, QString::fromStdString(string{"Upload finished successfully!"})));
	}
	else
	{
		blog(LOG_WARNING, "[%s] Uploading of file '%s' failed with Code: "
				"{Curl: %ld, Http: %ld}", obs_module_name(), filePath.c_str(),
				curlReturnCode, httpReturnCode);
		stringstream strstr{};
		strstr << "Upload stopped with error: \"" << getUploadErrorDescription(curlReturnCode, httpReturnCode) << "\"";
		QMetaObject::invokeMethod(guiLauncher, "createAndShowInfoBoxWidget",
				Qt::QueuedConnection,
				Q_ARG(QString, QString::fromStdString(strstr.str())));
	}

	return;
}


static void workerFunction()
{
	string filePath{""};
	bool wasRecording = false;

	while (shouldWorkerRun)
	{
		if (isRecording())
		{
			filePath = getOutputFilePath(getActiveOutput());

			wasRecording = true;
		}
		else if (!isRecording() && wasRecording) // Recording finished
		{
			blog(DEF_LOGLEVEL, "[%s] Recording of file '%s' finished", obs_module_name(), filePath.c_str());

			// inform GUI
			QMetaObject::invokeMethod(guiLauncher, "createAndShowDialogOpencastWidget",
					Qt::QueuedConnection,
					Q_ARG(QString, QString::fromStdString(filePath)));

			wasRecording = false;
		}

		this_thread::sleep_for(chrono::milliseconds(250));
	}

	blog(DEF_LOGLEVEL, "[%s] Worker function ended", obs_module_name());
}


// OBS Plugin load and unload

bool obs_module_load()
{
	blog(DEF_LOGLEVEL, "Module '%s' loading", obs_module_name());

	// no cleanup / delete needed, as QT will cleanup according to the parent child hierachy
	guiLauncher = new GuiLauncher{(QMainWindow *) obs_frontend_get_main_window()};
	OpencastSettings * opencastSettingsDialog = new OpencastSettings{(QMainWindow *) obs_frontend_get_main_window()};
	blog(DEF_LOGLEVEL, "[%s] Gui's created created", obs_module_name());

	QAction * menu_action = (QAction*) obs_frontend_add_tools_menu_qaction("OpencastIngestPlugin Settings");
	auto actionTriggered = [opencastSettingsDialog]() { opencastSettingsDialog->prepareAndShow(); };
	menu_action->connect(menu_action, &QAction::triggered, actionTriggered);
	blog(DEF_LOGLEVEL, "[%s] Menu entry for Settings added", obs_module_name());

	shouldWorkerRun = true;
	workerThread = thread(workerFunction);
	blog(DEF_LOGLEVEL, "[%s] WorkerThread launched", obs_module_name());

	blog(DEF_LOGLEVEL, "Module '%s' loaded", obs_module_name());

	blog(DEF_LOGLEVEL, "[%s] Module Description: '%s'", obs_module_name(), obs_module_description());
	blog(DEF_LOGLEVEL, "[%s] Module Author: '%s'", obs_module_name(), obs_module_author());

	return true;
}


void obs_module_unload(void)
{
	blog(DEF_LOGLEVEL, "Module '%s' unloading", obs_module_name());

	shouldWorkerRun = false;
	workerThread.join();

	blog(DEF_LOGLEVEL, "[%s] WorkerThread stopped", obs_module_name());

	blog(DEF_LOGLEVEL, "Module '%s' unloaded", obs_module_name());

	return;
}
