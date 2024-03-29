#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "../Display/Display.h"

#define CAMERA_VIEWINV_AS_VIEW	1
#define CAMERA_LINEARIZED_DEPTH	0  // if set to 1 then water post effect won't work correctly :'(

namespace ElixirEngine
{
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	static const unsigned int s_uCameraFrustumPlanesCount = 6;

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	struct AABB
	{
		Vector3	m_aCorners[8];
	};

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	class DisplayCamera : public CoreObject
	{
	public:
		struct CreateInfo
		{
			CreateInfo();

			Vector3		m_oPos;
			Vector3		m_oRot;
			float		m_fX;				// viewport left
			float		m_fY;				// viewport top
			float		m_fWidth;			// viewport width
			float		m_fHeight;			// viewport height
			float		m_fDegreeFovy;
			float		m_fAspectRatio;
			float		m_fZNear;
			float		m_fZFar;
			bool		m_bPerspectiveMode;
		};
		typedef CreateInfo* CreateInfoPtr;
		typedef CreateInfo& CreateInfoRef;

		struct StateInfo
		{
			ViewportPtr	m_pViewport;
			float		m_fFovy;
			float		m_fAspectRatio;
			float		m_fPixelSize;
			float		m_fNear;
			float		m_fFar;
		};
		typedef StateInfo* StateInfoPtr;
		typedef StateInfo& StateInfoRef;

		enum EMatrix
		{
			EMatrix_VIEW,
			EMatrix_VIEWINV,
			EMatrix_PROJ,
			EMatrix_VIEWPROJ,
			EMatrix_POSITION,
			EMatrix_ROTATION,
		};

		enum EFrustumPlane
		{
			EFrustumPlane_LEFT,
			EFrustumPlane_RIGHT,
			EFrustumPlane_TOP,
			EFrustumPlane_BOTTOM,
			EFrustumPlane_NEAR,
			EFrustumPlane_FAR,
			EFrustumPlane_COUNT // last enum member
		};

		enum EFrustumCorner
		{
			EFrustumCorner_FARTOPLEFT,
			EFrustumCorner_FARTOPRIGHT,
			EFrustumCorner_FARBOTTOMLEFT,
			EFrustumCorner_FARBOTTOMRIGHT,
			EFrustumCorner_NEARTOPLEFT,
			EFrustumCorner_NEARTOPRIGHT,
			EFrustumCorner_NEARBOTTOMLEFT,
			EFrustumCorner_NEARBOTTOMRIGHT,
			EFrustumCorner_COUNT // last enum member
		};

		enum ECollision
		{
			ECollision_OUT,
			ECollision_IN,
			ECollision_INTERSECT,
		};

		enum EHalfSpace
		{
			EHalfSpace_NEGATIVE = -1,
			EHalfSpace_ON_PLANE = 0,
			EHalfSpace_POSITIVE = 1,
		};

	public:
		DisplayCamera(DisplayRef _rDisplay);
		virtual ~DisplayCamera();

		virtual bool Create(const boost::any& _rConfig);
		virtual void Update();
		virtual void Release();

		ViewportPtr GetCurrentViewport();
		void SetViewport(const Key& _uNameKey);

		Vector3& GetPosition();
		Vector3& GetRotation();
		void GetDirs(Vector3& _oFrontDir, Vector3& _oRightDir, Vector3& _oUpDir);
		void GetDirs(const Matrix& _rMatrix, Vector3& _oFrontDir, Vector3& _oRightDir, Vector3& _oUpDir);
		MatrixPtr GetMatrix(const EMatrix& _eMatrix);

		const float& GetPixelSize() const;
		ECollision CollisionWithSphere(const Vector3& _rCenter, const float& _fRadius);
		ECollision CollisionWithAABB(AABBRef _rAABB);
		ECollision CollisionWithAABB(const fsVector3Vec& _rvAABB);

		void AddListener(CoreObjectPtr _pListener);
		void RemoveListener(CoreObjectPtr _pListener);

		void SetReflection(const bool _bState, PlanePtr _pPlane = NULL);
		void SetClipPlanes(const UInt _uCount, PlanePtr _pPlanes);

		Vector3Ptr GetFrustumCorners();
		void GetStateInfo(StateInfoRef _rInfo);

	protected:
		void UpdatePixelSize();
		void ExtractFrustumPlanes();
		void ExtractFrustumCorners();
		float DistanceToPoint(const Plane &_rPlane, const Vector3& _rPoint);
		EHalfSpace PointSideOfPlane(const Plane &_rPlane, const Vector3& _rPoint);

	protected:
		DisplayRef			m_rDisplay;

		Matrix				m_oMView;
		Matrix				m_oMViewInv;
		Matrix				m_oMProjection;
		Matrix				m_oMViewProj;
		Matrix				m_oMPosition;
		Matrix				m_oMRotation;
		Vector3				m_oVPosition;
		Vector3				m_oVRotation;

		Matrix				m_oMXRot;
		Matrix				m_oMYRot;
		Matrix				m_oMZRot;
		Matrix				m_oMXYRot;

		Viewport			m_oViewport;
		ViewportPtr			m_pPreviousViewport;
		ViewportPtr			m_pCurrentViewport;
		Key					m_uCurrentViewportKey;
		float				m_fFovy;
		float				m_fAspectRatio;
		float				m_fPixelSize;
		float				m_fNear;
		float				m_fFar;

		Plane				m_aFrustumPlanes[EFrustumPlane_COUNT];
		Plane				m_aFrustumNormals[EFrustumPlane_COUNT];
		float				m_aFrustumDistances[EFrustumPlane_COUNT];
		Vector3				m_aFrustumCorners[EFrustumCorner_COUNT];

		Key					m_uCameraPosKey;
		Key					m_uFrustumCornersKey;

		CoreObjectPtrVec	m_vListeners;

		PlanePtr			m_pReflectionPlane;
		bool				m_bReflection;

		UInt				m_uClipPlaneCount;
		PlanePtr			m_pClipPlanes;

		bool				m_bPerspectiveMode;

	private:
	};
}

#endif // __CAMERA_H__
