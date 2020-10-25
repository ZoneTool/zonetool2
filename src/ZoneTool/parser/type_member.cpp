#include "stdafx.hpp"
#include "type.hpp"
#include "type_member.hpp"

namespace zonetool
{
	std::size_t type_member::get_array_total_size()
	{
		if (array_size_.empty())
		{
			return 0;
		}

		auto size = array_size_[0];

		if (array_size_.size() > 1)
		{
			for (auto i = 1; i < array_size_.size(); i++)
			{
				size *= array_size_[i];
			}
		}

		return size;
	}
	void type_member::calculate_size()
	{
		if (this->size_.has_value())
		{
			return;
		}
		
		if (this->is_pointer())
		{
			this->size_ = sizeof std::uintptr_t;
		}
		else
		{
			this->size_ = this->type_ptr()->size();
		}

		if (this->is_array_)
		{
			this->size_ = this->size_.value() * get_array_total_size();
		}
	}
	std::size_t type_member::size()
	{
		if (!this->size_.has_value())
		{
			this->calculate_size();
		}

		return this->size_.value();
	}
	std::size_t type_member::offset()
	{
		if (!this->size_.has_value())
		{
			return 0;
		}

		return this->offset_.value();
	}
	type* type_member::type_ptr()
	{
		return this->type_;
	}
	bool type_member::is_pointer()
	{
		return this->pointer_depth_ >= 1;
	}
	std::size_t type_member::pointer_depth()
	{
		return this->pointer_depth_;
	}
	void type_member::set_offset(const std::size_t offset)
	{
		this->offset_ = offset;
	}
	bool type_member::is_array() const
	{
		return this->is_array_;
	}
	std::size_t type_member::array_depth() const
	{
		return this->array_size_.size();
	}
	std::size_t type_member::array_size(std::size_t index) const
	{
		return this->array_size_[index];
	}
	void type_member::set_expressions(const std::vector<type_expression>& expressions)
	{
		expressions_ = expressions;
	}
	type_expression* type_member::get_expression(const type_expression::expression_type expression_type)
	{
		for (auto& expression : expressions_)
		{
			if (expression.type() == expression_type)
			{
				return &expression;
			}
		}

		return nullptr;
	}
	bool type_member::has_expression(const type_expression::expression_type expression_type)
	{
		return get_expression(expression_type) != nullptr;
	}
}
