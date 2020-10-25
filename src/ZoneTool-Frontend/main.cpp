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

    auto fp = fopen(argv[1], "rb");
    if (fp)
    {
        auto size = std::filesystem::file_size(argv[1]);
        buffer.resize(size);

        fread(&buffer[0], size, 1, fp);
        fclose(fp);
    }

    reader = new zonetool::binary_reader(buffer.data(), buffer.size());

    XFile xfile;
    reader->load_data((std::uint8_t*)&xfile, sizeof xfile);
    reader->init_streams(8);

    reader->push_stream(0);
    zonetool::linkers::iw4::varXAssetList = (zonetool::linkers::iw4::XAssetList*)reader->align(3);
    zonetool::linkers::iw4::Load_XAssetList(true);
    reader->pop_stream();

    return 0;
}
