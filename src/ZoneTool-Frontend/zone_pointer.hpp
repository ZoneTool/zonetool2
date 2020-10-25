#pragma once

namespace zonetool
{
	class zone_pointer
	{
	public:
		zone_pointer() : packed_(0) {}
		zone_pointer(const std::uint32_t packed) : packed_(packed) {}
		zone_pointer(const std::uint32_t stream, const std::uint32_t offset) : stream_(stream), offset_(offset + 1) {}
		zone_pointer(const zone_pointer& other) : packed_(other.packed_) {}

		union
		{
			struct
			{
				std::uint32_t offset_ : 28;
				std::uint32_t stream_ : 4;
			};
			std::uint32_t packed_;
		};

	private:


	};
}
