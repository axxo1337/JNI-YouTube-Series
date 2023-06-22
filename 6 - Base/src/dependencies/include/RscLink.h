#ifndef RSCLINK_RSCLINK_H_
#define RSCLINK_RSCLINK_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <cstring>

class RscLinkHeading final
{
public:
	struct Entry
	{
		std::string name;
		uintmax_t size;
		unsigned int start;
		unsigned int end;
		std::uint8_t* ptr;
	};

	RscLinkHeading(std::uint8_t* p_heading, uintmax_t heading_size, std::uint8_t* p_data, uintmax_t data_size)
		: p_heading(p_heading), heading_size(heading_size), p_data(p_data), data_size(data_size)
	{
		ParseHeading();
		printf("[+] Parsed headings\n");
	}

private:
	Entry ParseEntry(std::string entry_str)
	{
		Entry entry;

		size_t pos{ entry_str.find("|", 0) };
		entry.name = entry_str.substr(0, pos);

		for (int i{ 0 }; i < 3; ++i)
		{
			size_t last_pos{ pos };
			pos = entry_str.find("|", pos + 1);

			if (pos == std::string::npos)
			{
				pos = entry_str.size();
			}

			char* ptr;
			uintmax_t val{ (uintmax_t)std::strtoul(entry_str.substr(last_pos + 1, pos - last_pos - 1).c_str(), &ptr, 10) };

			switch (i)
			{
			case 0:
				entry.size = val;
				break;
			case 1:
				entry.start = val;
				break;
			case 2:
				entry.end = val;
			}
		}

		entry.ptr = p_data + entry.start;

		return entry;
	}

	void ParseHeading()
	{
		std::string heading((char*)p_heading, heading_size);

		size_t pos{ heading.find("\r\n", 0) };

		Entry entry{ ParseEntry(heading.substr(0, pos)) };
		entries.emplace(std::pair{ entry.name, entry });

		while (pos != std::string::npos)
		{
			size_t last_pos{ pos };
			pos = heading.find("\r\n", pos + 2);

			if (pos == std::string::npos)
				break;

			entry = ParseEntry(heading.substr(last_pos + 2, pos - last_pos - 2));
			entries.emplace(std::pair{ entry.name, entry });
		}
	}

public:
	std::unordered_map<std::string, Entry> entries;

private:
	std::uint8_t* p_heading;
	uintmax_t heading_size;
	std::uint8_t* p_data;
	uintmax_t data_size;
};


class RscLink final
{
public:
	RscLink(std::uint8_t* p_heading, uintmax_t heading_size, std::uint8_t* p_data, uintmax_t data_size)
	{
		if (heading_size <= 1 && *(char*)(p_heading) == '\x00')
			printf("[!] No contents in Link.h, nothing to parse\n");
		else
			p_link_heading = std::make_unique<RscLinkHeading>(p_heading, heading_size, p_data, data_size);
	}

	RscLinkHeading::Entry operator[](std::string entry_name)
	{
		return p_link_heading->entries[entry_name];
	}

private:
	std::unique_ptr<RscLinkHeading> p_link_heading;
};

#endif
