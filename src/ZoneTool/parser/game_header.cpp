#include "stdafx.hpp"
#include "game_header.hpp"
#include "lexical_analyser.hpp"

#include <algorithm>

namespace zonetool
{
	game_header::game_header(const char* buffer, const std::size_t size)
	{
		this->header_buffer_ = std::string(buffer, size);
		
		this->register_primitive("int64", 8);
		this->register_primitive("uint64", 8);
		this->register_primitive("double", 8);
		this->register_primitive("int32", 4);
		this->register_primitive("uint32", 4);
		this->register_primitive("float", 4);
		this->register_primitive("int16", 2);
		this->register_primitive("uint16", 2);
		this->register_primitive("int8", 1);
		this->register_primitive("uint8", 1);
		this->register_primitive("char", 1);
		this->register_primitive("bool", 1);
		this->register_primitive("void", 0);

		this->register_custom("XString", 1);
		this->register_custom("TempString", 1);

		this->replace_token("_BYTE", "uint8");
		// this->replace_token("char", "int8");
		this->replace_token("unsigned char", "uint8");
		this->replace_token("__int8", "int8");
		this->replace_token("__uint8", "uint8");
		this->replace_token("unsigned __uint8", "uint8");

		this->replace_token("unsigned short", "uint16");
		this->replace_token("short", "int16");
		this->replace_token("SHORT", "int16");
		this->replace_token("USHORT", "uint16");
		this->replace_token("WORD", "uint16");
		this->replace_token("__int16", "int16");
		this->replace_token("__uint16", "uint16");
		this->replace_token("unsigned int16", "uint16");
		this->replace_token("unsigned uint16", "uint16");

		this->replace_token("DWORD", "uint32");
		this->replace_token("__int32", "int32");
		this->replace_token("__uint32", "uint32");
		this->replace_token("unsigned __int32", "uint32");
		this->replace_token("int ", "int32 ");						// keep the space!
		this->replace_token("int* ", "int32 ");						// keep the space!
		this->replace_token("unsigned int ", "uint32 ");			// keep the space!
		this->replace_token("unsigned int* ", "uint32* ");			// keep the space!
		this->replace_token("unsigned int32", "uint32");

		
		this->replace_token("__int64", "int64");
		this->replace_token("__uint64", "uint64");
		this->replace_token("long long", "int64");
		this->replace_token("unsigned long long", "uint64");
		this->replace_token("unsigned __uint64", "uint64");
		this->replace_token("unsigned int64", "uint64");
		this->replace_token("unsigned uint64", "uint64");

		// this->replace_token("::", "____");
		this->replace_token("const ", "");
		
		this->parse_header();
		this->finalize();
	}

