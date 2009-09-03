#include "stdafx.h"
#include "../Display/Display.h"
#include "../Display/Camera.h"
#include "../Display/Effect.h"
#include "../Display/Texture.h"
#include "../Display/Surface.h"

namespace ElixirEngine
{
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	DisplayVertexBuffer::DisplayVertexBuffer(DisplayRef _rDisplay)
	:	CoreObject(),
		m_rDisplay(_rDisplay),
		m_uBufferSize(0),
		m_uVertexSize(0),
		m_pVertexDecl(NULL),
		m_pVertexBuffer(NULL)
	{

	}

	DisplayVertexBuffer::~DisplayVertexBuffer()
	{

	}

	bool DisplayVertexBuffer::Create(const boost::any& _rConfig)
	{
		CreateInfo* pInfo = boost::any_cast<CreateInfo*>(_rConfig);
		bool bResult = (NULL == m_pVertexBuffer) && (0 != pInfo->m_uBufferSize) && (NULL != pInfo->m_pVertexElement);

		if (false != bResult)
		{
			m_uBufferSize = pInfo->m_uBufferSize;
			m_uVertexSize = pInfo->m_uVertexSize;
			HRESULT hResult =  m_rDisplay.GetDevicePtr()->CreateVertexBuffer(
				m_uBufferSize,
				0,
				0,
				D3DPOOL_DEFAULT,
				&m_pVertexBuffer,
				NULL);
			bResult = SUCCEEDED(hResult);
		}

		if (false != bResult)
		{
			HRESULT hResult = m_rDisplay.GetDevicePtr()->CreateVertexDeclaration(pInfo->m_pVertexElement, &m_pVertexDecl);
			bResult = SUCCEEDED(hResult);
		}

		return bResult;
	}

	void DisplayVertexBuffer::Update()
	{
		
	}

	void DisplayVertexBuffer::Release()
	{
		if (NULL != m_pVertexDecl)
		{
			m_pVertexDecl->Release();
			m_pVertexDecl = NULL;
		}
		if (NULL != m_pVertexBuffer)
		{
			m_pVertexBuffer->Release();
			m_pVertexBuffer = NULL;
		}
	}

	bool DisplayVertexBuffer::Set(const VoidPtr _pData)
	{
		VoidPtr pLockedData = NULL;
		HRESULT hResult = m_pVertexBuffer->Lock(0, m_uBufferSize, &pLockedData, 0);
		if (SUCCEEDED(hResult))
		{
			memcpy(pLockedData, _pData, m_uBufferSize);
			m_pVertexBuffer->Unlock();
		}
		return (SUCCEEDED(hResult));
	}

	bool DisplayVertexBuffer::Use()
	{
		VertexBufferPtr pCurrentVertexBuffer;
		unsigned int uOffset;
		unsigned int uStride;
		HRESULT hResult = m_rDisplay.GetDevicePtr()->GetStreamSource(0, &pCurrentVertexBuffer, &uOffset, &uStride);
		if ((SUCCEEDED(hResult)) && (pCurrentVertexBuffer != m_pVertexBuffer))
		{
			hResult = m_rDisplay.GetDevicePtr()->SetStreamSource(0, m_pVertexBuffer, 0, m_uVertexSize);
		}
		if (SUCCEEDED(hResult))
		{
			VertexDeclPtr pCurrentVertexDecl;
			m_rDisplay.GetDevicePtr()->GetVertexDeclaration(&pCurrentVertexDecl);
			if ((SUCCEEDED(hResult)) && (pCurrentVertexDecl != m_pVertexDecl))
			{
				hResult = m_rDisplay.GetDevicePtr()->SetVertexDeclaration(m_pVertexDecl);
			}
		}
		return (SUCCEEDED(hResult));
	}

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	DisplayIndexBuffer::DisplayIndexBuffer(DisplayRef _rDisplay)
	:	CoreObject(),
		m_rDisplay(_rDisplay),
		m_uBufferSize(0),
		m_b16Bits(false),
		m_pIndexBuffer(NULL)
	{

	}

	DisplayIndexBuffer::~DisplayIndexBuffer()
	{

	}

	bool DisplayIndexBuffer::Create(const boost::any& _rConfig)
	{
		CreateInfo* pInfo = boost::any_cast<CreateInfo*>(_rConfig);
		bool bResult = (NULL == m_pIndexBuffer) && (0 != pInfo->m_uBufferSize);

		if (false != bResult)
		{
			m_uBufferSize = pInfo->m_uBufferSize * (m_b16Bits ? sizeof(unsigned short) : sizeof(unsigned int));
			m_b16Bits = pInfo->m_b16Bits;
			HRESULT hResult =  m_rDisplay.GetDevicePtr()->CreateIndexBuffer(
				m_uBufferSize,
				0,
				m_b16Bits ? D3DFMT_INDEX16 : D3DFMT_INDEX32,
				D3DPOOL_DEFAULT,
				&m_pIndexBuffer,
				NULL);
			bResult = SUCCEEDED(hResult);
		}

		return bResult;
	}

