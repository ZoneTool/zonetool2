#include "stdafx.hpp"
#include "save.hpp"
#include <algorithm>

#undef min
#undef max

namespace zonetool::code_generator::save
{
	std::string sub_type(type* type);

	void handle_asset_pointer(type* type, std::string& code)
	{
		code += "void* Save_" + type->name() + "Ptr(bool atStreamStart, " + type->name() + "* asset)\n";
		code += "{\n";
		code += "//todo\n";
		code += "}\n\n";
	}

	void handle_asset(type* type, std::string& code)
	{

	}

	void handle_function(type* type, std::string& code)
	{
		for (auto& member : type->members())
		{
			const auto member_type = member.second.type_ptr();

			if (member_type->is_primitive() && member.second.is_pointer() && member_type->name() == "char" && !member.second.has_expression(type_expression::expression_type::count_expression))
			{
				code += "\tif (asset->" + member.first + ")\n";
				code += "\t{\n";
				code += "\t\tDB_AllocStreamPos(0);\n";
				code += "\t\tSave_Stream(true, asset->" + member.first + ", strlen(asset->" + member.first + ") + 1);\n";
				code += "\t\tasset->" + member.first + " = (char*)0xFFFFFFFF;\n";
				code += "\t}\n\n";
			}
			// if its not a pointer but its also not a primitive type, we do want to generate save code for it
			else if (!member.second.is_pointer() && !member_type->is_primitive())
			{
				code = sub_type(member_type) + code;
				code += "\tSave_" + member_type->name() + "(buf, false, &asset->" + member.first + ");\n\n";
			}
			// if its a pointer, but the type is not primitive, we want to generate code
			else if (member.second.is_pointer() && !member_type->is_primitive())
			{
				code = sub_type(member_type) + code;
				code += "\tif (asset->" + member.first + ")\n";
				code += "\t{\n";

				// handle alignment
				if (member.second.has_expression(type_expression::expression_type::alignment_expression))
				{
					const auto align_expr = member.second.get_expression(type_expression::expression_type::alignment_expression);
					code += "\t\tDB_AllocStreamPos((" + align_expr->expression() + ") - 1);\n";
				}
				else
				{
					const auto alignment = std::min(member_type->size(), 4u) - 1;
					code += "\t\tDB_AllocStreamPos(" + std::to_string(alignment) + ");\n";
				}
				
				// handle count expressions
				if (member.second.has_expression(type_expression::expression_type::count_expression))
				{
					const auto count_expr = member.second.get_expression(type_expression::expression_type::count_expression);

					code += "\t\tasset->" + member.first + " = (" + member_type->name() + "*)Save_Stream(true, asset->" + member.first + ", sizeof(" + member_type->name() + ") * (" + count_expr->expression() + "));\n\n";
					code += "\t\tfor (auto " + member.first + "Index = 0u; " + member.first + "Index < " + count_expr->expression() + "; " + member.first + "Index++)\n";
					code += "\t\t{\n";
					code += "\t\t\tSave_" + member_type->name() + "(false, &asset->" + member.first + "[" + member.first + "Index]);\n";
					code += "\t\t}\n\n";
					code += "\t\tasset->" + member.first + " = (" + member_type->name() + "*)0xFFFFFFFF;\n";
				}
				else
				{
					code += "\t\tSave_" + member_type->name() + "(true, asset->" + member.first + ");\n";
					code += "\t\tasset->" + member.first + " = (" + member_type->name() + "*)0xFFFFFFFF;\n";
				}
				
				code += "\t}\n\n";
			}
			// if its a pointer, and its a primitive, directly Save_Stream the data to disk
			else if (member.second.is_pointer() && member_type->is_primitive())
			{
				code += "\tif (asset->" + member.first + ")\n";
				code += "\t{\n";

				// handle alignment
				if (member.second.has_expression(type_expression::expression_type::alignment_expression))
				{
					const auto align_expr = member.second.get_expression(type_expression::expression_type::alignment_expression);
					code += "\t\tDB_AllocStreamPos((" + align_expr->expression() + ") - 1);\n";
				}
				else
				{
					const auto alignment = std::min(member_type->size(), 4u) - 1;
					code += "\t\tDB_AllocStreamPos(" + std::to_string(alignment) + ");\n";
				}

				// handle count expressions
				if (member.second.has_expression(type_expression::expression_type::count_expression))
				{
					const auto count_expr = member.second.get_expression(type_expression::expression_type::count_expression);

					code += "\t\tSave_Stream(true, asset->" + member.first + ", sizeof(" + member_type->name() + ") * (" + count_expr->expression() + "));\n";
					code += "\t\tasset->" + member.first + " = (" + member_type->name() + "*)0xFFFFFFFF;\n";
				}
				else
				{
					code += "\t\tSave_Stream(true, asset->" + member.first + ", sizeof(" + member_type->name() + "));\n";
				}
				
				code += "\t}\n\n";
			}
		}
	}
	
	std::string sub_type(type* type)
	{
		auto code = ""s;

		code += "void Save_" + type->name() + "(bool atStreamStart, " + type->name() + "* asset)\n";
		code += "{\n";

		code += "\tif (atStreamStart)\n";
		code += "\t{\n";
		code += "\t\tasset = (" + type->name() + "*)Save_Stream(atStreamStart, asset, sizeof(" + type->name() + "));\n";
		code += "\t}\n\n";

		handle_function(type, code);
		
		code += "}\n\n";
		
		return code;
	}
	
	std::string asset(type* type)
	{
		auto code = ""s;

		code += "void Save_" + type->name() + "(" + type->name() + "* asset)\n";
		code += "{\n";

		code += "\tasset = (" + type->name() + "*)Save_Stream(true, asset, sizeof(" + type->name() + "));\n";
		code += "\tDB_PushStreamPos(XFILE_BLOCK_VIRTUAL);\n\n";

		handle_function(type, code);

		code += "\tDB_PopStreamPos();\n";
		
		code += "}\n\n";
		
		return code;
	}
}
