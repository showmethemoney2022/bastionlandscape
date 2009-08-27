#include "stdafx.h"
#include "Landscape.h"
#include "../Display/Camera.h"
#include "../Display/Effect.h"
#include "../Display/EffectParam.h"

namespace ElixirEngine
{
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	LandscapeChunk::LandscapeChunk(Landscape& _rLandscape, DisplayRef _rDisplay, const unsigned int& _uLOD)
	:	DisplayObject(_rDisplay),
		m_rLandscape(_rLandscape),
		m_uStartVertexIndex(0),
		m_uLOD(_uLOD),
		m_pParent(NULL),
		m_oCenter(0.0f, 0.0f, 0.0f),
		m_oExtends(0.0f, 0.0f, 0.0f),
		m_pLODInfo(NULL),
		m_fMorphFactor(1.0f)
	{
		m_pChildren[ESubChild_NORTHWEST] =
		m_pChildren[ESubChild_NORTHEAST] =
		m_pChildren[ESubChild_SOUTHWEST] =
		m_pChildren[ESubChild_SOUTHEAST] = NULL;
	}

	LandscapeChunk::~LandscapeChunk()
	{

	}

	bool LandscapeChunk::Create(const boost::any& _rConfig)
	{
		bool bResult = true;
		CreateInfo* pInfo = boost::any_cast<CreateInfo*>(_rConfig);
		const Landscape::GlobalInfo& rGlobalInfo = m_rLandscape.GetGlobalInfo();
		m_pLODInfo = &(rGlobalInfo.m_pLODs[m_uLOD]);
		const unsigned int IndexX = pInfo->m_uX * rGlobalInfo.m_uQuadSize;
		const unsigned int IndexZ = pInfo->m_uZ * rGlobalInfo.m_uQuadSize;
		m_uStartVertexIndex = IndexX + IndexZ * m_pLODInfo->m_uVertexPerRawCount;

		// center
		const unsigned int uStartIndex = m_pLODInfo->m_uStartIndex;
		const unsigned int uStripSize = m_pLODInfo->m_uStripSize;
		Vector3 oTemp[2];
		m_rLandscape.GetVertexPosition(*m_pLODInfo, 0, m_uStartVertexIndex, oTemp[0]);
		m_rLandscape.GetVertexPosition(*m_pLODInfo, uStripSize - 1, m_uStartVertexIndex, oTemp[1]);
		m_oExtends.x = (oTemp[1].x - oTemp[0].x) / 2.0f;
		m_oExtends.y = 0.0f;
		m_oExtends.z = (oTemp[0].z - oTemp[1].z) / 2.0f;
		m_oCenter.x = oTemp[0].x + m_oExtends.x;
		m_oCenter.y = 0.0f;
		m_oCenter.z = oTemp[1].z + m_oExtends.z;

		if (0 < m_uLOD)
		{
			CreateInfo oLCCInfo;
			unsigned int uChild = ESubChild_NORTHWEST;
			for (unsigned int j = 0 ; 2 > j ; ++j)
			{
				for (unsigned int i = 0 ; 2 > i ; ++i)
				{
					LandscapeChunkPtr pLandscapeChunk = new LandscapeChunk(m_rLandscape, m_rDisplay, m_uLOD - 1);
					oLCCInfo.m_uX = pInfo->m_uX * 2 + i;
					oLCCInfo.m_uZ = pInfo->m_uZ * 2 + j;
					bResult = pLandscapeChunk->Create(boost::any(&oLCCInfo));
					if (false == bResult)
					{
						break;
					}
					m_pChildren[uChild] = pLandscapeChunk;
					++uChild;
				}
			}
		}

		return bResult;
	}

	void LandscapeChunk::Update()
	{

	}

	void LandscapeChunk::Release()
	{
	}

	void LandscapeChunk::RenderBegin()
	{
		DisplayEffectParamMORPHFACTOR::s_fMorphFactor = &m_fMorphFactor;
	}

