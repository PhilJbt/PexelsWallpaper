#include "Header.h"



PexelsWallpaper::PexelsWallpaper()
{
	//auto myConsole = GetConsoleWindow();
	//ShowWindow(myConsole, 1);

	//AllocConsole();
	//freopen("conin$", "r", stdin);
	//freopen("conout$", "w", stdout);
	//freopen("conout$", "w", stderr);
}

PexelsWallpaper::~PexelsWallpaper()
{
}

void PexelsWallpaper::Init(System::Windows::Forms::ProgressBar^ _progressBar)
{
	srand(unsigned int(time(0)));
	curl_global_init(CURL_GLOBAL_ALL);

	curl_list = nullptr;
	curl_element = nullptr;
	curl_file = nullptr;
	curl_list = curl_easy_init();
	curl_element = curl_easy_init();
	curl_file = curl_easy_init();
	listopt = NULL;
	filePointer = nullptr;

	bErr = false;

	// Get current path
	char buffer[512];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	szCurPath = std::string(buffer).substr(0, pos);
	szPhotoDlPath = szCurPath;
		szPhotoDlPath.append("\\PexelsWallpaper.jpg");
	szKeywordPath = szCurPath;
		szKeywordPath.append("\\config.txt");

	_progressBar->Value = 10;
	_progressBar->Update();
}

void PexelsWallpaper::GetKeywords(System::Windows::Forms::ProgressBar^ _progressBar)
{
	std::vector<std::string> listKeywords;
	std::ifstream file_keywork(szKeywordPath.c_str());
	std::string temp_str;

	if (file_keywork)
	{
		while (std::getline(file_keywork, temp_str))
			if (temp_str[0] != '#'
				&& temp_str.length() > 1)
				if (temp_str.find("Authorization:") == std::string::npos)
					listKeywords.push_back(temp_str);
				else
					szAuthApiKey = temp_str;

		std::string szKeywordRaw = listKeywords[rand() % listKeywords.size()];

		int j = 0;
		for (unsigned int i = 0; i < szKeywordRaw.length(); i++)
		{
			if (szKeywordRaw[i] == ' ')
				szKeyword += "%20";
			else
				szKeyword += szKeywordRaw[i];
		}

		if (szKeyword.size() == 0
			|| szKeyword == "popular")
		{
			MessageBox(NULL, "Keywords missing or not supported.", "Pexels Wallpaper Error", MB_ICONERROR | MB_OK);
			bErr = true;
		}
		else
			bErr = false;
	}
	else
	{
		MessageBox(NULL, "Can't open config.txt", "Pexels Wallpaper Error", MB_ICONERROR | MB_OK);
		bErr = true;
	}

	_progressBar->Value = 20;
	_progressBar->Update();
}

void PexelsWallpaper::GetPhotosList(System::Windows::Forms::ProgressBar^ _progressBar)
{
	if (curl_list)
	{
		std::string szRequest = "http://api.pexels.com/v1/search?query=";
		szRequest.append(szKeyword);
		szRequest.append("&per_page=40&page=0");

		// Push url & header
		curl_easy_setopt(curl_list, CURLOPT_URL, szRequest.c_str());
		listopt = curl_slist_append(listopt, szAuthApiKey.c_str());
		curl_easy_setopt(curl_list, CURLOPT_HTTPHEADER, listopt);
		//curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl_list, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl_list, CURLOPT_WRITEDATA, &readBuffer);

		_progressBar->Value = 30;
		_progressBar->Update();

		// Proceed request
		res = curl_easy_perform(curl_list);

		_progressBar->Value = 40;
		_progressBar->Update();

		// Check errors
		if (readBuffer.find("Key not authorised") != std::string::npos)
		{
			MessageBox(NULL, "Api authorization key invalid.", "Pexels Wallpaper Error", MB_ICONERROR | MB_OK);
			bErr = true;
		}
		else if (res != CURLE_OK)
		{
			std::string path_str = curl_easy_strerror(res);
			LPCTSTR path_lp = (LPCTSTR)path_str.c_str();
			MessageBox(NULL, path_lp, "Pexels Wallpaper Error", MB_ICONERROR | MB_OK);
			bErr = true;
		}
		else
		{
			// Verify resultat number
			int iNbrRes = stoi(readBuffer.substr(readBuffer.find("\"total_results\":") + 16, readBuffer.find_first_of(",") - readBuffer.find("\"total_results\":") - 16));
			iNbrPages = int(iNbrRes / 40);

			if (iNbrRes == 0)
			{
				std::string szErrMess = "The \"";
				szErrMess.append(szKeyword);
				szErrMess.append("\" research returns zero results.");

				MessageBox(NULL, szErrMess.c_str(), "Pexels Wallpaper Error", MB_ICONERROR | MB_OK);
				bErr = true;
			}
			else
			{
				// Var unset
				curl_easy_cleanup(curl_list);

				bErr = false;

				_progressBar->Value = 50;
				_progressBar->Update();
			}			
		}				
	}
	else
	{
		MessageBox(NULL, "Initialization #1 error", "Pexels Wallpaper Error", MB_ICONERROR | MB_OK);
		bErr = true;
	}	
}

