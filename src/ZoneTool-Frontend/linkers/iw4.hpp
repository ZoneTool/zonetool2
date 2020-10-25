#pragma once
#include "base_types.hpp"

namespace zonetool::linkers::iw4
{
	enum XFileBlock
	{
		XFILE_BLOCK_TEMP = 0x0,
		XFILE_BLOCK_PHYSICAL = 0x1,
		XFILE_BLOCK_RUNTIME = 0x2,
		XFILE_BLOCK_VIRTUAL = 0x3,
		XFILE_BLOCK_LARGE = 0x4,
		XFILE_BLOCK_CALLBACK = 0x5,
		XFILE_BLOCK_VERTEX = 0x6,
		XFILE_BLOCK_INDEX = 0x7,
		MAX_XFILE_COUNT = 0x8,
	};

	struct vec1_t
	{
		float vec[1];
	};
	struct vec2_t
	{
		float vec[2];
	};
	struct vec3_t
	{
		float vec[3];
	};
	struct vec4_t
	{
		float vec[3];
	};

	struct Bounds
	{
		float midPoint[3];
		float halfSize[3];
	};

	struct cplane_s
	{
		float normal[3];
		float dist;
		char type;
		char pad[3];
	};

	struct cbrush_t;
	struct BrushWrapper;

	struct DObjAnimMat
	{
		float quat[4];
		float trans[3];
		float transWeight;
	};

	struct XSurfaceVertexInfo
	{
		__int16 vertCount[4];
		unsigned __int16* vertsBlend;			//@ count_expr: "{asset}->vertCount[0] + ({asset}->vertCount[1] * 3) + ({asset}->vertCount[2] * 5) + ({asset}->vertCount[3] * 7)";
	};

	union GfxColor
	{
		unsigned int packed;
		char array[4];
	};

	union PackedTexCoords
	{
		unsigned int packed;
	};

	union PackedUnitVec
	{
		unsigned int packed;
		char array[4];
	};

	struct __declspec(align(16)) GfxPackedVertex
	{
		float xyz[3];
		float binormalSign;
		GfxColor color;
		PackedTexCoords texCoord;
		PackedUnitVec normal;
		PackedUnitVec tangent;
	};

	struct XSurfaceCollisionAabb
	{
		unsigned __int16 mins[3];
		unsigned __int16 maxs[3];
	};

	struct __declspec(align(16)) XSurfaceCollisionNode
	{
		XSurfaceCollisionAabb aabb;
		unsigned __int16 childBeginIndex;
		unsigned __int16 childCount;
	};

	struct __declspec(align(2)) XSurfaceCollisionLeaf
	{
		unsigned __int16 triangleBeginIndex;
	};

	struct XSurfaceCollisionTree
	{
		float trans[3];
		float scale[3];
		unsigned int nodeCount;
		XSurfaceCollisionNode* nodes;
		unsigned int leafCount;
		XSurfaceCollisionLeaf* leafs;
	};

	struct XRigidVertList
	{
		unsigned __int16 boneOffset;
		unsigned __int16 vertCount;
		unsigned __int16 triOffset;
		unsigned __int16 triCount;
		XSurfaceCollisionTree* collisionTree;
	};

	struct TriIndice									//@ align_expr: "16";
	{
		unsigned short v1;
		unsigned short v2;
		unsigned short v3;
	};

	struct XSurface
	{
		char tileMode;
		bool deformed;
		unsigned __int16 vertCount;
		unsigned __int16 triCount;
		char zoneHandle;
		unsigned __int16 baseTriIndex;
		unsigned __int16 baseVertIndex;
		TriIndice* triIndices;							//@ stream_expr: "XFILE_BLOCK_INDEX" @ after_expr: "vertList" @ count_expr: "{asset}->triCount";
		XSurfaceVertexInfo vertInfo;
		GfxPackedVertex* verts0;						//@ stream_expr: "XFILE_BLOCK_VERTEX" @ count_expr: "{asset}->vertCount";
		unsigned int vertListCount;
		XRigidVertList* vertList;						//@ count_expr: "{asset}->vertListCount";
		int partBits[6];
	};

	struct XModelSurfs
	{
		XString name;
		XSurface* surfs;
		unsigned __int16 numsurfs;
		int partBits[6];
	};

	struct XModelLodInfo
	{
		float dist;
		unsigned __int16 numsurfs;
		unsigned __int16 surfIndex;
		XModelSurfs* modelSurfs;
		int partBits[6];
		XSurface* surfs;								//@ ignore_expr: "";
		char lod;
		char smcBaseIndexPlusOne;
		char smcSubIndexMask;
		char smcBucket;
	};

	struct XModelCollTri_s
	{
		float plane[4];
		float svec[4];
		float tvec[4];
	};

	struct XModelCollSurf_s
	{
		XModelCollTri_s* collTris;
		int numCollTris;
		Bounds bounds;
		int boneIdx;
		int contents;
		int surfFlags;
	};

	struct XBoneInfo
	{
		Bounds bounds;
		float radiusSquared;
	};

	union XAnimIndices
	{
		char* _1;
		unsigned __int16* _2;
		void* data;
	};

	struct XAnimNotifyInfo
	{
		unsigned __int16 name;
		float time;
	};

	union XAnimDynamicFrames
	{
		char(*_1)[3];
		unsigned __int16(*_2)[3];
	};

	union XAnimDynamicIndices
	{
		char _1[1];
		unsigned __int16 _2[1];
	};

	struct __declspec(align(4)) XAnimPartTransFrames
	{
		float mins[3];
		float size[3];
		XAnimDynamicFrames frames;
		XAnimDynamicIndices indices;
	};

	union XAnimPartTransData
	{
		XAnimPartTransFrames frames;
		float frame0[3];
	};
	
	struct XAnimPartTrans
	{
		unsigned __int16 size;
		char smallTrans;
		XAnimPartTransData u;
	};

	struct __declspec(align(4)) XAnimDeltaPartQuatDataFrames2
	{
		int *frames;
		XAnimDynamicIndices indices;
	};

	union XAnimDeltaPartQuatData2
	{
		XAnimDeltaPartQuatDataFrames2 frames;
		int frame0;
	};

	struct XAnimDeltaPartQuat2
	{
		unsigned __int16 size;
		XAnimDeltaPartQuatData2 u;
	};

	struct __declspec(align(4)) XAnimDeltaPartQuatDataFrames
	{
		int64 *frames4;
		XAnimDynamicIndices indices;
	};

	union XAnimDeltaPartQuatData
	{
		XAnimDeltaPartQuatDataFrames frames;
		__int16 frame0[4];
	};

	struct XAnimDeltaPartQuat
	{
		unsigned __int16 size;
		XAnimDeltaPartQuatData u;
	};


	struct XAnimDeltaPart
	{
		XAnimPartTrans* trans;
		XAnimDeltaPartQuat2* quat2;
		XAnimDeltaPartQuat* quat;
	};

