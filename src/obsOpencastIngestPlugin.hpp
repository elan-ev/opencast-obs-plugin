#ifndef OBSOPENCASTINGESTPLUGIN_HPP
#define OBSOPENCASTINGESTPLUGIN_HPP

#include <string>

#include <libOpencastIngest.hpp>

namespace obsOpencastIngestPlugin
{

struct UploadRecordingServerInfo
{
	std::string host;
	libOpencastIngest_AuthType authType;
	std::string username;
	std::string password;
	std::string authCookie;
};


struct UploadRecordingData
{
	UploadRecordingServerInfo serverInfo;

	std::string flavorDCCatalog;
	std::string DCCatalog;
	std::string flavorTrack;
	std::string workflowId;
};


extern const int DEF_LOGLEVEL;

void uploadRecordingWithGuiNotification(UploadRecordingData rD, const std::string filePath);

}

#endif
