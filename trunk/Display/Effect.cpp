#include "stdafx.h"
#include "../Display/Camera.h"
#include "../Display/Effect.h"
#include "../Display/EffectParam.h"
#include "../Core/File.h"

namespace ElixirEngine
{
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	float* DisplayEffectParamTIME::s_fTime = NULL;
	float* DisplayEffectParamMORPHFACTOR::s_fMorphFactor = NULL;

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	DisplayEffect::DisplayEffect(DisplayRef _rDisplay)
	:	CoreObject(),
		m_rDisplay(_rDisplay),
		m_pEffect(NULL),
		m_vRenderList()
	{

	}

	DisplayEffect::~DisplayEffect()
	{

	}

	bool DisplayEffect::Create(const boost::any& _rConfig)
	{
		CreateInfo* pInfo = boost::any_cast<CreateInfo*>(_rConfig);
		FilePtr pFile = FS::GetRoot()->OpenFile(pInfo->m_strPath, pInfo->m_bIsText ? FS::EOpenMode_READTEXT : FS::EOpenMode_READBINARY);
		bool bResult = (NULL != pFile);
		if (false != bResult)
		{
			int sSize = pFile->Size();
			char* pSourceCode = new char[sSize];
			sSize = pFile->Read(pSourceCode, sSize);
			FS::GetRoot()->CloseFile(pFile);

			BufferPtr pCompilErrors;
			HRESULT hResult = D3DXCreateEffect(
				m_rDisplay.GetDevicePtr(),
				pSourceCode,
				sSize,
				NULL, // D3DXMACRO Defines,
				NULL, // LPD3DXINCLUDE Includes,
				D3DXSHADER_SKIPOPTIMIZATION | D3DXSHADER_DEBUG | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY,
				NULL, // LPD3DXEFFECTPOOL Pool,
				&m_pEffect,
				&pCompilErrors);

			bResult = SUCCEEDED(hResult);
			if (false == bResult)
			{
				char* ptr = static_cast<char*>(pCompilErrors->GetBufferPointer());
				int a = 0;
				++a;
			}
		}
		return bResult;
	}

	void DisplayEffect::Update()
	{

	}

	void DisplayEffect::Release()
	{
		if (NULL != m_pEffect)
		{
			m_pEffect->Release();
			m_pEffect = NULL;
		}
	}

	void DisplayEffect::RenderRequest(DisplayMaterialPtr _pDisplayMaterial)
	{
		if (m_vRenderList.end() == find(m_vRenderList.begin(), m_vRenderList.end(), _pDisplayMaterial))
		{
			m_vRenderList.push_back(_pDisplayMaterial);
		}
	}

	void DisplayEffect::Render()
	{
		struct RenderMaterialFunction
		{
			void Do(DisplayMaterialPtr _pDisplayMaterial)
			{
				_pDisplayMaterial->Render();
			}
		};

		static RenderMaterialFunction oRMF;
		for_each(m_vRenderList.begin(), m_vRenderList.end(), boost::bind(&RenderMaterialFunction::Do, &oRMF, _1));
		m_vRenderList.clear();
	}

	EffectPtr DisplayEffect::GetEffect()
	{
		return m_pEffect;
	}

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	DisplayMaterial::DisplayMaterial(DisplayMaterialManagerRef _rMaterialManager)
	:	CoreObject(),
		m_rMaterialManager(_rMaterialManager),
		m_pEffect(NULL),
		m_vRenderList(),
		m_vParams(),
		m_hTechnique(NULL)
	{

	}

	DisplayMaterial::~DisplayMaterial()
	{

	}

