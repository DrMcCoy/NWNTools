#ifndef ETS_MDLRTNODE_H
#define ETS_MDLRTNODE_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module	MdlRtNode.h - Root structure for model runtime nodes |
//
// This module contains the definition of the model runtine nodes
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
// $History: HexView.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "../_MathLib/Vector3.h"
#include "../_MathLib/Quaternion.h"
#include "../_NwnLib/NwnModel.h"
#include "DataRef.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CMdlRtNode
{
public:

// @access Construction and destruction
public:

	// @cmember General constructor

	CMdlRtNode (CNwnMdlNodeHeader *pNode);

	// @cmember General destructor

	virtual ~CMdlRtNode ();

// @access Public method
public:

// @access Public virtual method
public:

	// @cmember Render the nodes

	virtual void Render (CResModelRef &sRes);

	// @cmember Load the textures

	virtual void LoadTextures (CResModelRef &sRes, int *pasPalette);

	// @cmember Unload the textures

	virtual void UnloadTextures (CResModelRef &sRes);

// @access Public static methods
public:

	// @cmember Create the node and it's children

	static CMdlRtNode *CreateNodes (CResModelRef &sRes, int nOffset);

// @access Protected methods
protected:

	// @cmember Process a controller list

	void ProcessControllers (NwnMdlControllerKey *paControllerKey, 
        int nControllerKeyCount, float *pafControllerData);

// @access Protected methods
protected:

	// @cmember Original node

	CNwnMdlNodeHeader			*m_pNode;

	// @cmember Child nodes

	CAtlArray <CMdlRtNode *>	m_apRtNodes;

	// @cmember Position

	CVector3					m_vPosition;

	// @cmember Orientation

	CQuaternion					m_qOrientation;

	// @cmember Scale

	float						m_fScale;

	// @cmember Rendering alpha valuee

	float						m_fAlpha;

	// @cmember If true, this is a mesh to render

	bool						m_fMesh;

	// @cmember Pointer to the texture data ref

	CDataRef					m_pTexture;
};

#endif // ETS_MDLRTNODE_H
