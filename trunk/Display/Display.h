#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "../Core/Core.h"
#include "../Display/DisplayTypes.h"

namespace ElixirEngine
{
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	template<typename T>
	struct RenderFunction
	{
		void Do(T* pObject)
		{
			pObject->Render();
		}
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayVertexBuffer : public CoreObject
	{
	public:
		struct CreateInfo
		{
			unsigned int		m_uBufferSize;
			unsigned int		m_uVertexSize;
			VertexElementPtr	m_pVertexElement;
		};

	public:
		DisplayVertexBuffer(DisplayRef _rDisplay);
		virtual ~DisplayVertexBuffer();

		virtual bool Create(const boost::any& _rConfig);
		virtual void Update();
		virtual void Release();

		bool Set(const VoidPtr _pData);
		bool Use();

	protected:
		DisplayRef		m_rDisplay;
		unsigned int	m_uBufferSize;
		unsigned int	m_uVertexSize;
		VertexDeclPtr	m_pVertexDecl;
		VertexBufferPtr	m_pVertexBuffer;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayIndexBuffer : public CoreObject
	{
	public:
		struct CreateInfo
		{
			unsigned int	m_uBufferSize;
			bool			m_b16Bits;
		};

	public:
		DisplayIndexBuffer(DisplayRef _rDisplay);
		virtual ~DisplayIndexBuffer();

		virtual bool Create(const boost::any& _rConfig);
		virtual void Update();
		virtual void Release();

		bool Set(const VoidPtr _pData);
		bool Use();

	protected:
		DisplayRef		m_rDisplay;
		unsigned int	m_uBufferSize;
		bool			m_b16Bits;
		IndexBufferPtr	m_pIndexBuffer;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayObject : public CoreObject
	{
	public:
		DisplayObject(DisplayRef _rDisplay);
		virtual ~DisplayObject();

		virtual MatrixPtr GetWorldMatrix();
		virtual void SetMaterial(DisplayMaterialPtr _pMaterial);
		virtual DisplayMaterialPtr GetMaterial();

		virtual void RenderBegin() {};
		virtual void Render() = 0;
		virtual void RenderEnd() {};

	protected:
		DisplayRef			m_rDisplay;
		Matrix				m_oWorld;
		DisplayMaterialPtr	m_pMaterial;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class Display : public CoreObject
	{
	public:
		Display();
		virtual ~Display();

		virtual bool Create(const boost::any& _rConfig);
		virtual void Update();
		virtual void Release();

		bool OpenVideo(WindowData& _rWindowData);
		void CloseVideo();

		void UpdateRequest(CoreObjectPtr _pCoreObject);
		void RenderRequest(DisplayObjectPtr _pDisplayObject);
		void Render();

		DisplayVertexBufferPtr CreateVertexBuffer(DisplayVertexBuffer::CreateInfo& _rCreateInfo);
		void ReleaseVertexBuffer(DisplayVertexBufferPtr _pVertexBuffer);
		DisplayIndexBufferPtr CreateIndexBuffer(DisplayIndexBuffer::CreateInfo& _rCreateInfo);
		void ReleaseIndexBuffer(DisplayIndexBufferPtr _pIndexBuffer);

		DevicePtr GetDevicePtr() const;
		void GetResolution(unsigned int& _uWidth, unsigned int& _uHeight) const;
		DisplayMaterialManagerPtr GetMaterialManager();
		DisplayTextureManagerPtr GetTextureManager();
		DisplaySurfaceManagerPtr GetSurfaceManager();
		DisplayCameraPtr GetCurrentCamera();

		void SetCurrentWorldMatrix(MatrixPtr _pMatrix);
		MatrixPtr GetCurrentWorldMatrix();
		MatrixPtr GetCurrentWorldInvTransposeMatrix();

		void MRTRenderBeginPass(UIntRef _uIndex);
		void MRTRenderEndPass();
		void SetNormalProcessesList(DisplayNormalProcessPtrVecPtr _pNormalProcesses);
		void SetPostProcessesList(DisplayPostProcessPtrVecPtr _pPostProcesses);
		DisplayObjectPtr GetPostProcessGeometry();
		DisplayRenderTargetChainPtr GetRenderTargetChain();
		DisplayNormalProcessPtr GetCurrentNormalProcess();

		static unsigned int GetFormatBitsPerPixel(const D3DFORMAT& _eFormat);
		static bool IsPowerOf2(const unsigned int& _uValue, UIntPtr _pPowerLevel = NULL);
		static D3DFORMAT StringToDisplayFormat(const string& _strFormatName, const D3DFORMAT& _uDefaultFormat);
		static D3DFORMAT KeyToDisplayFormat(const Key& _uFormatNameKey, const D3DFORMAT& _uDefaultFormat);

	protected:
		typedef map<Key, D3DFORMAT>	DisplayFormatMap;
		static DisplayFormatMap s_mDisplayFormat;
		static void InitDisplayFormatMap();

		void RenderUpdate();

		Direct3DPtr						m_pDirect3D;
		DevicePtr						m_pDevice;
		DisplayEffectPtrVec				m_vRenderList;
		CoreObjectPtrVec				m_vUpdateList;
		DisplayCameraPtr				m_pCamera;
		DisplayMaterialManagerPtr		m_pMaterialManager;
		DisplayTextureManagerPtr		m_pTextureManager;
		DisplaySurfaceManagerPtr		m_pSurfaceManager;
		MatrixPtr						m_pWorldMatrix;
		DisplayPostProcessPtrVecPtr		m_pPostProcesses;
		DisplayEffectPtr				m_pDispFXPP;
		EffectPtr						m_pEffectPP;
		DisplayRenderTargetGeometryPtr	m_pPostProcessGeometry;
		DisplayRenderTargetChainPtr		m_pRTChain;
		DisplayNormalProcessPtrVecPtr	m_pNormalProcesses;
		DisplayNormalProcessPtr			m_pCurrentNormalProcess;
		Matrix							m_oWorldInvTransposeMatrix;
		unsigned int					m_uWidth;
		unsigned int					m_uHeight;

	private:
	};
}

#endif // __DISPLAY_H__