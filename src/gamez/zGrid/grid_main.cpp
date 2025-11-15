#include "zgrid.h"

#include "gamez/zNode/znode.h"
#include "gamez/zNode/node_world.h"
#include "gamez/zNode/node_saveload.h"

namespace zdb
{
    CCell::CCell(const CPnt3D* origin, f32 offset)
    {
        CPnt3D boxextents;
        
        m_bbox.m_max = *origin;

        boxextents.x = offset - 0.001f;
        boxextents.y = 0.0f;
        boxextents.z = boxextents.x;

        m_bbox.m_min = m_bbox.m_max;

        CPnt3D max = m_bbox.m_max;

        max.Add(origin, &boxextents);
        
        m_type = (u32)TYPE::NODE_TYPE_CELL;

        m_lightList.erase(m_lightList.begin(), m_lightList.end());
    }

    CGrid::CGrid()
    {
        
    }
    
    bool CGrid::Create(CWorld* world, const tag_GRID_PARAMS* params)
    {
        CBBox bbox;
        
        m_world = world;
        m_posts = params->m_posts;

        if (m_world->m_flatten)
        {
            CNode* parent = m_world->m_parent;
            bbox = parent->m_bbox;

            if (m_world->m_use_parent_bbox)
            {
                bbox = *parent->GetBBox();
            }
        }
        else
        {
            bbox = m_world->m_bbox;
        }

        m_origin = bbox.m_min;

        m_CellCount.cx = params->m_CellCount.cx;
        m_CellCount.cy = params->m_CellCount.cy;
        m_CellDim = params->m_CellDim;

        m_extents.top = 0.0f;
        m_extents.left = 0.0f;
        m_extents.right = m_CellDim * m_CellCount.cx;
        m_extents.bottom = m_CellDim * m_CellCount.cy;
        
        m_GridCellToI = sqrtf(m_CellDim * m_CellDim * 2.0f);
        m_InvCellDim = 1.0f / m_CellDim;
        
        m_TickNum = -1;
        
        m_AtomCnt = 16384;
        m_AtomFreeCnt = m_AtomCnt;
        m_AtomBuf = static_cast<CGridAtom*>(zmalloc(m_AtomCnt * 12));
        m_FreeAtoms = static_cast<CGridAtom**>(zmalloc(m_AtomCnt << 2));

        s32 atomidx = m_AtomCnt;

        // TODO:
        // this is stupid
        // for (u32 i = 0; i < m_AtomCnt; i++)
        // {
        //     atomidx = (s32)m_AtomBuf->Next;
        //     m_FreeAtoms
        // }

        m_AtomFreePtr = atomidx - 1;

        m_Atoms = static_cast<CGridAtom**>(zmalloc(m_CellCount.cx * m_CellCount.cy * sizeof(CGridAtom)));

        world->ReserveChildren(m_CellCount.cy * m_CellCount.cx);

        s32 i = 0;
        
        for (s32 y = 0; y < m_CellCount.cy; y++)
        {
            for (s32 x = 0; x < m_CellCount.cx; x++)
            {
                s32 cellidx = 0;
                s32 cx = 0;
                s32 cy = 0;
                CPnt3D cellpos;
                char buf[24];

                cellpos.y = m_origin.y;
                cellpos.x = m_origin.x + (x * m_CellDim);
                cellpos.z = m_origin.z + (y * m_CellDim);

                cellidx = x;

                if (x < 0)
                {
                    cellidx = 0;
                }

                if (cellidx <= m_CellCount.cx - 1)
                {
                    cx = cellidx;
                }

                cellidx = y;

                if (y < 0)
                {
                    cellidx = 0;
                }

                if (cellidx <= m_CellCount.cy - 1)
                {
                    cy = cellidx;
                }

                m_Atoms[cx + cy * m_CellCount.cx] = NULL;

                CCell* cell = new CCell(&cellpos, m_CellDim);
                sprintf_s(buf, 24, "cell%06d", i);
                cell->SetName(buf);
                world->AddChild(cell);
                i++;
            }
        }

        return true;
    }
	
    bool CGrid::Read(CSaveLoad& sload)
    {
        s32 posts = 16;
        f32 dimensions = 640.0f;
        s32 cx = 8;
        s32 cy = 8;

        if (!sload.m_zfile.Fetch("grid_params", this, sizeof(tag_GRID_PARAMS)))
        {
            return false;
        }
        else
        {
            return Create(sload.m_world, this);
        }

        return false;
    }

    void CGrid::Update(CNode* node)
    {
        for (u32 i = 0; i < node->m_AtomCnt; i++)
        {
            CGridAtom* atom = node->GetAtom(i);

            if (!atom->Next)
            {
                atom->Prev->Next = NULL;
            }
            else
            {
                atom->Prev->Next = atom->Next;
                atom->Next->Prev = atom->Prev;
            }

            if (m_AtomFreePtr < m_AtomFreeCnt - 1)
            {
                atom->Ent = NULL;
                atom->Next = NULL;
                atom->Prev = NULL;

                u32 ptr = m_AtomFreePtr;

                m_AtomFreePtr = ptr + 1;
                m_FreeAtoms[ptr] = atom;
            }
        }

        node->SetAtomCnt(0);
        gridAddNodeToGrids(node);
    }

