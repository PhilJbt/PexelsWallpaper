#pragma once

#include <iostream>
#include <curl/curl.h>
#include <cstdlib>
#include <time.h>
#include <vector>
#include <fstream>
#include <string>
#include <Windows.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wldap32.lib")

class PexelsWallpaper
{
public:
	PexelsWallpaper();
	~PexelsWallpaper();

	void Process(System::Windows::Forms::ProgressBar^);

private:
	bool bErr;

	// Keyword
	std::string szKeyword;
	std::string szKeywordPath;

	// cUrl
	CURL *curl_list;
	CURL *curl_element;
	CURL *curl_file;
	struct curl_slist *listopt;
	CURLcode res;
	std::string readBuffer;
	int iNbrPages;

	// Json
	rapidjson::Document jsonDocument;
	rapidjson::Value jsonPhotoUrlValue;
	std::string szPhotoUrlValue;

	// Download
	std::string szCurPath;
	std::string szPhotoDlPath;
	FILE *filePointer;

	// Api Key
	std::string szAuthApiKey;

	void Init(System::Windows::Forms::ProgressBar^);
	void GetKeywords(System::Windows::Forms::ProgressBar^);
	void GetPhotosList(System::Windows::Forms::ProgressBar^);
	void GetRandomPhotoUrl(System::Windows::Forms::ProgressBar^);
	void DownloadPhoto(System::Windows::Forms::ProgressBar^);
	void ApplyWallpaper(System::Windows::Forms::ProgressBar^);

	static size_t WriteCallback(void*, size_t, size_t, void*);
	static size_t WriteData(void*, size_t, size_t, FILE*);
};