	bool DisplayMaterial::Create(const boost::any& _rConfig)
	{
		CreateInfo* pInfo = boost::any_cast<CreateInfo*>(_rConfig);
		bool bResult = (NULL != pInfo->m_pEffect);

		if (false != bResult)
		{
			m_pEffect = pInfo->m_pEffect;

			const unsigned int uBufferSize = 1024;
			char szBuffer[uBufferSize];
			const int uCount = pInfo->m_pConfig->GetCount("material.params");
			string strSemanticValue;

			for (int i = 0 ; uCount > i ; ++i)
			{
				_snprintf(szBuffer, uBufferSize, "material.params.[%u]", i);
				ConfigShortcutPtr pShortcut = pInfo->m_pConfig->GetShortcut(string(szBuffer));
				if (NULL == pShortcut)
				{
					break;
				}

				bResult = pInfo->m_pConfig->GetValue(pShortcut, "semantic", strSemanticValue);
				if (false == bResult)
				{
					break;
				}

				DisplayEffectParam::CreateInfo oDEPCInfo = { pInfo->m_pConfig, pShortcut, this };
				DisplayEffectParamPtr pParam = m_rMaterialManager.CreateParam(strSemanticValue, boost::any(&oDEPCInfo));
				if (false == bResult)
				{
					break;
				}

				m_vParams.push_back(pParam);
			}
		}

		if (false != bResult)
		{
			string strTechniqueValue;
			if (pInfo->m_pConfig->GetValue("material.technique", strTechniqueValue))
			{
				m_hTechnique = m_pEffect->GetEffect()->GetTechniqueByName(strTechniqueValue.c_str());
			}
			else
			{
				m_hTechnique = m_pEffect->GetEffect()->GetTechnique(0);
			}
		}


		return bResult;
	}

	void DisplayMaterial::Update()
	{

	}

	void DisplayMaterial::Release()
	{
		struct ParamReleaseAndDeleteFunction
		{
			ParamReleaseAndDeleteFunction(DisplayMaterialManager& _rMaterialManager)
			:	m_rMaterialManager(_rMaterialManager)
			{
			}

			void operator()(DisplayEffectParamPtr _pParam)
			{
				m_rMaterialManager.ReleaseParam(_pParam);
			}

			DisplayMaterialManager&	m_rMaterialManager;
		};
		for_each(m_vParams.begin(), m_vParams.end(), ParamReleaseAndDeleteFunction(m_rMaterialManager));
		m_vParams.clear();
	}

	void DisplayMaterial::RenderRequest(DisplayObjectPtr _pDisplayObject)
	{
		if (m_vRenderList.end() == find(m_vRenderList.begin(), m_vRenderList.end(), _pDisplayObject))
		{
			m_vRenderList.push_back(_pDisplayObject);
		}
	}

	void DisplayMaterial::Render()
	{
		struct RenderObjectFunction
		{
			RenderObjectFunction(DisplayMaterialPtr _pMaterial)
			:	m_pMaterial(_pMaterial)
			{

			}

			void operator() (DisplayObjectPtr _pDisplayObject)
			{
				EffectPtr pEffect = m_pMaterial->GetEffect()->GetEffect();
				unsigned int uPassCount;
				pEffect->Begin(&uPassCount, 0);
				m_pMaterial->GetMaterialManager().GetDisplay().SetCurrentWorldMatrix(_pDisplayObject->GetWorldMatrix());
				m_pMaterial->UseParams();
				for (unsigned int iPass = 0 ; iPass < uPassCount ; ++iPass)
				{
					pEffect->BeginPass(iPass);
					_pDisplayObject->RenderBegin();
					pEffect->CommitChanges();
					_pDisplayObject->Render();
					_pDisplayObject->RenderEnd();
					pEffect->EndPass();
				}
				pEffect->End();
			}

			DisplayMaterialPtr	m_pMaterial;
		};
		m_pEffect->GetEffect()->SetTechnique(m_hTechnique);
		for_each(m_vRenderList.begin(), m_vRenderList.end(), RenderObjectFunction(this));
		m_vRenderList.clear();

	}

	void DisplayMaterial::UseParams()
	{
		struct ParamUseFunction
		{
			void Do(DisplayEffectParamPtr _pDisplayEffectParam)
			{
				_pDisplayEffectParam->Use();
			}
		};
		static ParamUseFunction oPUF;
		for_each(m_vParams.begin(), m_vParams.end(), boost::bind(&ParamUseFunction::Do, &oPUF, _1));
	}

