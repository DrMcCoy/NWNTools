#ifndef ETS_NWNOPTIONSPAGE_H
#define ETS_NWNOPTIONSPAGE_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module	NwnOptionsPage.h - NWN options page |
//
// This module contains the definition of the NWN options page.
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
// $History: NwnOptionsPage.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

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

//
// Class definitions
//

class CNwnOptionsPage : 
	public CPropertyPageImpl <CNwnOptionsPage>
{
// @access Enumerations and types
public:

	enum { IDD = IDD_NWN };

// @access Constructors
public:

	// @cmember General constructor

	CNwnOptionsPage ();

	// @cmember General destructor

	~CNwnOptionsPage ();

// @access Public virtual methods
public:

	// @cmember Set the active page

	BOOL OnSetActive ();

	// @cmember Kill the active page

	BOOL OnKillActive ();

// @access ATL dialog support
public:
	BEGIN_MSG_MAP (CTopicGeneralPage)
	    MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog)

	    COMMAND_HANDLER (IDC_LOAD_NWN_FROM, BN_CLICKED, OnLoadNwnFrom) 
	    COMMAND_HANDLER (IDC_NWN_DIRECTORY_BROWSE, BN_CLICKED, OnDirectoryBrowse) 

		CHAIN_MSG_MAP (CPropertyPageImpl <CNwnOptionsPage>);

		REFLECT_NOTIFICATIONS ();
	END_MSG_MAP ()

// @access Message handlers
protected:

	// @cmember Initialize the dialog
	
	LRESULT OnInitDialog (UINT uMsg, WPARAM wParam, 
		LPARAM lParam, BOOL &bHandled);

	// @cmember Change the button

	LRESULT OnLoadNwnFrom (WORD wNotifyCode, 
		WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		UpdateWindows ();
		return TRUE;
	}
	
	// @cmember Browse for a directory

	LRESULT OnDirectoryBrowse (WORD wNotifyCode, 
		WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		CString str;
		m_editNwnDirectory .GetWindowText (str);
		CFolderDialog dlg (m_hWnd);
		if (dlg .DoModal () == IDOK)
		{
			m_editNwnDirectory .SetWindowText (
				dlg .m_szFolderPath);
		}
		return TRUE;
	}
	
// @access Protected methods
protected:

	// @cmember Update the windows

	void UpdateWindows ()
	{
		bool fEnable = m_btnLoadNwnFrom .GetCheck () != 0;
		m_editNwnDirectory .EnableWindow (fEnable);
		m_btnNwnDirectory .EnableWindow (fEnable);
	}

// @access Public variables
public:

	CString			m_strNwnDirectory;

// @access Protected members
protected:

	// @cmember Beauty over speed button

	CButton			m_btnLoadNwnFrom;

	// @cmember NWN directory edit

	CEdit			m_editNwnDirectory;

	// @cmember NWN directory browse

	CButton			m_btnNwnDirectory;
};

#endif // ETS_NWNOPTIONSPAGE_H