	struct XAnimParts
	{
		XString name;
		unsigned __int16 dataByteCount;
		unsigned __int16 dataShortCount;
		unsigned __int16 dataIntCount;
		unsigned __int16 randomDataByteCount;
		unsigned __int16 randomDataIntCount;
		unsigned __int16 numframes;
		char flags;
		char boneCount[10];
		char notifyCount;
		char assetType;
		bool isDefault;
		unsigned int randomDataShortCount;
		unsigned int indexCount;
		float framerate;
		float frequency;
		unsigned __int16* names;							//@ count_expr: "{asset}->boneCount[9]";
		char* dataByte;
		__int16* dataShort;
		int* dataInt;
		__int16* randomDataShort;
		char* randomDataByte;
		int* randomDataInt;
		XAnimIndices indices;
		XAnimNotifyInfo* notify;							//@ count_expr: "{asset}->notifyCount" @ before_expr: "dataByte";
		XAnimDeltaPart* deltaPart;						//@ before_expr: "dataByte";
	};
	
	struct __declspec(align(4)) PhysPreset
	{
		XString name;
		int type;
		float mass;
		float bounce;
		float friction;
		float bulletForceScale;
		float explosiveForceScale;
		XString sndAliasPrefix;
		float piecesSpreadFraction;
		float piecesUpwardVelocity;
		bool tempDefaultToCylinder;
		bool perSurfaceSndAlias;
	};

	struct PhysGeomInfo
	{
		BrushWrapper* brushWrapper;
		int type;
		float orientation[3][3];
		Bounds bounds;
	};

	struct PhysMass
	{
		float centerOfMass[3];
		float momentsOfInertia[3];
		float productsOfInertia[3];
	};

	struct PhysCollmap
	{
		XString name;
		unsigned int count;
		PhysGeomInfo* geoms;
		PhysMass mass;
		Bounds bounds;
	};

	union GfxDrawSurf
	{
		unsigned __int64 packed;
	};

	struct __declspec(align(4)) MaterialInfo
	{
		XString name;
		char gameFlags;
		char sortKey;
		char textureAtlasRowCount;
		char textureAtlasColumnCount;
		GfxDrawSurf drawSurf;
		unsigned int surfaceTypeBits;
		unsigned __int16 hashIndex;
	};

	struct __declspec(align(2)) MaterialStreamRouting
	{
		char source;
		char dest;
	};

	struct MaterialVertexStreamRouting
	{
		MaterialStreamRouting data[13];
		unsigned int* decl[16];
	};

	struct MaterialVertexDeclaration
	{
		XString name;
		char streamCount;
		bool hasOptionalSource;
		MaterialVertexStreamRouting routing;
	};

	struct GfxVertexShaderLoadDef
	{
		unsigned int* program;						//@ count_expr: "{asset}->programSize";
		unsigned __int16 programSize;
		unsigned __int16 loadForRenderer;
	};

	struct MaterialVertexShaderProgram
	{
		unsigned int* vs;							//@ ignore_expr: "";
		GfxVertexShaderLoadDef loadDef;
	};

	struct MaterialVertexShader
	{
		XString name;
		MaterialVertexShaderProgram prog;
	};

	struct GfxPixelShaderLoadDef
	{
		int* program;								//@ count_expr: "{asset}->programSize";
		unsigned __int16 programSize;
		unsigned __int16 loadForRenderer;
	};

	struct MaterialPixelShaderProgram
	{
		int* ps;									//@ ignore_expr: "";
		GfxPixelShaderLoadDef loadDef;
	};

	struct MaterialPixelShader
	{
		XString name;
		MaterialPixelShaderProgram prog;
	};

	struct MaterialArgumentCodeConst
	{
		unsigned __int16 index;
		char firstRow;
		char rowCount;
	};

	union MaterialArgumentDef						//@ enum_field_expr: "varMaterialShaderArgument->type";
	{
		float* literalConst;						//@ count_expr: "4" @ enum_val_expr: "1,7";
		MaterialArgumentCodeConst codeConst;
		unsigned int codeSampler;
		unsigned int nameHash;
	};

	struct MaterialShaderArgument
	{
		unsigned __int16 type;
		unsigned __int16 dest;
		MaterialArgumentDef u;
	};

	struct MaterialPass
	{
		MaterialVertexDeclaration* vertexDecl;
		MaterialVertexShader* vertexShader;
		MaterialPixelShader* pixelShader;
		char perPrimArgCount;
		char perObjArgCount;
		char stableArgCount;
		char customSamplerFlags;
		MaterialShaderArgument* args;				//@ count_expr: "{asset}->perPrimArgCount + {asset}->perObjArgCount + {asset}->stableArgCount";
	};

	struct MaterialTechnique						//@ size_expr: "sizeof(MaterialTechnique) - sizeof(MaterialPass)";
	{
		XString name;								//@ after_expr: "passArray";
		unsigned __int16 flags;
		unsigned __int16 passCount;
		MaterialPass passArray[1];					//@ count_expr: "{asset}->passCount";
	};

	struct MaterialTechniqueSet
	{
		XString name;
		char worldVertFormat;
		bool hasBeenUploaded;
		char unused[1];
		MaterialTechniqueSet* remappedTechniqueSet;		//@ ignore_expr: "";
		MaterialTechnique* techniques[48];
	};

	struct WaterWritable
	{
		float floatTime;
	};

	struct complex_s
	{
		float real;
		float imag;
	};

	struct __declspec(align(4)) GfxImageLoadDef		//@ size_expr: "sizeof(GfxImageLoadDef) - 4" @ stream_expr: "XFILE_BLOCK_TEMP";
	{
		char levelCount;
		char pad[3];
		int flags;
		int format;
		int resourceSize;
		char data[1];								//@ count_expr: "{asset}->resourceSize";
	};

	union GfxTexture								//@ enum_field_expr: "varGfxImage->mapType";
	{
		GfxImageLoadDef* loadDef;					//@ enum_val_expr: "default";
	};

	struct Picmip
	{
		char platform[2];
	};

	struct CardMemory
	{
		int platform[2];
	};

	struct GfxImage
	{
		GfxTexture texture;
		char mapType;
		char semantic;
		char category;
		bool useSrgbReads;
		Picmip picmip;
		bool noPicmip;
		char track;
		CardMemory cardMemory;
		unsigned __int16 width;
		unsigned __int16 height;
		unsigned __int16 depth;
		bool delayLoadPixels;
		XString name;                 //@ before_expr: "texture";
	};

	struct water_t
	{
		WaterWritable writable;
		complex_s* H0;
		float* wTerm;
		int M;
		int N;
		float Lx;
		float Lz;
		float gravity;
		float windvel;
		float winddir[2];
		float amplitude;
		float codeConstant[4];
		GfxImage* image;
	};

	union MaterialTextureDefInfo					//@ enum_field_expr: "varMaterialTextureDef->semantic";
	{
		water_t* water;								//@ enum_value_expr: "11";
		GfxImage* image;							//@ enum_value_expr: "default";
	};

	struct MaterialTextureDef
	{
		unsigned int nameHash;
		char nameStart;
		char nameEnd;
		char samplerState;
		char semantic;
		MaterialTextureDefInfo u;
	};

	struct MaterialConstantDef							//@ align_expr: "16";
	{
		unsigned int nameHash;
		char name[12];
		float literal[4];
	};

	struct GfxStateBits
	{
		unsigned int loadBits[2];
	};