	DisplayEffectPtr DisplayMaterial::GetEffect()
	{
		return m_pEffect;
	}

	DisplayMaterialManagerRef DisplayMaterial::GetMaterialManager()
	{
		return m_rMaterialManager;
	}


	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	DisplayMaterialManager::DisplayMaterialManager(DisplayRef _rDisplay)
	:	CoreObject(),
		m_mEffects(),
		m_mMaterials(),
		m_mParamCreators(),
		m_rDisplay(_rDisplay)
	{

	}

	DisplayMaterialManager::~DisplayMaterialManager()
	{

	}

	bool DisplayMaterialManager::Create(const boost::any& _rConfig)
	{
		bool bResult = true;

		m_mParamCreators[MakeKey(string("WORLDVIEWPROJ"))] = boost::bind(&DisplayEffectParamWORLDVIEWPROJ::CreateParam, _1);
		m_mParamCreators[MakeKey(string("WORLD"))] = boost::bind(&DisplayEffectParamWORLD::CreateParam, _1);
		m_mParamCreators[MakeKey(string("VIEWINV"))] = boost::bind(&DisplayEffectParamVIEWINV::CreateParam, _1);
		m_mParamCreators[MakeKey(string("WORLDINVTRANSPOSE"))] = boost::bind(&DisplayEffectParamWORLDINVTRANSPOSE::CreateParam, _1);
		m_mParamCreators[MakeKey(string("TIME"))] = boost::bind(&DisplayEffectParamTIME::CreateParam, _1);
		m_mParamCreators[MakeKey(string("ENVIRONMENTTEX"))] = boost::bind(&DisplayEffectParamENVIRONMENTTEX::CreateParam, _1);
		m_mParamCreators[MakeKey(string("NORMALTEX"))] = boost::bind(&DisplayEffectParamNORMALTEX::CreateParam, _1);
		m_mParamCreators[MakeKey(string("MORPHFACTOR"))] = boost::bind(&DisplayEffectParamMORPHFACTOR::CreateParam, _1);

		return bResult;
	}

	void DisplayMaterialManager::Update()
	{

	}

	void DisplayMaterialManager::Release()
	{
		UnloadAll();
		m_mParamCreators.clear();
	}

	bool DisplayMaterialManager::LoadMaterial(const string& _strName, const string& _strPath)
	{
		Key uKey = MakeKey(_strName);
		DisplayMaterialPtrMap::iterator iPair = m_mMaterials.find(uKey);
		bool bResult = (m_mMaterials.end() == iPair);

		if (false != bResult)
		{
			Config::CreateInfo oCCInfo = { _strPath };
			Config oConfig;
			bResult = oConfig.Create(boost::any(&oCCInfo));

			if (false != bResult)
			{
				string strEffectPath;
				string strEffectName;
				DisplayEffectPtr pEffect;
				DisplayMaterialPtr pMaterial = NULL;

				if (false != bResult)
				{
					bResult = oConfig.GetValue(string("material.effect"), strEffectPath);
				}
				if (false != bResult)
				{
					FS::GetFileNameWithoutExt(strEffectPath, strEffectName);
					pEffect = GetEffect(strEffectName);
					if (NULL == pEffect)
					{
						bResult = LoadEffect(strEffectName, strEffectPath);
						pEffect = (false != bResult) ? GetEffect(strEffectName) : NULL;
					}
				}
				if (false != bResult)
				{
					DisplayMaterial::CreateInfo oDMCInfo = { &oConfig, pEffect };
					pMaterial = new DisplayMaterial(*this);
					bResult = pMaterial->Create(boost::any(&oDMCInfo));
				}
				if (false != bResult)
				{
					m_mMaterials[uKey] = pMaterial;
				}
				else if (NULL != pMaterial)
				{
					pMaterial->Release();
					delete pMaterial;
				}
			}
			oConfig.Release();
		}

		return bResult;
	}

	void DisplayMaterialManager::UnloadMaterial(const string& _strName)
	{
		Key uKey = MakeKey(_strName);
		UnloadMaterial(uKey);
	}

