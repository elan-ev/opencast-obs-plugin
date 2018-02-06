#include "utilsAndConfig.hpp"

#include <vector>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <stdlib.h>
#else
#include <libgen.h>
#include <unistd.h>
#endif
#include <sstream>

#include <obs-module.h>
#include <libconfig.h++>

#include <libOpencastIngest.hpp>


#ifdef _WIN32
#define R_OK 4
#endif


using namespace libOpencastIngest;
using namespace obsOpencastIngestPlugin::utilsAndConfig;
using namespace std;
using namespace libconfig;


static vector<string> recordingOutputsNameVector {"simple_file_output", "adv_file_output", "adv_ffmpeg_output"};


static bool isOutputNameActive(const string & name)
{
	obs_output_t * obsOutput = obs_get_output_by_name(name.c_str());

	bool outputActive = obs_output_active(obsOutput);

	obs_output_release(obsOutput);

	return outputActive;
}


static string getConfigPath()
{
	string configPath{""};

	char * cConfigPath = obs_module_config_path("main.cfg");

	configPath = cConfigPath;

	bfree(cConfigPath);
	cConfigPath = nullptr;

	return configPath;
}


static string getFolderFromPath(const string & path)
{
#ifdef _WIN32
	int len = path.find_last_of("/\\");

	return std::string::npos == len ? "" : path.substr(0, len);
#else
	char * tmpFolder = new char [path.length() + 1];

	copy(path.cbegin(), path.cend(), tmpFolder);

	string cfgFolder = dirname(tmpFolder);;

	delete[] tmpFolder;

	return cfgFolder;
#endif
}


static void writeFreshConfig()
{

	// Create Default UploadRecordingData
	UploadRecordingDataConfig rD{};

	rD.serverInfo.host = "http://localhost:8080/";
	rD.serverInfo.authType = LIBOPENCASTINGEST_DIGESTAUTH;
	rD.serverInfo.username = "opencast_system_account";
	rD.serverInfo.password = "CHANGE_ME";
	rD.serverInfo.authCookie = "";

	rD.flavorDCCatalog = "dublincore/episode";
	rD.flavorTrack = "presenter/source";
	rD.workflowId = "fast";

	// metaData
	rD.title = "";
	rD.subject = "";
	rD.description = "";
	rD.language = "";
	rD.rights = "";
	rD.license = "";
	rD.series = "";
	rD.presenters = "";
	rD.contributors = "";

	// Create Path
	string cfgPath = getConfigPath();
	string cfgFolder = getFolderFromPath(cfgPath);

#ifdef _WIN32
	_mkdir(cfgFolder.c_str());
#else
	mkdir(cfgFolder.c_str(), 0770);
#endif

	// save default config
	writeConfig(rD);

	return;
}