	struct Material										//@ name_expr: "info.name";
	{
		MaterialInfo info;
		char stateBitsEntry[48];
		char textureCount;
		char constantCount;
		char stateBitsCount;
		char stateFlags;
		char cameraRegion;
		MaterialTechniqueSet* techniqueSet;
		MaterialTextureDef* textureTable;				//@ count_expr: "{asset}->textureCount";
		MaterialConstantDef* constantTable;				//@ count_expr: "{asset}->constantCount";
		GfxStateBits* stateBitsTable;					//@ count_expr: "{asset}->stateBitsCount";
	};

	struct XModel
	{
		XString name;
		char numBones;
		char numRootBones;
		char numsurfs;
		char lodRampType;
		float scale;
		unsigned int noScalePartBits[6];
		unsigned __int16* boneNames;                  //@ count_expr: "{asset}->numBones";
		char* parentList;                             //@ count_expr: "{asset}->numBones - {asset}->numRootBones";
		__int16* quats;                               //@ count_expr: "4 * ({asset}->numBones - {asset}->numRootBones)";
		float* trans;                                 //@ count_expr: "3 * ({asset}->numBones - {asset}->numRootBones)";
		char* partClassification;                     //@ count_expr: "{asset}->numBones";
		DObjAnimMat* baseMat;                         //@ count_expr: "{asset}->numBones";
		Material** materialHandles;                   //@ count_expr: "{asset}->numsurfs";
		XModelLodInfo lodInfo[4];
		char maxLoadedLod;
		char numLods;
		char collLod;
		char flags;
		XModelCollSurf_s* collSurfs;                  //@ count_expr: "{asset}->numCollSurfs";
		int numCollSurfs;
		int contents;
		XBoneInfo* boneInfo;                          //@ count_expr: "{asset}->numBones";
		float radius;
		Bounds bounds;
		int memUsage;
		bool bad;
		PhysPreset* physPreset;
		PhysCollmap* physCollmap;
	};

	struct FxEffectDefRef
	{
		XString name;
	};

	union FxElemVisuals					//@ enum_field_expr: "varFxElemDef->elemType";
	{
		Material* material;										//@ enum_value_expr: "0,1,2,3,4,5,6,11";
		XModel* model;												//@ enum_value_expr: "7";
		FxEffectDefRef effectDef;							//@ enum_value_expr: "12";
		XString soundName;										//@ enum_value_expr: "10";
	};

	struct FxElemMarkVisuals
	{
		Material* materials[2];
	};

	union FxElemDefVisuals			//@ use_if_expr: "";
	{
		FxElemMarkVisuals* markArray;					//@ enum_value_expr: "varFxElemDef->elemType == 11";
		FxElemVisuals* array;									//@ enum_value_expr: "varFxElemDef->visualCount > 1";
		FxElemVisuals instance;								//@ enum_value_expr: "default";
	};

	struct FxTrailVertex
	{
		float pos[2];
		float normal[2];
		float texCoord;
	};

	struct FxTrailDef
	{
		int scrollTimeMsec;
		int repeatDist;
		float invSplitDist;
		float invSplitArcDist;
		float invSplitTime;
		int vertCount;
		FxTrailVertex* verts;
		int indCount;
		unsigned __int16* inds;
	};

	struct FxSparkFountainDef
	{
		float gravity;
		float bounceFrac;
		float bounceRand;
		float sparkSpacing;
		float sparkLength;
		int sparkCount;
		float loopTime;
		float velMin;
		float velMax;
		float velConeFrac;
		float restSpeed;
		float boostTime;
		float boostFactor;
	};

	union FxElemExtendedDefPtr				//@ enum_field_expr: "varFxElemDef->elemType";
	{
		FxTrailDef* trailDef;											//@ enum_value_expr: "3";
		FxSparkFountainDef* sparkFountainDef;			//@ enum_value_expr: "6";
		void* unknownDef;													//@ ignore_expr: "";
	};

	struct FxElemVec3Range
	{
		float base[3];
		float amplitude[3];
	};

	struct FxIntRange
	{
		int base;
		int amplitude;
	};

	struct FxSpawnDefOneShot
	{
		FxIntRange count;
	};

	struct FxSpawnDefLooping
	{
		int intervalMsec;
		int count;
	};

	union FxSpawnDef
	{
		FxSpawnDefLooping looping;
		FxSpawnDefOneShot oneShot;
	};

	struct FxFloatRange
	{
		float base;
		float amplitude;
	};

	struct FxElemAtlas
	{
		char behavior;
		char index;
		char fps;
		char loopCount;
		char colIndexBits;
		char rowIndexBits;
		__int16 entryCount;
	};

	struct FxElemVelStateInFrame
	{
		FxElemVec3Range velocity;
		FxElemVec3Range totalDelta;
	};

	struct FxElemVelStateSample
	{
		FxElemVelStateInFrame local;
		FxElemVelStateInFrame world;
	};

	struct FxElemVisualState
	{
		char color[4];
		float rotationDelta;
		float rotationTotal;
		float size[2];
		float scale;
	};

	struct FxElemVisStateSample
	{
		FxElemVisualState base;
		FxElemVisualState amplitude;
	};

	const struct FxElemDef
	{
		int flags;
		FxSpawnDef spawn;
		FxFloatRange spawnRange;
		FxFloatRange fadeInRange;
		FxFloatRange fadeOutRange;
		float spawnFrustumCullRadius;
		FxIntRange spawnDelayMsec;
		FxIntRange lifeSpanMsec;
		FxFloatRange spawnOrigin[3];
		FxFloatRange spawnOffsetRadius;
		FxFloatRange spawnOffsetHeight;
		FxFloatRange spawnAngles[3];
		FxFloatRange angularVelocity[3];
		FxFloatRange initialRotation;
		FxFloatRange gravity;
		FxFloatRange reflectionFactor;
		FxElemAtlas atlas;
		char elemType;
		char visualCount;
		char velIntervalCount;
		char visStateIntervalCount;
		FxElemVelStateSample* velSamples;				//@ count_expr: "{asset}->velIntervalCount + 1";
		FxElemVisStateSample* visSamples;				//@ count_expr: "{asset}->visStateIntervalCount + 1";
		FxElemDefVisuals visuals;
		Bounds collBounds;
		FxEffectDefRef effectOnImpact;
		FxEffectDefRef effectOnDeath;
		FxEffectDefRef effectEmitted;
		FxFloatRange emitDist;
		FxFloatRange emitDistVariance;
		FxElemExtendedDefPtr extended;
		char sortOrder;
		char lightingFrac;
		char useItemClip;
		char fadeInfo;
	};

	struct FxEffectDef
	{
		XString name;
		int flags;
		int totalSize;
		int msecLoopingLife;
		int elemDefCountLooping;
		int elemDefCountOneShot;
		int elemDefCountEmission;
		FxElemDef* elemDefs;						//@ count_expr: "{asset}->elemDefCountLooping + {asset}->elemDefCountOneShot + {asset}->elemDefCountEmission";
	};

	struct cLeafBrushNodeLeaf_t
	{
		unsigned __int16* brushes;
	};

	struct cLeafBrushNodeChildren_t
	{
		float dist;
		float range;
		unsigned __int16 childOffset[2];
	};

	union cLeafBrushNodeData_t
	{
		cLeafBrushNodeLeaf_t leaf;
		cLeafBrushNodeChildren_t children;
	};

	struct cLeafBrushNode_s
	{
		char axis;
		__int16 leafBrushCount;
		int contents;
		cLeafBrushNodeData_t data;
	};

