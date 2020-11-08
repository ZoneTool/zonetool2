#include "stdafx.hpp"
#include "base_types.hpp"
#include "zone_pointer.hpp"
#include "binary_reader.hpp"
#include "linkers/iw4.hpp"


extern zonetool::binary_reader* reader;

namespace zonetool::linkers::iw4
{
	extern const char* (*DB_XAssetGetNameHandler[ASSET_TYPE_COUNT])(XAssetHeader*);
}

void log_to_file(const std::string& log)
{
	static auto* fp = fopen("loading-zt.txt", "a");
	fprintf(fp, "%s\n", log.data());
	fflush(fp);

	// ZONETOOL_INFO("%s", log.data());
}

void DB_DirtyDiscError()
{
	log_to_file(va("DB_DirtyDiscError: Disc Read Error."));

	printf("Disc Read Error.\n");
	__debugbreak();
}

void Load_Stream(bool atStreamStart, void* data, int count)
{
	if (atStreamStart)
	{
		log_to_file(va("Load_Stream: Reading %u bytes.", count));
		reader->store_pointer(static_cast<std::size_t>(count) + 0x30);
		reader->seek(static_cast<std::size_t>(count));
	}
}

void Load_XStringCustom(XString* str)
{
	const char* s = *str;
	do
		reader->load_data((std::uint8_t*)s, 1);
	while (*s++ != 0);
}

void Load_XString(bool atStreamStart)
{
	if (*varXString)
	{
		if (*varXString == (XString)0xFFFFFFFF)
		{
			*varXString = (XString)DB_AllocStreamPos(0);
			Load_Stream(true, *varXString, strlen(*varXString) + 1);
			// Load_XStringCustom(varXString);
		}
		else
		{
			DB_ConvertOffsetToPointer((void**)varXString);
		}
	}
}

void Load_TempStringCustom(TempString* str)
{
	Load_XStringCustom((XString*)str);
	// Might need SL_GetString later for ScriptString's
}

void Load_TempString(bool atStreamStart)
{
	if (*varTempString)
	{
		if (*varTempString == (XString)0xFFFFFFFF)
		{
			*varTempString = (XString)DB_AllocStreamPos(0);
			Load_Stream(true, *varTempString, strlen(*varTempString) + 1);
			// Load_TempStringCustom(varTempString);
		}
		else
		{
			DB_ConvertOffsetToPointer((void**)varTempString);
		}
	}
}

void Save_Stream(bool atStreamStart, void* data, int count, void** out)
{

}

void DB_ConvertOffsetToPointer(void** pointer)
{
	log_to_file(va("DB_ConvertOffsetToPointer: Converting pointer 0x%08X", *pointer));
	*pointer = reader->resolve_pointer(std::uint32_t(*pointer));
}

void DB_ConvertOffsetToAlias(void** pointer)
{
	log_to_file(va("DB_ConvertOffsetToAlias: Converting pointer 0x%08X", *pointer));
	*pointer = reader->resolve_pointer(std::uint32_t(*pointer));
}

void DB_PushStreamPos(int stream)
{
	log_to_file(va("DB_PushStreamPos: Setting stream to %u", stream));
	reader->push_stream(static_cast<std::uint32_t>(stream));
}

void DB_PopStreamPos()
{
	log_to_file(va("DB_PopStreamPos: Popped stream"));
	reader->pop_stream();
}

std::uint8_t* DB_GetStreamPos()
{
	return reader->at();
}

void DB_IncStreamPos(int size)
{
	reader->increment_stream(size);
}

// TODO: this should be void** DB_InsertPointer()
void* DB_InsertPointer()
{
	log_to_file(va("DB_InsertPointer: Inserting pointer"));

	//return nullptr;
	// TODO: 3 should not be hardcoded
	reader->push_stream(3);
	void *pos = DB_AllocStreamPos(3);
	DB_IncStreamPos(4);
	reader->pop_stream();
	return pos;
}

void* DB_FindStreamData(void*)
{
	return nullptr;
}

void* sub_44AF80(void**, void**)
{
	return nullptr;
}

void DB_FindStreamDataOrInsert(void*, void*)
{

}

void* DB_AllocStreamPos(int alignment)
{
	assert(alignment >= 0);

	if (alignment > 0)
	{
		assert(alignment % 2 != 0);
	}

	log_to_file(va("DB_AllocStreamPos: Aligning buffer by %i", alignment));
	return reader->align(static_cast<std::uint32_t>(alignment));
}

const char* g_assetNames[] =
{
	"physpreset",
	"phys_collmap",
	"xanim",
	"xmodelsurfs",
	"xmodel",
	"material",
	"pixelshader",
	"vertexshader",
	"vertexdecl",
	"techset",
	"image",
	"sound",
	"sndcurve",
	"loaded_sound",
	"col_map_sp",
	"col_map_mp",
	"com_map",
	"game_map_sp",
	"game_map_mp",
	"map_ents",
	"fx_map",
	"gfx_map",
	"lightdef",
	"ui_map",
	"font",
	"menufile",
	"menu",
	"localize",
	"weapon",
	"snddriverglobals",
	"fx",
	"impactfx",
	"aitype",
	"mptype",
	"character",
	"xmodelalias",
	"rawfile",
	"stringtable",
	"leaderboarddef",
	"structureddatadef",
	"tracer",
	"vehicle",
	"addon_map_ents",
};

void DB_AddXAsset(int type, void** asset)
{
	auto asset_name = zonetool::linkers::iw4::DB_XAssetGetNameHandler[type]((zonetool::linkers::iw4::XAssetHeader*)asset);
	printf("Loaded asset \"%s\" of type \'%s\'.\n", asset_name, g_assetNames[type]);
}
