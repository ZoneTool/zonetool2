#pragma once

namespace zonetool
{
	struct stored_pointer
	{
		zone_pointer zone_ptr;
		std::uint8_t* file_ptr;
		std::size_t data_size;
	};

	class binary_reader
	{
	public:
		binary_reader(std::uint8_t* buffer, const std::size_t size);
		void init_streams(const std::uint32_t num_streams);
		void load_data(std::uint8_t* buffer, std::size_t size);
		void seek(std::int32_t size);
		void increment_stream(std::int32_t size);
		std::uint8_t* align(std::uint32_t alignment);
		void push_stream(std::uint32_t stream);
		void pop_stream();
		std::uint8_t* at();
		void store_pointer(const std::size_t size);
		std::uint8_t* resolve_pointer(const zone_pointer& pointer);

	private:
		std::size_t pos_;
		std::uint8_t* buffer_;
		std::size_t buffer_size_;
		std::vector<std::uint32_t> streams_;
		std::stack<std::uint32_t> stream_queue_;

		std::vector<stored_pointer> stored_pointers_;
	};
}