	struct ClipMaterial
	{
		XString name;
		int surfaceFlags;
		int contents;
	};

	struct cStaticModel_s
	{
		XModel* xmodel;
		float origin[3];
		float invScaledAxis[3][3];
		Bounds absBounds;
	};

	struct CollisionBorder
	{
		float distEq[3];
		float zBase;
		float zSlope;
		float start;
		float length;
	};

	struct CollisionPartition
	{
		char triCount;
		char borderCount;
		char firstVertSegment;
		int firstTri;
		CollisionBorder* borders;
	};

	union CollisionAabbTreeIndex
	{
		int firstChildIndex;
		int partitionIndex;
	};

	struct __declspec(align(16)) CollisionAabbTree
	{
		float midPoint[3];
		unsigned __int16 materialIndex;
		unsigned __int16 childCount;
		float halfSize[3];
		CollisionAabbTreeIndex u;
	};

	struct cLeaf_t
	{
		unsigned __int16 firstCollAabbIndex;
		unsigned __int16 collAabbCount;
		int brushContents;
		int terrainContents;
		Bounds bounds;
		int leafBrushNode;
	};

	struct cmodel_t
	{
		Bounds bounds;
		float radius;
		cLeaf_t leaf;
	};

	struct TriggerModel
	{
		int contents;
		unsigned __int16 hullCount;
		unsigned __int16 firstHull;
	};

	struct TriggerSlab
	{
		float dir[3];
		float midPoint;
		float halfSize;
	};

	struct TriggerHull
	{
		Bounds bounds;
		int contents;
		unsigned __int16 slabCount;
		unsigned __int16 firstSlab;
	};

	struct MapTriggers
	{
		unsigned int count;
		TriggerModel* models;						//@ count_expr: "{asset}->count";
		unsigned int hullCount;
		TriggerHull* hulls;
		unsigned int slabCount;
		TriggerSlab* slabs;
	};

	struct __declspec(align(2)) Stage
	{
		XString name;
		float origin[3];
		unsigned __int16 triggerIndex;
		char sunPrimaryLightIndex;
	};

	struct __declspec(align(4)) MapEnts
	{
		XString name;
		char* entityString;							//@ count_expr: "{asset}->numEntityChars";
		int numEntityChars;
		MapTriggers trigger;
		Stage* stages;
		char stageCount;
	};

	struct SModelAabbNode
	{
		Bounds bounds;
		unsigned __int16 firstChild;
		unsigned __int16 childCount;
	};

	struct GfxPlacement
	{
		float quat[4];
		float origin[3];
	};

	enum DynEntityType
	{
		DYNENT_TYPE_INVALID = 0x0,
		DYNENT_TYPE_CLUTTER = 0x1,
		DYNENT_TYPE_DESTRUCT = 0x2,
		DYNENT_TYPE_COUNT = 0x3,
	};

	struct DynEntityDef
	{
		DynEntityType type;
		GfxPlacement pose;
		XModel* xModel;
		unsigned __int16 brushModel;
		unsigned __int16 physicsBrushModel;
		FxEffectDef* destroyFx;
		PhysPreset* physPreset;
		int health;
		PhysMass mass;
		int contents;
	};

	struct DynEntityPose
	{
		GfxPlacement pose;
		float radius;
	};

	struct DynEntityClient
	{
		int physObjId;
		unsigned __int16 flags;
		unsigned __int16 lightingHandle;
		int health;
	};

	struct DynEntityColl
	{
		unsigned __int16 sector;
		unsigned __int16 nextEntInSector;
		float linkMins[2];
		float linkMaxs[2];
	};

	struct cbrushside_t
	{
		cplane_s* plane;
		unsigned __int16 materialNum;
		char firstAdjacentSideOffset;
		char edgeCount;
	};

	struct cNode_t
	{
		cplane_s* plane;
		__int16 children[2];
	};

	struct cbrush_t
	{
		unsigned __int16 numsides;
		unsigned __int16 glassPieceIndex;
		cbrushside_t* sides;
		char* baseAdjacentSide;
		__int16 axialMaterialNum[2][3];
		char firstAdjacentSideOffsets[2][3];
		char edgeCount[2][3];
	};

	struct BrushWrapper
	{
		Bounds bounds;
		cbrush_t brush;
		int totalEdgeCount;
		cplane_s* planes;
	};

	struct __declspec(align(64)) clipMap_t
	{
		XString name;
		int isInUse;
		int planeCount;
		cplane_s* planes;
		unsigned int numStaticModels;
		cStaticModel_s* staticModelList;		//@ count_expr: "{asset}->numStaticModels";
		unsigned int numMaterials;
		ClipMaterial* materials;
		unsigned int numBrushSides;
		cbrushside_t* brushsides;
		unsigned int numBrushEdges;
		char* brushEdges;
		unsigned int numNodes;
		cNode_t* nodes;
		unsigned int numLeafs;
		cLeaf_t* leafs;
		unsigned int leafbrushNodesCount;
		cLeafBrushNode_s* leafbrushNodes;
		unsigned int numLeafBrushes;
		unsigned __int16* leafbrushes;
		unsigned int numLeafSurfaces;
		unsigned int* leafsurfaces;
		unsigned int vertCount;
		vec3_t* verts;							//@ count_expr: "{asset}->vertCount";
		int triCount;
		unsigned __int16* triIndices;
		char* triEdgeIsWalkable;
		int borderCount;
		CollisionBorder* borders;
		int partitionCount;
		CollisionPartition* partitions;
		int aabbTreeCount;
		CollisionAabbTree* aabbTrees;
		unsigned int numSubModels;
		cmodel_t* cmodels;						//@ count_expr: "{asset}->numSubModels";
		unsigned __int16 numBrushes;
		cbrush_t* brushes;
		Bounds* brushBounds;
		int* brushContents;
		MapEnts* mapEnts;						//@ after_expr: "smodelNodes";
		unsigned __int16 smodelNodeCount;
		SModelAabbNode* smodelNodes;
		unsigned __int16 dynEntCount[2];
		DynEntityDef* dynEntDefList[2];
		DynEntityPose* dynEntPoseList[2];
		DynEntityClient* dynEntClientList[2];
		DynEntityColl* dynEntCollList[2];
		unsigned int checksum;
	};

	struct RawFile
	{
		XString name;
		int compressedLen;
		int len;
		char* buffer;							//@ count_expr: "({asset}->compressedLen) ? {asset}->compressedLen : {asset}->len + 1";
	};

	struct TracerDef
	{
		XString name;
		Material* material;
		unsigned int drawInterval;
		float speed;
		float beamLength;
		float beamWidth;
		float screwRadius;
		float screwDist;
		float colors[5][4];
	};

	struct MSSSpeakerLevels
	{
		int speaker;
		int numLevels;
		float levels[2];
	};

	struct MSSChannelMap
	{
		int speakerCount;
		MSSSpeakerLevels speakers[6];
	};

	struct SpeakerMap
	{
		bool isDefault;
		XString name;
		MSSChannelMap channelMaps[4];
	};

	struct _AILSOUNDINFO
	{
		int format;
		char* data_ptr;						//@ ignore_expr: "";
		unsigned int data_len;
		unsigned int rate;
		int bits;
		int channels;
		unsigned int samples;
		unsigned int block_size;
		void* initial_ptr;				//@ ignore_expr: "";
	};

