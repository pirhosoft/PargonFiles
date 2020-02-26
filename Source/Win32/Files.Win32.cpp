#include "Pargon/Files/Directory.h"
#include "Pargon/Files/File.h"
#include "Pargon/Serialization/StringWriter.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <codecvt>
#include <memory>
#include <objbase.h>
#include <shellapi.h>
#include <shlobj.h>
#include <Windows.h>

using namespace Pargon;

auto Directory::GetExists() const -> bool
{
	WIN32_FILE_ATTRIBUTE_DATA data;
	BOOL result = GetFileAttributesExA(_path.begin(), GetFileExInfoStandard, &data);
	return (result != 0) && (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

auto Directory::EnsureCreated() const -> bool
{
	if (!GetExists())
	{
		MovingUp().EnsureCreated();
		if (CreateDirectoryA(_path.begin(), NULL) == 0)
			return false;
	}

	return true;
}

auto Directory::Delete() const -> bool
{
	auto path = std::make_unique<char[]>(_path.Length() + 2);

	std::copy(_path.begin(), _path.end(), path.get());

	path[_path.Length()] = '\0';
	path[_path.Length() + 1] = '\0';

	SHFILEOPSTRUCTA operation;
	operation.hwnd = 0;
	operation.wFunc = FO_DELETE;
	operation.pFrom = path.get();
	operation.pTo = NULL;
	operation.fFlags = FOF_NO_UI;

	auto result = ::SHFileOperationA(&operation);

	return result == 0;
}

auto Directory::GetContents() const -> DirectoryContents
{
	DirectoryContents contents;

	WIN32_FIND_DATAA data;
	HANDLE handle = INVALID_HANDLE_VALUE;
	char search[MAX_PATH];

	strcpy_s(search, MAX_PATH, _path.begin());
	strcat_s(search, MAX_PATH, "\\*");

	handle = FindFirstFileExA(search, FindExInfoStandard, &data, FindExSearchNameMatch, NULL, 0);

	if (handle == INVALID_HANDLE_VALUE)
		return contents;

	do
	{
		auto length = static_cast<int>(std::char_traits<char>::length(data.cFileName));

		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ((length == 1) && (data.cFileName[0] == '.'))
				continue;

			if ((length == 2) && (data.cFileName[0] == '.') && (data.cFileName[1] == '.'))
				continue;

			auto directory = MovingDown({ data.cFileName, length });
			contents.Directories.Add(directory);
		}
		else
		{
			auto file = GetFile({ data.cFileName, length });
			contents.Files.Add(file);
		}
	}
	while (FindNextFileA(handle, &data) != 0);

	FindClose(handle);
	return contents;
}

namespace
{
	auto ConvertToUtf8(wchar_t* path) -> String
	{
		return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(path);
	}

	auto GetApplicationName() -> String
	{
		wchar_t path[MAX_PATH + 1];
		GetModuleFileName(NULL, path, MAX_PATH);
		auto directory = ConvertToUtf8(path);

		auto slash = LastIndexOf(directory, '\\');
		auto dot = LastIndexOf(directory, '.');

		return slash == String::InvalidIndex || dot == String::InvalidIndex ? "" : directory.GetView(slash + 1, dot - slash - 1);
	}

	auto ApplicationName() -> StringView
	{
		static String _name = GetApplicationName();
		return _name;
	}
}

auto Pargon::GetApplicationDirectory() -> Directory
{
	auto name = ApplicationName();

	wchar_t path[MAX_PATH + 1];
	GetCurrentDirectory(MAX_PATH, path);
	auto directory = ConvertToUtf8(path);

	return Directory(std::move(directory));
}

auto Pargon::GetDataDirectory() -> Directory
{
	auto name = ApplicationName();

	PWSTR path;
	SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &path);
	auto directory = ConvertToUtf8(path);
	CoTaskMemFree(path);

	return Directory(FormatString("{}\\{}", directory, name));
}

auto Pargon::GetDocumentsDirectory() -> Directory
{
	auto name = ApplicationName();

	PWSTR path;
	SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &path);
	auto directory = ConvertToUtf8(path);
	CoTaskMemFree(path);

	return Directory(FormatString("{}\\{}", directory, name));
}

auto Pargon::GetTemporaryDirectory() -> Directory
{
	auto name = ApplicationName();

	wchar_t path[MAX_PATH + 1];
	GetTempPath(MAX_PATH, path);
	auto directory = ConvertToUtf8(path);

	return Directory(FormatString("{}\\{}", directory, name));
}

auto File::GetLastModifiedTime() const -> Pargon::Time
{
	WIN32_FILE_ATTRIBUTE_DATA data;
	if (!GetFileAttributesExA(_path.begin(), GetFileExInfoStandard, &data))
		return 0_ms;

	LARGE_INTEGER time, adjustment;
	adjustment.QuadPart = 116444736000000000;
	time.HighPart = data.ftLastWriteTime.dwHighDateTime;
	time.LowPart = data.ftLastWriteTime.dwLowDateTime;
	time.QuadPart -= adjustment.QuadPart;

	return Time::FromMilliseconds(time.QuadPart / 10000);
}

auto File::Delete() const -> bool
{
	return DeleteFileA(_path.begin()) == TRUE;
}
