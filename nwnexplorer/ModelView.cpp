//-----------------------------------------------------------------------------
// 
// @doc
//
// @module	ModelView.cpp - Model view window |
//
// This module contains the definition of the model view window.
//
// Copyright (c) 2002-2003 - Edward T. Smith
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are 
// met:
// 
// 1. Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
// 2. Neither the name of Edward T. Smith nor the names of its contributors 
//    may be used to endorse or promote products derived from this software 
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// @end
//
// $History: ModelView.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "ModelView.h"
#include "ModelWnd.h"
#include "../_MathLib/Vector2.h"
#include "../_NmcLib/Nmc.h"

//
// Debug NEW
//

#if defined (_DEBUG) && !defined (_WIN32_WCE)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Globals
//

extern D3DCOLOR *g_apPalettes [NwnPalette__Count];
extern CNwnKeyFile g_sKeyFile;
extern CNwnKeyFile g_sPatchFile;
extern CNwnKeyFile g_sXP1KeyFile;
extern CNwnKeyFile g_sXP1PatchFile;
extern CDataCache g_sResourceCache;

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelView> constructor.
//
// @parm CData * | pFile | Memory file
//
// @parm NwnResType | nResType | Resource type
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelView::CModelView (CData *pFile, NwnResType nResType) : 
	m_wndImage (pFile, nResType, &m_sModelContext)
{

	//
	// Initialize variables
	//

	m_sRes .Set (pFile);
	m_nResType = nResType;
	for (int i = 0; i < _countof (m_sModelContext .anPalettes); i++)
        m_sModelContext .anPalettes [i] = 0;
	m_sModelContext .mView .MakeIdentity ();
	m_sModelContext .vView .m_x = 0.0f;
	m_sModelContext .vView .m_y = 0.0f;
	m_sModelContext .vView .m_z = 10.0f;
	m_dlgModelColor .m_Child .m_pModelContext = &m_sModelContext;
	m_dlgModelPos .m_Child .m_pModelContext = &m_sModelContext;
	m_pRtNodeRoot = NULL;

	//
	// Compile the model if need be
	//

	if (m_sRes .m_p != NULL && nResType == NwnResType_MDL)
	{
		unsigned char *pauchData = (unsigned char *) m_sRes .GetData ();
		if (pauchData != NULL &&
			(pauchData [0] != 0 || pauchData [1] != 0 || 
			 pauchData [2] != 0 || pauchData [3] != 0))
		{

			//
			// Compile
			//

			CNmcCache sCache;
			CNmcContext sCtx;
			sCtx .AddKeyFile (&g_sKeyFile);
			sCtx .AddKeyFile (&g_sPatchFile);
			sCtx .AddKeyFile (&g_sXP1KeyFile);
			sCtx .AddKeyFile (&g_sXP1PatchFile);
			sCtx .SetCache (&sCache);
			sCtx .SetPurgeNullFaces (true);
			CNwnMemoryStream *pStream = new CNwnMemoryStream (
				pFile ->m_strResName, pauchData, m_sRes .GetSize (), false);
			sCtx .AddStream (pStream);
			NmcParseModelFile (&sCtx);

			//
			// If we have an error, return
			//

			if (sCtx .GetErrors () > 0)
			{
				//FIXME
				//printf ("Compilation aborted with errors\n");
			}

			//
			// Make sure we have a model
			//

			else if (sCtx .GetCurrentModel () == NULL)
			{
				//FIXME
				//printf ("Error: File \"%s\" doesn't contain a valid model\n", 
				//	pszInFile);
			}

			//
			// Otherwise, we have a good model
			//

			else
			{

				//
				// Create the output
				//

				CNwnMemoryStream sModelStream;
				CNwnMemoryStream sRawStream;
				sCtx .GetCurrentModel () ->Serialize (
					&sModelStream, &sRawStream);

				//
				// Create the new data
				//

				UINT32 nModelDataSize = (UINT32) sModelStream .GetLength ();
				UINT32 nRawDataSize = (UINT32) sRawStream .GetLength ();
				UINT32 ulSize = sizeof (CNwnModel::Header) + nModelDataSize + 
					nRawDataSize;
				unsigned char *pauchData = (unsigned char *) malloc (ulSize);
				CNwnModel::Header *phdr = (CNwnModel::Header *) pauchData;
				phdr ->ulZero = 0;
				phdr ->ulRawDataOffset = nModelDataSize;
				phdr ->ulRawDataSize = nRawDataSize;
				memcpy (&pauchData [sizeof (CNwnModel::Header)],
					sModelStream .GetData (), nModelDataSize);
				memcpy (&pauchData [sizeof (CNwnModel::Header) + nModelDataSize],
					sRawStream .GetData (), nRawDataSize);

				//
				// Create the model object
				//

				m_sResModel .Set (new CData (&g_sResourceCache, 
					pFile ->m_pKeyFile, pFile ->m_pModuleFile, 
					pFile ->m_pBifFile, pFile ->m_strResName, NwnResType_MDL,
					CDataKey::Type_BinaryModel, NULL, 0, pauchData, ulSize, 
					true));
			}

			//
			// Free the model
			//

			if (sCtx .GetCurrentModel ())
                delete sCtx .GetCurrentModel ();
		}
		else
			m_sResModel .Set (m_sRes .m_p);
	}

 	//
	// Compute the bmin/bmax/average for the vertices
	//

	CVector3 vSum (0, 0, 0);
	CVector3 vMin (FLT_MAX, FLT_MAX, FLT_MAX);
	CVector3 vMax (FLT_MIN, FLT_MIN, FLT_MIN);
	int nVerts = 0;
	if (m_sResModel .m_p != NULL)
	{
		CNwnMdlModel *pHeader = (CNwnMdlModel *) m_sResModel .GetModelData  ();
		if (pHeader ->m_pGeometry .GetOffset () != 0)
		{
			CMatrix m;
			m .MakeIdentity ();
			nVerts = GetModelExtent (m,
				pHeader ->m_pGeometry .GetOffset (),
				vSum, vMin, vMax);
		}
	}
	if (nVerts != 0)
		vSum /= (float) nVerts;

	//
	// Set the center of rotation
	//

	m_sModelContext .vCenterOfRotation = (vMin + vMax) / 2.0f;

	//
	// Based on the size, set the model center (ok, this is trash,
	// but at the moment, no real need to make it any better)
	//

	float fSize = (vMax - vMin) .Length ();
	if (fSize > 6)
	{
		m_sModelContext .vView .m_z = 20.0f;
	}
	else if (fSize > 3)
	{
		m_sModelContext .vView .m_z = 10.0f;
	}
	else if (fSize > 1)
	{
		m_sModelContext .vView .m_z = 5.0f;
	}
	else if (fSize > .5f)
	{
		m_sModelContext .vView .m_z = 3.0f;
	}
	else
	{
		m_sModelContext .vView .m_z = 1.0f;
	}
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelView> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelView::~CModelView ()
{
	if (m_pRtNodeRoot)
	{
		g_wndModel .UnloadTextures (m_pRtNodeRoot);
		delete m_pRtNodeRoot;
	}
}

//-----------------------------------------------------------------------------
//
// @mfunc Initialize the window
//
// @parm UINT | uMsg | Message
//
// @parm WPARAM | wParam | Message w-parameter
//
// @parm LPARAM | lParam | Message l-parameter
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CModelView::OnCreate (UINT uMsg, 
	WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

	//
	// Load the palettes
	//

	if (g_apPalettes [0] == NULL)
	{
		g_apPalettes [NwnPalette_Metal1] = TgaLoad (IDR_PAL_ARMOR01, NULL, NULL);
		g_apPalettes [NwnPalette_Metal2] = TgaLoad (IDR_PAL_ARMOR02, NULL, NULL);
		g_apPalettes [NwnPalette_Leather1] = TgaLoad (IDR_PAL_LEATHER01, NULL, NULL);
		g_apPalettes [NwnPalette_Leather2] = TgaLoad (IDR_PAL_LEATHER01, NULL, NULL);
		g_apPalettes [NwnPalette_Cloth1] = TgaLoad (IDR_PAL_CLOTH01, NULL, NULL);
		g_apPalettes [NwnPalette_Cloth2] = TgaLoad (IDR_PAL_CLOTH01, NULL, NULL);
		g_apPalettes [NwnPalette_Skin] = TgaLoad (IDR_PAL_SKIN01, NULL, NULL);
		g_apPalettes [NwnPalette_Hair] = TgaLoad (IDR_PAL_HAIR01, NULL, NULL);
		g_apPalettes [NwnPalette_Tattoo1] = TgaLoad (IDR_PAL_TATTOO01, NULL, NULL);
		g_apPalettes [NwnPalette_Tattoo2] = TgaLoad (IDR_PAL_TATTOO01, NULL, NULL);
	}

	//
	// Create our windows
	//

	if (m_nResType == NwnResType_PLT || m_nResType == NwnResType_MDL)
	{
		m_RolloutContainer .Create (m_hWnd, _T (""), 
			WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE,
			0, 0, 0);
	}
	if (m_nResType == NwnResType_MDL)
	{
        m_dlgModelPos .Create (m_RolloutContainer, _T ("Camera"), 
			WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
		m_RolloutContainer .AddRollout (m_dlgModelPos);
	}
	if (m_nResType == NwnResType_PLT || m_nResType == NwnResType_MDL)
	{
        m_dlgModelColor .Create (m_RolloutContainer, _T ("Palette"), 
			WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
		m_RolloutContainer .AddRollout (m_dlgModelColor);
		if (m_nResType == NwnResType_MDL)
			m_RolloutContainer .ExpandRollout (m_dlgModelColor, false);
	}
	if (m_nResType == NwnResType_PLT || m_nResType == NwnResType_BMP ||
		m_nResType == NwnResType_TGA)
	{
		m_wndImage .Create (m_hWnd, CWindow::rcDefault, _T (""), 
			WS_CHILD |  WS_CLIPSIBLINGS | WS_VISIBLE, WS_EX_CLIENTEDGE, 
			(HMENU) 0, NULL);
	}
	else if (m_nResType == NwnResType_MDL)
	{
		g_wndModel .ShowModel (m_hWnd, m_sResModel, m_nResType, 
			&m_pRtNodeRoot, &m_sModelContext);
	}

	//
	// Update the image
	//

	m_wndImage .OnModelContextChanged ();
    g_wndModel .OnModelContextChanged (m_hWnd);

	//
	// Return FALSE
	//

	return false;
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the extent of the model
//
// @parm CMatrix & | m | Current translation matrix
//
// @parm int | nOffset | Offset of the node
//
// @parm CVector3 & | vSum | Sum of the verticies
//
// @parm CVector3 & | vMin | Min vertex
//
// @parm CVector3 & | vMax | Max vertex
//
// @rdesc Number of vertices.
//
//-----------------------------------------------------------------------------

int CModelView::GetModelExtent (CMatrix &m, int nOffset, 
	CVector3 &vSum, CVector3 &vMin, CVector3 &vMax)
{

	//
	// Get the node
	//

	CNwnMdlNodeHeader *pNode = (CNwnMdlNodeHeader *) 
		m_sResModel .GetModelData (nOffset);

	//
	// Process the controllers
	//

	CVector3 vPosition (0, 0, 0);
	CQuaternion qOrientation (0, 0, 0, 1);
	float fScale = 1.0f;
    NwnMdlControllerKey *paControllerKey = pNode ->m_aControllerKey 
		.GetData (m_sResModel .GetModelData ());
	int nControllerKeyCount = (int) pNode ->m_aControllerKey .GetCount ();
	float *pafControllerData = pNode ->m_aControllerData 
		.GetData (m_sResModel .GetModelData ());
	for (int i = 0; i < nControllerKeyCount; i++)
	{
		if (paControllerKey [i] .lType == NwnMdlCT_Position)
		{
			float *pafData = &pafControllerData [
				paControllerKey [i] .sDataOffset];
			vPosition .m_x = pafData [0];
			vPosition .m_y = pafData [1];
			vPosition .m_z = pafData [2];
		}
		else if (paControllerKey [i] .lType == NwnMdlCT_Orientation)
		{
			float *pafData = &pafControllerData [
				paControllerKey [i] .sDataOffset];
			qOrientation .m_x = pafData [0];
			qOrientation .m_y = pafData [1];
			qOrientation .m_z = pafData [2];
			qOrientation .m_w = pafData [3];
		}
		else if (paControllerKey [i] .lType == NwnMdlCT_Scale)
		{
			float *pafData = &pafControllerData [
				paControllerKey [i] .sDataOffset];
			fScale = pafData [0];
		}
	}

	//
	// Compute the new matrix
	//

	CMatrix mnew;
	CMatrix m2;
	m2 .SetQuaternion (qOrientation);
	m2 .SetTranslate (vPosition);
	if (fScale != 0)
	{
		CMatrix m3;
		m3 .MakeIdentity ();
		m3 .m_m [0] [0] = fScale;
		m3 .m_m [1] [1] = fScale;
		m3 .m_m [2] [2] = fScale;
		CMatrix m4;
		m2 .Multiply4 (m3, m4);//FIXME do it better
		m .Multiply4 (m4, mnew);
	}
	else
	{
		m .Multiply4 (m2, mnew);
	}

	//
	// If this is a mesh
	//

	int nVerts = 0;
	if ((pNode ->m_ulFlags & NwnMdlNF_HasMesh) != 0 &&
		pNode ->m_ulFlags != NwnMdlNT_AABBMesh)
	{
		CNwnMdlMeshHeader *pMesh = static_cast <CNwnMdlMeshHeader *> (pNode);
		if (pMesh ->m_usVertexCount != 0 && 
			pMesh ->m_pavVerts .GetOffset () != 0xFFFFFFFF)
		{
			CVector3 *pVerts = (CVector3 *) m_sResModel .GetRawData (
				pMesh ->m_pavVerts .GetOffset ());
			for (size_t nVert = 0; nVert < pMesh ->m_usVertexCount; nVert++)
			{
				CVector3 v;
				mnew .PreTranslate (pVerts [nVert], v);
				if (vMin .m_x > v .m_x)
					vMin .m_x = v .m_x;
				if (vMin .m_y > v .m_y)
					vMin .m_y = v .m_y;
				if (vMin .m_z > v .m_z)
					vMin .m_z = v .m_z;
				if (vMax .m_x < v .m_x)
					vMax .m_x = v .m_x;
				if (vMax .m_y < v .m_y)
					vMax .m_y = v .m_y;
				if (vMax .m_z < v .m_z)
					vMax .m_z = v .m_z;
				vSum += v;
			}
			nVerts += pMesh ->m_usVertexCount;
		}
	}

	//
	// Create the children
	//

	if (pNode ->m_apChildren .GetCount () > 0)
	{
		CNwnPointer <CNwnMdlNodeHeader> *pulOffsets = pNode ->
			m_apChildren .GetData (m_sResModel .GetModelData ());
		for (int i = 0; i < pNode ->m_apChildren .GetCount (); i++)
		{
			nVerts += GetModelExtent (mnew, pulOffsets [i] .GetOffset (), 
				vSum, vMin, vMax);
		}
	}
	return nVerts;

#ifdef XXX
		m_sResModel ->

		//
		// Compute the radius
		//

		for (size_t nVert = 0; nVert < pMeshTemp ->aVerts .GetCount (); nVert++)
		{
			CVector3 v (pVerts [nVert] - pMesh ->m_vBAverage);
			float fLength = v .Length ();
			if (pMesh ->m_fRadius < fLength)
				pMesh ->m_fRadius = fLength;
		}
	}
#endif
}

