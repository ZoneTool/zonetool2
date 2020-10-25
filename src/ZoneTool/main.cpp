#include "stdafx.hpp"
#include "parser/lexical_analyser.hpp"
#include "parser/game_header.hpp"
#include <filesystem>
// #include "chai/chai.hpp"
#include "parser/type_instance.hpp"
#include "code_generator/save.hpp"
#include "code_generator/code_generator.hpp"

int main(int argc, char** argv)
{
	// We'll do arg parsing later on.
	if (argc != 4)
	{
		ZONETOOL_INFO("usage: generator.exe <headerfile.h> <output.cpp> <namespace>");
		return 0;
	}

	auto fp = static_cast<FILE*>(nullptr);

	ZONETOOL_INFO("ZoneTool Transpiler by RektInator.");
	ZONETOOL_INFO("  \"No matter how hard or unlikely, if it's possible, it will be done.\"");
	
	std::vector<char> file_buffer;
	fp = fopen(argv[1], "rb");
	if (fp)
	{
		file_buffer.resize(std::filesystem::file_size(argv[1]));
		fread(&file_buffer[0], 1, file_buffer.size(), fp);
		fclose(fp);
	}
	
	zonetool::game_header header(file_buffer.data(), file_buffer.size());
	
	zonetool::code_generator::code_generator generator(header);
	generator.generate();

	auto code = generator.get_code(argv[3]);
	
	fp = fopen(argv[2], "wb");
	fwrite(code.data(), code.size(), 1, fp);
	fclose(fp);
}
