#include "stdafx.hpp"
#include "lexical_analyser.hpp"

namespace zonetool
{
	lexical_analyser::lexical_analyser(const char* buffer, const std::size_t size) : expression_index_(0), buffer_(buffer), buffer_size_(size)
	{
		const auto memory_size = 1024 * 1024 * 2;
		this->expression_memory_ = static_cast<char*>(VirtualAlloc(nullptr, memory_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
		
		if (!this->expression_memory_)
		{
			throw std::exception("could not allocate memory for expression parser!");
		}

		memset(this->expression_memory_, 0, memory_size);

		this->parse_tree();
	}

	lexical_analyser::~lexical_analyser()
	{
		VirtualFree(this->expression_memory_, 0, MEM_RELEASE);
	}

	const char* lexical_analyser::current()
	{
		if (this->expression_index_ >= this->expression_tree_.size())
		{
			return nullptr;
		}
		
		return this->expression_tree_[this->expression_index_];
	}

	const char* lexical_analyser::next()
	{
		this->expression_index_++;
		return this->current();
	}
	
	void lexical_analyser::register_expression(const char* expression, std::size_t& memory_pos, const std::size_t size)
	{		
		memcpy(&this->expression_memory_[memory_pos], expression, size);
		this->expression_tree_.push_back(&this->expression_memory_[memory_pos]);

		// ZONETOOL_INFO("expression %s", &this->expression_memory_[memory_pos]);
		
		memory_pos += size + 1;
	}
	
	void lexical_analyser::parse_tree()
	{
		auto pos = 0u;
		auto memory_pos = 0u;

		auto current_start = 0u;
		auto current_read = 0u;

		auto parsing_string = false;
		
		while (pos < this->buffer_size_)
		{
			if (this->buffer_[pos] == '"')
			{
				if (!parsing_string)
				{
					if (current_read > 0)
					{
						this->register_expression(&this->buffer_[current_start], memory_pos, current_read);
					}

					pos++;

					current_start = pos;
					current_read = 0;
				}
				else
				{
					this->register_expression(&this->buffer_[current_start], memory_pos, current_read);

					pos++;

					current_start = pos;
					current_read = 0;
				}
				
				parsing_string = !parsing_string;
			}
			else if (!parsing_string && (this->buffer_[pos] == ' ' ||
				this->buffer_[pos] == '\n' ||
				this->buffer_[pos] == '\r' ||
				this->buffer_[pos] == '\t' ||
				this->buffer_[pos] == 0))
			{
				if (current_read > 0)
				{
					this->register_expression(&this->buffer_[current_start], memory_pos, current_read);
				}

				pos++;
				
				current_start = pos;
				current_read = 0;
			}
			else if (!parsing_string && (this->buffer_[pos] == ';' ||
				this->buffer_[pos] == ':' ||
				this->buffer_[pos] == '*' || 
				this->buffer_[pos] == '{' || 
				this->buffer_[pos] == '}' ||
				this->buffer_[pos] == '[' ||
				this->buffer_[pos] == ']' ||
				this->buffer_[pos] == '(' ||
				this->buffer_[pos] == ')' ||
				//this->buffer_[pos] == '<' ||
				//this->buffer_[pos] == '>' ||
				this->buffer_[pos] == ',' ||
				this->buffer_[pos] == '.' || 
				this->buffer_[pos] == '=' || 
				this->buffer_[pos] == '@' || 
				this->buffer_[pos] == '/'))
			{
				if (current_read > 0)
				{
					this->register_expression(&this->buffer_[current_start], memory_pos, current_read);
				}

				this->register_expression(&this->buffer_[pos], memory_pos, 1);

				pos++;

				current_start = pos;
				current_read = 0;
			}
			else
			{
				current_read++;
				pos++;
			}
		}
	}

}
