#ifndef ZGRID_H
#define ZGRID_H
#include "grid_clutter.h"

#include "gamez/zNode/znode.h"
#include "gamez/zNode/node_light.h"

namespace zdb
{
	/// <summary>
	/// Doubly linked list of atoms that occupy a single cell.
	/// </summary>
	class CGridAtom
	{
	public:
		CNode* Ent;
		CGridAtom* Next;
		CGridAtom* Prev;
	};

	class CCell : public CNode
	{
	public:
		struct GRIDCELLATOM
		{
			CGridAtom* cellAtom;
			s32 ring;
			s32 x;
			s32 z;
		};

		CCell(const CPnt3D* origin, f32 offset);
	private:
		CLightList m_lightList;
		CClutterList m_clutterList;
		CDynTexList m_dyntexlist;
	};

	class COrderedCell
	{
	public:
		s32 m_gridCellAtomCnt;
		s32 m_rt_gridCellAtomCnt;
		s32 m_viewX;
		s32 m_viewZ;
		CCell::GRIDCELLATOM m_Atoms[MAX_NUM_CELL_ATOMS];
	};

	class CGrid : tag_GRID_PARAMS
	{
	public:
		CGrid();
		~CGrid();

		bool Create(CWorld* world, const tag_GRID_PARAMS* params);
		bool Read(CSaveLoad& sload);

		void Insert(CNode* node);

		CGridAtom* StartTraversalOrdered();
		CGridAtom* GetNextAtomOrdered();

		static u32 N_ATOMS;
	private:
		CFRect m_extents;
		CWorld* m_world;

		bool m_ClutterCreated;

		CGridAtom** m_Atoms;
		CGridAtom* m_AtomBuf;
		CGridAtom** m_FreeAtoms;
		s32 m_AtomFreeCnt;
		s32 m_AtomFreePtr;

		f32 m_InvCellDim;
		f32 m_GridCellToI;
		f32 m_ClutterToI;
		s32 m_TickNum;

		bool m_LineWalk;
		u8 m_RowCellIn;
		u8 m_CurCellOut;

		CPnt3D m_Pnts[5];
		CPnt3D m_Edge[5];

		s32 m_PntCnt;

		s32 m_ix;
		s32 m_ixb;

		s32 m_xx;
		s32 m_iz;

		s32 m_izb;
		s32 m_zz;
		s32 m_iz_start;
		s32 m_iz_next_row;
		s32 m_iz_dir;
		CGridAtom* m_NextAtom;

		CPnt3D m_origin;
		s32 m_ring;

		COrderedCell m_orderedCells;
	};
}

#endif