	struct MssSound
	{
		_AILSOUNDINFO info;
		char* data;				//@ count_expr: "varMssSound->info.data_len" @ stream_expr: "XFILE_BLOCK_TEMP";
	};

	struct LoadedSound
	{
		XString name;
		MssSound sound;
	};

	struct StreamFileNameRaw
	{
		XString dir;
		XString name;
	};

	struct StreamFileInfo
	{
		StreamFileNameRaw raw;
	};

	struct StreamFileName
	{
		StreamFileInfo info;
	};

	struct StreamedSound
	{
		StreamFileName filename;
	};

	union SoundFileRef					//@ enum_field_expr: "varSoundFile->type";
	{
		LoadedSound* loadSnd;			//@ enum_value_expr: "1";
		StreamedSound streamSnd;	//@ enum_value_expr: "default";
	};

	struct SoundFile
	{
		char type;
		char exists;
		SoundFileRef u;
	};

	struct SndCurve								//@ name_expr: "filename";
	{
		XString filename;
		unsigned __int16 knotCount;				//@ ignore_expr: "";
		float knots[16][2];
	};

	union $C8D87EB0090687D323381DFB7A82089C
	{
		float slavePercentage;
		float masterPercentage;
	};

	const struct snd_alias_t
	{
		XString aliasName;
		XString subtitle;
		XString secondaryAliasName;
		XString chainAliasName;
		XString mixerGroup;
		SoundFile* soundFile;
		int sequence;
		float volMin;
		float volMax;
		float pitchMin;
		float pitchMax;
		float distMin;
		float distMax;
		float velocityMin;
		int flags;
		$C8D87EB0090687D323381DFB7A82089C ___u15;
		float probability;
		float lfePercentage;
		float centerPercentage;
		int startDelay;
		SndCurve* volumeFalloffCurve;
		float envelopMin;
		float envelopMax;
		float envelopPercentage;
		SpeakerMap* speakerMap;
	};

	struct snd_alias_list_t	//@ name_expr: "aliasName";
	{
		XString aliasName;
		snd_alias_t* head;		//@ count_expr: "{asset}->count";
		int count;
	};

	struct StringTableCell
	{
		XString string;
		int hash;
	};

	struct StringTable
	{
		XString name;
		int columnCount;
		int rowCount;
		StringTableCell* values;				//@ count_expr: "{asset}->columnCount * {asset}->rowCount";
	};

	struct SndDriverGlobals
	{
		XString name;
	};

	struct menuDef_t;
	struct MenuEventHandlerSet;
	struct Statement_s;
	struct ExpressionSupportingData;

	struct columnInfo_s
	{
		int pos;
		int width;
		int maxChars;
		int alignment;
	};

	struct listBoxDef_s
	{
		int mousePos;
		int startPos[1];
		int endPos[1];
		int drawPadding;
		float elementWidth;
		float elementHeight;
		int elementStyle;
		int numColumns;
		columnInfo_s columnInfo[16];
		MenuEventHandlerSet* onDoubleClick;
		int notselectable;
		int noScrollBars;
		int usePaging;
		float selectBorder[4];
		Material* selectIcon;
	};

	struct editFieldDef_s
	{
		float minVal;
		float maxVal;
		float defVal;
		float range;
		int maxChars;
		int maxCharsGotoNext;
		int maxPaintChars;
		int paintOffset;
	};

	struct multiDef_s
	{
		XString dvarList[32];
		XString dvarStr[32];
		float dvarValue[32];
		int count;
		int strDef;
	};

	struct newsTickerDef_s
	{
		int feedId;
		int speed;
		int spacing;
		int lastTime;
		int start;
		int end;
		float x;
	};

	struct textScrollDef_s
	{
		int startTime;
	};

	union itemDefData_t							//@ enum_field_expr: "varitemDef_s->type";
	{
		listBoxDef_s* listBox;				//@ enum_value_expr: "6";
		editFieldDef_s* editField;		//@ enum_value_expr: "0,4,9,10,11,14,16,17,18,22,23";
		multiDef_s* multi;						//@ enum_value_expr: "12";
		XString enumDvarName;					//@ enum_value_expr: "13";
		newsTickerDef_s* ticker;			//@ enum_value_expr: "20";
		textScrollDef_s* scroll;			//@ enum_value_expr: "21";
		void* data;										//@ ignore_expr: "";
	};

	struct ExpressionString
	{
		XString string;
	};

	union operandInternalDataUnion	//@ enum_field_expr: "varOperand->dataType";
	{
		int intVal;										//@ enum_value_expr: "0";
		float floatVal;								//@ enum_value_expr: "1";
		ExpressionString stringVal;		//@ enum_value_expr: "2";
		Statement_s* function;				//@ enum_value_expr: "3";
	};

	enum expDataType
	{
		VAL_INT = 0x0,
		VAL_FLOAT = 0x1,
		VAL_STRING = 0x2,
		NUM_INTERNAL_DATATYPES = 0x3,
		VAL_FUNCTION = 0x3,
		NUM_DATATYPES = 0x4,
	};

	struct Operand
	{
		expDataType dataType;
		operandInternalDataUnion internals;
	};

	union entryInternalData		//@ enum_field_expr: "varexpressionEntry->type";
	{
		int op;									//@ enum_value_expr: "0";
		Operand operand;				//@ enum_value_expr: "1";
	};

	struct expressionEntry
	{
		int type;
		entryInternalData data;
	};

	struct Statement_s
	{
		int numEntries;
		expressionEntry* entries;			//@ count_expr: "{asset}->numEntries";
		ExpressionSupportingData* supportingData;
		int lastExecuteTime;
		Operand lastResult;						//@ ignore_expr: "";
	};

	struct UIFunctionList
	{
		int totalFunctions;
		Statement_s** functions;	//@ count_expr: "{asset}->totalFunctions";
	};

	struct StringList
	{
		int totalStrings;
		XString* strings;									//@ count_expr: "{asset}->totalStrings";
	};

	struct StaticDvar
	{
		void* dvar;											//@ ignore_expr: "";
		XString dvarName;
	};

	struct StaticDvarList
	{
		int numStaticDvars;
		StaticDvar** staticDvars;					//@ count_expr: "{asset}->numStaticDvars";
	};

	struct ExpressionSupportingData
	{
		UIFunctionList uifunctions;
		StaticDvarList staticDvarList;
		StringList uiStrings;
	};

	struct ConditionalScript
	{
		MenuEventHandlerSet* eventHandlerSet;			//@ after_expr: "eventExpression";
		Statement_s* eventExpression;
	};

	struct SetLocalVarData
	{
		XString localVarName;
		Statement_s* expression;
	};

	union EventData															//@ enum_field_expr: "varMenuEventHandler->eventType";
	{
		XString unconditionalScript;							//@ enum_value_expr: "0";
		ConditionalScript* conditionalScript;			//@ enum_value_expr: "1";
		MenuEventHandlerSet* elseScript;					//@ enum_value_expr: "2";
		SetLocalVarData* setLocalVarData;					//@ enum_value_expr: "3,4,5,6";
	};

