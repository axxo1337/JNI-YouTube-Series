#ifndef JNIMAPPER_H_
#define JNIMAPPER_H_

#include <map>
#include <cstdint>
#include <string>
#include <memory>
#include <cassert>

#include "JNITypes.h"

#include <json.hpp>
#include <jni.h>

class JNIMapper final
{
public:
	JNIMapper(const uint8_t* memory, JNIEnv* p_env)
	{
		using namespace nlohmann;
		
		parsed_map = nlohmann::json::parse(memory);
		
		printf("=========================== JNIMapper ===========================\n");

		for (json::iterator it_i = parsed_map.begin(); it_i != parsed_map.end(); ++it_i) 
		{
			static bool found_version{ false };

			if (!found_version && it_i.value().is_string())
			{
				found_version = true;
				continue;
			}

			jclass class_ptr = p_env->FindClass(parsed_map[it_i.key()]["name"].get<std::string>().c_str());

			if (class_ptr == nullptr)
			{
				printf("[-] Failed to get class %s ptr\n", it_i.key().c_str());
				continue;
			}
			else
				printf("[+] Got class %s\n", it_i.key().c_str());

			auto p_class = std::make_shared<JNIClass>(p_env, class_ptr);
			classes.emplace(std::pair{ it_i.key(), std::move(p_class) });

			JNIClass* p_mapped_class = classes[it_i.key()].get();

			/* Get field ids */
			for (json::iterator it_j = parsed_map[it_i.key()]["fields"].begin(); it_j != parsed_map[it_i.key()]["fields"].end(); ++it_j)
			{
				jfieldID field_id{ 0 };

				std::string hash = it_j.value().get<std::string>();
				std::string no_indicator_hash = hash.substr(2, hash.size() - 1);

				size_t delimiter = no_indicator_hash.find("|");
				
				std::string type = no_indicator_hash.substr(0, delimiter);
				std::string name = no_indicator_hash.substr(delimiter+1, no_indicator_hash.size()-1);

				bool is_static{ false };

				/* Check if the field is static or not */
				if (hash[0] == 'S')
				{
					field_id = p_env->GetStaticFieldID(class_ptr, name.c_str(), type.c_str());
					is_static = true;
				}
				else
					field_id = p_env->GetFieldID(class_ptr, name.c_str(), type.c_str());

				if (field_id == 0)
				{
					printf("      [-] Failed to get field %s\n", it_j.key().c_str());
					continue;
				}
				else
					printf("      [+] Got field %s\n", it_j.key().c_str());

				std::shared_ptr<JNIField> p_field;

				switch (type.c_str()[0])
				{
				case 'I':
				{
					p_field = std::make_shared<JNIFieldInt>(p_env, p_mapped_class, field_id, is_static);
				} break;

				case 'S':
				{
					p_field = std::make_shared<JNIFieldShort>(p_env, p_mapped_class, field_id, is_static);
				} break;

				case 'J':
				{
					p_field = std::make_shared<JNIFieldLong>(p_env, p_mapped_class, field_id, is_static);
				} break;

				case 'F':
				{
					p_field = std::make_shared<JNIFieldFloat>(p_env, p_mapped_class, field_id, is_static);
				} break;

				case 'D':
				{
					p_field = std::make_shared<JNIFieldDouble>(p_env, p_mapped_class, field_id, is_static);
				} break;

				case 'Z':
				{
					p_field = std::make_shared<JNIFieldBoolean>(p_env, p_mapped_class, field_id, is_static);
				} break;

				case 'B':
				{
					p_field = std::make_shared<JNIFieldByte>(p_env, p_mapped_class, field_id, is_static);
				} break;

				case 'C':
				{
					p_field = std::make_shared<JNIFieldChar>(p_env, p_mapped_class, field_id, is_static);
				} break;

				case 'L':
				{
					p_field = std::make_shared<JNIFieldObject>(p_env, p_mapped_class, field_id, is_static);
				} break;
				}

				p_mapped_class->fields.emplace(std::pair{it_j.key(), std::move(p_field)});
			}

			/* Get method ids */
			for (json::iterator it_j = parsed_map[it_i.key()]["methods"].begin(); it_j != parsed_map[it_i.key()]["methods"].end(); ++it_j)
			{
				jmethodID method_id{ 0 };

				std::string hash = it_j.value().get<std::string>();
				std::string no_indicator_hash = hash.substr(2, hash.size() - 1);

				size_t delimiter = no_indicator_hash.find("|");

				std::string sig = no_indicator_hash.substr(0, delimiter);
				size_t sig_delimiter = no_indicator_hash.find(")");

				std::string name = no_indicator_hash.substr(delimiter + 1, no_indicator_hash.size() - 1);

				bool is_static{ false };

				/* Check if the field is static or not */
				if (hash[0] == 'S')
				{
					method_id = p_env->GetStaticMethodID(class_ptr, name.c_str(), sig.c_str());
					is_static = true;
				}
				else
					method_id = p_env->GetMethodID(class_ptr, name.c_str(), sig.c_str());

				if (method_id == 0)
				{
					printf("      [-] Failed to get method %s\n", it_j.key().c_str());
					continue;
				}
				else
					printf("      [+] Got method %s\n", it_j.key().c_str());

				std::shared_ptr<JNIMethod> p_method;

				switch (sig.substr(sig_delimiter+1, sig.size()-1)[0])
				{
				case 'V':
				{
					p_method = std::make_shared<JNIMethodVoid>(p_env, p_mapped_class, method_id, is_static);
				} break;

				case 'I':
				{
					p_method = std::make_shared<JNIMethodInt>(p_env, p_mapped_class, method_id, is_static);
				} break;

				case 'S':
				{
					p_method = std::make_shared<JNIMethodShort>(p_env, p_mapped_class, method_id, is_static);
				} break;

				case 'J':
				{
					p_method = std::make_shared<JNIMethodLong>(p_env, p_mapped_class, method_id, is_static);
				} break;

				case 'F':
				{
					p_method = std::make_shared<JNIMethodFloat>(p_env, p_mapped_class, method_id, is_static);
				} break;

				case 'D':
				{
					p_method = std::make_shared<JNIMethodDouble>(p_env, p_mapped_class, method_id, is_static);
				} break;

				case 'Z':
				{
					p_method = std::make_shared<JNIMethodBoolean>(p_env, p_mapped_class, method_id, is_static);
				} break;

				case 'B':
				{
					p_method = std::make_shared<JNIMethodByte>(p_env, p_mapped_class, method_id, is_static);
				} break;

				case 'C':
				{
					p_method = std::make_shared<JNIMethodChar>(p_env, p_mapped_class, method_id, is_static);
				} break;

				case 'L':
				{
					p_method = std::make_shared<JNIMethodObject>(p_env, p_mapped_class, method_id, is_static);
				} break;
				}

				p_mapped_class->methods.emplace(std::pair{ it_j.key(), std::move(p_method) });
			}
		}

		parsed_map.begin();
	}

	nlohmann::json& GetParsedMap()
	{
		return parsed_map;
	}

public:
	std::map<std::string, std::shared_ptr<JNIClass>> classes;

private:
	nlohmann::json parsed_map;
};

#endif