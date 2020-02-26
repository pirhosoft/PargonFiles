#pragma once

#include "Pargon/Containers/Buffer.h"
#include "Pargon/Containers/String.h"
#include "Pargon/Files/Directory.h"
#include "Pargon/Types/Time.h"

namespace Pargon
{
	struct FileText
	{
		bool Exists;
		String Text;
	};

	struct FileData
	{
		bool Exists;
		Buffer Data;
	};

	class File
	{
	public:
		File() = default;
		explicit File(String&& path);

		auto Path() const -> StringView;

		auto GetExists() const -> bool;
		auto GetDirectory() const -> Directory;
		auto GetName() const -> String;
		auto GetExtension() const -> String;
		auto GetLastModifiedTime() const -> Time;

		auto Delete() const -> bool;

		auto ReadText() const -> FileText;
		auto ReadData() const -> FileData;

		auto WriteText(StringView text) const -> bool;
		auto WriteData(BufferView data) const -> bool;

	private:
		friend class Directory;

		String _path;
	};

	auto GetValidFilename(StringView basis) -> String;
	auto GetRelativeFilename(StringView from, StringView to) -> String;
}

inline
auto Pargon::File::Path() const -> StringView
{
	return _path;
}