	void DisplayIndexBuffer::Update()
	{

	}

	void DisplayIndexBuffer::Release()
	{
		if (NULL != m_pIndexBuffer)
		{
			m_pIndexBuffer->Release();
			m_pIndexBuffer = NULL;
		}
	}

	bool DisplayIndexBuffer::Set(const VoidPtr _pData)
	{
		VoidPtr pLockedData = NULL;
		HRESULT hResult = m_pIndexBuffer->Lock(0, m_uBufferSize, &pLockedData, 0);
		if (SUCCEEDED(hResult))
		{
			memcpy(pLockedData, _pData, m_uBufferSize);
			m_pIndexBuffer->Unlock();
		}
		return (SUCCEEDED(hResult));
	}

	bool DisplayIndexBuffer::Use()
	{
		IndexBufferPtr pCurrentIndexBuffer;
		HRESULT hResult = m_rDisplay.GetDevicePtr()->GetIndices(&pCurrentIndexBuffer);
		if ((SUCCEEDED(hResult)) && (pCurrentIndexBuffer != m_pIndexBuffer))
		{
			hResult = m_rDisplay.GetDevicePtr()->SetIndices(m_pIndexBuffer);
		}
		return (SUCCEEDED(hResult));
	}

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	DisplayObject::DisplayObject(DisplayRef _rDisplay)
	:	CoreObject(),
		m_rDisplay(_rDisplay),
		m_oWorld(),
		m_pMaterial(NULL)
	{
		D3DXMatrixIdentity(&m_oWorld);
	}

	DisplayObject::~DisplayObject()
	{

	}

	MatrixPtr DisplayObject::GetWorldMatrix()
	{
		return &m_oWorld;
	}

	void DisplayObject::SetMaterial(DisplayMaterialPtr _pMaterial)
	{
		m_pMaterial = _pMaterial;
	}

	DisplayMaterialPtr DisplayObject::GetMaterial()
	{
		return m_pMaterial;
	}

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	Display::Display()
	:	CoreObject(),
		m_pDirect3D(NULL),
		m_pDevice(NULL),
		m_vRenderList(),
		m_pCamera(NULL),
		m_pMaterialManager(NULL),
		m_pTextureManager(NULL),
		m_pSurfaceManager(NULL),
		m_pWorldMatrix(NULL),
		m_oWorldInvTransposeMatrix(),
		m_uWidth(0),
		m_uHeight(0)
	{

	}

	Display::~Display()
	{

	}

	bool Display::Create(const boost::any& _rConfig)
	{
		WindowData* pWindowData = boost::any_cast<WindowData*>(_rConfig);
		bool bResult = (NULL == m_pDevice);

		if (false != bResult)
		{
			m_pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
			bResult = (NULL != m_pDirect3D);
		}
		if (false != bResult)
		{
			bResult = OpenVideo(*pWindowData);
		}

		return bResult;
	}

	void Display::Update()
	{
		if (NULL != m_pDevice)
		{
			//m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(64, 0, 128), 1.0f, 0);
			//m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(128, 128, 128), 1.0f, 0);
			m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
			m_pDevice->BeginScene();
			m_pCamera->Update();
			Render();
			m_pDevice->EndScene();
			m_pDevice->Present(NULL, NULL, NULL, NULL);
		}
	}

	void Display::Release()
	{
		CloseVideo();

		if (NULL != m_pDirect3D)
		{
			m_pDirect3D->Release();
			m_pDirect3D = NULL;
		}
	}

