#include "stdafx.hpp"

#include <assert.h>

#include "parser/game_header.hpp"
#include "code_generator.hpp"

namespace zonetool::code_generator
{
	code_generator::code_generator(const game_header& header) : header_(header)
	{
		install_generator(std::bind(
			&code_generator::generator,
			this, 
			std::placeholders::_1, 
			std::placeholders::_2, 
			std::placeholders::_3, 
			std::placeholders::_4)
		);
	}

	std::size_t code_generator::calculate_align_size(const type* type)
	{
		return type->get_alignment() - 1;
	}

	void code_generator::generate_load(type* type, const std::string& function_name, std::string& code)
	{
		//int __cdecl Load_RawFile(bool a1)
		//{
		//	unsigned __int8* v1; // eax
		//	int v2; // ecx

		//	Load_Stream(a1, varRawFile, 16);
		//	DB_PushStreamPos(3u);
		//	varXString = varRawFile;
		//	Load_XString(0);
		//	if (*(varRawFile + 3))
		//	{
		//		*(varRawFile + 3) = DB_AllocStreamPos(0);
		//		v1 = *(varRawFile + 3);
		//		varConstChar = *(varRawFile + 3);
		//		v2 = *(varRawFile + 1);
		//		if (!v2)
		//			v2 = *(varRawFile + 2) + 1;
		//		Load_Stream(1, v1, v2);
		//	}
		//	return DB_PopStreamPos();
		//}

		auto variable_type = generate_variable_name(type, variable_type::var);

		if (type->has_expression(type_expression::expression_type::size_expression))
		{
			const auto count_expr = type->get_expression(type_expression::expression_type::size_expression);
			code += va("\tLoad_Stream(atStreamStart, %s, (%s));\n", variable_type.data(), count_expr->expression().data());
		}
		else
		{
			code += va("\tLoad_Stream(atStreamStart, %s, sizeof(%s));\n", variable_type.data(), type->name().data());
		}

		if (type->is_asset())
		{
			code += "\tDB_PushStreamPos(XFILE_BLOCK_VIRTUAL);\n";
		}

		code += "\n";

		// struct loading
		if (type->is_struct())
		{
			for (auto member : type->members())
			{
				// if member is enum, skip it.
				if (member.second.type_ptr()->is_enum() || member.second.has_expression(type_expression::expression_type::ignore_expression))
				{
					continue;
				}

				if (member.second.has_expression(type_expression::expression_type::stream_expression))
				{
					const auto* stream_expr = member.second.get_expression(type_expression::expression_type::stream_expression);
					code += va("\tDB_PushStreamPos(%s);\n", stream_expr->expression().data());
				}

				auto should_skip = false;
				if (member.second.is_pointer())
				{
					// alignment based on type
					if (member.second.has_expression(type_expression::expression_type::count_expression))
					{
						const auto count_expr = member.second.get_expression(type_expression::expression_type::count_expression);
						count_expr->set_asset(variable_type);

						if (member.second.pointer_depth() == 1)
						{
							auto mem_variable_type = generate_variable_name(member.second.type_ptr(), variable_type::var);
							auto mem_func_load = generate_function_name(member.second.type_ptr(), function_type::load_array);
							auto mem_func_allocload = generate_function_name(member.second.type_ptr(), function_type::allocload);

							code += va("\tif (%s->%s)\n", variable_type.data(), member.first.data());
							code += "\t{\n";

							if (member.second.has_expression(type_expression::expression_type::stream_expression) &&
								member.second.get_expression(type_expression::expression_type::stream_expression)->expression() == "XFILE_BLOCK_TEMP")
							{
								code += va("\t\t%s* pointer = %s->%s;\n", member.second.type_ptr()->name().data(), variable_type.data(), member.first.data());

								code += va("\t\tif (%s->%s == (%s*)0xFFFFFFFF || %s->%s == (%s*)0xFFFFFFFE)\n",
									variable_type.data(), member.first.data(), member.second.type_ptr()->name().data(),
									variable_type.data(), member.first.data(), member.second.type_ptr()->name().data());
								code += "\t\t{\n";
								code += va("\t\t\t%s->%s = %s();\n", variable_type.data(), member.first.data(), mem_func_allocload.data());
								code += va("\t\t\t%s = %s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
								code += va("\t\t\t%s** insertedPointer = nullptr;\n", member.second.type_ptr()->name().data());
								code += va("\t\t\tif (pointer == (%s*)0xFFFFFFFE)\n", member.second.type_ptr()->name().data());
								code += "\t\t\t{\n";
								code += va("\t\t\t\tinsertedPointer = (%s**)DB_InsertPointer();\n", member.second.type_ptr()->name().data());
								code += "\t\t\t}\n";
								code += va("\t\t\t%s(true, (%s));\n", mem_func_load.data(), count_expr->expression().data());
								code += "\t\t\tif (insertedPointer != nullptr)\n";
								code += "\t\t\t{\n";
								code += va("\t\t\t\t*insertedPointer = %s;\n", mem_variable_type.data());
								code += "\t\t\t}\n";
								code += "\t\t}\n";
								code += "\t\telse\n";
								code += "\t\t{\n";
								code += va("\t\t\tDB_ConvertOffsetToPointer((void**)&%s->%s);\n", variable_type.data(), member.first.data());
								code += "\t\t}\n";
							}
							else if (true || member.second.type_ptr()->is_referenced_by_multiple_structs())
							{
								code += va("\t\tif (%s->%s == (%s*)0xFFFFFFFF)\n", variable_type.data(), member.first.data(), member.second.type_ptr()->name().data());
								code += "\t\t{\n";
								code += va("\t\t\t%s->%s = %s();\n", variable_type.data(), member.first.data(), mem_func_allocload.data());
								code += va("\t\t\t%s = %s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
								code += va("\t\t\t%s(true, (%s));\n", mem_func_load.data(), count_expr->expression().data());
								code += "\t\t}\n";
								code += "\t\telse\n";
								code += "\t\t{\n";
								code += va("\t\t\tDB_ConvertOffsetToPointer((void**)&%s->%s);\n", variable_type.data(), member.first.data());
								code += "\t\t}\n";
							}
							else
							{
								code += va("\t\t%s->%s = %s();\n", variable_type.data(), member.first.data(), mem_func_allocload.data());
								code += va("\t\t%s = %s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
								code += va("\t\t%s(true, (%s));\n", mem_func_load.data(), count_expr->expression().data());
							}
							code += "\t}\n";
						}
						else if (member.second.pointer_depth() == 2)
						{
							auto mem_variable_type = generate_variable_name(member.second.type_ptr(), variable_type::var_ptr);
							auto mem_func_load = generate_function_name(member.second.type_ptr(), function_type::load_ptr_array);
							auto mem_func_allocload = generate_function_name(member.second.type_ptr(),function_type::allocload_ptr);

							code += va("\tif (%s->%s)\n", variable_type.data(), member.first.data());
							code += "\t{\n";

							if (true || member.second.type_ptr()->is_referenced_by_multiple_structs())
							{
								code += va("\t\tif (%s->%s == (%s**)0xFFFFFFFF)\n", variable_type.data(), member.first.data(), member.second.type_ptr()->name().data());
								code += "\t\t{\n";
								code += va("\t\t\t%s->%s = %s();\n", variable_type.data(), member.first.data(), mem_func_allocload.data());
								code += va("\t\t\t%s = %s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
								code += va("\t\t\t%s(true, (%s));\n", mem_func_load.data(), count_expr->expression().data());
								code += "\t\t}\n";
								code += "\t\telse\n";
								code += "\t\t{\n";
								code += va("\t\t\tDB_ConvertOffsetToPointer((void**)&%s->%s);\n", variable_type.data(), member.first.data());
								code += "\t\t}\n";
							}
							else
							{
								code += va("\t\t%s->%s = %s();\n", variable_type.data(), member.first.data(), mem_func_allocload.data());
								code += va("\t\t%s = %s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
								code += va("\t\t%s(true, (%s));\n", mem_func_load.data(), count_expr->expression().data());
							}
							code += "\t}\n";
						}
						
					}
					// no alignment here
					else if (member.second.is_array())
					{
						auto mem_variable_type = generate_variable_name(member.second.type_ptr(), variable_type::var_ptr);
						auto mem_func_load = generate_function_name(member.second.type_ptr(), function_type::load_ptr_array);

						code += va("\tif (%s->%s)\n", variable_type.data(), member.first.data());
						code += "\t{\n";
						code += va("\t\t%s = %s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
						code += va("\t\t%s(false, %u);\n", mem_func_load.data(), member.second.array_size());
						code += "\t}\n";
					}
					// check if type is an asset
					else if (member.second.type_ptr()->is_asset())
					{
						auto mem_variable_type = generate_variable_name(member.second.type_ptr(), variable_type::var_ptr);
						auto mem_func_load = generate_function_name(member.second.type_ptr(), function_type::load_ptr);

						code += va("\t%s = &%s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
						code += va("\t%s(false);\n", mem_func_load.data());
					}
					// alignment based on pointer size
					else
					{
						auto mem_variable_type = generate_variable_name(member.second.type_ptr(), variable_type::var_ptr);
						auto mem_func_load = generate_function_name(member.second.type_ptr(), function_type::load_ptr);
						auto mem_func_allocload = generate_function_name(member.second.type_ptr(), function_type::allocload);

						code += va("\tif (%s->%s)\n", variable_type.data(), member.first.data());
						code += "\t{\n";
						/*if (true || member.second.type_ptr()->is_referenced_by_multiple_structs())
						{
							code += va("\t\tif (%s->%s == (%s*)0xFFFFFFFF)\n", variable_type.data(), member.first.data(), member.second.type_ptr()->name().data());
							code += "\t\t{\n";
							code += va("\t\t\t%s->%s = %s();\n", variable_type.data(), member.first.data(), mem_func_allocload.data());
							code += va("\t\t\t%s = &%s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
							code += va("\t\t\t%s(true);\n", mem_func_load.data());
							code += "\t\t}\n";
							code += "\t\telse\n";
							code += "\t\t{\n";
							code += va("\t\t\tDB_ConvertOffsetToPointer((void**)&%s->%s);\n", variable_type.data(), member.first.data());
							code += "\t\t}\n";
						}
						else
						{
							code += va("\t\t%s->%s = %s();\n", variable_type.data(), member.first.data(), mem_func_allocload.data());
							code += va("\t\t%s = &%s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
							code += va("\t\t%s(true);\n", mem_func_load.data());
						}*/
						code += va("\t\t%s = &%s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
						code += va("\t\t%s(false);\n", mem_func_load.data());
						code += "\t}\n";
					}
				}
				else if (!member.second.type_ptr()->is_primitive())
				{
					if (member.second.is_array())
					{
						if (member.second.has_expression(type_expression::expression_type::count_expression))
						{
							const auto count_expr = member.second.get_expression(type_expression::expression_type::count_expression);
							count_expr->set_asset(variable_type);

							auto mem_variable_type = generate_variable_name(member.second.type_ptr(), variable_type::var);
							auto mem_func_load = generate_function_name(member.second.type_ptr(), function_type::load_array);

							code += va("\t%s = %s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
							code += va("\t%s(true, %s);\n", mem_func_load.data(), count_expr->expression().data());
						}
						else
						{
							auto mem_variable_type = generate_variable_name(member.second.type_ptr(), variable_type::var);
							auto mem_func_load = generate_function_name(member.second.type_ptr(), function_type::load_array);

							code += va("\t%s = %s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
							code += va("\t%s(false, %u);\n", mem_func_load.data(), member.second.array_size());
						}
					}
					else
					{
						auto mem_variable_type = generate_variable_name(member.second.type_ptr(), variable_type::var);
						auto mem_func_load = generate_function_name(member.second.type_ptr(), function_type::load);

						code += va("\t%s = &%s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
						code += va("\t%s(false);\n", mem_func_load.data());
					}
				}
				else
				{
					// static non-primitive fields with overwritten count
					if (member.second.has_expression(type_expression::expression_type::count_expression))
					{
						const auto count_expr = member.second.get_expression(type_expression::expression_type::count_expression);
						count_expr->set_asset(variable_type);

						auto mem_variable_type = generate_variable_name(member.second.type_ptr(), variable_type::var);
						auto mem_func_load = generate_function_name(member.second.type_ptr(), function_type::load_array);

						code += va("\t%s = %s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
						code += va("\t%s(true, %s);\n", mem_func_load.data(), count_expr->expression().data());
					}
					else
					{
						should_skip = true;
					}
				}

				if (member.second.has_expression(type_expression::expression_type::stream_expression))
				{
					code += "\tDB_PopStreamPos();\n";
				}

				if (should_skip)
				{
					continue;
				}

				code += "\n";
			}

			if (type->is_asset())
			{
				code += "\tDB_PopStreamPos();\n";
			}
		}

		// union loading
		else if (type->is_union())
		{
			if (type->has_expression(type_expression::expression_type::use_if_expression))
			{
				std::string keyword("if");
				std::pair<std::string, zonetool::type_member> default_member;

				for (auto member : type->members())
				{
					if (!member.second.has_expression(type_expression::expression_type::enum_value_expression))
					{
						ZONETOOL_WARNING("Member %s in %s has no enum value expression, skipping.", member.first.data(), type->name().data());
						continue;
					}

					auto enum_value_expr = member.second.get_expression(type_expression::expression_type::enum_value_expression);
					if (enum_value_expr->expression() == "default")
					{
						default_member = member;
						continue;
					}

					code += va("\t%s (%s)\n", keyword.data(), enum_value_expr->expression().data());
					code += "\t{\n";

					// if member is a pointer
					if (member.second.is_pointer())
					{
						if (member.second.has_expression(type_expression::expression_type::count_expression))
						{
							const auto count_expr = member.second.get_expression(type_expression::expression_type::count_expression);
							count_expr->set_asset(variable_type);

							auto mem_variable_type = generate_variable_name(member.second.type_ptr(), variable_type::var);
							auto mem_func_load = generate_function_name(member.second.type_ptr(), function_type::load_array);

							code += va("\t\t%s = %s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
							code += va("\t\t%s(true, (%s));\n", mem_func_load.data(), count_expr->expression().data());
						}
						//else if (member.second.type_ptr()->is_asset())
						//{
						//	auto mem_variable_type = generate_variable_name(member.second.type_ptr(), variable_type::var_ptr);
						//	auto mem_func_load = generate_function_name(member.second.type_ptr(), function_type::load_ptr);

						//	code += va("\t\t%s = &%s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
						//	code += va("\t\t%s(false);\n", mem_func_load.data());
						//}
						else
						{
							auto mem_variable_type = generate_variable_name(member.second.type_ptr(), variable_type::var_ptr);
							auto mem_func_load = generate_function_name(member.second.type_ptr(), function_type::load_ptr);

							code += va("\t\t%s = &%s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
							code += va("\t\t%s(false);\n", mem_func_load.data());
						}
					}
					// if member is a regular field
					else
					{
						auto mem_variable_type = generate_variable_name(member.second.type_ptr(), variable_type::var);
						auto mem_func_load = generate_function_name(member.second.type_ptr(), function_type::load);

						code += va("\t\t%s = &%s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
						code += va("\t\t%s(atStreamStart);\n", mem_func_load.data());
					}

					code += "\t}\n";

					keyword = "else if";
				}

				if (!default_member.first.empty())
				{
					code += "\telse\n";
					code += "\t{\n";

					// if default_member is a pointer
					if (default_member.second.is_pointer())
					{
						if (default_member.second.has_expression(type_expression::expression_type::count_expression))
						{
							const auto count_expr = default_member.second.get_expression(type_expression::expression_type::count_expression);
							count_expr->set_asset(variable_type);

							auto mem_variable_type = generate_variable_name(default_member.second.type_ptr(), variable_type::var);
							auto mem_func_load = generate_function_name(default_member.second.type_ptr(), function_type::load_array);

							code += va("\t\t%s = %s->%s;\n", mem_variable_type.data(), variable_type.data(), default_member.first.data());
							code += va("\t\t%s(true, (%s));\n", mem_func_load.data(), count_expr->expression().data());
						}
						//else if (default_member.second.type_ptr()->is_asset())
						//{
						//	auto mem_variable_type = generate_variable_name(default_member.second.type_ptr(), variable_type::var_ptr);
						//	auto mem_func_load = generate_function_name(default_member.second.type_ptr(), function_type::load_ptr);

						//	code += va("\t\t%s = &%s->%s;\n", mem_variable_type.data(), variable_type.data(), default_member.first.data());
						//	code += va("\t\t%s(false);\n", mem_func_load.data());
						//}
						else
						{
							auto mem_variable_type = generate_variable_name(default_member.second.type_ptr(), variable_type::var_ptr);
							auto mem_func_load = generate_function_name(default_member.second.type_ptr(), function_type::load_ptr);

							code += va("\t\t%s = &%s->%s;\n", mem_variable_type.data(), variable_type.data(), default_member.first.data());
							code += va("\t\t%s(false);\n", mem_func_load.data());
						}
					}
					// if default_member is a regular field
					else
					{
						auto mem_variable_type = generate_variable_name(default_member.second.type_ptr(), variable_type::var);
						auto mem_func_load = generate_function_name(default_member.second.type_ptr(), function_type::load);

						code += va("\t\t%s = &%s->%s;\n", mem_variable_type.data(), variable_type.data(), default_member.first.data());
						code += va("\t\t%s(atStreamStart);\n", mem_func_load.data());
					}

					code += "\t}\n";
				}
			}
			else
			{
				if (!type->has_expression(type_expression::expression_type::enum_field_expression))
				{
					ZONETOOL_WARNING("Can't generate code for union \"%s\" because it has no enum field specified.", type->name().data());
					return;
				}

				auto enum_field_expr = type->get_expression(type_expression::expression_type::enum_field_expression);

				// create switch case
				code += va("\tswitch (%s)\n", enum_field_expr->expression().data());
				code += "\t{\n";

				// loop through all members
				for (auto member : type->members())
				{
					// check if an enum index exists for the current value
					if (!member.second.has_expression(type_expression::expression_type::enum_value_expression))
					{
						ZONETOOL_WARNING("Member %s in %s has no enum value expression, skipping.", member.first.data(), type->name().data());
						continue;
					}

					// insert switch case
					auto enum_val_expr = member.second.get_expression(type_expression::expression_type::enum_value_expression);
					if (enum_val_expr->expression() == "default")
					{
						code += "\tdefault:\n";
					}
					else
					{
						if (enum_val_expr->expression().find(",") != std::string::npos)
						{
							auto values = split(enum_val_expr->expression(), { ',' });
							for (auto& enum_value : values)
							{
								code += va("\tcase %s:\n", enum_value.data());
							}
						}
						else
						{
							code += va("\tcase %s:\n", enum_val_expr->expression().data());
						}
					}

					// if member is a pointer
					if (member.second.is_pointer())
					{
						if (member.second.has_expression(type_expression::expression_type::count_expression))
						{
							const auto count_expr = member.second.get_expression(type_expression::expression_type::count_expression);
							count_expr->set_asset(variable_type);

							auto mem_variable_type = generate_variable_name(member.second.type_ptr(), variable_type::var);
							auto mem_func_load = generate_function_name(member.second.type_ptr(), function_type::load_array);

							code += va("\t\t%s = %s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
							code += va("\t\t%s(true, (%s));\n", mem_func_load.data(), count_expr->expression().data());
						}
						//else if (member.second.type_ptr()->is_asset())
						//{
						//	auto mem_variable_type = generate_variable_name(member.second.type_ptr(), variable_type::var_ptr);
						//	auto mem_func_load = generate_function_name(member.second.type_ptr(), function_type::load_ptr);

						//	code += va("\t\t%s = &%s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
						//	code += va("\t\t%s(false);\n", mem_func_load.data());
						//}
						else
						{
							auto mem_variable_type = generate_variable_name(member.second.type_ptr(), variable_type::var_ptr);
							auto mem_func_load = generate_function_name(member.second.type_ptr(), function_type::load_ptr);

							code += va("\t\t%s = &%s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
							code += va("\t\t%s(false);\n", mem_func_load.data());
						}
					}
					// if member is a regular field
					else
					{
						auto mem_variable_type = generate_variable_name(member.second.type_ptr(), variable_type::var);
						auto mem_func_load = generate_function_name(member.second.type_ptr(), function_type::load);

						code += va("\t\t%s = &%s->%s;\n", mem_variable_type.data(), variable_type.data(), member.first.data());
						code += va("\t\t%s(atStreamStart);\n", mem_func_load.data());
					}

					code += "\t\tbreak;\n";
				}

				code += "\t}\n";
			}
		}
	}

	void code_generator::generate_load_ptr(type* type, const std::string& function_name, std::string& code)
	{
		//int __cdecl Load_XModelPtr(bool a1)
		//{
		//	unsigned int v1; // edi
		//	_DWORD* v2; // esi

		//	Load_Stream(a1, varXModelPtr, 4);
		//	DB_PushStreamPos(0);
		//	v1 = *varXModelPtr;
		//	if (*varXModelPtr)
		//	{
		//		if (v1 < 0xFFFFFFFE)
		//		{
		//			DB_ConvertOffsetToAlias(varXModelPtr);
		//		}
		//		else
		//		{
		//			*varXModelPtr = DB_AllocStreamPos(3);
		//			varXModel = *varXModelPtr;
		//			v2 = 0;
		//			if (v1 == -2)
		//				v2 = DB_InsertPointer();
		//			Load_XModel(1);
		//			Load_XModelAsset(varXModelPtr);
		//			if (v2)
		//				*v2 = *varXModelPtr;
		//		}
		//	}
		//	return DB_PopStreamPos();
		//}

		auto load_func = generate_function_name(type, function_type::load);
		auto allocload_func = generate_function_name(type, function_type::allocload);
		auto variable_type_ptr = generate_variable_name(type, variable_type::var_ptr);
		auto variable_type = generate_variable_name(type, variable_type::var);

		if (type->is_asset() || (type->has_expression(type_expression::expression_type::stream_expression) && 
			(type->get_expression(type_expression::expression_type::stream_expression)->expression() == "0" || 
			type->get_expression(type_expression::expression_type::stream_expression)->expression() == "XFILE_BLOCK_TEMP")))
		{
			code += va("\tLoad_Stream(atStreamStart, %s, sizeof(%s*));\n", variable_type.data(), type->name().data());
			code += "\tDB_PushStreamPos(XFILE_BLOCK_TEMP);\n";
			code += va("\t%s* pointer = *%s;\n", type->name().data(), variable_type_ptr.data());
			code += va("\tif (*%s != nullptr)\n", variable_type_ptr.data());
			code += "\t{\n";
			code += va("\t\tif (*%s == (%s*)0xFFFFFFFF || *%s == (%s*)0xFFFFFFFE)\n", variable_type_ptr.data(), type->name().data(), variable_type_ptr.data(), type->name().data());
			code += "\t\t{\n";
			code += va("\t\t\t*%s = %s();\n", variable_type_ptr.data(), allocload_func.data());
			code += va("\t\t\t%s = *%s;\n", variable_type.data(), variable_type_ptr.data());
			code += va("\t\t\t%s** insertedPointer = nullptr;\n", type->name().data());
			code += va("\t\t\tif (pointer == (%s*)0xFFFFFFFE)\n", type->name().data());
			code += "\t\t\t{\n";
			code += va("\t\t\t\tinsertedPointer = (%s**)DB_InsertPointer();\n", type->name().data());
			code += "\t\t\t}\n";
			code += va("\t\t\t%s(true);\n", load_func.data());
			if (type->is_asset())
			{
				code += va("\t\t\t%sAsset(%s);\n", load_func.data(), variable_type_ptr.data());
			}
			code += "\t\t\tif (insertedPointer != nullptr)\n";
			code += "\t\t\t{\n";
			code += va("\t\t\t\t*insertedPointer = *%s;\n", variable_type_ptr.data());
			code += "\t\t\t}\n";
			code += "\t\t}\n";
			code += "\t\telse\n";
			code += "\t\t{\n";
			code += va("\t\t\tDB_ConvertOffsetToAlias((void**)%s);\n", variable_type_ptr.data());
			code += "\t\t}\n";
			code += "\t}\n";
			code += "\tDB_PopStreamPos();\n";
		}
		else
		{
			//unsigned __int8* __cdecl Load_ExpressionSupportingDataPtr(bool a1)
			//{
			//	unsigned __int8* result; // eax

			//	Load_Stream(a1, varExpressionSupportingDataPtr, 4);
			//	result = varExpressionSupportingDataPtr;
			//	if (*varExpressionSupportingDataPtr)
			//	{
			//		if (*varExpressionSupportingDataPtr == -1)
			//		{
			//			*varExpressionSupportingDataPtr = DB_AllocStreamPos(3);
			//			varExpressionSupportingData = *varExpressionSupportingDataPtr;
			//			result = Load_ExpressionSupportingData(1);
			//		}
			//		else
			//		{
			//			result = DB_ConvertOffsetToPointer(varExpressionSupportingDataPtr);
			//		}
			//	}
			//	return result;
			//}

			code += va("\tLoad_Stream(atStreamStart, %s, sizeof(%s));\n", variable_type.data(), type->name().data());
			code += va("\tif (*%s != nullptr)\n", variable_type_ptr.data());
			code += "\t{\n";

			if (type->is_referenced_by_multiple_structs())
			{
				code += va("\t\tif (*%s == (%s*)0xFFFFFFFF)\n", variable_type_ptr.data(), type->name().data());
				code += "\t\t{\n";
				code += va("\t\t\t*%s = %s();\n", variable_type_ptr.data(), allocload_func.data());
				code += va("\t\t\t%s = *%s;\n", variable_type.data(), variable_type_ptr.data());
				code += va("\t\t\t%s(true);\n", load_func.data());
				code += "\t\t}\n";
				code += "\t\telse\n";
				code += "\t\t{\n";
				code += va("\t\t\tDB_ConvertOffsetToPointer((void**)%s);\n", variable_type_ptr.data());
				code += "\t\t}\n";
			}
			else
			{
				code += va("\t\t*%s = %s();\n", variable_type_ptr.data(), allocload_func.data());
				code += va("\t\t%s = *%s;\n", variable_type.data(), variable_type_ptr.data());
				code += va("\t\t%s(true);\n", load_func.data());
			}

			code += "\t}\n";
		}
	}

	void code_generator::generate_load_array(type* type, const std::string& function_name, std::string& code)
	{
		//int __cdecl Load_snd_alias_tArray(bool a1, int a2)
		//{
		//	int v2; // esi
		//	int result; // eax
		//	unsigned __int8* v4; // edi

		//	v2 = a2;
		//	result = Load_Stream(a1, varsnd_alias_t, 100 * a2);
		//	if (a2 > 0)
		//	{
		//		v4 = varsnd_alias_t;
		//		do
		//		{
		//			varsnd_alias_t = v4;
		//			result = Load_snd_alias_t(0);
		//			v4 += 100;
		//			--v2;
		//		} while (v2);
		//	}
		//	return result;
		//}

		auto load_func = generate_function_name(type, function_type::load);
		auto variable_type = generate_variable_name(type, variable_type::var);

		code += va("\tLoad_Stream(atStreamStart, %s, sizeof(%s) * count);\n", variable_type.data(), type->name().data());
		code += va("\t%s* pointer = %s;\n", type->name().data(), variable_type.data());
		code += "\tfor (int i = 0; i < count; i++)\n";
		code += "\t{\n";
		code += va("\t\t%s = pointer;\n", variable_type.data());
		code += va("\t\t%s(false);\n", load_func.data());
		code += "\t\tpointer++;\n";
		code += "\t}\n";
	}

	void code_generator::generate_load_ptr_array(type* type, const std::string& function_name, std::string& code)
	{
		//int __cdecl Load_XModelPtrArray(bool a1, int a2)
		//{
		//	int v2; // esi
		//	int result; // eax
		//	unsigned __int8* v4; // edi

		//	v2 = a2;
		//	result = Load_Stream(a1, varXModelPtr, 4 * a2);
		//	if (a2 > 0)
		//	{
		//		v4 = varXModelPtr;
		//		do
		//		{
		//			varXModelPtr = v4;
		//			result = Load_XModelPtr(0);
		//			v4 += 4;
		//			--v2;
		//		} while (v2);
		//	}
		//	return result;
		//}

		auto load_ptr_func = generate_function_name(type, function_type::load_ptr);
		auto variable_type_ptr = generate_variable_name(type, variable_type::var_ptr);

		code += va("\tLoad_Stream(atStreamStart, %s, sizeof(%s*) * count);\n", variable_type_ptr.data(), type->name().data());
		code += va("\t%s** pointer = %s;\n", type->name().data(), variable_type_ptr.data());
		code += "\tfor (int i = 0; i < count; i++)\n";
		code += "\t{\n";
		code += va("\t\t%s = pointer;\n", variable_type_ptr.data());
		code += va("\t\t%s(false);\n", load_ptr_func.data());
		code += "\t\tpointer++;\n";
		code += "\t}\n";
	}

	void code_generator::generate_save(type* type, const std::string& function_name, std::string& code)
	{
		code += "\t// todo!\n";
	}
	
	void code_generator::generate_save_ptr(type* type, const std::string& function_name, std::string& code)
	{
		//void __cdecl Save_RawFilePtr(char atStreamPos)
		//{
		//	int v1; // eax

		//	Save_Stream(atStreamPos, varRawFilePtr, 4, &outRawFilePtr);
		//	DB_PushStreamPos(0);
		//	if (*(_DWORD*)varRawFilePtr)
		//	{
		//		*(_DWORD*)outRawFilePtr = DB_FindStreamData(*(void**)varRawFilePtr);
		//		if (!*(_DWORD*)outRawFilePtr)
		//		{
		//			*(_DWORD*)outRawFilePtr = DB_AllocStreamPos(3);
		//			varRawFile = *(RawFile**)varRawFilePtr;
		//			streamRawFile = *(RawFile**)outRawFilePtr;
		//			v1 = sub_44AF80(streamRawFilePtr, (void*)outRawFilePtr);
		//			DB_FindStreamDataOrInsert(*(void**)varRawFilePtr, v1);
		//			Save_RawFile(1);
		//		}
		//	}
		//	DB_PopStreamPos();
		//}

		auto save_type = generate_function_name(type, function_type::save);
		auto allocsave_type = generate_function_name(type, function_type::allocsave);
		auto variable_type_ptr = generate_variable_name(type, variable_type::var_ptr);
		auto variable_type = generate_variable_name(type, variable_type::var);
		auto out_type_ptr = generate_variable_name(type, variable_type::out_ptr);
		auto stream_type_ptr = generate_variable_name(type, variable_type::stream_ptr);
		auto stream_type = generate_variable_name(type, variable_type::stream);

		code += va("\tSave_Stream(atStreamStart, %s, sizeof(%s*), (void**)&%s);\n", variable_type_ptr.data(), type->name().data(), out_type_ptr.data());

		if (type->is_asset())
		{
			code += "\tDB_PushStreamPos(XFILE_BLOCK_TEMP);\n";
			code += va("\tif (*%s)\n", variable_type_ptr.data());
			code += "\t{\n";
			code += va("\t\t*%s = (%s*)DB_FindStreamData(*%s);\n", out_type_ptr.data(), type->name().data(), variable_type_ptr.data());
			code += va("\t\tassert(*%s != (%s*)0xFFFFFFFF);\n", out_type_ptr.data(), type->name().data());
			code += va("\t\tif (!*%s)\n", out_type_ptr.data());
			code += "\t\t{\n";
			code += va("\t\t\t*%s = %s();\n", out_type_ptr.data(), allocsave_type.data());
			code += va("\t\t\t%s = *%s;\n", variable_type.data(), variable_type_ptr.data());
			code += va("\t\t\t%s = *%s;\n", stream_type.data(), out_type_ptr.data());
			code += va("\t\t\tauto v1 = DB_GetStreamOffsetAndSetPointer((void**)%s, (void**)%s);\n", stream_type_ptr.data(), out_type_ptr.data());
			code += va("\t\t\tDB_FindStreamDataOrInsert((void*)*%s, v1);\n", variable_type_ptr.data());
			code += va("\t\t\t%s(true);\n", save_type.data());
			code += "\t\t}\n";
			code += "\t}\n";
			code += "\tDB_PopStreamPos();\n";
		}
		else
		{
			code += va("\tif (*%s)\n", variable_type_ptr.data());
			code += "\t{\n";
			code += va("\t\t*%s = (%s*)DB_FindStreamData(*%s);\n", out_type_ptr.data(), type->name().data(), variable_type_ptr.data());
			code += va("\t\tassert(*%s != (%s*)0xFFFFFFFF);\n", out_type_ptr.data(), type->name().data());
			code += va("\t\tif (!*%s)\n", out_type_ptr.data());
			code += "\t\t{\n";
			code += va("\t\t\t*%s = %s();\n", out_type_ptr.data(), allocsave_type.data());
			code += va("\t\t\t%s = *%s;\n", variable_type.data(), variable_type_ptr.data());
			code += va("\t\t\t%s = *%s;\n", stream_type.data(), out_type_ptr.data());
			code += va("\t\t\tauto v1 = DB_GetStreamOffsetAndSetPointer((void**)%s, (void**)%s);\n", stream_type_ptr.data(), out_type_ptr.data());
			code += va("\t\t\tDB_FindStreamDataOrInsert((void*)*%s, v1);\n", variable_type_ptr.data());
			code += va("\t\t\t%s(true);\n", save_type.data());
			code += "\t\t}\n";			
			code += "\t}\n";

			//unsigned __int8* __cdecl Load_ExpressionSupportingDataPtr(bool a1)
			//{
			//	unsigned __int8* result; // eax

			//	Load_Stream(a1, varExpressionSupportingDataPtr, 4);
			//	result = varExpressionSupportingDataPtr;
			//	if (*varExpressionSupportingDataPtr)
			//	{
			//		if (*varExpressionSupportingDataPtr == -1)
			//		{
			//			*varExpressionSupportingDataPtr = DB_AllocStreamPos(3);
			//			varExpressionSupportingData = *varExpressionSupportingDataPtr;
			//			result = Load_ExpressionSupportingData(1);
			//		}
			//		else
			//		{
			//			result = DB_ConvertOffsetToPointer(varExpressionSupportingDataPtr);
			//		}
			//	}
			//	return result;
			//}
		}
	}

	void code_generator::generate_save_array(type* type, const std::string& function_name, std::string& code)
	{
		//int __cdecl Save_SpeakerMapSourceArray(char a1, int count)
		//{
		//	int result; // eax
		//	int v3; // ebx
		//	int v4; // edi
		//	char* v5; // esi

		//	Save_Stream(a1, dword_655E50, 0x10 * count, &dword_655BAC);
		//	result = count;
		//	v5 = (char*)dword_655E50;
		//	v4 = dword_655B2C;
		//	v3 = dword_655BAC;
		//	if (count > 0)
		//	{
		//		do
		//		{
		//			dword_655E50 = v5;
		//			dword_655B2C = v4;
		//			dword_655BAC = v3;
		//			Save_SpeakerMapSource(0);
		//			v5 += 16;
		//			v4 += 16;
		//			v3 += 16;
		//			--count;
		//		} while (count);
		//	}
		//	return result;
		//}

		auto load_func = generate_function_name(type, function_type::save);
		auto variable_type = generate_variable_name(type, variable_type::var);
		auto out_type = generate_variable_name(type, variable_type::out);
		auto stream_type = generate_variable_name(type, variable_type::stream);

		code += va("\tSave_Stream(atStreamStart, %s, sizeof(%s) * count, (void**)&%s);\n", variable_type.data(), type->name().data(), out_type.data());
		code += "\tfor (int i = 0; i < count; i++)\n";
		code += "\t{\n";
		code += va("\t\t%s(false);\n", load_func.data());
		code += va("\t\t%s++;\n", variable_type.data(), type->name().data());
		code += va("\t\t%s++;\n", out_type.data(), type->name().data());
		code += va("\t\t%s++;\n", stream_type.data(), type->name().data());
		code += "\t}\n";
	}

	void code_generator::generate_save_ptr_array(type* type, const std::string& function_name, std::string& code)
	{
		//void __cdecl Save_menuDef_ptrArray(bool atStreamPos, int count)
		//{
		//	void** v2; // esi
		//	void** v3; // edi
		//	void** v4; // ebx

		//	Save_Stream(atStreamPos, varmenuDef_ptr, 4 * count, &outmenuDef_ptr);
		//	v2 = varmenuDef_ptr;
		//	v3 = streammenuDef_ptr;
		//	v4 = outmenuDef_ptr;
		//	if (count > 0)
		//	{
		//		do
		//		{
		//			varmenuDef_ptr = v2;
		//			streammenuDef_ptr = v3;
		//			outmenuDef_ptr = v4;
		//			Save_menuDef_ptr(0);
		//			++v2;
		//			++v3;
		//			++v4;
		//			--count;
		//		} while (count);
		//	}
		//}

		auto save_ptr_func = generate_function_name(type, function_type::save_ptr);
		auto variable_type_ptr = generate_variable_name(type, variable_type::var_ptr);
		auto out_type_ptr = generate_variable_name(type, variable_type::out_ptr);
		auto stream_type_ptr = generate_variable_name(type, variable_type::stream_ptr);

		code += va("\tSave_Stream(atStreamStart, %s, sizeof(%s*) * count, (void**)&%s);\n", variable_type_ptr.data(), type->name().data(), out_type_ptr.data());
		code += "\tfor (int i = 0; i < count; i++)\n";
		code += "\t{\n";
		code += va("\t\t%s(false);\n", save_ptr_func.data());
		code += va("\t\t%s++;\n", variable_type_ptr.data(), type->name().data());
		code += va("\t\t%s++;\n", out_type_ptr.data(), type->name().data());
		code += va("\t\t%s++;\n", stream_type_ptr.data(), type->name().data());
		code += "\t}\n";
	}

	void code_generator::generate_save_asset(type* type, const std::string& function_name, std::string& code)
	{
		code += "\t// todo!\n";
	}

	void code_generator::generate_load_asset(type* type, const std::string& function_name, std::string& code)
	{
		auto variable_type_ptr = generate_variable_name(type, variable_type::var_ptr);
		code += va("\tDB_AddXAsset(%s, (void**)%s);\n", type->get_asset_type().data(), variable_type_ptr.data());
	}

	void code_generator::generate_allocload(type* type, const std::string& function_name, std::string& code)
	{
		size_t alignment = type->get_alignment();
		if (type->has_expression(type_expression::expression_type::alignment_expression))
		{
			alignment = std::stoi(type->get_expression(type_expression::expression_type::alignment_expression)->expression().data());
		}
		else
		{
			if (type->size() < alignment)
			{
				ZONETOOL_INFO("Type size for type %s is below its alignment, setting alignment to %u.", type->name().data(), type->size());
				alignment = type->size() - 1;

				assert(alignment >= 0);
				if (alignment > 0)
				{
					assert(alignment % 2 != 0);
				}
			}
		}

		code += va("\treturn (%s*)DB_AllocStreamPos(%i);\n", type->name().data(), alignment - 1);
	}

	void code_generator::generate_allocload_ptr(type* type, const std::string& function_name, std::string& code)
	{
		size_t alignment = type->get_alignment();
		if (type->has_expression(type_expression::expression_type::alignment_expression))
		{
			alignment = std::stoi(type->get_expression(type_expression::expression_type::alignment_expression)->expression().data());
		}

		code += va("\treturn (%s**)DB_AllocStreamPos(%i);\n", type->name().data(), alignment - 1);
	}

	void code_generator::generate_name(type* type, const std::string& function_name, std::string& code)
	{
		std::string field("name");
		if (type->has_expression(type_expression::expression_type::name_expression))
		{
			field = type->get_expression(type_expression::expression_type::name_expression)->expression();
		}

		code += va("\treturn (*(%s**)header)->%s;\n", type->name().data(), field.data());
	}

	code_generator::generator_result code_generator::generator(type* type, const function_type function_type, const std::string& function_name, std::string& code)
	{
		// don't generate code for enums
		if (type->is_enum())
		{
			return generator_result::skip;
		}

		// don't generate code for void type
		if (type->name() == "void"s)
		{
			return generator_result::skip;
		}

		// don't generate Load_*Asset functions if we're not handling an asset type
		if ((function_type == function_type::load_asset || function_type == function_type::save_asset) && !type->is_asset())
		{
			return generator_result::skip;
		}

		// don't generate Load_XString function
		if (function_type == function_type::load && (type->name() == "XString" || type->name() == "TempString"))
		{
			return generator_result::skip;
		}

		// don't generate Load_XString function
		if (function_type == function_type::name && (!type->is_asset() || type->name() == "XAssetList"))
		{
			return generator_result::skip;
		}

		switch (function_type)
		{
		case function_type::allocload:
			generate_allocload(type, function_name, code);
			break;
		case function_type::allocload_ptr:
			generate_allocload_ptr(type, function_name, code);
			break;
		case function_type::allocsave:
			generate_allocload(type, function_name, code);
			break;
		case function_type::allocsave_ptr:
			generate_allocload_ptr(type, function_name, code);
			break;
		case function_type::load:
			generate_load(type, function_name, code);
			break;
		case function_type::load_ptr:
			generate_load_ptr(type, function_name, code);
			break;
		case function_type::load_array:
			generate_load_array(type, function_name, code);
			break;
		case function_type::load_ptr_array:
			generate_load_ptr_array(type, function_name, code);
			break;
		case function_type::load_asset:
			generate_load_asset(type, function_name, code);
			break;
		case function_type::save:
			generate_save(type, function_name, code);
			break;
		case function_type::save_ptr:
			generate_save_ptr(type, function_name, code);
			break;
		case function_type::save_array:
			generate_save_array(type, function_name, code);
			break;
		case function_type::save_ptr_array:
			generate_save_ptr_array(type, function_name, code);
			break;
		case function_type::save_asset:
			generate_save_asset(type, function_name, code);
			break;
		case function_type::name:
			generate_name(type, function_name, code);
			break;
		default:
			break;
		}

		return generator_result::generated;
	}

	void code_generator::install_generator(const callback& cb)
	{
		function_generator_ = cb;
	}

	std::string code_generator::generate_variable_name(const type* type, const variable_type variable_type)
	{
		std::string variable_name;

		switch (variable_type)
		{
		case variable_type::var:
			variable_name = va("var%s", type->name().data());
			break;
		case variable_type::var_ptr:
			variable_name = va("var%sPtr", type->name().data());
			break;
		case variable_type::out:
			variable_name = va("out%s", type->name().data());
			break;
		case variable_type::out_ptr:
			variable_name = va("out%sPtr", type->name().data());
			break;
		case variable_type::stream:
			variable_name = va("stream%s", type->name().data());
			break;
		case variable_type::stream_ptr:
			variable_name = va("stream%sPtr", type->name().data());
			break;
		}

		return variable_name;
	}

	std::string code_generator::generate_variable_signature(const type* type, const variable_type variable_type)
	{
		auto variable_name = generate_variable_name(type, variable_type);
		std::string variable_signature;

		switch (variable_type)
		{
		case variable_type::var:
		case variable_type::out:
		case variable_type::stream:
			variable_signature = va("%s* %s;", type->name().data(), variable_name.data());
			break;
		case variable_type::var_ptr:
		case variable_type::out_ptr:
		case variable_type::stream_ptr:
			variable_signature = va("%s** %s;", type->name().data(), variable_name.data());
			break;
		}

		return variable_signature;
	}

	std::string code_generator::generate_function_name(const type* type, const function_type function_type)
	{
		std::string function_name;

		switch (function_type)
		{
		case function_type::save:
			function_name = va("Save_%s", type->name().data());
			break;
		case function_type::save_array:
			function_name = va("Save_%sArray", type->name().data());
			break;
		case function_type::save_ptr:
			function_name = va("Save_%sPtr", type->name().data());
			break;
		case function_type::save_ptr_array:
			function_name = va("Save_%sPtrArray", type->name().data());
			break;
		case function_type::save_asset:
			function_name = va("Save_%sAsset", type->name().data());
			break;
		case function_type::allocsave:
			function_name = va("AllocSave_%s", type->name().data());
			break;
		case function_type::allocsave_ptr:
			function_name = va("AllocSave_%sPtr", type->name().data());
			break;
		case function_type::load:
			function_name = va("Load_%s", type->name().data());
			break;
		case function_type::load_array:
			function_name = va("Load_%sArray", type->name().data());
			break;
		case function_type::load_ptr:
			function_name = va("Load_%sPtr", type->name().data());
			break;
		case function_type::load_ptr_array:
			function_name = va("Load_%sPtrArray", type->name().data());
			break;
		case function_type::load_asset:
			function_name = va("Load_%sAsset", type->name().data());
			break;
		case function_type::allocload:
			function_name = va("AllocLoad_%s", type->name().data());
			break;
		case function_type::allocload_ptr:
			function_name = va("AllocLoad_%sPtr", type->name().data());
			break;
		case function_type::name:
			function_name = va("DB_%sGetName", type->name().data());
			break;
		}

		return function_name;
	}

	std::string code_generator::generate_function_signature(const type* type, const function_type function_type)
	{
		auto function_name = generate_function_name(type, function_type);
		std::string function_sig;

		switch (function_type)
		{
		case function_type::save:
			function_sig = va("void %s(bool atStreamStart)", function_name.data());
			break;
		case function_type::save_array:
			function_sig = va("void %s(bool atStreamStart, int count)", function_name.data());
			break;
		case function_type::save_ptr:
			function_sig = va("void %s(bool atStreamStart)", function_name.data());
			break;
		case function_type::save_ptr_array:
			function_sig = va("void %s(bool atStreamStart, int count)", function_name.data());
			break;
		case function_type::save_asset:
			function_sig = va("void %s(%s** asset)", function_name.data(), type->name().data());
			break;
		case function_type::allocsave:
			function_sig = va("%s* %s()", type->name().data(), function_name.data());
			break;
		case function_type::allocsave_ptr:
			function_sig = va("%s** %s()", type->name().data(), function_name.data());
			break;
		case function_type::load:
			function_sig = va("void %s(bool atStreamStart)", function_name.data());
			break;
		case function_type::load_array:
			function_sig = va("void %s(bool atStreamStart, int count)", function_name.data());
			break;
		case function_type::load_ptr:
			function_sig = va("void %s(bool atStreamStart)", function_name.data());
			break;
		case function_type::load_ptr_array:
			function_sig = va("void %s(bool atStreamStart, int count)", function_name.data());
			break;
		case function_type::load_asset:
			function_sig = va("void %s(%s** asset)", function_name.data(), type->name().data());
			break;
		case function_type::allocload:
			function_sig = va("%s* %s()", type->name().data(), function_name.data());
			break;
		case function_type::allocload_ptr:
			function_sig = va("%s** %s()", type->name().data(), function_name.data());
			break;
		case function_type::name:
			function_sig = va("const char *%s(XAssetHeader* header)", function_name.data(), type->name().data());
			break;
		}

		return function_sig;
	}

	void code_generator::generate()
	{
		for (auto& type : header_.types())
		{
			for (auto i = std::uint32_t(function_type::save); i < std::uint32_t(function_type::max); i++)
			{
				std::string function_name = generate_function_name(type.get(), function_type(i));
				std::string function_sig = generate_function_signature(type.get(), function_type(i));
				std::string code;

				code += function_sig + "\n";
				code += "{\n";

				auto result = function_generator_(type.get(), function_type(i), function_name, code);

				code += "}\n";

				if (result == generator_result::generated)
				{
					generated_code_[function_sig] = code;
				}
			}
		}
	}

	std::string code_generator::get_code(const std::string& _namespace)
	{
		std::string code;

		// insert header
		code += "// ======================================================================\n";
		code += "// Code generated by zonetool-transpiler.\n";
		code += "// Do not modify the contents of this file. Any change will be\n";
		code += "//   overwritten when re-running the tool.\n";
		code += "// \n";
		code += "// \"No matter how hard or unlikely, if it's possible, it will be done.\"\n";
		code += "// ======================================================================\n";
		code += "\n";

		// insert includes
		code += "#include \"stdafx.hpp\"\n";
		code += "#include \"base_types.hpp\"\n";
		code += va("#include \"%s.hpp\"\n", _namespace.data());
		code += "\n";

		// insert headers
		code += va("namespace zonetool::linkers::%s\n", _namespace.data());
		code += "{\n";
		for (auto& generated : generated_code_)
		{
			code += generated.first;
			code += ";\n";
		}
		code += "\n";

		// insert vars
		for (auto& type : header_.types())
		{
			if (type->name() != "XString" && type->name() != "TempString")
			{
				code += generate_variable_signature(type.get(), variable_type::var) + "\n";
			}

			code += generate_variable_signature(type.get(), variable_type::var_ptr) + "\n";
			code += generate_variable_signature(type.get(), variable_type::out) + "\n";
			code += generate_variable_signature(type.get(), variable_type::out_ptr) + "\n";
			code += generate_variable_signature(type.get(), variable_type::stream) + "\n";
			code += generate_variable_signature(type.get(), variable_type::stream_ptr) + "\n";
		}
		code += "\n";

		// insert full functions
		for (auto& generated : generated_code_)
		{
			code += generated.second;
			code += "\n";
		}

		auto asset_list = header_.find_type("XAssetHeader");
		auto asset_types = header_.find_type("XAssetType");

		auto get_asset_for_type = [&](std::string& asset_type) -> zonetool::type*
		{
			for (auto& member : asset_list->members())
			{
				if (member.second.type_ptr()->get_asset_type().ends_with(asset_type))
				{
					return member.second.type_ptr();
				}
			}

			return nullptr;
		};

		code += "const char *(*DB_XAssetGetNameHandler[ASSET_TYPE_COUNT])(XAssetHeader*) = \n";
		code += "{\n";
		for (auto& asset_type : asset_types->enum_members())
		{
			if (asset_type.first == "ASSET_TYPE_COUNT")
			{
				break;
			}

			auto type = get_asset_for_type(asset_type.first);
			if (type)
			{
				auto get_name_func = generate_function_name(type, function_type::name);
				code += va("\t%s,\n", get_name_func.data());
			}
			else
			{
				code += "\tnullptr,\n";
			}
		}
		code += "};\n";

		// end namespace brace
		code += "}\n";

		return code;
	}
}
