#pragma once
#include <optional>
#include "type_expression.hpp"

namespace zonetool
{
	class type;
	class type_member
	{
	public:
		type_member() = default;
		type_member(const type_member& other) = default; // : size_(other.size_), offset_(other.offset_), is_pointer_(other.is_pointer_), is_array_(other.is_array_), array_size_(other.array_size_), type_(other.type_) {}
		type_member(type* type, const std::size_t pointer_depth) : pointer_depth_(pointer_depth), is_array_(false), array_size_(0), type_(type) {}
		type_member(type* type, const std::size_t pointer_depth, const std::vector<std::size_t> array_size) : pointer_depth_(pointer_depth), is_array_(true), array_size_(array_size), type_(type) {}
		
		type* type_ptr();
		bool is_pointer();
		std::size_t pointer_depth();
		void set_offset(const std::size_t offset);

		void calculate_size();

		void set_expressions(const std::vector<type_expression>& expressions);
		type_expression* get_expression(const type_expression::expression_type expression_type);
		bool has_expression(const type_expression::expression_type expression_type);

		std::size_t get_array_total_size();
		
		std::size_t size();
		std::size_t offset();

		[[nodiscard]] bool is_array() const;
		[[nodiscard]] std::size_t array_depth() const;
		[[nodiscard]] std::size_t array_size(std::size_t index = 0) const;
		
	private:
		std::optional<std::size_t> size_;
		std::optional<std::size_t> offset_;

		std::size_t pointer_depth_;
		bool is_array_;
		std::vector<std::size_t> array_size_;
		type* type_;

		std::vector<type_expression> expressions_;
		
	};
}
