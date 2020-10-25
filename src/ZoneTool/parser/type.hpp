#pragma once
#include <unordered_map>
#include "type_member.hpp"
#include <optional>
#include <memory>

namespace zonetool
{
	class type
	{
	public:
		enum class type_variant : std::uint8_t
		{
			_struct,
			_union,
			_enum,
		};
		
		using member_array = std::vector<std::pair<std::string, type_member>>;
		using enum_array = std::vector<std::pair<std::string, std::uint32_t>>;
		
		type(std::string type_name);
		type(std::string type_name, bool is_primitive, std::size_t size);
		type(std::string type_name, type* alias_type);

		void inc_usage_count();
		bool is_referenced_by_multiple_structs() const;

		void calculate_size();
		type_variant get_type_variant() const;
		void set_type_variant(const type_variant variant);

		std::string get_asset_type();
		void set_asset_type(const std::string& type);
		void make_asset();
		std::size_t get_alignment() const;
		void set_alignment(const std::size_t alignment);
		
		[[nodiscard]] bool is_primitive() const;
		std::size_t size();
		[[nodiscard]] std::string name() const;
		[[nodiscard]] bool is_asset() const;
		[[nodiscard]] bool is_union() const;
		[[nodiscard]] bool is_struct() const;
		[[nodiscard]] bool is_enum() const;

		void add_enum_member(const std::string& name, int value);
		void add_member(const std::string& name, type_member member);

		type_member* get_member(const std::string& name)
		{
			for (auto& member : members_)
			{
				if (member.first == name)
				{
					return &member.second;
				}
			}

			return nullptr;
		}
		bool has_member(const std::string& name)
		{
			return get_member(name) != nullptr;
		}
		
		member_array& members();
		enum_array& enum_members();

		void set_expressions(const std::vector<type_expression>& expressions);
		type_expression* get_expression(const type_expression::expression_type expression_type);
		bool has_expression(const type_expression::expression_type expression_type);
		
		void set_members(const member_array& members);

	private:
		std::size_t type_alignment_;
		type_variant type_variant_;
		bool is_primitive_;
		std::string type_name_;
		member_array members_;
		enum_array enum_members_;
		std::optional<std::size_t> type_size_;
		type* alias_;
		bool is_asset_;
		std::string asset_type_;
		std::uint32_t usage_count_;
		std::vector<type_expression> expressions_;

	};
}