	void LandscapeChunk::Render()
	{
		if (m_rLandscape.UseLODVertexBuffer(m_uLOD) && m_rLandscape.SetIndices())
		{
			const unsigned int uVertexCount = m_pLODInfo->m_uNumVertices;
			const unsigned int uStartIndex = m_pLODInfo->m_uStartIndex;
			const unsigned int uStripSize = m_pLODInfo->m_uStripSize - 2;
			m_rDisplay.GetDevicePtr()->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, m_uStartVertexIndex, 0, uVertexCount, uStartIndex, uStripSize);
		}
	}

	void LandscapeChunk::Traverse(LandscapeChunkPtrVecRef _rRenderList, const Vector3& _rCamPos, const float& _fPixelSize)
	{
		const Landscape::GlobalInfo& rGlobalInfo = m_rLandscape.GetGlobalInfo();
		MatrixPtr pWorld = m_rLandscape.GetWorldMatrix();
		const Vector3 oWorld(pWorld->_41, pWorld->_42, pWorld->_43);
		const Vector3 oDelta = oWorld + m_oCenter - _rCamPos;
		const float fExtends = D3DXVec3Length(&m_oExtends);

		if (DisplayCamera::ECollision_OUT != m_rDisplay.GetCurrentCamera()->CollisionWithSphere(oWorld + m_oCenter, fExtends))
		{
#if LANDSCAPE_USE_HIGHEST_LOD_ONLY
			if (0 == m_uLOD)
#else // LANDSCAPE_USE_HIGHEST_LOD_ONLY
#if 0
			Vector3 aPoints[8];
			aPoints[0] = oDelta + Vector3(m_oExtends.x, m_oExtends.y, m_oExtends.z);
			aPoints[1] = oDelta + Vector3(-m_oExtends.x, -m_oExtends.y, -m_oExtends.z);
			aPoints[2] = oDelta + Vector3(-m_oExtends.x, m_oExtends.y, m_oExtends.z);
			aPoints[3] = oDelta + Vector3(m_oExtends.x, -m_oExtends.y, -m_oExtends.z);
			aPoints[4] = oDelta + Vector3(m_oExtends.x, -m_oExtends.y, m_oExtends.z);
			aPoints[5] = oDelta + Vector3(-m_oExtends.x, m_oExtends.y, -m_oExtends.z);
			aPoints[6] = oDelta + Vector3(m_oExtends.x, m_oExtends.y, -m_oExtends.z);
			aPoints[7] = oDelta + Vector3(-m_oExtends.x, -m_oExtends.y, m_oExtends.z);
			float fDistance = FLT_MAX;
			for (unsigned int i = 0 ; 8 > i ; ++i)
			{
				const float fDelta = D3DXVec3Length(&aPoints[i]);
				fDistance = (fDistance > fDelta) ? fDelta : fDistance;
			}
			fDistance = (1.0f <= fDistance) ? fDistance : 1.0f;
#else
			const float fDelta = D3DXVec3Length(&oDelta);
			const float fRawDistance = (fDelta - fExtends);
			const float fDistance = (1.0f <= fRawDistance) ? fRawDistance : 1.0f;
#endif
			const float fVertexErrorLevel = (m_pLODInfo->m_uGeometricError / fDistance) * _fPixelSize;

			if (fVertexErrorLevel <= rGlobalInfo.m_fPixelErrorMax)
				//if ((rGlobalInfo.m_fPixelErrorMax <= fRatio) || (0 == m_uLOD))
				//const float fRatio = fDistance / fExtends;
				//if ((1.0f <= fRatio) || (0 == m_uLOD))
#endif
			{
				//m_fMorphFactor = ((2.0f * fVertexErrorLevel) / rGlobalInfo.m_fPixelErrorMax) - 1.0f;
				//m_fMorphFactor = (0.0f != fVertexErrorLevel) ? m_fMorphFactor : 1.0f;
				//m_fMorphFactor = (0.0f > m_fMorphFactor) ? 0.0f : ((1.0f < m_fMorphFactor) ? 1.0f : m_fMorphFactor);
				_rRenderList.push_back(this);
			}
			else if (0 != m_uLOD)
			{
				for (unsigned int i = 0 ; ESubChild_COUNT > i ; ++i)
				{
					m_pChildren[i]->Traverse(_rRenderList, _rCamPos, _fPixelSize);
				}
			}
		}
		else
		{
			++m_rLandscape.m_uOutOfFrustum;
		}
	}

	unsigned int LandscapeChunk::GetLODID() const
	{
		return m_uLOD;
	}
}