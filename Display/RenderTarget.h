#ifndef __RENDERTARGET_H__
#define __RENDERTARGET_H__

#include "../Core/Core.h"
#include "../Display/DisplayTypes.h"
#include "../Display/Texture.h"
#include "../Core/Config.h"

namespace ElixirEngine
{
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayRenderTargetGeometry : public DisplayObject
	{
	public:
		struct CreateInfo
		{
			UInt	m_uWidth;
			UInt	m_uHeight;
		};

	public:
		// This is the vertex format used with the quad during post-process.
		struct Vertex
		{
			float x, y, z, rhw;
			float tu, tv;       // Texcoord for post-process source
			float tu2, tv2;     // Texcoord for the original scene

			const static VertexElement	s_aDecl[4];
		};

	public:
		DisplayRenderTargetGeometry(DisplayRef _rDisplay);
		virtual ~DisplayRenderTargetGeometry();

		virtual bool Create(const boost::any& _rConfig);
		virtual void Update();
		virtual void Release();

		virtual void RenderBegin();
		virtual void Render();
		virtual void RenderEnd();

	protected:
		Vertex			m_aQuad[4];
		VertexBufferPtr	m_pPreviousVertexBuffer;
		VertexDeclPtr	m_pPreviousVertexDecl;
		VertexDeclPtr	m_pVertDeclPP;
		unsigned int	m_uPreviousVBOffset;
		unsigned int	m_uPreviousVBStride;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayRenderTarget : public CoreObject
	{
	public:
		struct CreateInfo
		{
			string		m_strName;
			UInt		m_uWidth;
			UInt		m_uHeight;
			D3DFORMAT	m_uFormat;
			UInt		m_uIndex;
		};

		enum ERenderMode
		{
			ERenderMode_UNKNOWNPROCESS,
			ERenderMode_RESETPROCESS,
			ERenderMode_NORMALPROCESS,
			ERenderMode_POSTPROCESS
		};

	public:
		DisplayRenderTarget(DisplayRef _rDisplay);
		virtual ~DisplayRenderTarget();

		virtual bool Create(const boost::any& _rConfig);
		virtual void Update();
		virtual void Release();

		void RenderBegin(const ERenderMode& _eMode);
		void RenderBeginPass(UIntRef _uIndex);
		void RenderEndPass();
		void RenderEnd();

		DisplayTexturePtr GetTexture();

	protected:
		enum ERenderState
		{
			ERenderState_UNKNOWN,
			ERenderState_RENDERBEGIN,
			ERenderState_RENDERBEGINPASS,
			ERenderState_RENDERENDPASS,
			ERenderState_RENDEREND,
		};

		static const UInt	c_u1stBuffer = 0;
		static const UInt	c_u2ndBuffer = 1;
		static const UInt	c_uOriginalBuffer = 2;
		static const UInt	c_uBufferCount = 3;

		string				m_strName;
		DisplayRef			m_rDisplay; 
		DisplayTexturePtr	m_pDoubleBufferTex[3];
		SurfacePtr			m_pDoubleBufferSurf[3];
		SurfacePtr			m_pPreviousBufferSurf;
		DisplayTexturePtr	m_pCurrentBufferTex;
		UInt				m_uCurrentBuffer;
		UInt				m_uIndex;
		Key					m_uRTSemanticNameKey;
		Key					m_uORTSemanticNameKey;
		ERenderState		m_eRenderState;
		ERenderMode			m_eMode;
		bool				m_bFirstRender;

	private:
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayRenderTargetChain : public CoreObject
	{
	public:
		struct CreateInfo
		{
			string		m_strName;
			UInt		m_uWidth;
			UInt		m_uHeight;
			D3DFORMAT	m_uFormat;
			UInt		m_uBufferCount;
		};

	public:
		DisplayRenderTargetChain(DisplayRef _rDisplay);
		virtual ~DisplayRenderTargetChain();

		virtual bool Create(const boost::any& _rConfig);
		virtual void Update();
		virtual void Release();

		void RenderBegin(const DisplayRenderTarget::ERenderMode& _eMode);
		void RenderBeginPass(UIntRef _uIndex);
		void RenderEndPass();
		void RenderEnd();

		DisplayTexturePtr GetTexture(UIntRef _uBufferIndex);

	protected:
		DisplayRenderTargetPtrVec	m_vGBuffer;
		DisplayRef					m_rDisplay;

	private:
	};
}

#endif // __RENDERTARGET_H__