	enum EventType
	{
		EVENT_UNCONDITIONAL = 0x0,
		EVENT_IF = 0x1,
		EVENT_ELSE = 0x2,
		EVENT_SET_LOCAL_VAR_BOOL = 0x3,
		EVENT_SET_LOCAL_VAR_INT = 0x4,
		EVENT_SET_LOCAL_VAR_FLOAT = 0x5,
		EVENT_SET_LOCAL_VAR_STRING = 0x6,
		EVENT_COUNT = 0x7,
	};

	struct MenuEventHandler
	{
		EventData eventData;
		char eventType;
	};

	struct MenuEventHandlerSet
	{
		int eventHandlerCount;
		MenuEventHandler** eventHandlers;			//@ count_expr: "{asset}->eventHandlerCount";
	};

	struct ItemKeyHandler
	{
		int key;
		MenuEventHandlerSet* action;
		ItemKeyHandler* next;
	};

	struct ItemFloatExpression
	{
		int target;
		Statement_s* expression;
	};

	struct menuTransition
	{
		int transitionType;
		int targetField;
		int startTime;
		float startVal;
		float endVal;
		float time;
		int endTriggerType;
	};

	struct rectDef_s
	{
		float x;
		float y;
		float w;
		float h;
		char horzAlign;
		char vertAlign;
	};

	struct windowDef_t
	{
		XString name;
		rectDef_s rect;
		rectDef_s rectClient;
		XString group;
		int style;
		int border;
		int ownerDraw;
		int ownerDrawFlags;
		float borderSize;
		int staticFlags;
		int dynamicFlags[1];
		int nextTime;
		float foreColor[4];
		float backColor[4];
		float borderColor[4];
		float outlineColor[4];
		float disableColor[4];
		Material* background;
	};

	struct itemDef_s
	{
		windowDef_t window;
		rectDef_s textRect[1];
		int type;
		int dataType;
		int alignment;
		int fontEnum;
		int textAlignMode;
		float textalignx;
		float textaligny;
		float textscale;
		int textStyle;
		int gameMsgWindowIndex;
		int gameMsgWindowMode;
		XString text;
		int itemFlags;
		menuDef_t* parent;								//@ ignore_expr: "";
		MenuEventHandlerSet* mouseEnterText;
		MenuEventHandlerSet* mouseExitText;
		MenuEventHandlerSet* mouseEnter;
		MenuEventHandlerSet* mouseExit;
		MenuEventHandlerSet* action;
		MenuEventHandlerSet* accept;
		MenuEventHandlerSet* onFocus;
		MenuEventHandlerSet* leaveFocus;
		XString dvar;
		XString dvarTest;
		ItemKeyHandler* onKey;
		XString enableDvar;
		XString localVar;
		int dvarFlags;
		snd_alias_list_t* focusSound;
		float special;
		int cursorPos[1];
		itemDefData_t typeData;
		int imageTrack;
		int floatExpressionCount;
		ItemFloatExpression* floatExpressions;		//@ count_expr: "{asset}->floatExpressionCount";
		Statement_s* visibleExp;
		Statement_s* disabledExp;
		Statement_s* textExp;
		Statement_s* materialExp;
		float glowColor[4];
		bool decayActive;
		int fxBirthTime;
		int fxLetterTime;
		int fxDecayStartTime;
		int fxDecayDuration;
		int lastSoundPlayedTime;
	};

	struct menuDef_t									//@ name_expr: "window.name";
	{
		windowDef_t window;
		XString font;
		int fullScreen;
		int itemCount;
		int fontIndex;
		int cursorItem[1];
		int fadeCycle;
		float fadeClamp;
		float fadeAmount;
		float fadeInAmount;
		float blurRadius;
		MenuEventHandlerSet* onOpen;
		MenuEventHandlerSet* onCloseRequest;          //@ after_expr: "onClose";
		MenuEventHandlerSet* onClose;
		MenuEventHandlerSet* onESC;
		ItemKeyHandler* onKey;
		Statement_s* visibleExp;
		XString allowedBinding;
		XString soundName;
		int imageTrack;
		float focusColor[4];
		Statement_s* rectXExp;
		Statement_s* rectYExp;
		Statement_s* rectWExp;
		Statement_s* rectHExp;
		Statement_s* openSoundExp;
		Statement_s* closeSoundExp;
		itemDef_s** items;														//@ count_expr: "{asset}->itemCount";
		menuTransition scaleTransition[1];
		menuTransition alphaTransition[1];
		menuTransition xTransition[1];
		menuTransition yTransition[1];
		ExpressionSupportingData* expressionData;     //@ before_expr: "window";
	};

	struct MenuList
	{
		XString name;
		int menuCount;
		menuDef_t** menus;							//@ count_expr: "{asset}->menuCount";
	};

	struct ComPrimaryLight
	{
		char type;
		char canUseShadowMap;
		char exponent;
		char unused;
		float color[3];
		float dir[3];
		float origin[3];
		float radius;
		float cosHalfFovOuter;
		float cosHalfFovInner;
		float cosHalfFovExpanded;
		float rotationLimit;
		float translationLimit;
		XString defName;
	};

	struct ComWorld
	{
		XString name;
		int isInUse;
		unsigned int primaryLightCount;
		ComPrimaryLight* primaryLights;
	};

	struct LocalizeEntry
	{
		XString value;								//@ after_expr: "name";
		XString name;							
	};

	struct __declspec(align(2)) G_GlassPiece
	{
		unsigned __int16 damageTaken;
		unsigned __int16 collapseTime;
		int lastStateChangeTime;
		char impactDir;
		char impactPos[2];
	};

	struct G_GlassName
	{
		XString nameStr;
		unsigned __int16 name;
		unsigned __int16 pieceCount;
		unsigned __int16* pieceIndices;			//@ count_expr: "{asset}->pieceCount";
	};

	struct G_GlassData
	{
		G_GlassPiece* glassPieces;				//@ count_expr: "{asset}->pieceCount";
		unsigned int pieceCount;
		unsigned __int16 damageToWeaken;
		unsigned __int16 damageToDestroy;
		unsigned int glassNameCount;
		G_GlassName* glassNames;				//@ count_expr: "{asset}->glassNameCount";
		char pad[108];
	};

	struct GameWorldMp
	{
		XString name;
		G_GlassData* g_glassData;
	};

	struct FxGlassPieceState
	{
		float texCoordOrigin[2];
		unsigned int supportMask;
		unsigned __int16 initIndex;
		unsigned __int16 geoDataStart;
		char defIndex;
		char pad[5];
		char vertCount;
		char holeDataCount;
		char crackDataCount;
		char fanDataCount;
		unsigned __int16 flags;
		float areaX2;
	};

	struct FxGlassPieceDynamics
	{
		int fallTime;
		int physObjId;
		int physJointId;
		float vel[3];
		float avel[3];
	};

	struct FxGlassVertex
	{
		__int16 x;
		__int16 y;
	};

	struct FxGlassHoleHeader
	{
		unsigned __int16 uniqueVertCount;
		char touchVert;
		char pad[1];
	};

	struct FxGlassCrackHeader
	{
		unsigned __int16 uniqueVertCount;
		char beginVertIndex;
		char endVertIndex;
	};

	union FxGlassGeometryData
	{
		FxGlassVertex vert;
		FxGlassHoleHeader hole;
		FxGlassCrackHeader crack;
		char asBytes[4];
		__int16 anonymous[2];
	};