namespace obsOpencastIngestPlugin
{
namespace utilsAndConfig
{

string getOutputFilePath(const string & outputName)
{
	obs_output_t * obsOutput = obs_get_output_by_name(outputName.c_str());

	obs_data_t * settings = obs_output_get_settings(obsOutput);

	string outputFilePath = obs_data_get_string(settings, "path");

	obs_data_release(settings);
	obs_output_release(obsOutput);

	return outputFilePath;
}


string getActiveOutput()
{
	string ret = "";

	for (string & outputName : recordingOutputsNameVector)
	{
		if (isOutputNameActive(outputName))
		{
			ret = outputName;
			break;
		}
	}

	return ret;
}


bool isRecording()
{
	bool ret = false;

	for (string & outputName : recordingOutputsNameVector)
	{
		if (isOutputNameActive(outputName))
		{
			ret = true;
			break;
		}
	}

	return ret;
}


// accesses File getConfigPath(), is not thread-safe
void writeConfig(const UploadRecordingDataConfig & rD)
{
	Config cfg{};

	Setting & root = cfg.getRoot();

	Setting & server = root.add("serverInfo", Setting::TypeGroup);
	Setting & trackInfo = root.add("trackInfo", Setting::TypeGroup);
	Setting & metaData = root.add("metaData", Setting::TypeGroup);

	server.add("host", Setting::TypeString) = rD.serverInfo.host;
	server.add("authType", Setting::TypeString) = authTypeToString(rD.serverInfo.authType);
	server.add("username", Setting::TypeString) = rD.serverInfo.username;
	server.add("password", Setting::TypeString) = rD.serverInfo.password;
	server.add("authCookie", Setting::TypeString) = rD.serverInfo.authCookie;

	trackInfo.add("flavorDCCatalog", Setting::TypeString) = rD.flavorDCCatalog;
	trackInfo.add("flavorTrack", Setting::TypeString) = rD.flavorTrack;
	trackInfo.add("workflowId", Setting::TypeString) = rD.workflowId;

	// metaData
	metaData.add("title", Setting::TypeString) = rD.title;
	metaData.add("subject", Setting::TypeString) = rD.subject;
	metaData.add("description", Setting::TypeString) = rD.description;
	metaData.add("language", Setting::TypeString) = rD.language;
	metaData.add("rights", Setting::TypeString) = rD.rights;
	metaData.add("license", Setting::TypeString) = rD.license;
	metaData.add("series", Setting::TypeString) = rD.series;
	metaData.add("presenters", Setting::TypeString) = rD.presenters;
	metaData.add("contributors", Setting::TypeString) = rD.contributors;

	string cfgPath = getConfigPath();

	try
	{
		cfg.writeFile(cfgPath.c_str());
	}
	catch (const FileIOException & fioex)
	{
		blog(LOG_ERROR, "[%s] Couldn't write config file '%s', fatal failure", obs_module_name(), cfgPath.c_str());
	}

	return;
}

// accesses File getConfigPath(), is not thread-safe
// also creates a new config if there is no config
UploadRecordingDataConfig readConfig()
{
	Config cfg{};
	UploadRecordingDataConfig rD{};

	string cfgPath = getConfigPath();

	blog(DEF_LOGLEVEL, "[%s] Loading config file '%s'", obs_module_name(), cfgPath.c_str());

	// if necessary switch to ifstream to check if file is accessable
	// check if the file is 'accessable' / already exists, if not, try to create a new one
#ifdef _WIN32
	if(_access(cfgPath.c_str(), R_OK) == -1)
#else
	if(access(cfgPath.c_str(), R_OK) == -1)
#endif
	{
		blog(LOG_WARNING, "[%s] config file '%s' doesn't exist, writing new one (maybe first launch / bad access rights)", obs_module_name(), cfgPath.c_str());

		writeFreshConfig();
	}

	try
	{
		cfg.readFile(cfgPath.c_str());
	}
	catch (const ParseException & cfgex)
	{
		blog(LOG_ERROR, "[%s] Couldn't parse config file '%s', fatal failure", obs_module_name(), cfgPath.c_str());
		return UploadRecordingDataConfig{};
	}
	catch (const FileIOException & fioex)
	{
		blog(LOG_ERROR, "[%s] Couldn't access config file '%s', fatal failure", obs_module_name(), cfgPath.c_str());
		return UploadRecordingDataConfig{};
	}

	try
	{
		Setting & root = cfg.getRoot();

		Setting & server = root["serverInfo"];
		Setting & track = root["trackInfo"];
		Setting & metaData = root["metaData"];

		bool allOptionsThere = true;

		// Server cfg
		allOptionsThere &= server.lookupValue("host", rD.serverInfo.host);
		string tmpAuthType{};
		allOptionsThere &= server.lookupValue("authType", tmpAuthType);
		allOptionsThere &= server.lookupValue("username", rD.serverInfo.username);
		allOptionsThere &= server.lookupValue("password", rD.serverInfo.password);
		allOptionsThere &= server.lookupValue("authCookie", rD.serverInfo.authCookie);

		// Track cfg
		allOptionsThere &= track.lookupValue("flavorDCCatalog", rD.flavorDCCatalog);
		allOptionsThere &= track.lookupValue("flavorTrack", rD.flavorTrack);
		allOptionsThere &= track.lookupValue("workflowId", rD.workflowId);

		// MetaData Info
		allOptionsThere &= metaData.lookupValue("title", rD.title);
		allOptionsThere &= metaData.lookupValue("subject", rD.subject);
		allOptionsThere &= metaData.lookupValue("description", rD.description);
		allOptionsThere &= metaData.lookupValue("language", rD.language);
		allOptionsThere &= metaData.lookupValue("rights", rD.rights);
		allOptionsThere &= metaData.lookupValue("license", rD.license);
		allOptionsThere &= metaData.lookupValue("series", rD.series);
		allOptionsThere &= metaData.lookupValue("presenters", rD.presenters);
		allOptionsThere &= metaData.lookupValue("contributors", rD.contributors);

		if (!allOptionsThere)
		{
			// this throws a exception which carries no useful information
			throw SettingNotFoundException{root, "not specified"};
		}


		// authType
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
		else
		{
			blog(LOG_ERROR, "[%s] Setting 'server.authType' in config file '%s' only supports the following options: "
					"['DIGESTAUTH', 'BASICAUTH', 'COOKIEAUTH']"
					", fatal failure", obs_module_name(), cfgPath.c_str());
			return UploadRecordingDataConfig{};
		}

	}
	catch (const SettingNotFoundException &nfex)
	{
		blog(LOG_ERROR, "[%s] Missing Setting in config file '%s', fatal failure", obs_module_name(), cfgPath.c_str());
		return UploadRecordingDataConfig{};
	}

	return rD;
}


string authTypeToString(libOpencastIngest_AuthType at)
{
	switch(at)
	{
	case LIBOPENCASTINGEST_DIGESTAUTH:
	{
		return "DIGESTAUTH";
	}
	case LIBOPENCASTINGEST_BASICAUTH:
	{
		return "BASICAUTH";
	}
	case LIBOPENCASTINGEST_COOKIEAUTH:
	{
		return "COOKIEAUTH";
	}
	}

	// Default
	return "DIGESTAUTH";
}


string getUploadErrorDescription(long curlReturnCode, long httpReturnCode)
{
	switch (curlReturnCode)
	{
	case 7:
		return "Couldn't connect to host! Are you using the correct Url?";
		break;
	default: // e.g. 0
		break;
	}
	switch (httpReturnCode)
	{
	case 401:
		return "Authentication failed! Are you using the correct Username/Password?";
		break;
	case 404:
		return "Couldn't connect to api! Are you using the correct Url?";
		break;
	default: // e.g. 0
		break;
	}

	stringstream strstr{};
	strstr << "Unknown Error, Errorcode: {Curl: " << curlReturnCode <<
			", Http: " << httpReturnCode << "}";
	return strstr.str();
}


UploadRecordingData UploadRecordingDataConfig::toUploadRecordingData(const std::string & created)
{
	UploadRecordingData rD{};

	rD.serverInfo = this->serverInfo;

	rD.DCCatalog = libOpencastIngest::generateDCCatalog(
			created,
			this->title,
			this->subject,
			this->description,
			this->language,
			this->rights,
			this->license,
			this->series,
			this->presenters,
			this->contributors
			);

	rD.flavorDCCatalog = this->flavorDCCatalog;
	rD.flavorTrack = this->flavorTrack;
	rD.workflowId = this->workflowId;

	return rD;
}

}
}
