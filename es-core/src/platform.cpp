#include "platform.h"
#include <stdlib.h>
#include <boost/filesystem.hpp>
#include <iostream>
#if !defined(WIN32)
#include <sys/statvfs.h>
#endif
#include <sstream>
#include "Settings.h"

#include <fstream>


#ifdef WIN32
#include <codecvt>
#endif

std::string getHomePath()
{
#if !defined(WIN32)
	std::string homePath;

	// this should give you something like "/home/YOUR_USERNAME" on Linux and "C:\Users\YOUR_USERNAME\" on Windows
	const char * envHome = getenv("HOME");
	if(envHome != nullptr)
	{
		homePath = envHome;
	}

#ifdef WIN32
	// but does not seem to work for Windows XP or Vista, so try something else
	if (homePath.empty()) {
		const char * envDir = getenv("HOMEDRIVE");
		const char * envPath = getenv("HOMEPATH");
		if (envDir != nullptr && envPath != nullptr) {
			homePath = envDir;
			homePath += envPath;

			for(unsigned int i = 0; i < homePath.length(); i++)
				if(homePath[i] == '\\')
					homePath[i] = '/';
		}
	}
#endif

	// convert path to generic directory seperators
	boost::filesystem::path genericPath(homePath);
	return genericPath.generic_string();
#else
    return "D:\\Workspace\\EmulationStation.data";
#endif
}

int runShutdownCommand()
{
#ifdef WIN32 // windows
	return system("shutdown -s -t 0");
#else // osx / linux
	return system("poweroff");
#endif
}

int runRestartCommand()
{
#ifdef WIN32 // windows
	return system("shutdown -r -t 0");
#else // osx / linux
	return system("reboot");
#endif
}

int runSystemCommand(const std::string& cmd_utf8)
{
#ifdef WIN32
	// on Windows we use _wsystem to support non-ASCII paths
	// which requires converting from utf8 to a wstring
	typedef std::codecvt_utf8<wchar_t> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;
	std::wstring wchar_str = converter.from_bytes(cmd_utf8);
	return _wsystem(wchar_str.c_str());
#else
	return system((cmd_utf8 + " 2> /userdata/system/logs/es_launch_stderr.log | head -300 > /userdata/system/logs/es_launch_stdout.log").c_str());
#endif
}
