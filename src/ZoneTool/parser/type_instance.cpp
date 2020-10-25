#include "stdafx.hpp"
#include "type.hpp"
#include "type_member.hpp"
#include "type_instance.hpp"

namespace zonetool
{
	std::size_t type_instance::get_member_offset(const std::string& name)
	{
		auto& members = this->type_->members();
		for (auto& member : members)
		{
			if (member.first == name)
			{
				return member.second.offset();
			}
		}

		ZONETOOL_FATAL("type %s has no member %s.", this->type_->name().data(), name.data());
	}

	std::uint8_t* type_instance::data()
	{
		return this->pointer_;
	}
	type* type_instance::type_ptr()
	{
		return this->type_;
	}
}
