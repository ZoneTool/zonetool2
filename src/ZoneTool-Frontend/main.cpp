#include "stdafx.hpp"
#include "zone_pointer.hpp"
#include "binary_reader.hpp"
#include "linkers/iw4.hpp"
#include <filesystem>

namespace zonetool::linkers::iw4
{
    void Load_XAssetList(bool atStreamStart);
    extern XAssetList* varXAssetList;
}

zonetool::binary_reader* reader;

struct XFile
{
    int size;
    int externalSize;
    int blockSize[8];
};

int main(int argc, char** argv)
{
    std::vector<std::uint8_t> buffer;

    ZONETOOL_INFO("ZoneTool Transpiler by RektInator.");
    ZONETOOL_INFO("  \"No matter how hard or unlikely, if it's possible, it will be done.\"");

    auto fastfile = ""s;
	
    if (argc == 2)
    {
        fastfile = argv[1];
    }
    else
    {
    	ZONETOOL_INFO("Enter the path to the fastfile you want to load:");
    	printf("fastfile: ");
	    std::getline(std::cin, fastfile);
    }
	
    auto* fp = fopen(fastfile.data(), "rb");
    if (fp)
    {
        const auto size = std::filesystem::file_size(fastfile);
        buffer.resize(size);

        fread(&buffer[0], size, 1, fp);
        fclose(fp);
    }
    else
    {
	    ZONETOOL_ERROR("Fastfile \"%s\" not found!", fastfile.data());
    	return 1;
    }

    reader = new zonetool::binary_reader(buffer.data(), buffer.size());

    XFile xfile = {};
    reader->load_data(reinterpret_cast<std::uint8_t*>(&xfile), sizeof xfile);
    reader->init_streams(8);

    reader->push_stream(0);
    zonetool::linkers::iw4::varXAssetList = reinterpret_cast<zonetool::linkers::iw4::XAssetList*>(reader->align(3));
    zonetool::linkers::iw4::Load_XAssetList(true);
    reader->pop_stream();

    delete reader;
	
    return 0;
}
