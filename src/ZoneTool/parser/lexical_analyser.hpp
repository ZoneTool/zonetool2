#pragma once

namespace zonetool
{
	class lexical_analyser
	{
	public:
		lexical_analyser(const char* buffer, const std::size_t size);
		~lexical_analyser();

		std::string find_closest_expression(const std::vector<std::string>& expressions);
		std::string take_until(const std::string& str);
		
		const char* current();
		const char* next();
		
	private:
		std::size_t expression_index_;
		std::vector<const char*> expression_tree_;
		char* expression_memory_;

		const char* buffer_;
		std::size_t buffer_size_;

		void register_expression(const char* expression, std::size_t& memory_pos, const std::size_t size);
		void parse_tree();
		
	};
}
