/*
	Author: Arkensor
	Desc: You know this does filewriting stuff, what did you expect?
	Thanks to Killzone_Kid for his Blog and to my mate Maurice for some magic c++ code!

	---

	This project (A3Log) is licensed under:

	Attribution-NonCommercial-ShareAlike 3.0

	You are free to:
	Share � copy and redistribute the material in any medium or format
	Adapt � remix, transform, and build upon the material
	The licensor cannot revoke these freedoms as long as you follow the license terms.

	Under the following terms:
	Attribution � You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
	NonCommercial � You may not use the material for commercial purposes.
	ShareAlike � If you remix, transform, or build upon the material, you must distribute your contributions under the same license as the original.
	No additional restrictions � You may not apply legal terms or technological measures that legally restrict others from doing anything the license permits.

	http://creativecommons.org/licenses/by-nc-sa/3.0/
*/

#include "INIReader.h"
#include <Windows.h>
#include <cstdio>
#include <string>
#include <ctime>
#include <fstream>
#include <sstream>
#include <map>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <iostream>

std::map<const std::string, std::shared_ptr<std::ostream>> logMap;
std::queue<std::string> *tickets = new std::queue<std::string>;
std::condition_variable ticket_check;
std::string timeStampFormat = "";
std::string fileNameForDebug = "";
std::mutex workerMtx;
std::mutex mainMtx;
bool ticket_notified = false;
bool worker_working = false;
bool timeStampEnabled = false;
bool dateEnabled = false;
bool customlogsonly = false;
bool debugOutput = false;

