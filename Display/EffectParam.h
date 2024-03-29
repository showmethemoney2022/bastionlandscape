#ifndef __EFFECTPARAM_H__
#define __EFFECTPARAM_H__

#include "../Display/DisplayTypes.h"
#include "../Display/Texture.h"
#include "../Display/Camera.h"
#include "../Core/CoreTypes.h"
#include "../Core/Scripting.h"

namespace ElixirEngine
{
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParam : public CoreObject
	{
	public:
		struct CreateInfo
		{
			CreateInfo()
			:	m_oLuaObject(NULL),
				m_pMaterial(NULL),
				m_hSemantic(NULL),
				m_uSemanticKey(0)
			{
			}

			CreateInfo(
				LuaObject			_oLuaObject,
				DisplayMaterialPtr	_pMaterial,
				Handle				_hSemantic,
				Key					_uSemanticKey)
			:	m_oLuaObject(_oLuaObject),
				m_pMaterial(_pMaterial),
				m_hSemantic(_hSemantic),
				m_uSemanticKey(_uSemanticKey)
			{
			}

			LuaObject			m_oLuaObject;
			DisplayMaterialPtr	m_pMaterial;
			Handle				m_hSemantic;
			Key					m_uSemanticKey;
		};
		typedef CreateInfo* CreateInfoPtr;
		typedef CreateInfo& CreateInfoRef;

	public:
		DisplayEffectParam(DisplayMaterialRef _rMaterial)
		:	m_rMaterial(_rMaterial),
			m_pEffect(_rMaterial.GetEffect()->GetEffect()),
			m_hData(NULL),
			m_uSemanticKey(0)
		{

		}

		virtual ~DisplayEffectParam()
		{

		}

		virtual bool Create(const boost::any& _rConfig)
		{
			CreateInfo* pInfo = boost::any_cast<CreateInfo*>(_rConfig);

			m_uSemanticKey = pInfo->m_uSemanticKey;
			m_hData = pInfo->m_hSemantic;
			bool bResult = ((NULL != m_hData) && (0 != m_uSemanticKey));

			if (false == pInfo->m_oLuaObject.IsNil())
			{
				bResult = CreateFromLuaConfig(*pInfo);
			}

			return bResult;
		}

		virtual bool Use() = 0;

	protected:
		virtual bool CreateFromLuaConfig(CreateInfoRef _rInfo)
		{
			LuaObject oSemantic = _rInfo.m_oLuaObject["semantic"];

			if (false == oSemantic.IsNil())
			{
				const string strSemanticName = oSemantic.GetString();
				m_uSemanticKey = MakeKey(strSemanticName);
				m_hData = _rInfo.m_pMaterial->GetEffect()->GetHandleBySemanticKey(m_uSemanticKey);
			}

			return ((NULL != m_hData) && (0 != m_uSemanticKey));
		}

	protected:
		DisplayMaterialRef	m_rMaterial;
		EffectPtr			m_pEffect;
		Handle				m_hData;
		Key					m_uSemanticKey;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamWORLDVIEWPROJ : public DisplayEffectParam
	{
	public:
		DisplayEffectParamWORLDVIEWPROJ(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial),
			m_oWVP(),
			m_pWorld(NULL),
			m_pViewProj(NULL)
		{

		}

		virtual ~DisplayEffectParamWORLDVIEWPROJ()
		{

		}

		virtual bool Use()
		{
			m_pViewProj = Display::GetInstance()->GetCurrentCamera()->GetMatrix(DisplayCamera::EMatrix_VIEWPROJ);
			m_pWorld = Display::GetInstance()->GetCurrentWorldMatrix();
			if ((NULL != m_pWorld) && (NULL != m_pViewProj))
			{
				HRESULT hResult = m_pEffect->SetMatrix(m_hData, D3DXMatrixMultiply(&m_oWVP, m_pWorld, m_pViewProj));
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamWORLDVIEWPROJ(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		Matrix		m_oWVP;
		MatrixPtr	m_pWorld;
		MatrixPtr	m_pViewProj;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamWORLD : public DisplayEffectParam
	{
	public:
		DisplayEffectParamWORLD(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial),
			m_pWorld(NULL)
		{

		}

		virtual ~DisplayEffectParamWORLD()
		{

		}

		virtual bool Use()
		{
			m_pWorld = Display::GetInstance()->GetCurrentWorldMatrix();
			if (NULL != m_pWorld)
			{
				HRESULT hResult = m_pEffect->SetMatrix(m_hData, m_pWorld);
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamWORLD(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		MatrixPtr	m_pWorld;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamVIEW : public DisplayEffectParam
	{
	public:
		DisplayEffectParamVIEW(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial),
			m_pView(NULL)
		{

		}

		virtual ~DisplayEffectParamVIEW()
		{

		}

		virtual bool Use()
		{
			m_pView = Display::GetInstance()->GetCurrentCamera()->GetMatrix(DisplayCamera::EMatrix_VIEW);
			if (NULL != m_pView)
			{
				HRESULT hResult = m_pEffect->SetMatrix(m_hData, m_pView);
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamVIEW(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		MatrixPtr	m_pView;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamWORLDVIEW : public DisplayEffectParam
	{
	public:
		DisplayEffectParamWORLDVIEW(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial),
			m_m4WorldView(),
			m_pView(NULL),
			m_pWorld(NULL)
		{

		}

		virtual ~DisplayEffectParamWORLDVIEW()
		{

		}

		virtual bool Use()
		{
			m_pView = Display::GetInstance()->GetCurrentCamera()->GetMatrix(DisplayCamera::EMatrix_VIEW);
			m_pWorld = Display::GetInstance()->GetCurrentWorldMatrix();
			if ((NULL != m_pWorld) && (NULL != m_pView))
			{
				D3DXMatrixMultiply(&m_m4WorldView, m_pWorld, m_pView);
				HRESULT hResult = m_pEffect->SetMatrix(m_hData, &m_m4WorldView);
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamWORLDVIEW(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		Matrix		m_m4WorldView;
		MatrixPtr	m_pView;
		MatrixPtr	m_pWorld;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamVIEWINV : public DisplayEffectParam
	{
	public:
		DisplayEffectParamVIEWINV(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial),
			m_pViewInv(NULL)
		{

		}

		virtual ~DisplayEffectParamVIEWINV()
		{

		}

		virtual bool Use()
		{
			m_pViewInv = Display::GetInstance()->GetCurrentCamera()->GetMatrix(DisplayCamera::EMatrix_VIEWINV);
			if (NULL != m_pViewInv)
			{
				HRESULT hResult = m_pEffect->SetMatrix(m_hData, m_pViewInv);
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamVIEWINV(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		MatrixPtr	m_pViewInv;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamVIEWPROJ : public DisplayEffectParam
	{
	public:
		DisplayEffectParamVIEWPROJ(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial),
			m_pViewProj(NULL)
		{

		}

		virtual ~DisplayEffectParamVIEWPROJ()
		{

		}

		virtual bool Use()
		{
			m_pViewProj = Display::GetInstance()->GetCurrentCamera()->GetMatrix(DisplayCamera::EMatrix_VIEWPROJ);
			if (NULL != m_pViewProj)
			{
				HRESULT hResult = m_pEffect->SetMatrix(m_hData, m_pViewProj);
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamVIEWPROJ(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		MatrixPtr	m_pViewProj;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamPROJ : public DisplayEffectParam
	{
	public:
		DisplayEffectParamPROJ(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial),
			m_pProj(NULL)
		{

		}

		virtual ~DisplayEffectParamPROJ()
		{

		}

		virtual bool Use()
		{
			m_pProj = Display::GetInstance()->GetCurrentCamera()->GetMatrix(DisplayCamera::EMatrix_PROJ);
			if (NULL != m_pProj)
			{
				HRESULT hResult = m_pEffect->SetMatrix(m_hData, m_pProj);
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamPROJ(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		MatrixPtr	m_pProj;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamWORLDINVTRANSPOSE : public DisplayEffectParam
	{
	public:
		DisplayEffectParamWORLDINVTRANSPOSE(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial),
			m_pWorld(NULL)
		{

		}

		virtual ~DisplayEffectParamWORLDINVTRANSPOSE()
		{

		}

		virtual bool Use()
		{
			m_pWorld = Display::GetInstance()->GetCurrentWorldInvTransposeMatrix();
			if (NULL != m_pWorld)
			{
				HRESULT hResult = m_pEffect->SetMatrix(m_hData, m_pWorld);
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamWORLDINVTRANSPOSE(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		MatrixPtr	m_pWorld;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamENVIRONMENTTEX : public DisplayEffectParam
	{
	public:
		DisplayEffectParamENVIRONMENTTEX(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial),
			m_pTexture(NULL)
		{

		}

		virtual ~DisplayEffectParamENVIRONMENTTEX()
		{

		}

		virtual bool Create(const boost::any& _rConfig)
		{
			CreateInfo* pInfo = boost::any_cast<CreateInfo*>(_rConfig);
			bool bResult = false;

			if (false == pInfo->m_oLuaObject.IsNil())
			{
				bResult = CreateFromLuaConfig(*pInfo);
			}

			return bResult;
		}

		virtual bool Use()
		{
			if (NULL == m_pTexture)
			{
				DisplayTextureManagerPtr pTextureManager = Display::GetInstance()->GetTextureManager();
				if (false != pTextureManager->Load(m_strName, m_strPath, DisplayTexture::EType_CUBE))
				{
					m_pTexture = pTextureManager->Get(m_strName);
				}
			}
			if (NULL != m_pTexture)
			{
				HRESULT hResult = m_pEffect->SetTexture(m_hData, m_pTexture->GetBase());
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamENVIRONMENTTEX(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		virtual bool CreateFromLuaConfig(CreateInfoRef _rInfo)
		{
			LuaObject oLuaValue = _rInfo.m_oLuaObject["semantic"];
			bool bResult = (false == oLuaValue.IsNil());

			if (false != bResult)
			{
				const string strSemanticName = oLuaValue.GetString();
				m_uSemanticKey = MakeKey(strSemanticName);
				m_hData = _rInfo.m_pMaterial->GetEffect()->GetHandleBySemanticKey(m_uSemanticKey);
				bResult = (NULL != m_hData);
			}
			if (false != bResult)
			{
				oLuaValue = _rInfo.m_oLuaObject["name"];
				bool bResult = (false == oLuaValue.IsNil());
				if (false != bResult)
				{
					m_strName = oLuaValue.GetString();
				}
			}
			if (false != bResult)
			{
				oLuaValue = _rInfo.m_oLuaObject["value"];
				bool bResult = (false == oLuaValue.IsNil());
				if (false != bResult)
				{
					m_strPath = oLuaValue.GetString();
				}
			}

			return bResult;
		}

	protected:
		DisplayTexturePtr	m_pTexture;
		string				m_strName;
		string				m_strPath;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamNORMALTEX : public DisplayEffectParam
	{
	public:
		DisplayEffectParamNORMALTEX(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial),
			m_pTexture(NULL)
		{

		}

		virtual ~DisplayEffectParamNORMALTEX()
		{

		}

		virtual bool Create(const boost::any& _rConfig)
		{
			CreateInfo* pInfo = boost::any_cast<CreateInfo*>(_rConfig);
			bool bResult = false;

			if (false == pInfo->m_oLuaObject.IsNil())
			{
				bResult = CreateFromLuaConfig(*pInfo);
			}

			return bResult;
		}

		virtual bool Use()
		{
			if (NULL == m_pTexture)
			{
				DisplayTextureManagerPtr pTextureManager = Display::GetInstance()->GetTextureManager();
				if (false != pTextureManager->Load(m_strName, m_strPath, DisplayTexture::EType_2D))
				{
					m_pTexture = pTextureManager->Get(m_strName);
				}
			}
			if (NULL != m_pTexture)
			{
				HRESULT hResult = m_pEffect->SetTexture(m_hData, m_pTexture->GetBase());
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamNORMALTEX(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		virtual bool CreateFromLuaConfig(CreateInfoRef _rInfo)
		{
			LuaObject oLuaValue = _rInfo.m_oLuaObject["semantic"];
			bool bResult = (false == oLuaValue.IsNil());

			if (false != bResult)
			{
				const string strSemanticName = oLuaValue.GetString();
				m_uSemanticKey = MakeKey(strSemanticName);
				m_hData = _rInfo.m_pMaterial->GetEffect()->GetHandleBySemanticKey(m_uSemanticKey);
				bResult = (NULL != m_hData);
			}
			if (false != bResult)
			{
				oLuaValue = _rInfo.m_oLuaObject["name"];
				bool bResult = (false == oLuaValue.IsNil());
				if (false != bResult)
				{
					m_strName = oLuaValue.GetString();
				}
			}
			if (false != bResult)
			{
				oLuaValue = _rInfo.m_oLuaObject["value"];
				bool bResult = (false == oLuaValue.IsNil());
				if (false != bResult)
				{
					m_strPath = oLuaValue.GetString();
				}
			}

			return bResult;
		}

	protected:
		DisplayTexturePtr	m_pTexture;
		string				m_strName;
		string				m_strPath;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamFLOAT : public DisplayEffectParam
	{
	public:
		DisplayEffectParamFLOAT(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial)
		{

		}

		virtual ~DisplayEffectParamFLOAT()
		{

		}

		virtual bool Use()
		{
			m_pData = m_rMaterial.GetMaterialManager().GetFloatBySemantic(m_uSemanticKey);
			if (NULL != m_pData)
			{
				HRESULT hResult = m_pEffect->SetFloat(m_hData, *m_pData);
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamFLOAT(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		FloatPtr	m_pData;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamVECTOR2 : public DisplayEffectParam
	{
	public:
		DisplayEffectParamVECTOR2(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial)
		{

		}

		virtual ~DisplayEffectParamVECTOR2()
		{

		}

		virtual bool Use()
		{
			m_pData = m_rMaterial.GetMaterialManager().GetVector2BySemantic(m_uSemanticKey);
			if (NULL != m_pData)
			{
				m_oData.x = m_pData->x;
				m_oData.y = m_pData->y;
				HRESULT hResult = m_pEffect->SetVector(m_hData, &m_oData);
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamVECTOR2(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		Vector4		m_oData;
		Vector2*	m_pData;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamVECTOR3 : public DisplayEffectParam
	{
	public:
		DisplayEffectParamVECTOR3(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial)
		{

		}

		virtual ~DisplayEffectParamVECTOR3()
		{

		}

		virtual bool Use()
		{
			m_pData = m_rMaterial.GetMaterialManager().GetVector3BySemantic(m_uSemanticKey);
			if (NULL != m_pData)
			{
				m_oData.x = m_pData->x;
				m_oData.y = m_pData->y;
				m_oData.z = m_pData->z;
				HRESULT hResult = m_pEffect->SetVector(m_hData, &m_oData);
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamVECTOR3(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		Vector4		m_oData;
		Vector3*	m_pData;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamVECTOR4 : public DisplayEffectParam
	{
	public:
		DisplayEffectParamVECTOR4(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial)
		{

		}

		virtual ~DisplayEffectParamVECTOR4()
		{

		}

		virtual bool Use()
		{
			m_pData = m_rMaterial.GetMaterialManager().GetVector4BySemantic(m_uSemanticKey);
			if (NULL != m_pData)
			{
				HRESULT hResult = m_pEffect->SetVector(m_hData, m_pData);
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamVECTOR4(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		Vector4*	m_pData;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamMATRIX : public DisplayEffectParam
	{
	public:
		DisplayEffectParamMATRIX(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial)
		{

		}

		virtual ~DisplayEffectParamMATRIX()
		{

		}

		virtual bool Use()
		{
			m_pData = m_rMaterial.GetMaterialManager().GetMatrixBySemantic(m_uSemanticKey);
			if (NULL != m_pData)
			{
				HRESULT hResult = m_pEffect->SetMatrix(m_hData, m_pData);
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamMATRIX(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		MatrixPtr	m_pData;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamDIFFUSETEX : public DisplayEffectParam
	{
	public:
		DisplayEffectParamDIFFUSETEX(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial),
			m_pTexture(NULL)
		{

		}

		virtual ~DisplayEffectParamDIFFUSETEX()
		{

		}

		virtual bool Create(const boost::any& _rConfig)
		{
			CreateInfo* pInfo = boost::any_cast<CreateInfo*>(_rConfig);
			bool bResult = false;

			if (false == pInfo->m_oLuaObject.IsNil())
			{
				bResult = CreateFromLuaConfig(*pInfo);
			}

			return bResult;
		}

		virtual bool Use()
		{
			if (NULL == m_pTexture)
			{
				DisplayTextureManagerPtr pTextureManager = Display::GetInstance()->GetTextureManager();
				pTextureManager->Load(m_strName, m_strPath, DisplayTexture::EType_2D);
				m_pTexture = pTextureManager->Get(m_strName);
			}
			if (NULL != m_pTexture)
			{
				HRESULT hResult = m_pEffect->SetTexture(m_hData, m_pTexture->GetBase());
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamDIFFUSETEX(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		virtual bool CreateFromLuaConfig(CreateInfoRef _rInfo)
		{
			LuaObject oLuaValue = _rInfo.m_oLuaObject["semantic"];
			bool bResult = (false == oLuaValue.IsNil());

			if (false != bResult)
			{
				const string strSemanticName = oLuaValue.GetString();
				m_uSemanticKey = MakeKey(strSemanticName);
				m_hData = _rInfo.m_pMaterial->GetEffect()->GetHandleBySemanticKey(m_uSemanticKey);
				bResult = (NULL != m_hData);
			}
			if (false != bResult)
			{
				oLuaValue = _rInfo.m_oLuaObject["name"];
				bool bResult = (false == oLuaValue.IsNil());
				if (false != bResult)
				{
					m_strName = oLuaValue.GetString();
				}
			}
			if (false != bResult)
			{
				oLuaValue = _rInfo.m_oLuaObject["value"];
				bool bResult = (false == oLuaValue.IsNil());
				if (false != bResult)
				{
					m_strPath = oLuaValue.GetString();
				}
			}

			return bResult;
		}

	protected:
		DisplayTexturePtr	m_pTexture;
		string				m_strName;
		string				m_strPath;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamSEMANTICTEX : public DisplayEffectParam
	{
	public:
		DisplayEffectParamSEMANTICTEX(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial),
			m_pTexture(NULL)
		{

		}

		virtual ~DisplayEffectParamSEMANTICTEX()
		{

		}

		virtual bool Use()
		{
			DisplayTextureManagerPtr pTextureManager = Display::GetInstance()->GetTextureManager();
			m_pTexture = pTextureManager->GetBySemantic(m_uSemanticKey);
			if (NULL != m_pTexture)
			{
				HRESULT hResult = m_pEffect->SetTexture(m_hData, m_pTexture->GetBase());
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamSEMANTICTEX(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		DisplayTexturePtr	m_pTexture;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamFRUSTUMCORNERS : public DisplayEffectParam
	{
	public:
		DisplayEffectParamFRUSTUMCORNERS(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial)
		{

		}

		virtual ~DisplayEffectParamFRUSTUMCORNERS()
		{

		}

		virtual bool Use()
		{
			Vector3* pData = m_rMaterial.GetMaterialManager().GetVector3BySemantic(m_uSemanticKey);
			if (NULL != pData)
			{
				for (UInt i = 0 ; DisplayCamera::EFrustumCorner_COUNT > i ; ++i)
				{
					m_aData[i] = Vector4(pData[i].x, pData[i].y, pData[i].z, 0.0f);
				}
				HRESULT hResult = m_pEffect->SetVectorArray(m_hData, m_aData, DisplayCamera::EFrustumCorner_COUNT);
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamFRUSTUMCORNERS(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		Vector4	m_aData[DisplayCamera::EFrustumCorner_COUNT];

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayEffectParamSTRUCT : public DisplayEffectParam
	{
	public:
		DisplayEffectParamSTRUCT(DisplayMaterialRef _rMaterial)
		:	DisplayEffectParam(_rMaterial),
			m_pData(NULL),
			m_uSize(0)
		{

		}

		virtual ~DisplayEffectParamSTRUCT()
		{

		}

		virtual bool Use()
		{
			m_pData = m_rMaterial.GetMaterialManager().GetStructBySemantic(m_uSemanticKey, m_uSize);
			if (NULL != m_pData)
			{
				HRESULT hResult = m_pEffect->SetValue(m_hData, m_pData, m_uSize);
				return SUCCEEDED(hResult);
			}
			return false;
		}

		static DisplayEffectParamPtr CreateParam(const boost::any& _rConfig)
		{
			DisplayEffectParam::CreateInfo* pDEPCInfo = boost::any_cast<DisplayEffectParam::CreateInfo*>(_rConfig);
			DisplayEffectParamPtr pParam = new DisplayEffectParamSTRUCT(*(pDEPCInfo->m_pMaterial));
			if (false == pParam->Create(_rConfig))
			{
				CoreObject::ReleaseDeleteReset(pParam);
			}
			return pParam;
		}

	protected:
		VoidPtr m_pData;
		UInt	m_uSize;

	private:
	};
}

#endif // __EFFECTPARAM_H__