	struct FxGlassDef
	{
		float halfThickness;
		float texVecs[2][2];
		GfxColor color;
		Material* material;
		Material* materialShattered;
		PhysPreset* physPreset;
	};

	struct FxSpatialFrame
	{
		float quat[4];
		float origin[3];
	};

	struct $E43DBA5037697D705289B74D87E76C70
	{
		FxSpatialFrame frame;
		float radius;
	};

	union FxGlassPiecePlace
	{
		$E43DBA5037697D705289B74D87E76C70 __s0;
		unsigned int nextFree;
	};

	struct FxGlassInitPieceState
	{
		FxSpatialFrame frame;
		float radius;
		float texCoordOrigin[2];
		unsigned int supportMask;
		float areaX2;
		char defIndex;
		char vertCount;
		char fanDataCount;
		char pad[1];
	};

	struct __declspec(align(1)) char16				//@ align_expr: "16";
	{
		char data;
	};

	struct float16									//@ align_expr: "16";
	{
		float data;
	};

	struct FxGlassSystem
	{
		int time;
		int prevTime;
		unsigned int defCount;
		unsigned int pieceLimit;
		unsigned int pieceWordCount;
		unsigned int initPieceCount;
		unsigned int cellCount;
		unsigned int activePieceCount;
		unsigned int firstFreePiece;
		unsigned int geoDataLimit;
		unsigned int geoDataCount;
		unsigned int initGeoDataCount;
		FxGlassDef* defs;							//@ count_expr: "{asset}->defCount";
		FxGlassPiecePlace* piecePlaces;				//@ count_expr: "{asset}->pieceLimit";
		FxGlassPieceState* pieceStates;				//@ count_expr: "{asset}->pieceLimit";
		FxGlassPieceDynamics* pieceDynamics;		//@ count_expr: "{asset}->pieceLimit";
		FxGlassGeometryData* geoData;				//@ count_expr: "{asset}->geoDataLimit";
		unsigned int* isInUse;						//@ count_expr: "{asset}->pieceWordCount";
		unsigned int* cellBits;						//@ count_expr: "{asset}->pieceWordCount * {asset}->cellCount";
		char16* visData;							//@ count_expr: "({asset}->pieceLimit + 15) & 0xFFFFFFF0";
		vec3_t* linkOrg;							//@ count_expr: "{asset}->pieceLimit";
		float16* halfThickness;						//@ count_expr: "({asset}->pieceLimit + 3) & 0xFFFFFFFC";
		unsigned __int16* lightingHandles;			//@ count_expr: "{asset}->initPieceCount";
		FxGlassInitPieceState* initPieceStates;		//@ count_expr: "{asset}->initPieceCount";
		FxGlassGeometryData* initGeoData;			//@ count_expr: "{asset}->initGeoDataCount";
		bool needToCompactData;
		char initCount;
		float effectChanceAccum;
		int lastPieceDeletionTime;
	};

	struct FxWorld
	{
		XString name;
		FxGlassSystem glassSys;
	};

	struct GfxLightImage
	{
		GfxImage* image;
		char samplerState;
	};

	struct GfxLightDef
	{
		XString name;
		GfxLightImage attenuation;
		int lmapLookupStart;
	};

	struct Glyph
	{
		unsigned __int16 letter;
		char x0;
		char y0;
		char dx;
		char pixelWidth;
		char pixelHeight;
		float s0;
		float t0;
		float s1;
		float t1;
	};

	struct Font_s						//@ name_expr: "fontName";
	{
		XString fontName;
		int pixelHeight;
		int glyphCount;
		Material* material;
		Material* glowMaterial;
		Glyph* glyphs;
	};

	enum LbColType
	{
		LBCOL_TYPE_NUMBER = 0x0,
		LBCOL_TYPE_TIME = 0x1,
		LBCOL_TYPE_LEVELXP = 0x2,
		LBCOL_TYPE_PRESTIGE = 0x3,
		LBCOL_TYPE_BIGNUMBER = 0x4,
		LBCOL_TYPE_PERCENT = 0x5,
		LBCOL_TYPE_COUNT = 0x6,
	};

	enum LbAggType
	{
		LBAGG_TYPE_MIN = 0x0,
		LBAGG_TYPE_MAX = 0x1,
		LBAGG_TYPE_SUM = 0x2,
		LBAGG_TYPE_LAST = 0x3,
		LBAGG_TYPE_COUNT = 0x4,
	};

	struct LbColumnDef
	{
		XString name;
		int id;
		int propertyId;
		bool hidden;
		XString statName;
		LbColType type;
		int precision;
		LbAggType agg;
	};

	struct LeaderboardDef
	{
		XString name;
		int id;
		int columnCount;
		int xpColId;
		int prestigeColId;
		LbColumnDef* columns;
	};

	struct __declspec(align(4)) StructuredDataEnumEntry
	{
		XString string;
		unsigned __int16 index;
	};

	struct StructuredDataEnum
	{
		int entryCount;
		int reservedEntryCount;
		StructuredDataEnumEntry* entries;			//@ count_expr: "{asset}->entryCount";
	};

	union StructuredDataTypeUnion
	{
		unsigned int stringDataLength;
		int enumIndex;
		int structIndex;
		int indexedArrayIndex;
		int enumedArrayIndex;
	};

	enum StructuredDataTypeCategory
	{
		DATA_INT = 0x0,
		DATA_BYTE = 0x1,
		DATA_BOOL = 0x2,
		DATA_STRING = 0x3,
		DATA_ENUM = 0x4,
		DATA_STRUCT = 0x5,
		DATA_INDEXED_ARRAY = 0x6,
		DATA_ENUM_ARRAY = 0x7,
		DATA_FLOAT = 0x8,
		DATA_SHORT = 0x9,
		DATA_COUNT = 0xA,
	};

	struct StructuredDataType
	{
		StructuredDataTypeCategory type;
		StructuredDataTypeUnion u;
	};

	struct StructuredDataStructProperty
	{
		XString name;
		StructuredDataType type;
		unsigned int offset;
	};

	struct StructuredDataStruct
	{
		int propertyCount;
		StructuredDataStructProperty* properties;		//@ count_expr: "{asset}->propertyCount";
		int size;
		unsigned int bitOffset;
	};

	struct StructuredDataIndexedArray
	{
		int arraySize;
		StructuredDataType elementType;				//@ count_expr: "{asset}->elementSize";
		unsigned int elementSize;
	};

	struct StructuredDataEnumedArray
	{
		int enumIndex;
		StructuredDataType elementType;				//@ count_expr: "{asset}->elementSize";
		unsigned int elementSize;
	};

	struct StructuredDataDef
	{
		int version;
		unsigned int formatChecksum;
		int enumCount;
		StructuredDataEnum* enums;
		int structCount;
		StructuredDataStruct* structs;
		int indexedArrayCount;
		StructuredDataIndexedArray* indexedArrays;
		int enumedArrayCount;
		StructuredDataEnumedArray* enumedArrays;
		StructuredDataType rootType;
		unsigned int size;
	};

	struct StructuredDataDefSet
	{
		XString name;
		unsigned int defCount;
		StructuredDataDef* defs;
	};

