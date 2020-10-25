#include "stdafx.hpp"
#include <algorithm>

#include "base_functions.hpp"

#include "zone_pointer.hpp"
#include "binary_reader.hpp"

namespace zonetool
{
	binary_reader::binary_reader(std::uint8_t* buffer, const std::size_t size) : pos_(0), buffer_(buffer), buffer_size_(size)
	{

	}

	void binary_reader::init_streams(const std::uint32_t num_streams)
	{
		// alloc enough streams
		streams_.resize(num_streams);
		memset(&streams_[0], 0, streams_.size() * sizeof(std::uint32_t));
	}

	void binary_reader::load_data(std::uint8_t* buffer, std::size_t size)
	{
		if (pos_ + size > buffer_size_)
		{
			DB_DirtyDiscError();
			return;
		}

		memcpy(buffer, this->at(), size);
		increment_stream(size);
		pos_ += size;
	}

	void binary_reader::seek(std::int32_t size)
	{
		if (pos_ + size > buffer_size_)
		{
			DB_DirtyDiscError();
			return;
		}

		increment_stream(size);
		pos_ += size;
	}

	void binary_reader::increment_stream(std::int32_t size)
	{
		if (!stream_queue_.empty())
		{
			streams_[stream_queue_.top()] += size;
		}
	}

	std::uint8_t* binary_reader::align(std::uint32_t alignment)
	{
		streams_[stream_queue_.top()] = ~alignment & (alignment + streams_[stream_queue_.top()]);
		return &buffer_[pos_];
	}

	void binary_reader::push_stream(std::uint32_t stream)
	{
		stream_queue_.push(stream);
	}

	void binary_reader::pop_stream()
	{
		stream_queue_.pop();
	}

	std::uint8_t* binary_reader::at()
	{
		return &buffer_[pos_];
	}

	void binary_reader::store_pointer(const std::size_t size)
	{
		if (!stream_queue_.empty())
		{
			stored_pointer pointer;

			pointer.zone_ptr = zone_pointer(stream_queue_.top(), streams_[stream_queue_.top()]);
			pointer.file_ptr = this->at();
			pointer.data_size = size;

			// store pointer
			stored_pointers_.emplace_back(pointer);
		}
	}

	std::uint8_t* binary_reader::resolve_pointer(const zone_pointer& pointer)
	{
		// check if exact pointer exists
		const auto itr = std::find_if(stored_pointers_.begin(), stored_pointers_.end(), [pointer](const stored_pointer& stored_ptr)
		{
			if (stored_ptr.zone_ptr.offset_ == pointer.offset_ && stored_ptr.zone_ptr.stream_ == pointer.stream_)
			{
				return true;
			}

			return false;
		});
		if (itr != stored_pointers_.end())
		{
			return itr->file_ptr;
		}

		// fuck it doesn't, find nearest pointer and add difference
		stored_pointer* nearest_pointer = nullptr;
		for (auto& stored_ptr : stored_pointers_)
		{
			if (stored_ptr.zone_ptr.stream_ != pointer.stream_)
			{
				continue;
			}

			if (stored_ptr.zone_ptr.offset_ < pointer.offset_)
			{
				if (pointer.offset_ - stored_ptr.zone_ptr.offset_ < stored_ptr.data_size)
				{
					nearest_pointer = &stored_ptr;
					break;
				}
			}
		}

		if (!nearest_pointer)
		{
			DB_DirtyDiscError();
			return nullptr;
		}

		auto difference = (pointer.offset_ - nearest_pointer->zone_ptr.offset_) - 0x30;
		return nearest_pointer->file_ptr + difference;
	}
}