bool fileExists(const char *fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

std::string strToLower(std::string str)
{
	for (unsigned int i = 0; i < str.length(); i++)
	{
		str[i] = tolower(str[i]);
	}

	return str;
}

void loadConfig()
{
	bool safeload = true;
	std::string inifile = "";
	std::string folderName = "";
	std::string extendetFolderName = "";
	const char *path;
	char config_value[80] = "";
	LPSTR parameter = GetCommandLineA();
	int lengh = strlen(parameter);
	for (int i = 0; i<lengh; i++) {
		char *xff = (&parameter[i]);
		if (strncmp("-A3Log", xff, sizeof("-A3Log") - 1) == 0) {
			sscanf_s(xff + sizeof("-A3Log"), "%s", config_value);
		}
	}
	std::string result = std::string(config_value);

	if (result.back() == '"')
	{
		path = result.substr(0, result.size() - 1).c_str();
	}
	else {
		path = result.c_str();
	}

	if (fileExists(path))
	{
		inifile = path;
	}
	else if (fileExists("@A3Log/A3Log.ini"))
	{
		inifile = "@A3Log/A3Log.ini";
	}
	else if (fileExists("A3Log.ini"))
	{
		inifile = "A3Log.ini";
	}
	else {
		safeload = false;
		std::ofstream writefileError("A3Log-Error.log", std::ios_base::app | std::ios_base::out);
		writefileError << "[A3Log] :: [Error] Could not find A3Log.ini file! Make sure the file exists and that it is placed withing the arma3server root directory, or the place where the dll is placed too!" << std::endl;
		writefileError.close();
	}

	if (safeload) {
		INIReader reader(inifile);

		if (reader.ParseError() > 0) {
			std::ofstream writefileError("A3Log-Error.log", std::ios_base::app | std::ios_base::out);
			writefileError << "[A3Log] :: [Error] Could not parse A3Log.ini file! Make sure the file exists and it is in the same folder as the dll or in the arma3server root directory, and if there are any syntax errors in the A3Log.ini!" << std::endl;
			writefileError << "[A3Log] :: [Error] in line: " << reader.ParseError() << std::endl;
			writefileError.close();
		}
		else {

			if (reader.GetBoolean("Settings", "UseCustomDirectory", false))
			{
				folderName = reader.Get("Settings", "CustomDirectory", "A3Log-Logfiles");

				char full[_MAX_PATH];
				if (_fullpath(full, folderName.c_str(), _MAX_PATH) != NULL)
				{
					folderName = full;
				}

				CreateDirectoryA(folderName.c_str(), NULL);
			} else {
				folderName = "A3Log-Logfiles";
				CreateDirectoryA(folderName.c_str(), NULL);
			}

			if (reader.GetBoolean("Settings", "EnableConsoleOutput", false)) {
				debugOutput = true;
				AllocConsole();
				SetConsoleTitle(TEXT("A3Log Console - by Arkensor"));
				FILE *stream;
				freopen_s(&stream, "CONOUT$", "w", stdout);
				std::cout << "A3Log Console initalized ..." << std::endl;
			}

			timeStampEnabled = reader.GetBoolean("Settings", "EnableTimestamp", false);

			dateEnabled = reader.GetBoolean("Settings", "EnableTimestampDate", false);

			customlogsonly = (reader.GetBoolean("Settings", "UseCustomLogsOnly", false));

			fileNameForDebug = reader.Get("Settings", "FileName", "A3Log");

			if (reader.Get("Settings", "TimestampFormat", "24") == "24")
			{
				timeStampFormat = "%X";
			}
			else {
				timeStampFormat = "%I:%M:%S %p";
			}

			for (std::string logType : reader.getSections()) {

				if (customlogsonly && logType == "settings") { continue; }

				if (reader.GetBoolean("Settings", "SeperateCustomLogs", false))
				{
					if (reader.GetBoolean("Settings", "CombineCustomByDate", false))
					{
						char timechararray[30];
						time_t rawtime;
						struct tm timeinfo;
						time(&rawtime);
						localtime_s(&timeinfo, &rawtime);
						strftime(timechararray, 30, "%Y-%m-%d", &timeinfo);
						extendetFolderName = folderName + "/" + timechararray;
						CreateDirectoryA(extendetFolderName.c_str(), NULL);
						extendetFolderName = folderName + "/" + timechararray + "/" + reader.Get(logType, "FileName", "A3Log");
					} else {
						extendetFolderName = folderName + "/" + reader.Get(logType, "FileName", "A3Log");
					}

					CreateDirectoryA(extendetFolderName.c_str(), NULL);
				}
				else {
					if (reader.GetBoolean("Settings", "CombineCustomByDate", false))
					{
						char timechararray[30];
						time_t rawtime;
						struct tm timeinfo;
						time(&rawtime);
						localtime_s(&timeinfo, &rawtime);
						strftime(timechararray, 30, "%Y-%m-%d", &timeinfo);
						extendetFolderName = folderName + "/" + timechararray;
						CreateDirectoryA(extendetFolderName.c_str(), NULL);
						extendetFolderName = folderName + "/" + timechararray;
					}
					else {
						extendetFolderName = folderName;
					}
				}

				if (reader.GetBoolean("Settings", "EnableFileTimestamp", false))
				{
					time_t rawtime;
					struct tm timeinfo;
					char timechararray[30];
					time(&rawtime);
					localtime_s(&timeinfo, &rawtime);
					if (reader.GetBoolean("Settings", "ExtendedTimestamp", false))
					{
						if (reader.Get("Settings", "TimestampFormat", "24") == "24")
						{
							strftime(timechararray, 30, "%Y-%m-%d_%H-%M-%S", &timeinfo);
						}
						else {
							strftime(timechararray, 30, "%Y-%m-%d_%I-%M-%S %p", &timeinfo);
						}
					}
					else {
						strftime(timechararray, 30, "%Y-%m-%d", &timeinfo);
					}

					std::string FileName = extendetFolderName + "/" + reader.Get(logType, "FileName", "A3Log") + "_" + timechararray + "." + reader.Get(logType, "FileExtension", "log");
					logMap.insert(std::pair<std::string, std::shared_ptr<std::ostream>>(logType, std::make_shared<std::ofstream>(FileName, std::ios_base::app | std::ios_base::out)));
				}
				else {
					std::string FileName = extendetFolderName + "/" + reader.Get(logType, "FileName", "A3Log") + "." + reader.Get(logType, "FileExtension", "log");
					logMap.insert(std::pair<std::string, std::shared_ptr<std::ostream>>(logType, std::make_shared<std::ofstream>(FileName, std::ios_base::app | std::ios_base::out)));
				}
			}
		}
	}
}

void logAction(std::string input) {

	char timechararray[30];
	char dateArray[30];
	if (timeStampEnabled)
	{
		time_t rawtime;
		struct tm timeinfo;
		time(&rawtime);
		localtime_s(&timeinfo, &rawtime);
		strftime(timechararray, 30, timeStampFormat.c_str(), &timeinfo);
		if (dateEnabled)
		{
			strftime(dateArray, 30, "%Y-%m-%d", &timeinfo);
		}
	}

	size_t pos = input.find("\037");

	if (pos != std::string::npos) {

		const std::string logName = input.substr(0, pos);

		std::shared_ptr<std::ostream> stream = logMap[strToLower(logName)];

		if (stream != nullptr) {

			if (timeStampEnabled)
			{
				if (dateEnabled)
				{
					(*stream) << "[" << dateArray << " / " << timechararray << "] " << input.substr(pos + 1, input.size()) << std::endl;
					if (debugOutput) {
						std::cout << logName << ": " << "[" << dateArray << " / " << timechararray << "] " << input.substr(pos + 1, input.size()) << std::endl;
					}
				}
				else {
					(*stream) << "[" << timechararray << "] " << input.substr(pos + 1, input.size()) << std::endl;
					if (debugOutput) {
						std::cout << logName << ": " << "[" << timechararray << "] " << input.substr(pos + 1, input.size()) << std::endl;
					}
				}

			}
			else {
				(*stream) << input.substr(pos + 1, input.size()) << std::endl;
				if (debugOutput) {
					std::cout << input.substr(pos + 1, input.size()) << std::endl;
				}
			}
		}
		else {

			if (customlogsonly)
			{
				//WoW this is so much todo ... just took the time and this is the worst code ever ... but still it does that it should for now
				std::ofstream writefileError("A3Log-Error.log", std::ios_base::app | std::ios_base::out);
				if (timeStampEnabled)
				{
					if (dateEnabled)
					{
						writefileError << "Note: You are have got UseCustomLogsOnly enabled but this message has no category (please fix): " << "[" << dateArray << " / " << timechararray << "] " << "[" << logName << "] " << input.substr(pos + 1, input.size()) << std::endl;
					}
					else {
						writefileError << "Note: You are have got UseCustomLogsOnly enabled but this message has no category (please fix): " << "[" << timechararray << "] " << "[" << logName << "] " << input.substr(pos + 1, input.size()) << std::endl;
					}
				}
				else {
					writefileError << "Note: You are have got UseCustomLogsOnly enabled but this message has no category (please fix): " << "[" << logName << "] " << input.substr(pos + 1, input.size()) << std::endl;
				}
				writefileError.close();
			}
			else {
				std::shared_ptr<std::ostream> stream = logMap["settings"];

				if (stream != nullptr) {

					if (timeStampEnabled)
					{
						if (dateEnabled)
						{
							(*stream) << "[" << dateArray << " / " << timechararray << "] " << "[" << logName << "] " << input.substr(pos + 1, input.size()) << std::endl;
							if (debugOutput) {
								std::cout << fileNameForDebug << ": " << "[" << dateArray << " / " << timechararray << "] " << "[" << logName << "] " << input.substr(pos + 1, input.size()) << std::endl;
							}
						}
						else {
							(*stream) << "[" << timechararray << "] " << "[" << logName << "] " << input.substr(pos + 1, input.size()) << std::endl;
							if (debugOutput) {
								std::cout << fileNameForDebug << ": " << "[" << timechararray << "] " << "[" << logName << "] " << input.substr(pos + 1, input.size()) << std::endl;
							}
						}
					}
					else {
						(*stream) << "[" << logName << "] " << input.substr(pos + 1, input.size()) << std::endl;
						if (debugOutput) {
							std::cout << fileNameForDebug << ": " << "[" << logName << "] " << input.substr(pos + 1, input.size()) << std::endl;
						}
					}
				}
			}
		}
	}
	else {

		if (customlogsonly)
		{
			std::ofstream writefileError("A3Log-Error.log", std::ios_base::app | std::ios_base::out);
			if (timeStampEnabled)
			{
				if (dateEnabled)
				{
					writefileError << "Note: You are have got UseCustomLogsOnly enabled but this message has no category (please fix): " << "[" << dateArray << " / " << timechararray << "] " << input.substr(pos + 1, input.size()) << std::endl;
				}
				else {
					writefileError << "Note: You are have got UseCustomLogsOnly enabled but this message has no category (please fix): " << "[" << timechararray << "] " << input.substr(pos + 1, input.size()) << std::endl;
				}
			}
			else {
				writefileError << "Note: You are have got UseCustomLogsOnly enabled but this message has no category (please fix): " << input.substr(pos + 1, input.size()) << std::endl;
			}
			writefileError.close();
		}
		else {
			std::shared_ptr<std::ostream> stream = logMap["settings"];

			if (stream != nullptr) {

				if (timeStampEnabled)
				{
					if (dateEnabled)
					{
						(*stream) << "[" << dateArray << " / " << timechararray << "] " << input.substr(pos + 1, input.size()) << std::endl;
						if (debugOutput) {
							std::cout << fileNameForDebug << ": " << "[" << dateArray << " / " << timechararray << "] " << input.substr(pos + 1, input.size()) << std::endl;
						}
					}
					else {
						(*stream) << "[" << timechararray << "] " << input.substr(pos + 1, input.size()) << std::endl;
						if (debugOutput) {
							std::cout << fileNameForDebug << ": " << "[" << timechararray << "] " << input.substr(pos + 1, input.size()) << std::endl;
						}
					}
				}
				else {
					(*stream) << input.substr(pos + 1, input.size()) << std::endl;
					if (debugOutput) {
						std::cout << fileNameForDebug << ": " << input.substr(pos + 1, input.size()) << std::endl;
					}
				}
			}
		}
	}
}

void worker() {

	while (true) {

		std::unique_lock<std::mutex> locker(mainMtx);

		do {
			ticket_check.wait(locker);
		} while (!ticket_notified);

		workerMtx.lock();

		std::queue<std::string>* ticketCopy = tickets;

		tickets = new std::queue<std::string>;

		workerMtx.unlock();

		while (!(*ticketCopy).empty()) {
			std::string message = (*ticketCopy).front();
			(*ticketCopy).pop();
			logAction(message);
		}

		ticket_notified = false;
	}
}

extern "C" {
	__declspec (dllexport) void __stdcall RVExtension(char *output, int outputSize, const char *function);
}

void __stdcall RVExtension(char *output, int outputSize, const char *function) {

	static bool b;
	if (!b)
	{
		loadConfig();
		b = true;
	}

	if (!strcmp(function, "version"))
	{
		strncpy_s(output, 4, "1.6", _TRUNCATE);
	}
	else
	{
		std::unique_lock<std::mutex> locker(mainMtx);

		workerMtx.lock();

		(*tickets).push(std::string(function));

		workerMtx.unlock();

		ticket_notified = true;
		ticket_check.notify_one();

		if (!worker_working) {
			worker_working = true;
			std::thread workerthread(worker);
			workerthread.detach();
		}

		strncpy_s(output, 1, "", _TRUNCATE);
	}

	return;
}