    void CGrid::SetTraversalBoundary(const CPnt3D* point, s32 tick, bool lineWalk)
    {
        // TODO: Implement this function

        SetTraversalExtents(point, tick, lineWalk);

        if (tick == 1)
        {
            m_TickNum++;
            return;
        }

        for (u32 i = 0; i < m_PntCnt; i++)
        {

        }

        m_TickNum++;
    }

    void CGrid::SetTraversalExtents(const CPnt3D* point, s32 tick, bool lineWalk)
    {
        // TODO: Implement this function
    }

    CGridAtom* CGrid::StartTraversal()
    {
        m_xx = m_ix;
        m_zz = m_iz;
        m_iz_start = m_zz;
        m_iz_next_row = m_izb;
        m_NextAtom = NULL;
        m_zz--;
        m_RowCellIn = 0;
        m_CurCellOut = 1;
        m_iz_dir = 1;
        return m_NextAtom = GetNextAtom();
    }

    CGridAtom* CGrid::StartTraversalOrdered()
    {
        m_NextAtom = NULL;
        return m_NextAtom = GetNextAtomOrdered();
    }

    CGridAtom* CGrid::GetNextAtom()
    {
        // TODO: Implement this function
        return m_NextAtom;
    }

    CGridAtom* CGrid::GetNextAtomOrdered()
    {
        CNode* atom_node = NULL;

        while (!m_NextAtom)
        {
            if (!m_NextAtom)
            {
                s32 runtimeAtomCnt = m_orderedCells.m_rt_gridCellAtomCnt;

                if (runtimeAtomCnt == m_orderedCells.m_gridCellAtomCnt)
                {
                    return NULL;
                }

                m_ring = m_orderedCells.m_Atoms[runtimeAtomCnt].ring;
                runtimeAtomCnt = m_orderedCells.m_rt_gridCellAtomCnt;
                m_orderedCells.m_rt_gridCellAtomCnt = runtimeAtomCnt + 1;
                m_NextAtom = m_orderedCells.m_Atoms[runtimeAtomCnt].cellAtom;
            }
            else
            {
                m_NextAtom = m_NextAtom->Next;
            }

            atom_node = m_NextAtom->Ent;

            if (!atom_node->m_active)
            {
                break;
            }
        }
        
        return m_NextAtom;
    }

    void CGrid::gridRemoveNodeFromGrids(CNode* node)
    {
        for (u32 i = 0; i < node->m_AtomCnt; i++)
        {
            CGridAtom* atom = node->GetAtom(i);

            if (!atom->Next)
            {
                atom->Prev->Next = NULL;
            }
            else
            {
                atom->Prev->Next = atom->Next;
                atom->Next->Prev = atom->Prev;
            }

            if (m_AtomFreePtr < m_AtomFreeCnt - 1)
            {
                atom->Ent = NULL;
                atom->Next = NULL;
                atom->Prev = NULL;

                u32 ptr = m_AtomFreePtr;

                m_AtomFreePtr = ptr + 1;
                m_FreeAtoms[ptr] = atom;
            }
        }

        node->SetAtomCnt(0);
    }

    CGridAtom** CGrid::gridGetAtomBasePtr(s32 x, s32 y)
    {
        u32 xCellCount = 0;
        u32 yCellCount = 0;
        u32 cellLength = 0;

        if (x < 0)
            x = 0;

        xCellCount = m_CellCount.cx;
        cellLength = xCellCount - 1;

        if (x <= cellLength)
            cellLength = x;

        if (y < 0)
            y = 0;

        yCellCount = m_CellCount.cy - 1;

        if (y <= yCellCount)
            yCellCount = y;

        return m_Atoms + cellLength + yCellCount * xCellCount;
    }

    bool CGrid::gridGetNextIndex()
    {
        bool valid = false;

        if (m_PntCnt < 3)
        {
            if (m_zz < m_izb - 1)
            {
                valid = true;
                m_zz++;
            }
            else if (m_xx < m_ixb - 1)
            {
                valid = true;
                m_xx++;
                m_zz = m_iz;
                m_RowCellIn = 0;
            }
        }
        else
        {
            if (m_CurCellOut == 0 && m_zz < m_iz_next_row)
                m_iz_next_row = m_zz;

            if (m_iz_dir < 0)
            {
                if (m_CurCellOut == 0 && m_iz < m_zz)
                {
                    m_zz--;
                    return true;
                }

                m_zz = m_iz_start;
                m_iz_dir = 1;
                m_CurCellOut = 0;
            }

            if (m_RowCellIn == 0 || m_CurCellOut == 0 && m_zz < m_izb - 1)
            {
                valid = true;
                m_zz++;
            }
            else if (m_xx < m_ixb - 1)
            {
                m_xx++;

                if (m_iz_next_row == m_izb)
                {
                    m_zz = m_iz;
                    m_iz_dir = 1;
                }
                else
                {
                    m_zz = m_iz_next_row;

                    if (m_iz < m_zz)
                        m_zz++;

                    m_iz_dir = -1;
                }

                valid = true;
                m_iz_next_row = m_izb;
                m_iz_start = m_zz;
                m_RowCellIn = 0;
            }
            else
            {
                m_RowCellIn = 0;
                m_CurCellOut = 0;
                valid = false;
            }
        }

        return valid;
    }

    CNode* CGridAtom::GetCell()
    {
        if (Ent->m_type != (u32)CNode::TYPE::NODE_TYPE_CELL)
            return NULL;

        return Ent;
    }
}