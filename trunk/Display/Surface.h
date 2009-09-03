#ifndef __SURFACE_H__
#define __SURFACE_H__

#include "../Core/Core.h"
#include "../Display/DisplayTypes.h"
#include "../Display/Display.h"

namespace ElixirEngine
{
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplaySurface : public CoreObject
	{
	public:
		struct CreateInfo
		{
			string	m_strName;
			string	m_strPath;
		};

	public:
		DisplaySurface(DisplayRef _rDisplay);
		virtual ~DisplaySurface();

		virtual bool Create(const boost::any& _rConfig);
		virtual void Update();
		virtual void Release();

		ImageInfoRef GetInfo();
		VoidPtr Lock(const bool& _bReadOnly);
		VoidPtr GetDataXY(const unsigned int& _uX, const unsigned int& _uY);
		void Unlock();

	protected:
		ImageInfo		m_oInfo;
		LockedRect		m_oLockedRect;
		DisplayRef		m_rDisplay;
		SurfacePtr		m_pSurface;
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplaySurfaceManager : public CoreObject
	{
	public:
		DisplaySurfaceManager(DisplayRef _rDisplay);
		virtual ~DisplaySurfaceManager();

		virtual bool Create(const boost::any& _rConfig);
		virtual void Update();
		virtual void Release();

		bool Load(const string& _strName, const string& _strPath);
		void Unload(const string& _strName);
		DisplaySurfacePtr Get(const string& _strName);
		void UnloadAll();

	protected:
		DisplayRef				m_rDisplay;
		DisplaySurfacePtrMap	m_mSurfaces;
	};

}

#endif // __SURFACE_H__