#pragma once

namespace zonetool
{
	class type;

	class type_instance
	{
	public:
		type_instance(type* type, std::uint8_t* pointer) : type_(type), pointer_(pointer) {}

		std::size_t get_member_offset(const std::string& member);
		std::uint8_t* data();
		type* type_ptr();

		template <typename T> T get_member_value(const std::string& member)
		{
			const auto offset = get_member_offset(member);
			return *reinterpret_cast<T*>(data() + offset);
		}

		template <typename T> T cast_to()
		{
			T asset_struct = {};

			if (type_->size() > sizeof T)
			{
				ZONETOOL_FATAL("invalid cast");
			}

			memcpy(&asset_struct, this->data(), type_->size());

			return asset_struct;
		}

		
	private:
		type* type_;
		std::uint8_t* pointer_;
		
	};
}
