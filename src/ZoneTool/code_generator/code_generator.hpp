#pragma once

namespace zonetool::code_generator
{
	class code_generator
	{
	public:
		enum class generator_result
		{
			skip,
			generated,
			
			max,
			min = skip,
		};
		enum class function_type
		{
			save,
			save_array,
			save_ptr,
			save_ptr_array,
			save_asset,

			allocsave,
			allocsave_ptr,

			load,
			load_array,
			load_ptr,
			load_ptr_array,
			load_asset,

			allocload,
			allocload_ptr,

			name,

			max,
			min = save,
		};
		enum class variable_type
		{
			var,
			var_ptr,
			out,
			out_ptr,
			stream,
			stream_ptr,

			max,
			min = var,
		};
		using callback = std::function<generator_result(type*, const function_type, const std::string&, std::string&)>;

		code_generator(const game_header& header);
		void install_generator(const callback& cb);
		void generate();
		std::string get_code(const std::string& _namespace);

	private:
		std::size_t calculate_align_size(const type* type);

		std::string generate_variable_name(const type* type, const variable_type variable_type);
		std::string generate_variable_signature(const type* type, const variable_type variable_type);
		std::string generate_function_name(const type* type, const function_type function_type);
		std::string generate_function_signature(const type* type, const function_type function_type);

		void generate_load(type* type, const std::string& function_name, std::string& code);
		void generate_load_ptr(type* type, const std::string& function_name, std::string& code);
		void generate_load_array(type* type, const std::string& function_name, std::string& code);
		void generate_load_ptr_array(type* type, const std::string& function_name, std::string& code);
		void generate_load_asset(type* type, const std::string& function_name, std::string& code);

		void generate_allocload(type* type, const std::string& function_name, std::string& code);
		void generate_allocload_ptr(type* type, const std::string& function_name, std::string& code);

		void generate_save(type* type, const std::string& function_name, std::string& code);
		void generate_save_ptr(type* type, const std::string& function_name, std::string& code);
		void generate_save_array(type* type, const std::string& function_name, std::string& code);
		void generate_save_ptr_array(type* type, const std::string& function_name, std::string& code);
		void generate_save_asset(type* type, const std::string& function_name, std::string& code);
		
		void generate_name(type* type, const std::string& function_name, std::string& code);

		generator_result generator(type*, const function_type, const std::string&, std::string&);

		std::map<std::string, std::string> generated_code_;
		callback function_generator_;
		game_header header_;

	};
}
