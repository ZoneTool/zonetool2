#include "stdafx.hpp"
#include "type.hpp"
#include <utility>
#include <algorithm>

#undef max

namespace zonetool
{
	type::type(std::string type_name) : type_alignment_(4), type_variant_(type_variant::_struct), is_primitive_(false), type_name_(std::move(type_name)), alias_(nullptr), is_asset_(false), usage_count_(0) { }
	type::type(std::string type_name, const bool is_primitive, const std::size_t size) : type_alignment_(4), type_variant_(type_variant::_struct), is_primitive_(is_primitive), type_name_(std::move(type_name)), type_size_(size), alias_(nullptr), is_asset_(false), usage_count_(0) { }
	type::type(std::string type_name, type* alias_type) : type_alignment_(alias_type->type_alignment_), type_variant_(alias_type->type_variant_), is_primitive_(alias_type->is_primitive_), type_name_(std::move(type_name)), alias_(alias_type), is_asset_(false), usage_count_(0) { }

	void type::set_type_variant(const type::type_variant variant)
	{
		this->type_variant_ = variant;
	}

	bool type::is_union() const
	{
		return this->type_variant_ == type_variant::_union;
	}
	bool type::is_struct() const
	{
		return this->type_variant_ == type_variant::_struct;
	}
	bool type::is_enum() const
	{
		return this->type_variant_ == type_variant::_enum;
	}

	void type::set_alignment(const std::size_t alignment)
	{
		this->type_alignment_ = alignment;
	}

	std::size_t type::get_alignment() const
	{
		return this->type_alignment_;
	}
	
	type::type_variant type::get_type_variant() const
	{
		return this->type_variant_;
	}

	void type::inc_usage_count()
	{
		this->usage_count_++;
	}

	bool type::is_referenced_by_multiple_structs() const
	{
		return this->usage_count_ > 1;
	}

	void type::calculate_size()
	{
		if (this->type_size_.has_value() || this->is_primitive_)
		{
			return;
		}
		
		if (this->type_variant_ == type_variant::_enum)
		{
			this->type_size_ = 4;
			return;
		}
		
		auto offset = 0u;
		for (auto& type : members_)
		{
			if (this->type_variant_ == type_variant::_struct)
			{
				if (offset % type_alignment_ != 0)
				{
					const auto bytes_left = type_alignment_ - (offset % type_alignment_);
					if (type.second.type_ptr()->is_primitive())
					{
						if (bytes_left < type.second.type_ptr()->size())
						{
							// align offset for next member: primitive does not fit in leftover padding
							offset += bytes_left;
						}
					}
					else
					{
						// align offset for next member: non-primitive types never fit in padding
						offset += bytes_left;
					}
				}
				
				// set type offset
				type.second.set_offset(offset);

				// increment offset
				offset += type.second.size();
			}
			else if (this->type_variant_ == type_variant::_union)
			{
				// offsets for unions are always at offset 0
				type.second.set_offset(0);

				// increment size if needed
				offset = std::max(offset, type.second.size());
			}	
		}

		// align struct size
		if (offset % type_alignment_ != 0)
		{
			offset += (type_alignment_ - (offset % type_alignment_));
		}
		
		// set size
		this->type_size_ = offset;
	}
	
	std::size_t type::size()
	{
		if (this->alias_)
		{
			return this->alias_->size();
		}
		
		if (!this->type_size_.has_value())
		{
			this->calculate_size();
		}

		return this->type_size_.value();
	}
	
	std::string type::name() const
	{
		return this->type_name_;
	}
	
	type::member_array& type::members()
	{
		// return alias data if the type is an alias for another type
		if (this->alias_)
		{
			return this->alias_->members();
		}
		
		return this->members_;
	}

	type::enum_array& type::enum_members()
	{
		if (this->alias_)
		{
			return this->alias_->enum_members();
		}

		return this->enum_members_;
	}

	void type::add_enum_member(const std::string& name, int value)
	{
		this->enum_members_.emplace_back(std::make_pair(name, value));
	}

	void type::make_asset()
	{
		this->is_asset_ = true;
	}

	bool type::is_primitive() const
	{
		return this->is_primitive_;
	}
	
	void type::add_member(const std::string& name, type_member member)
	{
		this->members_.emplace_back(name, member);
	}

	bool type::is_asset() const
	{
		return this->is_asset_;
	}

	std::string type::get_asset_type()
	{
		return this->asset_type_;
	}

	void type::set_asset_type(const std::string& type)
	{
		this->asset_type_ = type;
	}

	void type::set_expressions(const std::vector<type_expression>& expressions)
	{
		expressions_ = expressions;
	}

	type_expression* type::get_expression(const type_expression::expression_type expression_type)
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
	bool type::has_expression(const type_expression::expression_type expression_type)
	{
		return get_expression(expression_type) != nullptr;
	}

	void type::set_members(const member_array& members)
	{
		this->members_ = members;
	}
}