	void DisplayMaterialManager::UnloadMaterial(const Key& _uNameKey)
	{
		DisplayMaterialPtrMap::iterator iPair = m_mMaterials.find(_uNameKey);
		bool bResult = (m_mMaterials.end() != iPair);
		if (false != bResult)
		{
			iPair->second->Release();
			delete iPair->second;
			m_mMaterials.erase(iPair);
		}
	}

	DisplayMaterialPtr DisplayMaterialManager::GetMaterial(const string& _strName)
	{
		Key uKey = 0;
		uKey = MakeKey(_strName);
		DisplayMaterialPtrMap::iterator iPair = m_mMaterials.find(uKey);
		bool bResult = (m_mMaterials.end() != iPair);
		DisplayMaterialPtr pResult = (false != bResult) ? iPair->second : NULL;
		return pResult;
	}

	bool DisplayMaterialManager::LoadEffect(const string& _strName, const string& _strPath)
	{
		Key uKey = MakeKey(_strName);
		DisplayEffectPtrMap::iterator iPair = m_mEffects.find(uKey);
		bool bResult = (m_mEffects.end() == iPair);
		if (false != bResult)
		{
			DisplayEffect::CreateInfo oDECInfo = { _strPath, true };
			DisplayEffectPtr pEffect = new DisplayEffect(m_rDisplay);
			bResult = pEffect->Create(boost::any(&oDECInfo));
			if (false != bResult)
			{
				m_mEffects[uKey] = pEffect;
			}
			else
			{
				pEffect->Release();
				delete pEffect;
			}
		}
		return bResult;
	}

	void DisplayMaterialManager::UnloadEffect(const string& _strName)
	{
		Key uKey = 0;
		uKey = MakeKey(_strName);
		DisplayEffectPtrMap::iterator iPair = m_mEffects.find(uKey);
		bool bResult = (m_mEffects.end() != iPair);
		if (false != bResult)
		{
			iPair->second->Release();
			delete iPair->second;
			m_mEffects.erase(iPair);
		}
	}

	DisplayEffectPtr DisplayMaterialManager::GetEffect(const string& _strName)
	{
		Key uKey = 0;
		uKey = MakeKey(_strName);
		DisplayEffectPtrMap::iterator iPair = m_mEffects.find(uKey);
		bool bResult = (m_mEffects.end() != iPair);
		DisplayEffectPtr pResult = (false != bResult) ? iPair->second : NULL;
		return pResult;
	}

	DisplayEffectParamPtr DisplayMaterialManager::CreateParam(const string& _strSemanticName, const boost::any& _rConfig)
	{
		const Key uKey = MakeKey(_strSemanticName);
		DisplayEffectParamPtr pResult = NULL;

		CreateParamFunc pCreateParam = m_mParamCreators[uKey];
		if (NULL != pCreateParam)
		{
			pResult = pCreateParam(_rConfig);
		}

		return pResult;
	}

	void DisplayMaterialManager::ReleaseParam(DisplayEffectParamPtr _pParam)
	{
		_pParam->Release();
		delete _pParam;
	}

	void DisplayMaterialManager::UnloadAll()
	{
		struct MaterialReleaseAndDeleteFunction
		{
			void operator()(pair<Key, DisplayMaterialPtr> _iPair)
			{
				_iPair.second->Release();
				delete _iPair.second;
			}
		};
		for_each(m_mMaterials.begin(), m_mMaterials.end(), MaterialReleaseAndDeleteFunction());
		m_mMaterials.clear();

		struct EffectReleaseAndDeleteFunction
		{
			void operator()(pair<Key, DisplayEffectPtr> _iPair)
			{
				_iPair.second->Release();
				delete _iPair.second;
			}
		};
		for_each(m_mEffects.begin(), m_mEffects.end(), EffectReleaseAndDeleteFunction());
		m_mEffects.clear();
	}

	DisplayRef DisplayMaterialManager::GetDisplay()
	{
		return m_rDisplay;
	}
}