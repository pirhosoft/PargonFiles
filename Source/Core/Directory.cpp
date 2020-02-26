#include "Pargon/Files/Directory.h"
#include "Pargon/Files/File.h"
#include "Pargon/Serialization/StringWriter.h"

using namespace Pargon;

Directory::Directory(DirectoryRoot root)
{
	switch (root)
	{
		case DirectoryRoot::Application: *this = ApplicationDirectory(); break;
		case DirectoryRoot::Data: *this = DataDirectory(); break;
		case DirectoryRoot::Documents: *this = DocumentsDirectory(); break;
		case DirectoryRoot::Temporary: *this = TemporaryDirectory(); break;
	}
}

auto Directory::GetName() const -> String
{
	auto slash = LastIndexOf(_path, '/');
	return slash == String::InvalidIndex ? _path : _path.GetView(slash + 1).GetString();
}

auto Directory::GetParts() const -> List<StringView>
{
	return Split(_path, "/");
}

void Directory::Move(StringView relativePath)
{
	auto parts = SplitOnAny(relativePath, "/\\");

	for (auto& part : parts)
	{
		if (part == "..")
			MoveUp();
		else if (part != ".")
			MoveDown(part);
	}
}

void Directory::MoveUp()
{
	auto slash = LastIndexOf(_path, '/');
	_path = slash == String::InvalidIndex ? "" : _path.GetView(0, slash);
}

void Directory::MoveDown(StringView directoryName)
{
	auto cleaned = TrimFromEnd(ReplaceAll(directoryName, '\\', '/'), '/');

	if (!_path.IsEmpty())
		_path.Append('/');

	_path.Append(cleaned);
}

auto Directory::Moving(StringView relativePath) const -> Directory
{
	auto copy = *this;
	copy.Move(relativePath);
	return copy;
}

auto Directory::MovingUp() const -> Directory
{
	auto copy = *this;
	copy.MoveUp();
	return copy;
}

auto Directory::MovingDown(StringView directoryName) const -> Directory
{
	auto copy = *this;
	copy.MoveDown(directoryName);
	return copy;
}

auto Directory::GetFile(StringView filename) const -> File
{
	return File(FormatString("{}/{}", _path, filename));
}

Directory::Directory(String&& path) :
	_path(std::move(path))
{
	_path.ReplaceAll("\\", "/");
}

auto Pargon::ApplicationDirectory() -> const Directory&
{
	static Directory _directory = GetApplicationDirectory();
	return _directory;
}

auto Pargon::DataDirectory() -> const Directory&
{
	static Directory _directory = GetDataDirectory();
	return _directory;
}

auto Pargon::DocumentsDirectory() -> const Directory&
{
	static Directory _directory = GetDocumentsDirectory();
	return _directory;
}

auto Pargon::TemporaryDirectory() -> const Directory&
{
	static Directory _directory = GetTemporaryDirectory();
	return _directory;
}
