#include "stdafx.h"
#include "Core.h"

namespace ElixirEngine
{
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	CoreObjectPtrMap CoreObject::s_mObjects;

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	WindowData::WindowData()
	:	m_pCreateWindow(NULL),
		m_hInstance(NULL),
		m_hPrevInstance(NULL),
		m_lpCmdLine(NULL),
		m_nCmdShow(0),
		m_hAccelTable(NULL),
		m_hWnd(NULL),
		m_oClientRect(),
		m_sColorMode(0),
		m_bFullScreen(false)
	{

	}

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	CoreObject::CoreObject()
	{
		s_mObjects[this] = 1;
	}

	CoreObject::~CoreObject()
	{
		s_mObjects.erase(s_mObjects.find(this));
	}

	bool CoreObject::Create(const boost::any& _rConfig)
	{
		return true;
	}

	void CoreObject::Update()
	{

	}

	void CoreObject::Release()
	{

	}

}