void PexelsWallpaper::GetRandomPhotoUrl(System::Windows::Forms::ProgressBar^ _progressBar)
{
	if (curl_element)
	{
		readBuffer = "";

		std::string szRequest = "http://api.pexels.com/v1/search?query=";
		szRequest.append(szKeyword);
		szRequest.append("&per_page=40&page=");
		szRequest.append(std::to_string(rand() % iNbrPages));



		// Push url & header
		curl_easy_setopt(curl_element, CURLOPT_URL, szRequest.c_str());
		curl_easy_setopt(curl_element, CURLOPT_HTTPHEADER, listopt);
		//curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl_element, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl_element, CURLOPT_WRITEDATA, &readBuffer);

		_progressBar->Value = 60;
		_progressBar->Update();

		// Proceed request
		res = curl_easy_perform(curl_element);

		_progressBar->Value = 70;
		_progressBar->Update();

		// Check errors
		if (res != CURLE_OK)
		{
			std::string path_str = curl_easy_strerror(res);
			LPCTSTR path_lp = (LPCTSTR)path_str.c_str();
			MessageBox(NULL, path_lp, "Pexels Wallpaper Error", MB_ICONERROR | MB_OK);
			bErr = true;
		}

		// Var unset
		curl_easy_cleanup(curl_element);

		bErr = false;
	}
	else
	{
		MessageBox(NULL, "Initialization #2 error", "Pexels Wallpaper Error", MB_ICONERROR | MB_OK);
		bErr = true;
	}

	//

	jsonDocument.Parse(readBuffer.c_str());
	jsonPhotoUrlValue = jsonDocument["photos"][rand() % jsonDocument["photos"].Capacity()]["src"]["original"];
	szPhotoUrlValue = jsonPhotoUrlValue.GetString();

	if (szPhotoUrlValue[4] == 's')
		szPhotoUrlValue.erase(4, 1);

	_progressBar->Value = 80;
	_progressBar->Update();
}

void PexelsWallpaper::DownloadPhoto(System::Windows::Forms::ProgressBar^ _progressBar)
{
	filePointer = fopen(szPhotoDlPath.c_str(), "wb");

	if (curl_file)
	{
		if (filePointer != nullptr)
		{
			curl_easy_setopt(curl_file, CURLOPT_URL, szPhotoUrlValue.c_str());
			curl_easy_setopt(curl_file, CURLOPT_WRITEFUNCTION, WriteData);
			curl_easy_setopt(curl_file, CURLOPT_WRITEDATA, filePointer);
			curl_easy_perform(curl_file);
			curl_easy_cleanup(curl_file);
			fclose(filePointer);

			bErr = false;
		}
		else
		{
			MessageBox(NULL, "File creation error", "Pexels Wallpaper Error", MB_ICONERROR | MB_OK);
			bErr = true;
		}
	}
	else
	{
		MessageBox(NULL, "Initialization #2 error", "Pexels Wallpaper Error", MB_ICONERROR | MB_OK);
		bErr = true;
	}

	// Global unset
	curl_global_cleanup();

	_progressBar->Value = 90;
	_progressBar->Update();
}

void PexelsWallpaper::ApplyWallpaper(System::Windows::Forms::ProgressBar^ _progressBar)
{
	if (SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (PVOID)szPhotoDlPath.c_str(), SPIF_UPDATEINIFILE) != TRUE)
	{
		MessageBox(NULL, "Wallpaper appliance error", "Pexels Wallpaper Error", MB_ICONERROR | MB_OK);
		bErr = true;
	}
	else
		bErr = false;

	_progressBar->Value = 100;
	_progressBar->Update();
}

void PexelsWallpaper::Process(System::Windows::Forms::ProgressBar^ _progressBar)
{
	Init(_progressBar);
	
	if (!bErr) GetKeywords(_progressBar);
	if (!bErr) GetPhotosList(_progressBar);
	if (!bErr) GetRandomPhotoUrl(_progressBar);
	if (!bErr) DownloadPhoto(_progressBar);
	if (!bErr) ApplyWallpaper(_progressBar);
}

size_t PexelsWallpaper::WriteData(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

size_t PexelsWallpaper::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}
