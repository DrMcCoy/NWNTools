//-----------------------------------------------------------------------------
// 
// @doc
//
// @module	NwnOptionsPage.cpp - Nwn options page |
//
// This module contains the definition of the Nwn options page
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
// $History: NwnOptionsPage.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "NwnOptionsPage.h"
#include "nwnexplorer.h"

#if defined (_DEBUG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern TCHAR g_szAppName [];

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnOptionsPage> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnOptionsPage::CNwnOptionsPage ()
{

	//
	// Set the icon and enable help
	//

#if !defined (_WIN32_WCE)
//	m_psp .dwFlags |= PSP_HASHELP | PSP_USEICONID;
//	m_psp .pszIcon = MAKEINTRESOURCE (IDR_IO_MONITOR);
#endif
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnOptionsPage> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnOptionsPage::~CNwnOptionsPage ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Initialize the dialog
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

LRESULT CNwnOptionsPage::OnInitDialog (UINT uMsg, 
	WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

	//
	// Center the sheet
	//

	CenterPropertySheet (GetPropertySheet ());

	//
	// Subclass the controls
	//

	m_btnLoadNwnFrom = GetDlgItem (IDC_LOAD_NWN_FROM);
	m_editNwnDirectory = GetDlgItem (IDC_NWN_DIRECTORY);
	m_btnNwnDirectory = GetDlgItem (IDC_NWN_DIRECTORY_BROWSE);
	m_editNwnDirectory .LimitText (_MAX_PATH);
	return 0;
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the page active
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

BOOL CNwnOptionsPage::OnSetActive ()
{

	//
	// Set the fields
	//

	if (m_strNwnDirectory .IsEmpty ())
	{
		m_btnLoadNwnFrom .SetCheck (0);
	}
	else
	{
		m_btnLoadNwnFrom .SetCheck (1);
		m_editNwnDirectory .SetWindowText (m_strNwnDirectory);
	}

	//
	// Update the windows
	//

	UpdateWindows ();
	return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Kill the page active
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

BOOL CNwnOptionsPage::OnKillActive ()
{
	CString str;

	//
	// If we are using default directories
	//

	if (m_btnLoadNwnFrom .GetCheck () == 0)
	{
		m_strNwnDirectory .Empty ();
	}
	else
	{
		m_editNwnDirectory .GetWindowText (m_strNwnDirectory);
		m_strNwnDirectory .TrimRight ();
		m_strNwnDirectory .TrimLeft ();
		if (m_strNwnDirectory .IsEmpty ())
		{
			str .Format (IDS_ERR_NO_DIRECTORY);
			::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
			return FALSE;
		}
		if (m_strNwnDirectory [m_strNwnDirectory .GetLength () - 1] != '\\' &&
			m_strNwnDirectory [m_strNwnDirectory .GetLength () - 1] != '/' &&
			m_strNwnDirectory [m_strNwnDirectory .GetLength () - 1] != ':')
			m_strNwnDirectory += _T ("\\");
	}

	//
	// Validate
	//

	str = m_strNwnDirectory;
	if (str .IsEmpty ())
		str = GetNwnDirectory ();

	//
	// Try to open the dialog file
	//

	str += "dialog.tlk";
	FILE *fp = _tfopen (str, "rb");
	if (fp == NULL)
	{
		str .Format (IDS_ERR_INV_DIRECTORY);
		::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
		return FALSE;
	}
	else
		fclose (fp);
	return TRUE;
}

