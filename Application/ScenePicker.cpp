#include "stdafx.h"
#include "../Application/Application.h"
#include "../Application/Scene.h"

namespace BastionGame
{
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	ScenePicker::ScenePicker()
	:	m_pScene(NULL),
		m_vTraversedObjects(),
		m_f3RayBegin(0.0f, 0.0f, 0.0),
		m_f3RayEnd(0.0f, 0.0f, 0.0),
		m_f3Pick(0.0f, 0.0f, 0.0),
		m_bActive(false)
	{
	}

	bool ScenePicker::Create(const boost::any& _rConfig)
	{
		CreateInfoPtr pInfo = boost::any_cast<CreateInfoPtr>(_rConfig);
		const bool bResult = (NULL != pInfo) && (NULL != pInfo->m_pScene);
		m_pScene = pInfo->m_pScene;
		return bResult;
	}

	void ScenePicker::Activate(const bool _bState)
	{
		m_bActive = _bState && (NULL != m_pScene);
	}

	void ScenePicker::Update(OctreeObjectPtrVecRef _rvOctreeObjects)
	{
		if (false != m_bActive)
		{
			DisplayPtr pDisplay = Display::GetInstance();

			DisplayObjectPtr pPickCursorObject = NULL;

			// pick cursor
			DisplayNormalProcessPtr pNormalProcess = pDisplay->GetCurrentNormalProcess();
			if ((NULL != pNormalProcess) && (MakeKey(string("base")) == pNormalProcess->GetNameKey()))
			{
				pPickCursorObject = static_cast<DisplayObjectPtr>(m_pScene->GetHierarchyObject(MakeKey(string("picking00"))));
				const Key uCurrentRenderStage = pDisplay->GetCurrentRenderStage()->GetNameKey();
				const Key uRenderStageName = pPickCursorObject->GetRenderStage();
				if (uCurrentRenderStage == uRenderStageName)
				{
					m_vTraversedObjects = _rvOctreeObjects;
					OctreeObjectPtrVec vPickedOcytreeObjects;
					DisplayCameraPtr pCamera = pDisplay->GetCurrentCamera();
					const Vector3 f3MousePos = m_pScene->GetApplication().GetMousePos();
					Vector3 f3ScreenBegin(f3MousePos.x, f3MousePos.y, 0.0f);
					Vector3 f3ScreenEnd(f3MousePos.x, f3MousePos.y, 1.0f);
					pDisplay->Unproject(&f3ScreenBegin, &m_f3RayBegin, pCamera);
					pDisplay->Unproject(&f3ScreenEnd, &m_f3RayEnd, pCamera);
					UpdatePicking(&m_f3RayBegin, &m_f3RayEnd, &m_f3Pick, vPickedOcytreeObjects);
					pDisplay->RenderRequest(uRenderStageName, pPickCursorObject);
					// uncomment following line to restrict rendered objects to the ones colliding with the pick ray
					//_rvOctreeObjects = vPickedOcytreeObjects;
				}
			}
		}
	}

	void ScenePicker::PickObjects(const Vector3& _f3RayBegin, const Vector3& _f3RayEnd, CoreObjectPtrVec& _rvObjects, OctreeObjectPtrVecRef _rvOctreeObjects)
	{
		const fsVector3 fs3RayBegin(_f3RayBegin.x, _f3RayBegin.y, _f3RayBegin.z);
		const fsVector3 fs3RayEnd(_f3RayEnd.x, _f3RayEnd.y, _f3RayEnd.z);
		fsVector3 fs3Intersect1;
		fsVector3 fs3Intersect2;

		_rvOctreeObjects.clear();

		CoreObjectPtrVec::iterator iObjectEnd = _rvObjects.end();
		OctreeObjectPtrVec::iterator iOctreeObject = m_vTraversedObjects.begin();
		OctreeObjectPtrVec::iterator iOctreeEnd = m_vTraversedObjects.end();
		while (iOctreeEnd != iOctreeObject)
		{
			OctreeObjectPtr pOctreeObject = *iOctreeObject;
			if (false != pOctreeObject->RayIntersect(fs3RayBegin, fs3RayEnd, fs3Intersect1, fs3Intersect2))
			{
				CoreObjectPtr pCoreObject = dynamic_cast<CoreObjectPtr>(pOctreeObject);
				if ((NULL != pCoreObject) && (iObjectEnd == find(_rvObjects.begin(), iObjectEnd, pCoreObject)))
				{
					_rvObjects.push_back(pCoreObject);
					iObjectEnd = _rvObjects.end();

					const CoreObjectPtrVec& rvChildren = pCoreObject->GetChildren();
					if (false == rvChildren.empty())
					{
						CoreObjectPtrVec::const_iterator iChild = rvChildren.begin();
						CoreObjectPtrVec::const_iterator iChildEnd = rvChildren.end();
						while (iChildEnd != iChild)
						{
							if (iObjectEnd == find(_rvObjects.begin(), iObjectEnd, *iChild))
							{
								_rvObjects.push_back(*iChild);
								iObjectEnd = _rvObjects.end();
							}
							++iChild;
						}
					}
				}
				_rvOctreeObjects.push_back(pOctreeObject);
			}
			++iOctreeObject;
		}
	}

	void ScenePicker::UpdatePicking(const Vector3Ptr _f3RayBegin, const Vector3Ptr _f3RayEnd, Vector3Ptr _f3Out, OctreeObjectPtrVecRef _rvOctreeObjects)
	{
		DisplayObjectPtr pDisplayObject = static_cast<DisplayObjectPtr>(m_pScene->GetHierarchyObject(MakeKey(string("picking00"))));
		if (NULL != pDisplayObject)
		{
			DisplayPtr pDisplay = Display::GetInstance();

			CoreObjectPtrVec vObjects;
			PickObjects(*_f3RayBegin, *_f3RayEnd, vObjects, _rvOctreeObjects);

			if (false == vObjects.empty())
			{
				Vector3 f3Delta;
				Vector3 f3Pick;
				Vector3 f3Intersect;
				float fNearDist = FLT_MAX;

				CoreObjectPtrVec::iterator iObject = vObjects.begin();
				CoreObjectPtrVec::iterator iEnd = vObjects.end();
				while (iEnd != iObject)
				{
					DisplayObjectPtr pDisplayObject = dynamic_cast<DisplayObjectPtr>(*iObject);
					if (NULL != pDisplayObject)
					{
						if (false != pDisplayObject->RayIntersect(*_f3RayBegin, *_f3RayEnd, f3Intersect))
						{
							f3Delta = f3Intersect - *_f3RayBegin;
							const float fLength = D3DXVec3Length(&f3Delta);
							if (fNearDist > fLength)
							{
								fNearDist = fLength;
								f3Pick = f3Intersect;
							}
						}
					}
					++iObject;
				}

				if (FLT_MAX != fNearDist)
				{
					Matrix mPos;
					D3DXMatrixTranslation(&mPos, f3Pick.x, f3Pick.y, f3Pick.z);
					pDisplayObject->SetWorldMatrix(mPos);
					*_f3Out = f3Pick;
				}
			}
		}
	}
}
