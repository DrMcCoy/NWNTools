#ifndef ETS_NWNEXPLORER_H
#define ETS_NWNEXPLORER_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module	nwnexplorer.h - Global definitions |
//
// This module contains the global definitions
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
// $History: CnfMainWnd.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "resource.h"
#include "../_NwnLib/NwnDefines.h"
#include "../_MathLib/Matrix.h"
#include "../_MathLib/Vector3.h"
#include "Data.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Enumerations
//
//-----------------------------------------------------------------------------

enum TB_TextStyle
{
	TBTS_NoText					= 0,
	TBTS_BottomText				= 1,
	TBTS_RightText				= 2,
};

enum TB_IconStyle
{
	TBIS_SmallIcons				= 0,
	TBIS_LargeIcons				= 1,
};

enum ResClass
{
	ResClass__First				= 0,
	ResClass_Misc				= 0,
	ResClass_Sound				= 1,
	ResClass_Texture			= 2,
	ResClass_GameData			= 3,
	ResClass_TextureInfo		= 4,
	ResClass_Ini				= 5,
	ResClass_Image				= 6,
	ResClass_Model				= 7,
	ResClass_UI					= 8,
	ResClass_Script				= 9,
	ResClass_Compiled_Script	= 10,
	ResClass_Dialog				= 11,
	ResClass_Blueprint_Palette	= 12,
	ResClass_BP_Waypoint		= 13,
	ResClass_BP_Trigger			= 14,
	ResClass_BP_Sound			= 15,
	ResClass_BP_Placeable		= 16,
	ResClass_BP_Item			= 17,
	ResClass_BP_Encounter		= 18,
	ResClass_BP_Door			= 19,
	ResClass_BP_Creatures		= 20,
	ResClass_BP_Merchant		= 21,
	ResClass_Sound_Set			= 22,
	ResClass_Graphical_Palette	= 23,
	ResClass_Model_Server		= 24,
	ResClass_Area				= 25,
	ResClass_Module				= 26,
	ResClass_Plot				= 27,
	ResClass_Texture_DDS		= 28,
	ResClass_Debug_Script		= 29,
	ResClass__Last				= 30,
};

//-----------------------------------------------------------------------------
//
// Structures
//
//-----------------------------------------------------------------------------

struct TB_Command
{
	UINT			nCmd;
	UINT			fsStyle;
	UINT			iBitmap;
};

struct ModelContext
{
	CMatrix			mView;
	CVector3		vView;
	CVector3		vCenterOfRotation;
	int				anPalettes [NwnPalette__Count];

};

//-----------------------------------------------------------------------------
//
// Messages
//
//-----------------------------------------------------------------------------

#define WM_APP_MDLCTXCHANGED		(WM_APP + 10)

//-----------------------------------------------------------------------------
//
// Profile strings
//
//-----------------------------------------------------------------------------

#define PROFILE_WINPLACEMENT		_T ("WindowPlacement")
#define PROFILE_NWN_DIRECTORY		_T ("NwnDirectory")
#define PROFILE_TBTEXTSTYLE			_T ("TBTextStyle")
#define PROFILE_TBICONSTYLE			_T ("TBIconStyle")
#define PROFILE_TBBUTTONS			_T ("TBButtons")

#define PROFILE_TBTEXTSTYLE_DEFAULT	TBTS_BottomText
#define PROFILE_TBICONSTYLE_DEFAULT	TBIS_LargeIcons

//-----------------------------------------------------------------------------
//
// Bitmap routines
//
//-----------------------------------------------------------------------------

SIZE GetBitmapSize (HBITMAP hBitmap);

//-----------------------------------------------------------------------------
//
// Other general routines
//
//-----------------------------------------------------------------------------

bool ExtractString (LPTSTR pszReturn, int nMaxChar, 
	LPCTSTR pszSource, int nSubString, TCHAR cSeperator);
void CenterPropertySheet (CWindow &sSheet);
HMENU LoadPopupMenu (HINSTANCE hInstance, UINT id, UINT uSubMenu);
bool IsValidBlueprintName (LPCTSTR pszName);
bool IsValidTableName (LPCTSTR pszName);
bool IsValidProfileName (LPCTSTR pszName);
BOOL GetDllVersion (LPCTSTR pszDll, DWORD &dwMajor, DWORD &dwMinor);
int DecodeToolbarButtons (LPCTSTR pszText, TBBUTTON *pTBBtns, int nMaxButtons);

DWORD GetSetting (LPCTSTR pszName, DWORD dwDefault);
bool SetSetting (LPCTSTR pszName, DWORD dwValue);
bool GetSetting (LPCTSTR pszName, LPCTSTR pszDefault, 
				 LPTSTR pszValue, int cchMaxText);
bool SetSetting (LPCTSTR pszName, LPCTSTR pszValue);
D3DCOLOR *TgaLoad (unsigned char *pauchData, int *pnWidth, int *pnHeight);
D3DCOLOR *TgaLoad (UINT nID, int *pnWidth, int *pnHeight);
HBITMAP TgaLoad (unsigned char *pauchData);

ResClass GetResourceClass (NwnResType nType);
CString GetNwnDirectory ();

#endif // ETS_NWNEXPLORER_H
