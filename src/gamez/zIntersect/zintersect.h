#ifndef ZINTERSECT_H
#define ZINTERSECT_H
#include <vector>

#include "gamez/zMath/zmath.h"
#include "gamez/zSystem/zsys.h"

namespace zdb
{
    class CNode;
    class CSaveLoad;
}

namespace zdb
{
    struct DI_PARAMS
    {
        CPnt4D m_nrm;
        u32 m_region;
        u32 m_refcount;
        u32 m_ditype : 2;
        u32 m_ptcount : 8;
        u32 m_material : 8;
        u32 m_cameratype : 2;
        u32 m_appflags : 3;
        u32 m_inside : 1;
        u32 m_shadow : 2;
        u32 m_reserved : 6;
    };

    struct IntersectStruct
    {
        CPnt3D m_pos;
        CPnt3D m_norm;
        CNode* m_node;
    };

    struct DiIntersect
    {
        DiIntersect() {}
        DiIntersect(s32 count);
        
        u32 m_Type : 16;
        u32 m_IntersectCharacters : 1;
        u32 m_AltitudeCharacters : 1;
        u32 m_ProximityCharacters : 1;
        u32 m_Unused : 13;

        CPnt3D m_Tail;
        CPnt3D m_Tip;
        CPnt3D m_MTail;
        CPnt3D m_MTip;

        s32 m_Cnt;
        s32 m_BufCnt;

        CNode* m_Node;
        CNode* m_TreeDoneNode;
        IntersectStruct* m_Intersects;
    };

    class CHit : public IntersectStruct
    {
    public:
        CPnt3D m_hit0;
        CPnt3D m_hit1;
    };
    
    class CDI : public DI_PARAMS
    {
    public:
        enum class CDIType
        {
            type_00,
            type_01,
            type_02,
            type_03,
            type_04
        };
        
        CDI();

        static CDI* Create(CSaveLoad& sload);

        void Allocate(size_t size);
        void Free();
        bool Read(CSaveLoad& sload);

        s32 GetEdgeIntersects(CPnt3D* firstPoint, CPnt3D* secondPoint, CPnt3D* edge) const;
        s32 GetEdgeIntersectsY(CPnt3D* firstPoint, CPnt3D* secondPoint) const;
    private:
        CPnt4D* m_pts;
    };

    class CDIVector : public std::vector<CDI*>
    {
    public:
        bool Exists(CDI* di);
    };

    class CDIPoly : public CDI
    {
        
    };
}
#endif