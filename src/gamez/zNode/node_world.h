#ifndef NODE_WORLD_H
#define NODE_WORLD_H
#include "node_wind.h"

namespace zdb
{
	class CWorld : public CNode
	{
		friend class CCamera;
	public:
		CWorld(const char* name);
		~CWorld();

		static CWorld* m_world;

		static f32 m_scale;
		static f32 m_invscale;

		static s32 GetVersion();
		static CModel* GetModel(const char* name);
		static CTexHandle* GetTexHandle(char* name);

		static void Init() {}
		static void Uninit() {}

		void diTick();
		void Update() {}

		bool Initalize();

		void AddChild(CNode* child);
		void AddLandmark(CNode* landmark);
		void AddTextureAssetCharacter(const CNode& textureAsset);
		void ReserveChildren(s32 count);

		s32 GenerateLandmarkList();
		void LandmarkList_Search(CNode* node, CStack& matstack, CNodeVector& landmarks);

		void DeleteChildren();
		void DeleteLandmark(const CNode& landmark);

		bool DismemberWorldModel();

		void ClearLightMapList();
		void ClearShadowList();
		void WipeoutTextureAssetCharacters();

		void ComputeLightIntensity(f32 intensity, const CPnt3D& point, f32* lightRef);

		// undefined4 GetTextureByName(const char* name) const;

		CCamera* m_camera;

		u32 m_default_soiltype;
		char* m_default_soiltype_name;

		DiIntersect* m_diInt;
		DiIntersect* m_diIntDelay;
		s32 m_diIntDelayCnt;

		CWind m_Wind;

		_cellData** m_cellData;

		s32 m_maxOverlap;

		// TODO:
		// implement these two container types
		// CRenderMapVector m_shadows;
		// CLightMapVector m_lightMaps;

		CLOD_Object m_LOD_Object;
		CMaterial_Object m_Material_Object;
		CSubSurf_Object m_SubSurf_Object;
		CPreLight_Object m_PreLight_Object;
		CProjectedMap_Object m_ProjectedMap_Object;
		CScrollingTexture_Object m_ScrollingTexture_Object;

		bool m_night_mission;

		CPnt3D m_lensfx_nvg;
		CPnt3D m_lensfx_starlightscope;

		_globalLight m_gLight;

		bool m_gLightDirUpdate;
		bool m_gLightColUpdate;
		CPnt3D m_shadow_vector;

		CGrid* m_grid;
		CNodeVector m_landmarks;
		u32 m_numNoFarClipNodes;

		// TODO:
		// implement this type
		// CTextureAssetCharacterList m_TextureAssetCharacters;
		// CTexList m_TextureCharacterCommon;
		// CTexList m_TextureDecals;
		CTexture* m_shadowTex[4];
		CTexHandle* m_shadowTexH;
	};
}
#endif