	bool Display::OpenVideo(const WindowData& _rWindowData)
	{
		bool bResult = (NULL == m_pDevice);
		HRESULT hResult;

		if (false != bResult)
		{
			D3DPRESENT_PARAMETERS oD3DPP;
			memset(&oD3DPP, 0 ,sizeof(D3DPRESENT_PARAMETERS));

			oD3DPP.EnableAutoDepthStencil = TRUE;
			oD3DPP.AutoDepthStencilFormat = D3DFMT_D16;
			oD3DPP.Windowed = _rWindowData.m_bFullScreen ? FALSE : TRUE;
			oD3DPP.BackBufferWidth = _rWindowData.m_oClientRect.right;
			oD3DPP.BackBufferHeight = _rWindowData.m_oClientRect.bottom;
			oD3DPP.BackBufferFormat = _rWindowData.m_bFullScreen ? D3DFMT_X8R8G8B8 : D3DFMT_UNKNOWN;
			oD3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
			oD3DPP.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

			m_uWidth = _rWindowData.m_oClientRect.right;
			m_uHeight = _rWindowData.m_oClientRect.bottom;

			hResult = m_pDirect3D->CreateDevice(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				_rWindowData.m_hWnd,
				D3DCREATE_HARDWARE_VERTEXPROCESSING,
				&oD3DPP,
				&m_pDevice);

			bResult = SUCCEEDED(hResult);
		}

		if (false != bResult)
		{
			m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		}

		if (false != bResult)
		{
			DisplayCamera::CreateInfo oDCCInfo;
			m_pCamera = new DisplayCamera(*this);
			bResult = m_pCamera->Create(boost::any(&oDCCInfo));
		}

		if (false != bResult)
		{
			m_pMaterialManager = new DisplayMaterialManager(*this);
			bResult = m_pMaterialManager->Create(boost::any(0));
		}

		if (false != bResult)
		{
			m_pTextureManager = new DisplayTextureManager(*this);
			bResult = m_pTextureManager->Create(boost::any(0));
		}

		if (false != bResult)
		{
			m_pSurfaceManager = new DisplaySurfaceManager(*this);
			bResult = m_pSurfaceManager->Create(boost::any(0));
		}

		return bResult;
	}

	void Display::CloseVideo()
	{
		if (NULL != m_pSurfaceManager)
		{
			m_pSurfaceManager->Release();
			delete m_pSurfaceManager;
			m_pSurfaceManager = NULL;
		}
		if (NULL != m_pTextureManager)
		{
			m_pTextureManager->Release();
			delete m_pTextureManager;
			m_pTextureManager = NULL;
		}
		if (NULL != m_pMaterialManager)
		{
			m_pMaterialManager->Release();
			delete m_pMaterialManager;
			m_pMaterialManager = NULL;
		}

		if (NULL != m_pCamera)
		{
			m_pCamera->Release();
			delete m_pCamera;
			m_pCamera = NULL;
		}

		if (NULL != m_pDevice)
		{
			m_pDevice->Release();
			m_pDevice = NULL;
		}
	}

	void Display::RenderRequest(DisplayObjectPtr _pDisplayObject)
	{
		DisplayMaterialPtr pMaterial = _pDisplayObject->GetMaterial();
		DisplayEffectPtr pEffect = pMaterial->GetEffect();
		if (m_vRenderList.end() == find(m_vRenderList.begin(), m_vRenderList.end(), pEffect))
		{
			m_vRenderList.push_back(pEffect);
		}
		pEffect->RenderRequest(pMaterial);
		pMaterial->RenderRequest(_pDisplayObject);
	}

	void Display::Render()
	{
		struct RenderEffectFunction
		{
			void operator()(DisplayEffectPtr _pDisplayEffect)
			{
				_pDisplayEffect->Render();
			}
		};
		for_each(m_vRenderList.begin(), m_vRenderList.end(), RenderEffectFunction());
		m_vRenderList.clear();
	}

	DisplayVertexBufferPtr Display::CreateVertexBuffer(DisplayVertexBuffer::CreateInfo& _rCreateInfo)
	{
		DisplayVertexBufferPtr pVertexBuffer = new DisplayVertexBuffer(*this);
		if (false == pVertexBuffer->Create(boost::any(&_rCreateInfo)))
		{
			ReleaseVertexBuffer(pVertexBuffer);
			pVertexBuffer = NULL;
		}
		return pVertexBuffer;
	}

	void Display::ReleaseVertexBuffer(DisplayVertexBufferPtr _pVertexBuffer)
	{
		_pVertexBuffer->Release();
		delete _pVertexBuffer;
	}

	DisplayIndexBufferPtr Display::CreateIndexBuffer(DisplayIndexBuffer::CreateInfo& _rCreateInfo)
	{
		DisplayIndexBufferPtr pIndexBuffer = new DisplayIndexBuffer(*this);
		if (false == pIndexBuffer->Create(boost::any(&_rCreateInfo)))
		{
			ReleaseIndexBuffer(pIndexBuffer);
			pIndexBuffer = NULL;
		}
		return pIndexBuffer;
	}

	void Display::ReleaseIndexBuffer(DisplayIndexBufferPtr _pIndexBuffer)
	{
		_pIndexBuffer->Release();
		delete _pIndexBuffer;
	}

	DevicePtr Display::GetDevicePtr() const
	{
		return m_pDevice;
	}

	void Display::GetResolution(unsigned int& _uWidth, unsigned int& _uHeight) const
	{
		_uWidth = m_uWidth;
		_uHeight = m_uHeight;
	}

