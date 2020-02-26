#pragma once

#include "Pargon/Containers/List.h"
#include "Pargon/Containers/String.h"

namespace Pargon
{
	class File;
	class Directory;

	struct DirectoryContents
	{
		List<File> Files;
		List<Directory> Directories;
	};

	enum class DirectoryRoot
	{
		Application,
		Data,
		Documents,
		Temporary
	};

	class Directory
	{
	public:
		Directory(DirectoryRoot root);

		auto Path() const -> StringView;

		auto GetExists() const -> bool;
		auto GetName() const -> String;
		auto GetParts() const -> List<StringView>;
		auto GetContents() const -> DirectoryContents;

		auto EnsureCreated() const -> bool;
		auto Delete() const -> bool;

		void Move(StringView relativePath);
		void MoveUp();
		void MoveDown(StringView directoryName);

		auto Moving(StringView relativePath) const -> Directory;
		auto MovingUp() const -> Directory;
		auto MovingDown(StringView directoryName) const -> Directory;

		auto GetFile(StringView filename) const -> File;

	private:
		friend class File;
		friend auto GetApplicationDirectory() -> Directory;
		friend auto GetDataDirectory() -> Directory;
		friend auto GetDocumentsDirectory() -> Directory;
		friend auto GetTemporaryDirectory() -> Directory;

		explicit Directory(String&& path);

		String _path;
	};

	auto GetApplicationDirectory() -> Directory;
	auto GetDataDirectory() -> Directory;
	auto GetDocumentsDirectory() -> Directory;
	auto GetTemporaryDirectory() -> Directory;

	auto ApplicationDirectory() -> const Directory&;
	auto DataDirectory() -> const Directory&;
	auto DocumentsDirectory() -> const Directory&;
	auto TemporaryDirectory() -> const Directory&;
}

inline
auto Pargon::Directory::Path() const -> StringView
{
	return _path;
}
