#include "Pargon/Files/File.h"

#include <algorithm>
#include <fstream>
#include <iterator>

using namespace Pargon;

File::File(String&& path) :
	_path(std::move(path))
{
	_path.ReplaceAll("\\", "/");
}

auto File::GetExists() const -> bool
{
	std::ifstream in(_path.begin(), std::ios::in | std::ios::binary);
	return in.is_open();
}

auto File::GetDirectory() const -> Directory
{
	auto slash = LastIndexOf(_path, '/');
	auto path = slash == String::InvalidIndex ? "" : _path.GetView(0, slash);

	return Directory(path);
}

auto File::GetName() const -> String
{
	auto slash = LastIndexOf(_path, '/');
	auto dot = LastIndexOf(_path, '.');
	return slash == String::InvalidIndex || dot == String::InvalidIndex ? _path : _path.GetView(slash + 1, dot - slash - 1).GetString();
}

auto File::GetExtension() const -> String
{
	auto index = LastIndexOf(_path, '.');
	return index == String::InvalidIndex ? ""_s : _path.GetView(index + 1).GetString();
}

auto File::ReadText() const -> FileText
{
	std::ifstream stream(_path.begin(), std::ios::in | std::ios::binary);

	if (stream.is_open())
	{
		auto string = std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
		return { true, std::move(string) };
	}

	return { false };
}

auto File::ReadData() const -> FileData
{
	Buffer buffer;
	std::ifstream stream(_path.begin(), std::ios::in | std::ios::binary);

	if (stream.is_open())
	{
		stream.seekg(0, std::ios::end);
		auto size = static_cast<unsigned int>(stream.tellg());
		auto data = buffer.Reserve(size);
	
		stream.seekg(0, std::ios::beg);
		stream.read(reinterpret_cast<char*>(data.begin()), size);

		return { true, std::move(buffer) };
	}

	return { false };
}

auto File::WriteText(StringView text) const -> bool
{
	GetDirectory().EnsureCreated();

	std::ofstream out(_path.begin(), std::ios::out | std::ios::binary);

	if (out.is_open())
	{
		out.write(text.begin(), text.Length());
		return true;
	}

	return false;
}

auto File::WriteData(BufferView data) const -> bool
{
	GetDirectory().EnsureCreated();

	std::ofstream out(_path.begin(), std::ios::out | std::ios::binary);

	if (out.is_open())
	{
		out.write(reinterpret_cast<const char*>(data.begin()), data.Size());
		return true;
	}

	return false;
}

auto Pargon::GetValidFilename(StringView basis) -> Pargon::String
{
	auto noSpaces = ReplaceAll(basis, ' ', '_');
	auto noForwardSlashes = ReplaceAll(noSpaces, '/', '-');
	auto noBackwardSlashes = ReplaceAll(noForwardSlashes, '\\', '-');

	return noBackwardSlashes;
}

auto Pargon::GetRelativeFilename(StringView from, StringView to) -> String
{
	auto froms = SplitOnAny(from, "/\\");
	auto tos = SplitOnAny(to, "/\\");

	auto i = 0;
	while (i < froms.Count() && i < tos.Count() && (froms.Item(i) == tos.Item(i)))
		i++;

	auto ups = froms.Count() - i;
	auto result = String();

	for (auto up = 0; up < ups; up++)
		result.Append("../");

	for (; i < tos.LastIndex(); i++)
	{
		result.Append(tos.Item(i));
		result.Append('/');
	}

	return Append(result, tos.Last());
}