	DisplayMaterialManagerPtr Display::GetMaterialManager()
	{
		return m_pMaterialManager;
	}

	DisplayTextureManagerPtr Display::GetTextureManager()
	{
		return m_pTextureManager;
	}

	DisplaySurfaceManagerPtr Display::GetSurfaceManager()
	{
		return m_pSurfaceManager;
	}

	DisplayCameraPtr Display::GetCurrentCamera()
	{
		return m_pCamera;
	}

	void Display::SetCurrentWorldMatrix(MatrixPtr _pMatrix)
	{
		m_pWorldMatrix = _pMatrix;
		D3DXMatrixInverse(&m_oWorldInvTransposeMatrix, NULL, &m_oWorldInvTransposeMatrix);
		D3DXMatrixTranspose(&m_oWorldInvTransposeMatrix, &m_oWorldInvTransposeMatrix);
	}

	MatrixPtr Display::GetCurrentWorldMatrix()
	{
		return m_pWorldMatrix;
	}

	MatrixPtr Display::GetCurrentWorldInvTransposeMatrix()
	{
		return &m_oWorldInvTransposeMatrix;
	}

	unsigned int Display::GetFormatBitsPerPixel(const D3DFORMAT& _eFormat)
	{
		switch (_eFormat)
		{
			//case D3DFMT_UNKNOWN:
			case D3DFMT_R8G8B8:
			{
				return 24;
			}
			case D3DFMT_A8R8G8B8:
			case D3DFMT_X8R8G8B8:
			case D3DFMT_A2B10G10R10:
			case D3DFMT_A8B8G8R8:
			case D3DFMT_X8B8G8R8:
			case D3DFMT_G16R16:
			case D3DFMT_A2R10G10B10:
			case D3DFMT_X8L8V8U8:
			case D3DFMT_Q8W8V8U8:
			case D3DFMT_V16U16:
			case D3DFMT_A2W10V10U10:
			case D3DFMT_D32:
			case D3DFMT_D24S8:
			case D3DFMT_D24X8:
			case D3DFMT_D24X4S4:
			case D3DFMT_D32F_LOCKABLE:
			case D3DFMT_D24FS8:
		#if !defined(D3D_DISABLE_9EX)
			case D3DFMT_D32_LOCKABLE:
		#endif // !D3D_DISABLE_9EX
			case D3DFMT_INDEX32:
			case D3DFMT_G16R16F:
			case D3DFMT_R32F:
			{
				return 32;
			}
			case D3DFMT_R5G6B5:
			case D3DFMT_X1R5G5B5:
			case D3DFMT_A1R5G5B5:
			case D3DFMT_A4R4G4B4:
			case D3DFMT_A8R3G3B2:
			case D3DFMT_X4R4G4B4:
			case D3DFMT_A8L8:
			case D3DFMT_V8U8:
			case D3DFMT_L6V5U5:
			case D3DFMT_R8G8_B8G8:
			case D3DFMT_G8R8_G8B8:
			case D3DFMT_D16_LOCKABLE:
			case D3DFMT_D15S1:
			case D3DFMT_D16:
			case D3DFMT_L16:
			case D3DFMT_INDEX16:
			case D3DFMT_R16F:
			case D3DFMT_CxV8U8:
			{
				return 16;
			}
			case D3DFMT_R3G3B2:
			case D3DFMT_A8:
			case D3DFMT_A8P8:
			case D3DFMT_P8:
			case D3DFMT_L8:
			case D3DFMT_A4L4:
		#if !defined(D3D_DISABLE_9EX)
			case D3DFMT_S8_LOCKABLE:
		#endif // !D3D_DISABLE_9EX
			{
				return 8;
			}
			case D3DFMT_A16B16G16R16:
			case D3DFMT_G32R32F:
			case D3DFMT_Q16W16V16U16:
			case D3DFMT_A16B16G16R16F:
			{
				return 64;
			}
			case D3DFMT_A32B32G32R32F:
			{
				return 128;
			}
		#if !defined(D3D_DISABLE_9EX)
			case D3DFMT_A1:
			{
				return 1;
			}
		#endif // !D3D_DISABLE_9EX
			case D3DFMT_UYVY:
			case D3DFMT_YUY2:
			case D3DFMT_DXT1:
			case D3DFMT_DXT2:
			case D3DFMT_DXT3:
			case D3DFMT_DXT4:
			case D3DFMT_DXT5:
			case D3DFMT_VERTEXDATA:
			case D3DFMT_MULTI2_ARGB8:
		#if !defined(D3D_DISABLE_9EX)
			case D3DFMT_BINARYBUFFER:
		#endif // !D3D_DISABLE_9EX
			{
				return 0;
			}
		}
		return 0;
	}
}
