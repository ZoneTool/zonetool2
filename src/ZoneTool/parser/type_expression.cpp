#include "stdafx.hpp"
#include "lexical_analyser.hpp"
#include "type_expression.hpp"

namespace zonetool
{
	std::vector<type_expression> type_expression::parse_expressions(lexical_analyser* analyser)
	{
		std::vector<type_expression> expressions;
		
		while (analyser->current() != ";"s)
		{
			if (analyser->current() == "@"s)
			{
				// skip @ token
				analyser->next();
			}

			const auto expression_type = analyser->current();	// expression type

			// verify if the expression is in the following format:
			// @ count_expr : asset->rowCount * asset->columnCount
			
			if (analyser->next() != ":"s)
			{
				ZONETOOL_FATAL("expected \":\" after %s.", expression_type);
			}

			// read past : token
			analyser->next();

			// parse expression type
			if (expression_type == "count_expr"s)
			{
				expressions.emplace_back(expression_type::count_expression, analyser->current());
			}
			else if (expression_type == "align_expr"s)
			{
				expressions.emplace_back(expression_type::alignment_expression, analyser->current());
			}
			else if (expression_type == "stream_expr"s)
			{
				expressions.emplace_back(expression_type::stream_expression, analyser->current());
			}
			else if (expression_type == "ignore_expr"s)
			{
				expressions.emplace_back(expression_type::ignore_expression, analyser->current());
			}
			else if (expression_type == "enum_field_expr"s)
			{
				expressions.emplace_back(expression_type::enum_field_expression, analyser->current());
			}
			else if (expression_type == "enum_val_expr"s || expression_type == "enum_value_expr"s)
			{
				expressions.emplace_back(expression_type::enum_value_expression, analyser->current());
			}
			else if (expression_type == "before_expr"s)
			{
				expressions.emplace_back(expression_type::before_expression, analyser->current());
			}
			else if (expression_type == "after_expr"s)
			{
				expressions.emplace_back(expression_type::after_expression, analyser->current());
			}
			else if (expression_type == "size_expr"s)
			{
				expressions.emplace_back(expression_type::size_expression, analyser->current());
			}
			else if (expression_type == "name_expr"s)
			{
				expressions.emplace_back(expression_type::name_expression, analyser->current());
			}
			else if (expression_type == "use_if_expr"s)
			{
				expressions.emplace_back(expression_type::use_if_expression, analyser->current());
			}
			else
			{
				ZONETOOL_FATAL("invalid expression type %s.", expression_type);
			}

			// read past , token
			if (analyser->next() != ";"s)
			{
				analyser->next();
			}
		}

		return expressions;
	}
}
