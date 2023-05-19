#include "JNI.h"

#include <vector>
#include <regex>

std::string JNI::DecodeSignature(std::string method)
{
	static std::regex regex("((?!\x20|\x09).)*");

	static std::map<std::string, std::string> types = {
		{"boolean", "Z"},
		{"byte", "B"},
		{"char", "C"},
		{"short", "S"},
		{"int", "I"},
		{"long", "J"},
		{"float", "F"},
		{"double", "D"},
		{"void", "V"},
		{"String", "Ljava/lang/String;"},
		{"boolean[]", "[Z"},
		{"byte[]", "[B"},
		{"char[]", "[C"},
		{"short[]", "[S"},
		{"int[]", "[I"},
		{"long[]", "[J"},
		{"float[]", "[F"},
		{"double[]", "[D"},
		{"String[]", "[Ljava/lang/String;"},
		// Exceptions
		{"public", ""},
		{"private", ""},
		{"static", ""},
	};

	// Tokenize start
	std::vector<std::string> args = {};
	std::vector<std::string> tokens = {};
	std::sregex_iterator first(method.begin(), method.end(), regex), last;

	while (first != last)
	{
		std::string str = first->str();

		if (str == "")
		{
			first++;
			continue;
		}

		size_t token_char_found_pos = str.find("(");
		if (token_char_found_pos != str.npos)
			break;

		tokens.push_back(str);
		first++;
	}

	// Tokenize args
	size_t method_args_start_pos = method.find("(");

	if (method_args_start_pos != method.npos)
	{
		method = method.substr(0, method.find(")"));

		std::string args = method.substr(method_args_start_pos + 1);
		std::sregex_iterator args_first(args.begin(), args.end(), regex), args_last;

		while (args_first != args_last)
		{
			std::string args_str = args_first->str();

			if (args_str == "")
			{
				args_first++;
				continue;
			}

			tokens.push_back(args_str);
			args_first++;
		}
	}

	// Parse tokens
	std::vector<std::string> parsed_tokens = {};

	for (int i{}; i < tokens.size(); i++)
	{
		std::string token = tokens[i];

		if (types.find(token) == types.end())
			continue;

		std::string type = types[token];

		if (type == "")
			continue;

		parsed_tokens.push_back(type);
	}

	std::string result = "(";

	for (int i{ 1 }; i < parsed_tokens.size(); i++)
		result.append(parsed_tokens[i]);

	result.append(")");
	result.append(parsed_tokens[0]);

	return result;
}