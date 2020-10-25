#pragma once

void DB_DirtyDiscError();
void Load_Stream(bool atStreamStart, void* data, int count);
void Save_Stream(bool atStreamStart, void* data, int count, void** out);
void DB_ConvertOffsetToPointer(void** pointer);
void DB_ConvertOffsetToAlias(void** pointer);
void DB_PushStreamPos(int stream);
void DB_PopStreamPos();
void* DB_InsertPointer();
void* DB_FindStreamData(void*);
void* sub_44AF80(void**, void**);
void DB_FindStreamDataOrInsert(void*, void*);
void* DB_AllocStreamPos(int alignment);
void Load_XString(bool atStreamStart);
void Load_TempString(bool atStreamStart);
void DB_AddXAsset(int type, void** asset);
