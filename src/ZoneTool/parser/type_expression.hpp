#pragma once
#include <utility>
#include <vector>

namespace zonetool
{
	class lexical_analyser;
	class type_expression
	{
	public:
		enum class expression_type
		{
			alignment_expression,
			count_expression,
			stream_expression,
			ignore_expression,
			enum_field_expression,
			enum_value_expression,
			before_expression,
			after_expression,
			size_expression,
			name_expression,
			use_if_expression,
		};
		
		type_expression(const expression_type type, std::string expression) : type_(type), expression_(std::move(expression)) { }
		
		static std::vector<type_expression> parse_expressions(lexical_analyser* analyser);

		[[nodiscard]] expression_type type() const
		{
			return type_;
		}

		[[nodiscard]] std::string expression() const
		{
			return expression_;
		}

		void set_asset(const std::string& asset)
		{
			replace(expression_, "{asset}", asset);
		}
		
	private:
		expression_type type_;
		std::string expression_;
		
	};
}
