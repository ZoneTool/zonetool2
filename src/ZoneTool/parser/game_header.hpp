#pragma once
#include "type.hpp"

namespace zonetool
{
	class game_header
	{
	public:
		game_header(const char* buffer, const std::size_t size);

		type* find_type(const std::string& type_name);
		std::vector<std::shared_ptr<type>>& types();

	private:
		std::vector<std::shared_ptr<type>> types_;
		std::unordered_map<std::string, type*> type_map_;

		std::string header_buffer_;

		type* find_or_register_type(const std::string& type_name);
		void register_alias(const std::string& type_name, const std::string& alias_name);
		void register_custom(const std::string& type_name, const std::size_t size);
		void register_primitive(const std::string& type_name, const std::size_t size);
		void replace_token(const std::string& token, const std::string& new_token);
		void parse_comment(lexical_analyser& analyser);
		void parse_expressions(lexical_analyser& analyser, type* type_ptr, const std::string& member_name);
		void parse_expressions(lexical_analyser& analyser, type* type_ptr);
		void parse_header();
		void finalize();
		
	};
}