	void string_replace(std::string& str, const std::string& from, const std::string& to)
	{
		if (from.empty())
		{
			return;
		}
		
		auto start_pos = 0u;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) 
		{
			str.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}
	}

	std::vector<std::shared_ptr<type>>& game_header::types()
	{
		return types_;
	}

	void game_header::replace_token(const std::string& token, const std::string& new_token)
	{
		string_replace(header_buffer_, token, new_token);
	}

	type* game_header::find_type(const std::string& type_name)
	{
		const auto itr = this->type_map_.find(type_name);
		if (itr != this->type_map_.end())
		{
			return itr->second;
		}

		ZONETOOL_FATAL("unknown type %s.", type_name.data());
		
		return nullptr;
	}

	type* game_header::find_or_register_type(const std::string& type_name)
	{
		const auto itr = this->type_map_.find(type_name);
		if (itr != this->type_map_.end())
		{
			return itr->second;
		}

		this->types_.push_back(std::make_shared<type>(type_name));

		const auto registered_type = this->types_[this->types_.size() - 1];
		this->type_map_[type_name] = registered_type.get();

		return registered_type.get();
	}

	void game_header::register_alias(const std::string& type_name, const std::string& alias_name)
	{
		const auto itr = this->type_map_.find(alias_name);
		if (itr != this->type_map_.end())
		{
			this->types_.push_back(std::make_shared<type>(type_name, itr->second));

			const auto registered_type = this->types_[this->types_.size() - 1];
			this->type_map_[type_name] = registered_type.get();
		}
	}
	
	void game_header::register_custom(const std::string& type_name, const std::size_t size)
	{
		this->types_.push_back(std::make_shared<type>(type_name, false, size));

		const auto registered_type = this->types_[this->types_.size() - 1];
		registered_type->set_alignment(std::min(4u, size));

		this->type_map_[type_name] = registered_type.get();
	}

	void game_header::register_primitive(const std::string& type_name, const std::size_t size)
	{
		this->types_.push_back(std::make_shared<type>(type_name, true, size));
		
		const auto registered_type = this->types_[this->types_.size() - 1];
		registered_type->set_alignment(std::min(4u, size));

		this->type_map_[type_name] = registered_type.get();
	}
	
	void game_header::parse_comment(lexical_analyser& analyser)
	{
		// to do: parse comments
		ZONETOOL_FATAL("comments not yet supported");
	}

	void game_header::parse_expressions(lexical_analyser& analyser, type* type_ptr, const std::string& member_name)
	{
		if (analyser.current() == "/"s)
		{
			analyser.next();

			if (analyser.current() == "/"s)
			{
				// either a comment or expression
				analyser.next();

				// expression
				if (analyser.current() == "@"s)
				{
					auto expressions = type_expression::parse_expressions(&analyser);
					type_ptr->get_member(member_name)->set_expressions(expressions);

					if (analyser.current() != ";"s)
					{
						ZONETOOL_FATAL("Expected ; after expression statement.");
					}

					analyser.next();
				}
				else
				{
					parse_comment(analyser);
				}
			}
		}
	}

	void game_header::parse_expressions(lexical_analyser& analyser, type* type_ptr)
	{
		if (analyser.current() == "/"s)
		{
			analyser.next();

			if (analyser.current() == "/"s)
			{
				// either a comment or expression
				analyser.next();

				// expression
				if (analyser.current() == "@"s)
				{
					auto expressions = type_expression::parse_expressions(&analyser);
					type_ptr->set_expressions(expressions);

					analyser.next();
				}
				else
				{
					parse_comment(analyser);
				}
			}
		}
	}

	void game_header::parse_header()
	{
		lexical_analyser analyser(header_buffer_.data(), header_buffer_.size());

		while (analyser.current())
		{
			// parse past namespaces
			if (analyser.current() == "namespace"s)
			{
				while (analyser.next() != "{"s)
				{

				}

				analyser.next();
				continue;
			}

			// parse past using statements
			if (analyser.current() == "using"s)
			{
				while (analyser.next() != ";"s)
				{

				}

				analyser.next();
				continue;
			}

			if (analyser.current() == "struct"s || analyser.current() == "union"s)
			{
				auto struct_type_ptr = analyser.current();

				auto alignment = 4u;
				analyser.next();

				if (analyser.current() == "__declspec"s)
				{
					if (analyser.next() != "("s)
					{
						ZONETOOL_FATAL("unexpected token %s", analyser.current());	
					}
					
					if (analyser.next() == "align"s)
					{
						if (analyser.next() != "("s)
						{
							ZONETOOL_FATAL("unexpected token %s", analyser.current());	
						}
						
						alignment = std::stoul(analyser.next());

						if (analyser.next() != ")"s)
						{
							ZONETOOL_FATAL("unexpected token %s", analyser.current());
						}
					}
					else
					{
						ZONETOOL_FATAL("unknown declspec keyword %s", analyser.current());
					}

					if (analyser.next() != ")"s)
					{
						ZONETOOL_FATAL("unexpected token %s", analyser.current());
					}
					analyser.next();
				}
				
				const auto struct_name = analyser.current();
				analyser.next();
				
				// create type with current name
				const auto type_ptr = this->find_or_register_type(struct_name);

				// parse type expressions
				parse_expressions(analyser, type_ptr);

				// 
				auto is_parsing_assets = false;

				// set alignment
				type_ptr->set_alignment(alignment);
				
				// set "type of type" to union if we're parsing an union
				if (struct_type_ptr == "union"s)
				{
					type_ptr->set_type_variant(type::type_variant::_union);
				}

				// members of this union should be marked as assets
				if (type_ptr->get_type_variant() == type::type_variant::_union && struct_name == "XAssetHeader"s)
				{
					is_parsing_assets = true;
				}
				
				// type reference to a non-yet existing type
				if (analyser.current() == ";"s)
				{
					
				}
				// parsing the type data
				else if (analyser.current() == "{"s)
				{
					analyser.next();
					
					// parse struct members
					while (analyser.current() != "}"s)
					{
						const auto type_name = analyser.current();
						auto pointer_depth = 0u;

						const auto member_type_ptr = this->find_or_register_type(type_name);
						
						// mark type as asset because its inside of the XAssetHeader union
						if (is_parsing_assets)
						{
							member_type_ptr->make_asset();
						}

						analyser.next();

						while (analyser.current() == "*"s)
						{
							pointer_depth++;
							analyser.next();
						}

						// parse "float (*verts)[3];"
						auto weird_array_parsing = false;
						if (analyser.current() == "("s)
						{
							if (analyser.next() == "*"s)
							{
								// mark as pointer
								pointer_depth++;

								analyser.next();
								weird_array_parsing = true;
							}
							else
							{
								ZONETOOL_FATAL("expected * after (.");
							}
						}

						const auto member_name = analyser.current();
						analyser.next();

						// parse "float (*verts)[3];"
						if (weird_array_parsing)
						{
							if (analyser.current() != ")"s)
							{
								ZONETOOL_FATAL("expected ) after field name.");
							}

							analyser.next();
						}
						
						// parse arrays
						if (analyser.current() == "["s)
						{
							std::vector<std::size_t> array_sizes;

							while (analyser.current() == "["s)
							{
								const auto array_size = analyser.next();
								analyser.next();
								analyser.next();

								array_sizes.push_back(std::stoul(array_size));
							}

							type_member member(member_type_ptr, pointer_depth, array_sizes);
							type_ptr->add_member(member_name, member);
						}
						else
						{
							type_member member(member_type_ptr, pointer_depth);
							type_ptr->add_member(member_name, member);
						}

						// increment usagecount of type
						type_ptr->inc_usage_count();

						// parse expressions
						if (analyser.current() == ";"s)
						{
							analyser.next();

							// parse member expressions
							parse_expressions(analyser, type_ptr, member_name);

							// get member
							auto member_ptr = type_ptr->get_member(member_name);

							// 
							if (member_ptr->type_ptr()->is_asset())
							{
								if (member_ptr->has_expression(type_expression::expression_type::enum_value_expression))
								{
									member_ptr->type_ptr()->set_asset_type(member_ptr->get_expression(type_expression::expression_type::enum_value_expression)->expression());
								}
							}
						}
						else
						{
							analyser.next();
						}
					}
				}
			}
			else if (analyser.current() == "enum"s)
			{
				analyser.next();

				const auto enum_name = analyser.current();
				const auto type_ptr = this->find_or_register_type(enum_name);
				type_ptr->set_type_variant(type::type_variant::_enum);
				
				analyser.next();
				if (analyser.current() == ";"s)
				{
					// printf("referenced enum %s\n", enum_name);
				}
				else if (analyser.current() == "{"s)
				{
					auto cur_enum_index = 0;
					
					analyser.next();
					// printf("parsing enum %s\n", enum_name);

					while (analyser.current() != "}"s)
					{
						const auto enum_value = analyser.current();
						analyser.next();

						if (analyser.current() == "="s)
						{
							analyser.next();
							const auto real_value = analyser.current();

							// printf("mapping %s to %s\n", enum_value, real_value);

							cur_enum_index = std::stoi(real_value);
						}
						else
						{
							// printf("mapping %s to %u\n", enum_value, cur_enum_index);
						}

						type_ptr->add_enum_member(enum_value, cur_enum_index);
						cur_enum_index++;

						if (analyser.next() == ","s)
						{
							analyser.next();
						}
					}
				}
				else
				{
					printf("parse error when parsing enum %s!\n", enum_name);
				}
			}

			// jump to next expression
			analyser.next();
		}
	}

	using member_pair = std::pair<std::string, type_member>;
	using after_member_pair = std::pair<std::string, std::pair<std::string, type_member>>;
	bool operator==(const member_pair& l, const std::string& r)
	{
		return l.first == r;
	}
	bool operator==(const after_member_pair& l, const std::string& r)
	{
		return l.first == r;
	}

	void game_header::finalize()
	{
		for (auto& type : types_)
		{
			type->calculate_size();
		}

		// modify member
		for (auto& type : types_)
		{
			auto members = type->members();

			std::vector<std::pair<std::string, std::pair<std::string, type_member>>> after_expressions;
			std::vector<std::pair<std::string, type_member>> sorted_members;

			// sort members for writing
			for (auto& member : members)
			{
				if (member.second.has_expression(type_expression::expression_type::before_expression))
				{
					auto expr = member.second.get_expression(type_expression::expression_type::before_expression);
					auto before_field = expr->expression();

					const auto itr = std::find(sorted_members.begin(), sorted_members.end(), before_field);
					sorted_members.insert(itr, member);
				}
				else if (member.second.has_expression(type_expression::expression_type::after_expression))
				{
					auto expr = member.second.get_expression(type_expression::expression_type::after_expression);
					auto after_field = expr->expression();

					after_expressions.push_back({ after_field, member });
				}
				else
				{
					sorted_members.push_back(member);

					const auto itr = std::find(after_expressions.begin(), after_expressions.end(), member.first);
					if (itr != after_expressions.end())
					{
						sorted_members.push_back(itr->second);
					}
				}
			}

			// detect count fields
			for (auto i = 0u; i < sorted_members.size(); i++)
			{
				// skip fields that can't have count fields
				if (!sorted_members[i].second.is_array() && !sorted_members[i].second.is_pointer())
				{
					continue;
				}

				// skip overlaoded count expressions
				if (sorted_members[i].second.has_expression(type_expression::expression_type::count_expression))
				{
					continue;
				}

				// 
				// example: planes
				// count field: planeCount

				//
				// example: brushes
				// count field: numBrushes

				// 
				const auto itr = std::find_if(sorted_members.begin(), sorted_members.end(), [&sorted_members, i](auto& p1)
				{
					if (to_lower(p1.first) == va("num%s", to_lower(sorted_members[i].first).data()) || 
						to_lower(p1.first) == va("%scount", to_lower(sorted_members[i].first).data()) || 
						to_lower(p1.first) == va("%scount", to_lower(sorted_members[i].first).substr(0, sorted_members[i].first.size() - 1).data()))
					{
						if (!p1.second.has_expression(type_expression::expression_type::ignore_expression))
						{
							return true;
						}
					}

					return false;
				});

				// check if we've found a count field
				if (itr == sorted_members.end())
				{
					continue;
				}

				// we did! lets make a count expression
				ZONETOOL_INFO("Found a count field for \"%s\"! count field is \"%s\".", sorted_members[i].first.data(), itr->first.data());

				// set count expression
				sorted_members[i].second.set_expressions({ type_expression(type_expression::expression_type::count_expression, va("{asset}->%s", itr->first.data())) });
			}

			type->set_members(sorted_members);
		}
	}
}