	enum XAssetType
	{
		ASSET_TYPE_PHYSPRESET = 0x0,
		ASSET_TYPE_PHYSCOLLMAP = 0x1,
		ASSET_TYPE_XANIMPARTS = 0x2,
		ASSET_TYPE_XMODEL_SURFS = 0x3,
		ASSET_TYPE_XMODEL = 0x4,
		ASSET_TYPE_MATERIAL = 0x5,
		ASSET_TYPE_PIXELSHADER = 0x6,
		ASSET_TYPE_VERTEXSHADER = 0x7,
		ASSET_TYPE_VERTEXDECL = 0x8,
		ASSET_TYPE_TECHNIQUE_SET = 0x9,
		ASSET_TYPE_IMAGE = 0xA,
		ASSET_TYPE_SOUND = 0xB,
		ASSET_TYPE_SOUND_CURVE = 0xC,
		ASSET_TYPE_LOADED_SOUND = 0xD,
		ASSET_TYPE_CLIPMAP_SP = 0xE,
		ASSET_TYPE_CLIPMAP_MP = 0xF,
		ASSET_TYPE_COMWORLD = 0x10,
		ASSET_TYPE_GAMEWORLD_SP = 0x11,
		ASSET_TYPE_GAMEWORLD_MP = 0x12,
		ASSET_TYPE_MAP_ENTS = 0x13,
		ASSET_TYPE_FXWORLD = 0x14,
		ASSET_TYPE_GFXWORLD = 0x15,
		ASSET_TYPE_LIGHT_DEF = 0x16,
		ASSET_TYPE_UI_MAP = 0x17,
		ASSET_TYPE_FONT = 0x18,
		ASSET_TYPE_MENULIST = 0x19,
		ASSET_TYPE_MENU = 0x1A,
		ASSET_TYPE_LOCALIZE_ENTRY = 0x1B,
		ASSET_TYPE_WEAPON = 0x1C,
		ASSET_TYPE_SNDDRIVER_GLOBALS = 0x1D,
		ASSET_TYPE_FX = 0x1E,
		ASSET_TYPE_IMPACT_FX = 0x1F,
		ASSET_TYPE_AITYPE = 0x20,
		ASSET_TYPE_MPTYPE = 0x21,
		ASSET_TYPE_CHARACTER = 0x22,
		ASSET_TYPE_XMODELALIAS = 0x23,
		ASSET_TYPE_RAWFILE = 0x24,
		ASSET_TYPE_STRINGTABLE = 0x25,
		ASSET_TYPE_LEADERBOARD = 0x26,
		ASSET_TYPE_STRUCTURED_DATA_DEF = 0x27,
		ASSET_TYPE_TRACER = 0x28,
		ASSET_TYPE_VEHICLE = 0x29,
		ASSET_TYPE_ADDON_MAP_ENTS = 0x2A,
		ASSET_TYPE_COUNT = 0x2B,
		ASSET_TYPE_STRING = 0x2B,
		ASSET_TYPE_ASSETLIST = 0x2C,
	};

	struct ScriptStringList
	{
		int count;
		TempString* strings;                       //@ count_expr: "{asset}->count";
	};

	struct XAsset;
	struct XAssetList
	{
		ScriptStringList stringList;
		int assetCount;
		XAsset* assets;                             //@ count_expr: "{asset}->assetCount";
	};

	union XAssetHeader								//@ enum_field_expr: "varXAsset->type";
	{
		PhysPreset* physPreset;                     //@ enum_value_expr: "XAssetType::ASSET_TYPE_PHYSPRESET";
		PhysCollmap* physCollmap;                   //@ enum_value_expr: "XAssetType::ASSET_TYPE_PHYSCOLLMAP";
		XAnimParts* parts;													//@ enum_value_expr: "XAssetType::ASSET_TYPE_XANIMPARTS";
		XModelSurfs* modelSurfs;                    //@ enum_value_expr: "XAssetType::ASSET_TYPE_XMODEL_SURFS";
		XModel* xmodel;                             //@ enum_value_expr: "XAssetType::ASSET_TYPE_XMODEL";
		Material* material;                         //@ enum_value_expr: "XAssetType::ASSET_TYPE_MATERIAL";
		MaterialPixelShader* pixelShader;           //@ enum_value_expr: "XAssetType::ASSET_TYPE_PIXELSHADER";
		MaterialVertexShader* vertexShader;         //@ enum_value_expr: "XAssetType::ASSET_TYPE_VERTEXSHADER";
		MaterialVertexDeclaration* vertexDecl;      //@ enum_value_expr: "XAssetType::ASSET_TYPE_VERTEXDECL";
		MaterialTechniqueSet* techset;              //@ enum_value_expr: "XAssetType::ASSET_TYPE_TECHNIQUE_SET";
		GfxImage* image;                            //@ enum_value_expr: "XAssetType::ASSET_TYPE_IMAGE";
		snd_alias_list_t* sound;										//@ enum_value_expr: "XAssetType::ASSET_TYPE_SOUND";
		SndCurve* sndCurve;							//@ enum_value_expr: "XAssetType::ASSET_TYPE_SOUND_CURVE";
		FxEffectDef* fxEffectDef;                   //@ enum_value_expr: "XAssetType::ASSET_TYPE_FX";
		clipMap_t* clipMap;                         //@ enum_value_expr: "XAssetType::ASSET_TYPE_CLIPMAP_MP";
		ComWorld* comWorld;							//@ enum_value_expr: "XAssetType::ASSET_TYPE_COMWORLD";
		GameWorldMp* gameWorldMp;					//@ enum_value_expr: "XAssetType::ASSET_TYPE_GAMEWORLD_MP";
		MapEnts* mapEnts;                           //@ enum_value_expr: "XAssetType::ASSET_TYPE_MAP_ENTS";
		FxWorld* fxWorld;							//@ enum_value_expr: "XAssetType::ASSET_TYPE_FXWORLD";
		GfxLightDef* lightDef;											//@ enum_value_expr: "XAssetType::ASSET_TYPE_LIGHT_DEF";
		Font_s* font;																//@ enum_value_expr: "XAssetType::ASSET_TYPE_FONT";
		MenuList* menuList;							//@ enum_value_expr: "XAssetType::ASSET_TYPE_MENULIST";
		menuDef_t* menu;							//@ enum_value_expr: "XAssetType::ASSET_TYPE_MENU";
		LocalizeEntry* localizeEntry;				//@ enum_value_expr: "XAssetType::ASSET_TYPE_LOCALIZE_ENTRY";
		RawFile* rawFile;							//@ enum_value_expr: "XAssetType::ASSET_TYPE_RAWFILE";
		StringTable* stringTable;					//@ enum_value_expr: "XAssetType::ASSET_TYPE_STRINGTABLE";
		LeaderboardDef* leaderboardDef;							//@ enum_value_expr: "XAssetType::ASSET_TYPE_LEADERBOARD";
		StructuredDataDefSet* structuredDataDefSet;	//@ enum_value_expr: "XAssetType::ASSET_TYPE_STRUCTURED_DATA_DEF";
		TracerDef* tracerDef;						//@ enum_value_expr: "XAssetType::ASSET_TYPE_TRACER";
		XAssetList* assetList;						//@ enum_value_expr: "XAssetType::ASSET_TYPE_ASSETLIST";
	};

	struct XAsset
	{
		XAssetType type;
		XAssetHeader asset;
	};
}
