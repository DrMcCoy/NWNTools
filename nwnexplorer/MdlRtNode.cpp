//-----------------------------------------------------------------------------
// 
// @doc
//
// @module	MdlRtNode.cpp - Resource data source |
//
// This module contains the definition of the resource data source.
//
// Copyright (c) 2003 - Edward T. Smith
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
// $History: DataCache.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "MdlRtNode.h"
#include "../_MathLib/Matrix.h"
#include <gl/gl.h>

//
// Debug NEW
//

#if defined (_DEBUG) && !defined (_WIN32_WCE)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Externals
//

extern CDataCache g_sResourceCache;

//-----------------------------------------------------------------------------
//
// @mfunc <c CMdlRtNode> constructor.
//
// @parm CNwnMdlNodeHeader * | pNode | Pointer to the parent node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CMdlRtNode::CMdlRtNode (CNwnMdlNodeHeader *pNode) : m_vPosition (0, 0, 0),
	m_qOrientation (0, 0, 0, 1)
{
	m_pNode = pNode;
	m_fScale = 1.0f;
	m_fAlpha = 1.0f;
	m_fMesh = false;
	m_pTexture = NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CMdlRtNode> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CMdlRtNode::~CMdlRtNode ()
{
	for (int i = 0; i < m_apRtNodes .GetCount (); i++)
		delete m_apRtNodes [i];
}

//-----------------------------------------------------------------------------
//
// @mfunc Create the node and it's children
//
// @parm CResModelRef & | sRes | Resource 
//
// @parm int | nOffset | Offset to the node in the geometry
//
// @rdesc Pointer to the node
//
//-----------------------------------------------------------------------------

CMdlRtNode *CMdlRtNode::CreateNodes (CResModelRef &sRes, int nOffset)
{

	//
	// Get the node
	//

	CNwnMdlNodeHeader *pNode = (CNwnMdlNodeHeader *) 
		sRes .GetModelData (nOffset);

	//
	// Normally, each type of node would have it's own type of
	// runtime structure.  But currently, I am just going to support
	// the basic mesh type.
	//

	CMdlRtNode *pRtNode = new CMdlRtNode (pNode);
	pRtNode ->m_fMesh = (pNode ->m_ulFlags & NwnMdlNF_HasMesh) != 0 &&
		pNode ->m_ulFlags != NwnMdlNT_AABBMesh;

	//
	// Create the children
	//

	if (pNode ->m_apChildren .GetCount () > 0)
	{
		CNwnPointer <CNwnMdlNodeHeader> *pulOffsets = pNode ->
			m_apChildren .GetData (sRes .GetModelData ());
		for (int i = 0; i < pNode ->m_apChildren .GetCount (); i++)
		{
			CMdlRtNode *pRtChild = CreateNodes (sRes, 
				pulOffsets [i] .GetOffset ());
			pRtNode ->m_apRtNodes .Add (pRtChild);
		}
	}

	//
	// Return the node
	//

	return pRtNode;
}

//-----------------------------------------------------------------------------
//
// @mfunc Render the node
//
// @parm CResModelRef & | sRes | Resource 
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMdlRtNode::Render (CResModelRef &sRes)
{
	
	//
	// Save the current matrix
	//

	glPushMatrix ();

	//
	// Look for controller data we are interested in
	//

	ProcessControllers (
		m_pNode ->m_aControllerKey .GetData (sRes .GetModelData ()),
		(int) m_pNode ->m_aControllerKey .GetCount (), 
		m_pNode ->m_aControllerData .GetData (sRes .GetModelData ()));

	//
	// Update the matrix
	//

	glTranslatef (m_vPosition .m_x, m_vPosition .m_y, m_vPosition .m_z);
	CMatrix m;
	m .MakeQuaternion (m_qOrientation);
	glMultMatrixf (m .m_m [0]);
	glScalef (m_fScale, m_fScale, m_fScale);

	//
	// If we have a mesh
	//

	if (m_fMesh)
	{
		CNwnMdlMeshHeader *pMesh = (CNwnMdlMeshHeader *) m_pNode;

		//
		// Draw (NOTE: Alpha images really must be depth queued.
		//		When they are not depth queued, the Z-plane prevents
		//		triangles deeper in the scene from painting properly.)
		//

		if (pMesh ->m_usVertexCount != 0 && 
			pMesh ->m_pavVerts .GetOffset () != 0xFFFFFFFF &&
			pMesh ->m_sVertexIndicesCount .GetCount () != 0 &&
			pMesh ->m_ulRender 
			// &&	m_pTexture != NULL
			)
		{

			//
			// Enable/Disable the texture
			//

			if (m_pTexture == NULL)
				glDisable (GL_TEXTURE_2D);
			else
			{
				GLuint *pname = (GLuint *) m_pTexture ->GetData (0);
				glEnable (GL_TEXTURE_2D);
				glBindTexture (GL_TEXTURE_2D, *pname);
				assert (GL_NO_ERROR == glGetError ());
			}

			//
			// Set the material
			//

			float fv [4];
			fv [3] = m_fAlpha;
			glMaterialf (GL_FRONT, GL_SHININESS, pMesh ->m_fShininess);
			
			fv [0] = pMesh ->m_vAmbient .m_x;
			fv [1] = pMesh ->m_vAmbient .m_y;
			fv [2] = pMesh ->m_vAmbient .m_z;
			glMaterialfv (GL_FRONT, GL_AMBIENT, fv);

			fv [0] = pMesh ->m_vDiffuse .m_x;
			fv [1] = pMesh ->m_vDiffuse .m_y;
			fv [2] = pMesh ->m_vDiffuse .m_z;
			glMaterialfv (GL_FRONT, GL_DIFFUSE, fv);

			fv [0] = pMesh ->m_vSpecular .m_x;
			fv [1] = pMesh ->m_vSpecular .m_y;
			fv [2] = pMesh ->m_vSpecular .m_z;
			glMaterialfv (GL_FRONT, GL_SPECULAR, fv);

			//
			// Initialize the veritices
			//

			glVertexPointer (3, GL_FLOAT, 0, (float *) sRes .GetRawData (
				pMesh ->m_pavVerts .GetOffset ()));

			//
			// Initialize the tex coords
			//

			if (pMesh ->m_pavTVerts [0] .GetOffset () != 0xFFFFFFFF)
			{
				glTexCoordPointer (2, GL_FLOAT, 0, (float *) sRes .GetRawData (
					pMesh ->m_pavTVerts [0] .GetOffset ()));
                glEnableClientState (GL_TEXTURE_COORD_ARRAY);
			}
			else
                glDisableClientState (GL_TEXTURE_COORD_ARRAY);

			//
			// Initialize the normal array
			//

			if (pMesh ->m_pavNormals .GetOffset () != 0xFFFFFFFF)
			{
				glNormalPointer (GL_FLOAT, 0, (float *) sRes .GetRawData (
					pMesh ->m_pavNormals .GetOffset ()));
                glEnableClientState (GL_NORMAL_ARRAY);
			}
			else
                glDisableClientState (GL_NORMAL_ARRAY);

			//
			// Draw
			//

			UINT32 *pulCounts = pMesh ->m_sVertexIndicesCount .
				GetData (sRes .GetModelData ());
			CNwnPointer <UINT16> *pulOffsets = pMesh ->m_sRawVertexIndices .
				GetData (sRes .GetModelData ());

			for (int i = 0; i < pMesh ->m_sRawVertexIndices .GetCount (); i++)
			{
				UINT16 *pusFaces = (UINT16 *) 
					sRes .GetRawData (pulOffsets [i] .GetOffset ());
				glDrawElements (GL_TRIANGLES, pulCounts [i],
					GL_UNSIGNED_SHORT, pusFaces);
			}
		}
	}

	//
	// Handle the children
	//

	for (int i = 0; i < m_apRtNodes .GetCount (); i++)
		m_apRtNodes [i] ->Render (sRes);

	//
	// Restore the matrix
	//

	glPopMatrix ();
	return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Load the textures
//
// @parm CResModelRef & | sRes | Resource 
//
// @parm int * | pasPalette | Texture palette
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMdlRtNode::LoadTextures (CResModelRef &sRes, int *pasPalette)
{

	//
	// If we have a texture, load it
	//

	if (m_fMesh)
	{
		CNwnMdlMeshHeader *pMesh = (CNwnMdlMeshHeader *) m_pNode;
		if (pMesh ->m_szTextures [0] [0] != 0)
		{
			m_pTexture = g_sResourceCache .LoadTexture (
				sRes ->m_pModuleFile, sRes ->m_pModuleFile == NULL &&
				sRes ->m_pKeyFile == NULL ? 
				(const char *) sRes ->m_strResName : NULL,
				pMesh ->m_szTextures [0], pasPalette);
		}
	}

	//
	// Handle the children
	//

	for (int i = 0; i < m_apRtNodes .GetCount (); i++)
		m_apRtNodes [i] ->LoadTextures (sRes, pasPalette);
	return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Unload the textures
//
// @parm CResModelRef & | sRes | Resource 
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMdlRtNode::UnloadTextures (CResModelRef &sRes)
{

	//
	// Clear the texture
	//

	m_pTexture = NULL;

	//
	// Handle the children
	//

	for (int i = 0; i < m_apRtNodes .GetCount (); i++)
		m_apRtNodes [i] ->UnloadTextures (sRes);
	return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Process the controller data
//
// @parm NwnMdlControllerKey * | paControllerKey | Controller key list
//
// @parm int | nControllerKeyCount | Controller key count
//
// @parm float * | pafControllerData | Controller data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMdlRtNode::ProcessControllers (NwnMdlControllerKey *paControllerKey, 
	int nControllerKeyCount, float *pafControllerData)
{
	
	//
	// Look for controller data we are interested in
	//

	for (int i = 0; i < nControllerKeyCount; i++)
	{
		if (paControllerKey [i] .lType == NwnMdlCT_Position)
		{
			float *pafData = &pafControllerData [
				paControllerKey [i] .sDataOffset];
			m_vPosition .m_x = pafData [0];
			m_vPosition .m_y = pafData [1];
			m_vPosition .m_z = pafData [2];
		}
		else if (paControllerKey [i] .lType == NwnMdlCT_Orientation)
		{
			float *pafData = &pafControllerData [
				paControllerKey [i] .sDataOffset];
			m_qOrientation .m_x = pafData [0];
			m_qOrientation .m_y = pafData [1];
			m_qOrientation .m_z = pafData [2];
			m_qOrientation .m_w = pafData [3];
		}
		else if (paControllerKey [i] .lType == NwnMdlCT_Scale)
		{
			float *pafData = &pafControllerData [
				paControllerKey [i] .sDataOffset];
			m_fScale = pafData [0];
		}
		else if (paControllerKey [i] .lType == NwnMdlCT_Alpha)
		{
			float *pafData = &pafControllerData [
				paControllerKey [i] .sDataOffset];
			m_fAlpha = pafData [0];
		}
	}
}

