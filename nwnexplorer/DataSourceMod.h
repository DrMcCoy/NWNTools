#ifndef ETS_DATASOURCEMOD_H
#define ETS_DATASOURCEMOD_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module	DataSource.h - Resource file data source |
//
// This module contains the definition of the resource file data sources.
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

#include "../_NwnLib/NwnModuleFile.h"
#include "DataSource.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Helper structures
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CDataSourceMod : public CDataSource
{
public:

	enum Type
	{
		Type_Root		= 0,
		TypeResourceClass	= 1,
		TypeResourceource	= 2,
	};

// @access Construction and destruction
public:

	// @cmember General constructor

	CDataSourceMod (CNwnModuleFile *pModFile, LPCTSTR pszName);

	// @cmember General destructor

	virtual ~CDataSourceMod ();

// @access Public method
public:

	// @cmember Does this element have children

	virtual bool HasChildren (DataElement *pElement);

	// @cmember Is this element a resource

	virtual bool IsResource (DataElement *pElement);

	// @cmember Can export

	virtual bool CanExport (DataElement *pElement);

	// @cmember Add the root item

	virtual HTREEITEM AddRoot (CTreeViewCtrl &tv, HTREEITEM hParent);

	// @cmember Add the children to the tree view

	virtual void AddChildren (CTreeViewCtrl &tv, 
		HTREEITEM hParent, DataElement *pElement);

	// @cmember Get the resource text

	virtual void GetResourceText (DataElement *pElement, 
		LPTSTR pszText, int cchMaxText);

	// @cmember Get the resource name

	virtual void GetResourceName (DataElement *pElement, 
		LPTSTR pszText, int cchMaxText, NwnResType *pnResType);

	// @cmember Load a resource

	virtual CData *LoadRes (DataElement *pElement);

	// @cmember Export resources

	virtual int Export (DataElement *pElement, 
		const char *pszFileName);

// @access Protected methods
protected:

	// @cmember Module file name

	CString				m_strModFileName;

	// @cmember Module file

	CNwnModuleFile		*m_pModFile;

	// @cmember Resource listing

	const CNwnModuleFile::Resource	*m_pasRes;

	// @cmember Element list

	DataElement			*m_pasElements;

	// @cmember Root element

	int					m_nRootElement;

	// @cmember First res class element

	int					m_nFirstResClassElement;
};

#endif // ETS_DATASOURCEMOD_H
