#ifndef UTILSANDCONFIG_HPP
#define UTILSANDCONFIG_HPP

#include <string>

#include "obsOpencastIngestPlugin.hpp"

namespace obsOpencastIngestPlugin
{
namespace utilsAndConfig
{

// on change update readConfig, writeConfig, writeFreshConfig,
// DialogOpencast::{get, set}Settings() and ofc
// this->toUploadRecordingData()
struct UploadRecordingDataConfig
{
	UploadRecordingServerInfo serverInfo;

	std::string flavorDCCatalog;
	std::string flavorTrack;
	std::string workflowId;

	// metadate for DCCatalog
	std::string title;
	std::string subject;
	std::string description;
	std::string language;
	std::string rights;
	std::string license;
	std::string series;
	std::string presenters;
	std::string contributors;

	UploadRecordingData toUploadRecordingData(const std::string & created);
};


std::string getActiveOutput();
bool isRecording();
std::string getOutputFilePath(const std::string & outputName);

UploadRecordingDataConfig readConfig();
void writeConfig(const UploadRecordingDataConfig & rD);

std::string authTypeToString(libOpencastIngest_AuthType at);

std::string getUploadErrorDescription(long curlReturnCode, long httpReturnCode);

}
}

#endif
