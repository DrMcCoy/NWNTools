//-----------------------------------------------------------------------------
// 
// @doc
//
// @module	nwnexplorer.cpp - Main module |
//
// This module contains the main.
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
// $History: Cnf.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "nwnexplorer.h"
#include "mainwnd.h"
#include <fmod/fmod.h>

//
// Debug NEW
//

#if defined (_DEBUG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Global strings
//

TCHAR g_szAppName [256];
HWND g_hWndMain;
CPrinter g_sPrinter;
CDevMode g_sDevMode;
CImageList g_ilSmall;
CImageList g_ilLarge;
HMODULE g_hFMODLibrary = NULL;
D3DCOLOR *g_apPalettes [NwnPalette__Count];

//
// Constant file names
//

LPCTSTR g_pszKeyRoot = _T ("Software\\Torlack\\nwnexplorer");

//
// Toolbar command list
//

TB_Command g_asCommands [] =
{
	{ ID_FILE_OPEN,					TBSTYLE_BUTTON, 0 },
	{ ID_APP_ABOUT,                 TBSTYLE_BUTTON, 11 },
};
int g_nCommands = _countof (g_asCommands);

//
// ATL module
//

CAppModule _Module;
BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

//
// Correct an ommision in the OnIdle
//

class CMyMessageLoop : public CMessageLoop
{
public:
	// override to change idle processing
	virtual BOOL OnIdle(int /*nIdleCount*/)
	{
		BOOL fContinue = FALSE;
		for(int i = 0; i < m_aIdleHandler.GetSize(); i++)
		{
			CIdleHandler* pIdleHandler = m_aIdleHandler[i];
			if(pIdleHandler != NULL)
			{
				if (pIdleHandler->OnIdle())
					fContinue = TRUE;
			}
		}
		return fContinue;
	}
};

#pragma pack(1)
typedef struct tagTgaHeader
{
    BYTE   IdLength;            // Image ID Field Length
    BYTE   CmapType;            // Color Map Type
    BYTE   ImageType;           // Image Type

    WORD   CmapIndex;           // First Entry Index
    WORD   CmapLength;          // Color Map Length
    BYTE   CmapEntrySize;       // Color Map Entry Size

    WORD   X_Origin;            // X-origin of Image
    WORD   Y_Origin;            // Y-origin of Image
    WORD   ImageWidth;          // Image Width
    WORD   ImageHeight;         // Image Height
    BYTE   PixelDepth;          // Pixel Depth
    BYTE   ImagDesc;            // Image Descriptor
} TGAHEADER;
#pragma pack()

#define TGA_Null 0
#define TGA_Map 1
#define TGA_RGB 2
#define TGA_Mono 3
#define TGA_RLEMap 9
#define TGA_RLERGB 10
#define TGA_RLEMono 11
#define TGA_CompMap 32
#define TGA_CompMap4 33

//-----------------------------------------------------------------------------
//
// @func Windows Main
//
// @rdesc Program exit code
//
//-----------------------------------------------------------------------------

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, int nCmdShow)
{
	USES_CONVERSION;

	//
	// Resolve UNICOWS's thunk (required)
	//

	::DefWindowProc (NULL, 0, 0, 0);

	//
	// Load the application name
	//

	::LoadString (_Module .GetResourceInstance (),
		IDR_NWNEXPLORER, g_szAppName, _countof (g_szAppName));

	//
	// Test for FMOD
	//

	g_hFMODLibrary = ::LoadLibrary (_T ("FMOD.DLL"));

	//
	// Initialize FMOD
	//

	if (g_hFMODLibrary != NULL)
	{
		if (FSOUND_GetVersion () < FMOD_VERSION)
		{
			//FIXME
		}
		if (!FSOUND_Init (44100, 32, FSOUND_INIT_GLOBALFOCUS))
		{
			//FIXME
			FSOUND_Close ();
			return 0;
		}
	}

	//
	// Create our image lists
	//

	{
		CDC dcMem;
		dcMem .CreateCompatibleDC (NULL);
		if (dcMem .GetDeviceCaps (BITSPIXEL) > 8)
		{
			g_ilSmall .Create (16, 16, ILC_COLOR16 | ILC_MASK, 16, 16);
			g_ilLarge .Create (24, 24, ILC_COLOR16 | ILC_MASK, 16, 16);
			CBitmap bmSmall;
			bmSmall .LoadBitmap (IDB_TOOLBAR_16_256COLOR);
			g_ilSmall .Add (bmSmall, RGB (255, 0, 255));
			CBitmap bmLarge;
			bmLarge .LoadBitmap (IDB_TOOLBAR_24_256COLOR);
			g_ilLarge .Add (bmLarge, RGB (255, 0, 255));
		}
		else
		{
			g_ilSmall .Create (16, 16, ILC_COLOR | ILC_MASK, 16, 16);
			g_ilLarge .Create (24, 24, ILC_COLOR | ILC_MASK, 16, 16);
			CBitmap bmSmall;
			bmSmall .LoadBitmap (IDB_TOOLBAR_16_16COLOR);
			g_ilSmall .Add (bmSmall, RGB (255, 0, 255));
			CBitmap bmLarge;
			bmLarge .LoadBitmap (IDB_TOOLBAR_24_16COLOR);
			g_ilLarge .Add (bmLarge, RGB (255, 0, 255));
		}
		g_ilSmall .SetBkColor (::GetSysColor (COLOR_3DFACE));
		g_ilLarge .SetBkColor (::GetSysColor (COLOR_3DFACE));
	}

	//
	// Enable leak checking
	//

#if defined (_DEBUG)
	_CrtSetDbgFlag (_CrtSetDbgFlag (_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif

	//
	// Initialize ATL
	//

	_Module.Init (ObjectMap, hInstance);

	//
	// Initialize the printer
	//

	g_sPrinter .OpenDefaultPrinter ();
	g_sDevMode .CopyFromPrinter (g_sPrinter);

	//
	// Validate the version of the common controls
	//

	DWORD dwComCtlMajor, dwComCtlMinor;
	if (!GetDllVersion (TEXT ("comctl32.dll"), 
	    dwComCtlMajor, dwComCtlMinor) || dwComCtlMajor < 5 ||
		(dwComCtlMajor == 5 && dwComCtlMinor < 80))
	{
		::MessageBox (NULL, 
			_T ("You are running an old version of comctl32.dll.\r\n")
			_T ("Please download a new version from:\r\n\r\n")
			_T ("http://www.torlack.com/other/50comupd.exe")
			, g_szAppName, MB_OK | MB_ICONHAND);
		return 0;
	}

	//
	// Initialize the common controls
	//

    INITCOMMONCONTROLSEX icex;    
	typedef WINCOMMCTRLAPI BOOL WINAPI _x (LPINITCOMMONCONTROLSEX); 
	_x *px;
	HINSTANCE hComCtl = LoadLibraryA ("comctl32.dll");
	px = (_x *) GetProcAddress (hComCtl, "InitCommonControlsEx");
	bool fWorked = false;
	if (px != NULL)
	{
		icex .dwSize = sizeof (INITCOMMONCONTROLSEX);
		icex .dwICC = ICC_WIN95_CLASSES | ICC_DATE_CLASSES | 
			ICC_USEREX_CLASSES | ICC_COOL_CLASSES;
		fWorked = (*px) (&icex) != 0;
	}
	if (!fWorked)
	{
		::MessageBox (NULL, 
	        _T ( "Unable to initialize COMCTL32.DLL"), 
			g_szAppName, MB_OK | MB_ICONHAND);
		return 0;
	}

	//
	// Create the message loop
	//

	CMyMessageLoop theLoop;
	_Module .AddMessageLoop (&theLoop);

	//
	// Create the window
	//

	CMainWnd sMainWnd;
	g_hWndMain = sMainWnd .Create ();
	if (g_hWndMain == NULL)
	{
		CString str (MAKEINTRESOURCE (IDS_ERR_MAIN_WINDOW));
		::MessageBox (NULL, str, g_szAppName, MB_OK | MB_ICONHAND);
		return 0;
	}

	//
	// Pump messages
	//

	theLoop .Run ();
	_Module .RemoveMessageLoop ();

	//
	// Delete the palettes
	//

	for (int i = 0; i < _countof (g_apPalettes); i++)
	{
		if (g_apPalettes [i])
			delete [] g_apPalettes [i];
	}

	//
	// Close up printer (Not required, but keeps Purify happy)
	//

	g_sPrinter .ClosePrinter ();
	g_sDevMode .Detach ();

	//
	// Close FMOD
	//

	if (g_hFMODLibrary != NULL)
	{
		FSOUND_Close ();
	}

	//
	// Terminate the module
	//

	_Module .Term ();
	return 0;
}

//-----------------------------------------------------------------------------
//
// @func Extract a substring
//
// @parm LPTSTR | pszReturn | Return string space
//
// @parm int | nMaxChan | Size of the return buffer in character
//
// @parm LPCTSTR | pszSource | Source string
//
// @parm int | nSubString | Substring to extract
//
// @parm TCHAR | cSeperator | Seperator character
//
// @rdesc Results of the extraction
//
//		@flag true | Substring was extracted
//		@flag false | Substring not found
//
//-----------------------------------------------------------------------------

bool ExtractString (LPTSTR pszReturn, int nMaxChar, 
	LPCTSTR pszSource, int nSubString, TCHAR cSeperator)
{

	//
	// Validate the return string
	//

	if (pszReturn == NULL || nMaxChar < 1) 
		return false;

	//
	// Validate substring
	//

	if (nSubString < 0 || pszSource == NULL)
	{
		pszReturn [0] = 0;
		return false;
	}

	//
	// Move the source string pointer forward until it points to
	// the start of the string to be extracted
	//

	for (; nSubString > 0; nSubString--) 
	{
	    pszSource = _tcschr (pszSource, cSeperator);
	    if (pszSource == NULL) 
		{
	        pszReturn [0] = 0;
			return false;
	    }
	    pszSource++;
	}

	//
	// Locate the end of the string
	//

	LPCTSTR psz = _tcschr (pszSource, cSeperator);
	int nLen = (int) (psz == NULL ? _tcslen (pszSource) : psz - pszSource);

	//
	// Clamp the length
	//

	if (nLen > nMaxChar - 1) 
		nLen = nMaxChar - 1;

	//
	// Copy the string
	//

	_tcsncpy (pszReturn, pszSource, nLen);
	pszReturn [nLen] = 0;	
	return true;
}

//-----------------------------------------------------------------------------
//
// @func Get a bitmap size
//
// @parm HBITMAP | hBitmap | Bitmap
//
// @rdesc Size of the bitmap
//
//-----------------------------------------------------------------------------

SIZE GetBitmapSize (HBITMAP hBitmap)
{
	BITMAP bm;
	::GetObject (hBitmap, sizeof (BITMAP), &bm);
	return CSize (bm .bmWidth, bm .bmHeight);
}

//-----------------------------------------------------------------------------
//
// @func Center the property sheet
//
// @parm CWindow & | sSheet | The sheet
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CenterPropertySheet (CWindow &sSheet)
{
	static LPCTSTR pszProp = _T ( "SheetCentered");
	if (::GetProp (sSheet .m_hWnd, pszProp) != 0)
		return;
	sSheet .CenterWindow (sSheet .GetParent ());
	::SetProp (sSheet .m_hWnd, pszProp, (HANDLE) 1);
}

//-----------------------------------------------------------------------------
//
// @func Load the given popup menu
//
// @parm HINSTANCE | hInstance | Instance of the resource
//
// @parm UINT | id | ID of the menu resource
//
// @parm UINT | uSubMenu | Sub menu index of the popup
//
// @rdesc Handle of the popup menu or NULL if invalid.
//
//-----------------------------------------------------------------------------

HMENU LoadPopupMenu (HINSTANCE hInstance, UINT id, UINT uSubMenu)
{
    HMENU hmParent = ::LoadMenu (hInstance, MAKEINTRESOURCE (id));
    if (hmParent == NULL) 
		return NULL;
    HMENU hmPopup = GetSubMenu (hmParent, uSubMenu);
    RemoveMenu (hmParent, uSubMenu, MF_BYPOSITION);
    DestroyMenu (hmParent);
    return hmPopup;
}

//-----------------------------------------------------------------------------
//
// @func Decode the buttons
//
// @parm LPCTSTR | pszText | Button string
//
// @parm TBBUTTON * | pTBBtns | Array of buttons
//
// @parm int | nMaxButtons | Max buttons
//
// @rdesc Number of buttons placed in the array or -1 if there is 
//		an error.
//
//-----------------------------------------------------------------------------

int DecodeToolbarButtons (LPCTSTR pszText, TBBUTTON *pTBBtns, int nMaxButtons)
{
	int c;

	//
	// Initialize to no buttons
	//

	int nButtons = 0;

	//
	// Loop while we have buttons
	//

	while (*pszText)
	{

		//
		// Skip whitespace
		//

		while (true)
		{
			c = _tcsnextc (pszText);
			if (!_istspace (c))
				break;
			pszText = _tcsinc (pszText);
		}

		//
		// Break if end of string
		//

		if (c == 0)
			break;

		//
		// Convert the next block of digits
		//

		LPTSTR pszEnd;
		errno = 0;
		unsigned long ulCmd = _tcstoul (pszText, &pszEnd, 10);
		if (errno != 0 || pszText == pszEnd)
			return -1;
		pszText = pszEnd;

		//
		// psz whitespace
		//

		while (true)
		{
			c = _tcsnextc (pszText);
			if (!_istspace (c))
				break;
			pszText = _tcsinc (pszText);
		}

		//
		// This MUST be a comma or 0
		//

		if (c == ',')
			pszText = _tcsinc (pszText);
		else if (c == 0)
			;
		else
			return -1;

		//
		// We have a value
		//

		if (ulCmd == 0)
		{
			if (nButtons >= nMaxButtons)
				break;
			pTBBtns [nButtons].iBitmap = 8;
			pTBBtns [nButtons].idCommand = 0;
			pTBBtns [nButtons].fsState = 0;
			pTBBtns [nButtons].fsStyle = TBSTYLE_SEP;
			pTBBtns [nButtons].dwData = 0;
			pTBBtns [nButtons].iString = 0;
			nButtons++;
		}
		else
		{
			int i;
			for (i = 0; i < g_nCommands; i++)
			{
				if (g_asCommands [i] .nCmd == (int) ulCmd)
					break;
			}
			if (i < g_nCommands)
			{
				if (nButtons >= nMaxButtons)
					break;
				pTBBtns [nButtons].iBitmap = g_asCommands [i] .iBitmap;
				pTBBtns [nButtons].idCommand = g_asCommands [i] .nCmd;
				pTBBtns [nButtons].fsState = TBSTATE_ENABLED;
				pTBBtns [nButtons].fsStyle = g_asCommands [i] .fsStyle;
				pTBBtns [nButtons].dwData = 0;
				pTBBtns [nButtons].iString = 0;
				nButtons++;
			}
		}
	}
	return nButtons;
}

//-----------------------------------------------------------------------------
//
// @func Test if a name is valid
//
// @parm LPCTSTR | pszName | Name of the rune library
//
// @rdesc True if the name is valid
//
//-----------------------------------------------------------------------------

bool IsValidTableName (LPCTSTR pszName)
{
	if (*pszName == 0)
		return false;
	while (*pszName)
	{
		int c = _tcsnextc (pszName);
		pszName = _tcsinc (pszName);
		if (!_istalnum (c) && c != '_')
			return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
//
// @func Test if a name is valid
//
// @parm LPCTSTR | pszName | Name of the rune library
//
// @rdesc True if the name is valid
//
//-----------------------------------------------------------------------------

bool IsValidBlueprintName (LPCTSTR pszName)
{
	if (*pszName == 0)
		return false;
	while (*pszName)
	{
		int c = _tcsnextc (pszName);
		pszName = _tcsinc (pszName);
		if (!_istalnum (c) && c != '_')
			return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
//
// @func Test if a name is valid
//
// @parm LPCTSTR | pszName | Name of the rune library
//
// @rdesc True if the name is valid
//
//-----------------------------------------------------------------------------

bool IsValidProfileName (LPCTSTR pszName)
{
	bool fHasStar = false;
	if (*pszName == 0)
		return false;
	while (*pszName)
	{
		int c = _tcsnextc (pszName);
		pszName = _tcsinc (pszName);
		if (!_istalnum (c) && c != '_')
		{
			if (c == '*')
			{
				if (fHasStar)
					return false;
				fHasStar = true;
			}
			else
                return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
//
// @func Get the version of the DLL
//
// @parm LPCTSTR | pszDll | Full path to the DLL
//
// @parm DWORD & | dwMajor | Major version number
//
// @parm DWORD & | dwMinor | Minor version number
//
// @rdesc Routine results
//
//		@flag true | if able to retrieve the version number
//		@flag false | if unable to retrieve the version number
//
//-----------------------------------------------------------------------------

BOOL GetDllVersion (LPCTSTR pszDll, DWORD &dwMajor, DWORD &dwMinor)
{
	HRESULT hResult = E_FAIL;

	//
	// Load the DLL
	//

	HINSTANCE hComCtl = LoadLibrary (pszDll);
	
	//
	// If loaded
	//

	if (hComCtl) 
	{

		//
		// Get the address of the DllGetVersion method.
		//

#if defined (_WIN32_WCE)
	    DLLGETVERSIONPROC pDllGetVersion = (DLLGETVERSIONPROC) 
			GetProcAddress (hComCtl, L"DllGetVersion");
#else
	    DLLGETVERSIONPROC pDllGetVersion = (DLLGETVERSIONPROC) 
			GetProcAddress (hComCtl, "DllGetVersion");
#endif

		//
		// If the method was found
		//

	    if (pDllGetVersion) 
		{

			//
			// Get the version information
			//

			DLLVERSIONINFO dvi;
	        memset (&dvi, 0, sizeof (dvi));
	        dvi .cbSize = sizeof (dvi);
			hResult = (*pDllGetVersion) (&dvi);

			//
			// Extract the version information
			//

			if (SUCCEEDED (hResult)) 
			{
				dwMajor = dvi .dwMajorVersion;
				dwMinor = dvi .dwMinorVersion;
			}
	    }

		//
		// Free the library
		//

	    FreeLibrary (hComCtl);
	}
	return SUCCEEDED (hResult);
}

//-----------------------------------------------------------------------------
//
// @func Get a setting from the register
//
// @parm LPCTSTR | pszName | Name of the setting
//
// @parm DWORD | dwDefault | Default value
//
// @rdesc Value.
//
//-----------------------------------------------------------------------------

DWORD GetSetting (LPCTSTR pszName, DWORD dwDefault)
{

	//
	// Open the parent
	//

	CRegKey rkParent;
	LONG lRet = rkParent .Open (HKEY_CURRENT_USER, g_pszKeyRoot);
	if (lRet != ERROR_SUCCESS)
		return dwDefault;

	//
	// Query the value
	//

	DWORD dwRet;
	lRet = rkParent .QueryDWORDValue (pszName, dwRet);
	if (lRet != ERROR_SUCCESS)
		return dwDefault;
	else
		return dwRet;
}

//-----------------------------------------------------------------------------
//
// @func Set a setting from the register
//
// @parm LPCTSTR | pszName | Name of the setting
//
// @parm DWORD | dwValue | New value
//
// @rdesc Success status.
//
//-----------------------------------------------------------------------------

bool SetSetting (LPCTSTR pszName, DWORD dwValue)
{

	//
	// Open the parent
	//

	CRegKey rkParent;
	LONG lRet = rkParent .Create (HKEY_CURRENT_USER, g_pszKeyRoot);
	if (lRet != ERROR_SUCCESS)
		return false;

	//
	// Set the value
	//

	lRet = rkParent .SetDWORDValue (pszName, dwValue);
	return lRet == ERROR_SUCCESS;
}
//-----------------------------------------------------------------------------
//
// @func Get a setting from the register
//
// @parm LPCTSTR | pszName | Name of the setting
//
// @parm LPCTSTR | pszDefault | Default value
//
// @parm LPTSTR | pszValue | Value
//
// @parm int | cchMaxText | Length of the buffer
//
// @rdesc Success status.
//
//-----------------------------------------------------------------------------

bool GetSetting (LPCTSTR pszName, LPCTSTR pszDefault, 
	LPTSTR pszValue, int cchMaxText)
{

	//
	// Open the parent
	//

	CRegKey rkParent;
	LONG lRet = rkParent .Open (HKEY_CURRENT_USER, g_pszKeyRoot);
	if (lRet != ERROR_SUCCESS)
	{
		_tcsncpy (pszValue, pszDefault, cchMaxText);
		pszValue [cchMaxText - 1] = 0;
		return false;
	}

	//
	// Query the value
	//

	DWORD dwLen = cchMaxText;
	lRet = rkParent .QueryStringValue (pszName, pszValue, &dwLen);
	if (lRet != ERROR_SUCCESS)
	{
		_tcsncpy (pszValue, pszDefault, cchMaxText);
		pszValue [cchMaxText - 1] = 0;
		return false;
	}
	else
		return true;
}

//-----------------------------------------------------------------------------
//
// @func Set a setting from the register
//
// @parm LPCTSTR | pszName | Name of the setting
//
// @parm LPCTSTR | pszValue | New value
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

bool SetSetting (LPCTSTR pszName, LPCTSTR pszValue)
{

	//
	// Open the parent
	//

	CRegKey rkParent;
	LONG lRet = rkParent .Create (HKEY_CURRENT_USER, g_pszKeyRoot);
	if (lRet != ERROR_SUCCESS)
		return false;

	//
	// Set the value
	//

	lRet = rkParent .SetStringValue (pszName, pszValue);
	return lRet == ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the resource class
//
// @parm NwnResType | nResType | Type of the resource
//
// @rdesc Resource class
//
//-----------------------------------------------------------------------------

ResClass GetResourceClass (NwnResType nType)
{
	switch (nType)
	{
		case NwnResType_RES: return ResClass_Misc;
		case NwnResType_BMP: return ResClass_Image;
		case NwnResType_MVE: return ResClass_Misc;
		case NwnResType_TGA: return ResClass_Texture;
		case NwnResType_WAV: return ResClass_Sound;
		case NwnResType_BMU: return ResClass_Sound;
		case NwnResType_PLT: return ResClass_Graphical_Palette;
		case NwnResType_INI: return ResClass_Ini;
		case NwnResType_MDL: return ResClass_Model;
		case NwnResType_NSS: return ResClass_Script;
		case NwnResType_NCS: return ResClass_Compiled_Script;
		case NwnResType_ARE: return ResClass_Area;
		case NwnResType_SET: return ResClass_Ini;
		case NwnResType_IFO: return ResClass_Module;
		case NwnResType_BIC: return ResClass_Misc;
		case NwnResType_WOK: return ResClass_Model_Server;
		case NwnResType_2DA: return ResClass_GameData;
		case NwnResType_DLG: return ResClass_Dialog;
		case NwnResType_TXI: return ResClass_TextureInfo;
		case NwnResType_GIT: return ResClass_Area;
		case NwnResType_UTI: return ResClass_BP_Item;
		case NwnResType_BTI: return ResClass_Misc;
		case NwnResType_UTC: return ResClass_BP_Creatures;
		case NwnResType_ITP: return ResClass_Blueprint_Palette;
		case NwnResType_UTT: return ResClass_BP_Trigger;
		case NwnResType_DDS: return ResClass_Texture_DDS;
		case NwnResType_UTS: return ResClass_BP_Sound;
		case NwnResType_LTR: return ResClass_Misc;
		case NwnResType_GFF: return ResClass_UI;
		case NwnResType_FAC: return ResClass_Module;
		case NwnResType_BTE: return ResClass_Misc;
		case NwnResType_UTE: return ResClass_BP_Encounter;
		case NwnResType_BTD: return ResClass_Misc;
		case NwnResType_UTD: return ResClass_BP_Door;
		case NwnResType_BTP: return ResClass_Misc;
		case NwnResType_UTP: return ResClass_BP_Placeable;
		case NwnResType_DTF: return ResClass_Misc;
		case NwnResType_GIC: return ResClass_Area;
		case NwnResType_GUI: return ResClass_UI;
		case NwnResType_CSS: return ResClass_Misc;
		case NwnResType_CCS: return ResClass_Misc;
		case NwnResType_BTM: return ResClass_Misc;
		case NwnResType_UTM: return ResClass_BP_Merchant;
		case NwnResType_DWK: return ResClass_Model_Server;
		case NwnResType_PWK: return ResClass_Model_Server;
		case NwnResType_BTG: return ResClass_Misc;
		case NwnResType_UTG: return ResClass_Misc;
		case NwnResType_JRL: return ResClass_Module;
		case NwnResType_SAV: return ResClass_Misc;
		case NwnResType_UTW: return ResClass_BP_Waypoint;
		case NwnResType_4PC: return ResClass_Misc;
		case NwnResType_SSF: return ResClass_Sound_Set;
		case NwnResType_NDB: return ResClass_Debug_Script;
		case NwnResType_PTM: return ResClass_Plot;
		case NwnResType_PTT: return ResClass_Plot;
		case 0x270C: return ResClass_Ini;
		case 0x000A: return ResClass_Ini;
		default: return ResClass_Misc;
	}
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the NWN directory from the registry
//
// @rdesc Directory
//
//-----------------------------------------------------------------------------

CString GetNwnDirectory ()
{
	CString str;

	//
	// Open the NWN registry key
	//

	CRegKey rkParent;
	LONG lRet = rkParent .Open (HKEY_LOCAL_MACHINE, 
		_T ("SOFTWARE\\BioWare\\NWN\\Neverwinter"),
		KEY_READ);

	//
	// Read the key
	//

	if (lRet == ERROR_SUCCESS)
	{
		TCHAR szText [_MAX_PATH];
		DWORD dwLen = _MAX_PATH;
		lRet = rkParent .QueryStringValue (_T ("Location"), szText, &dwLen);
		if (lRet == ERROR_SUCCESS)
		{
			str = szText;
			str += _T ("\\");
		}
	}
	return str;
}

//-----------------------------------------------------------------------------
//
// @func Read a single pixle from the TGA image
//
// @parm int | bpp | Bits per pixel
//
// @parm unsigned char * & | pauchData | Image data
//
// @parm D3DCOLOR * | pclrPalette | Color palette
//
// @rdesc Color in D3DCOLOR format
//
//-----------------------------------------------------------------------------

D3DCOLOR TgaReadPixel (int bpp, unsigned char *&pauchData,
	D3DCOLOR *pclrPalette)
{
	switch (bpp)
	{
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			return pclrPalette [*pauchData++];

		case 15:
			{
				UINT16 pixel = *((UINT16 *) pauchData++);
				pauchData += sizeof (UINT16);
				return RGBA_MAKE ((pixel >> 7) & 0x0F8, 
					(pixel >> 2) & 0xF8, (pixel << 3) & 0xF8, 0xFF);
			}

		case 16:
			{
				UINT16 pixel = *((UINT16 *) pauchData++);
				pauchData += sizeof (UINT16);
				return RGBA_MAKE ((pixel >> 7) & 0x0F8, 
					(pixel >> 2) & 0xF8, (pixel << 3) & 0xF8, 0xFF);
			}

		case 24:
			{
				D3DCOLOR clr = RGBA_MAKE (pauchData [0], pauchData [1], 
					pauchData [2], 0xFF);
				pauchData += 3;
				return clr;
			}

		case 32:
			{
				D3DCOLOR clr = RGBA_MAKE (pauchData [0], pauchData [1], 
					pauchData [2], pauchData [3]);
				pauchData += 4;
				return clr;
			}

		default:
			return RGBA_MAKE (0, 0, 0, 0xFF);
	}
}

//-----------------------------------------------------------------------------
//
// @func Read a line of uncompress TGA image
//
// @parm D3DCOLOR * | pout | Output data
//
// @parm int | nWidth | Width of the data
//
// @parm bool | fXReversed | Unused
//
// @parm int | bpp | Bits per pixel
//
// @parm unsigned char * & | pauchData | Image data
//
// @parm D3DCOLOR * | pclrPalette | Color palette
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void TgaExpandUncompressedLine (D3DCOLOR *pout, int nWidth, 
	bool fXReversed, int bpp, unsigned char *&pauchData,
	D3DCOLOR *pclrPalette)
{
	for (int x = 0; x < nWidth; x++)
	{
		*pout++ = TgaReadPixel (bpp, pauchData, pclrPalette);
	}
}

//-----------------------------------------------------------------------------
//
// @func Read a line of compress TGA image
//
// @parm D3DCOLOR * | pout | Output data
//
// @parm int | nWidth | Width of the data
//
// @parm bool | fXReversed | Unused
//
// @parm int | bpp | Bits per pixel
//
// @parm unsigned char * & | pauchData | Image data
//
// @parm D3DCOLOR * | pclrPalette | Color palette
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void TgaExpandCompressedLine (D3DCOLOR *pout, int nWidth, bool fXReversed, 
	int bpp, unsigned char *&pauchData, D3DCOLOR *pclrPalette)
{
	for (int x = 0; x < nWidth;)
	{

		int nCount = *pauchData++;
		if (nCount >= 128)
		{
			nCount -= 127; // Calculate real repeat count.
			D3DCOLOR clr = TgaReadPixel (bpp, pauchData, pclrPalette);
			for (int i = 0; i < nCount; i++)
				*pout++ = clr;
		}
		else
		{
			nCount++;
			for (int i = 0; i < nCount; i++)
			{
				*pout++ = TgaReadPixel (bpp, pauchData, pclrPalette);
			}
		}
		x += nWidth;
	}
}

//-----------------------------------------------------------------------------
//
// @func Load a TGA image
//
// @parm unsigned char * | pauchData | Image data
//
// @parm int * | pnWidth | Width of the image
//
// @parm int * | pnHeight | Height of the image
//
// @rdesc Address of the ARGB data.
//
//-----------------------------------------------------------------------------

D3DCOLOR *TgaLoad (unsigned char *pauchData, int *pnWidth, int *pnHeight)
{
	D3DCOLOR clrPalette [256];

	//
	// Get the image information
	//

	TGAHEADER *ptga = (TGAHEADER *) pauchData;
	int nWidth = ptga ->ImageWidth;
	int nHeight = ptga ->ImageHeight;
	int bpp = ptga ->PixelDepth;
	bool fXReversed = ((ptga ->ImagDesc & 16) == 16);
	bool fYReversed = ((ptga ->ImagDesc & 32) == 32);

	//
	// Allocate the data
	//

	D3DCOLOR *pDstBits = new D3DCOLOR [nWidth * nHeight];
	if (pDstBits == NULL)
		return NULL;

	//
	// Skip the id information
	//

	pauchData += sizeof (TGAHEADER) + ptga ->IdLength;

	//
	// If we have a palette
	//

	if (ptga ->CmapType != 0)
	{
		for (int i = 0; i < ptga ->CmapLength; i++)
		{
			clrPalette [i] = TgaReadPixel (ptga ->CmapEntrySize, 
				pauchData, NULL);
		}
	}

	//
	// Get the compression flag
	//

	bool fCompressed;
	switch (ptga ->ImageType)
	{
		case TGA_Map:
		case TGA_RGB:
		case TGA_Mono:
			fCompressed = false;
			break;
		case TGA_RLEMap:
		case TGA_RLERGB:
		case TGA_RLEMono:
			fCompressed = true;
			break;
		default:
			delete [] pDstBits;
			return NULL;
	}

	//
	// Loop through the lines
	//

	D3DCOLOR *poutStart = (D3DCOLOR *) pDstBits;
	for (int y = 0; y < nHeight; y++)
	{
		D3DCOLOR *pout;
		if (fYReversed)
			pout = &poutStart [(nHeight - y - 1) * nWidth];
		else
			pout = &poutStart [y * nWidth];

		if (fCompressed)
		{
			TgaExpandCompressedLine (pout, nWidth, fXReversed, 
				bpp, pauchData, clrPalette);
		}
		else
		{
			TgaExpandUncompressedLine (pout, nWidth, fXReversed,
				bpp, pauchData, clrPalette);
		}
	}

	//
	// Save the width and height
	//

	if (pnWidth)
		*pnWidth = ptga ->ImageWidth;
	if (pnHeight)
		*pnHeight = ptga ->ImageHeight;
	return pDstBits;
}

//-----------------------------------------------------------------------------
//
// @func Load a TGA image
//
// @parm UINT | nID | Resource ID
//
// @parm int * | pnWidth | Width of the image
//
// @parm int * | pnHeight | Height of the image
//
// @rdesc Address of the ARGB data.
//
//-----------------------------------------------------------------------------

D3DCOLOR *TgaLoad (UINT nID, int *pnWidth, int *pnHeight)
{
	//
	// Find the template
	//

	HRSRC hRsrc = FindResource (_Module .GetResourceInstance (),
		MAKEINTRESOURCE (nID), _T ("TGA_INCLUDE"));
	if (hRsrc == NULL)
		return NULL;

	//
	// Write the resource
	//

	DWORD dwSize = ::SizeofResource (_Module .GetResourceInstance (), hRsrc);
	HGLOBAL hSource = ::LoadResource (_Module .GetResourceInstance (), hRsrc);
	LPVOID pSource = ::LockResource (hSource);
	D3DCOLOR *pTga = TgaLoad ((unsigned char *) pSource, pnWidth, pnHeight);
	UnlockResource (hSource);
	::FreeResource (hSource);
	return pTga;
}

//-----------------------------------------------------------------------------
//
// @func Load a TGA image
//
// @parm unsigned char * | pauchData | Image data
//
// @parm int 
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

HBITMAP TgaLoad (unsigned char *pauchData)
{

	//
	// Load the image
	//

	int nWidth, nHeight;
	D3DCOLOR *pData = TgaLoad (pauchData, &nWidth, &nHeight);
	if (pData == NULL)
		return NULL;

	//
	// Initialize a DIB
	//

	BITMAPINFOHEADER BMI;
	BMI .biSize = sizeof (BITMAPINFOHEADER);
	BMI .biWidth = nWidth;
	BMI .biHeight = nHeight;
	BMI .biPlanes = 1;
	BMI .biBitCount = 32;
	BMI .biCompression = BI_RGB;
	BMI .biSizeImage = 0;
	BMI .biXPelsPerMeter = 0;
	BMI .biYPelsPerMeter = 0;
	BMI .biClrUsed = 0;
	BMI .biClrImportant = 0;
	
	//
	// Create DIB section in shared memory
	//

	COLORREF *pDstBits;
	CClientDC dc (::GetDesktopWindow ());
	HBITMAP hbm = ::CreateDIBSection (dc, (BITMAPINFO *)&BMI,
		DIB_RGB_COLORS, (void **) &pDstBits, 0, 0l);
	if (hbm == NULL)
		return NULL;

	//
	// Convert 
	//

	for (int i = 0; i < nWidth * nHeight; i++)
	{
		pDstBits [i] = RGB (
			RGBA_GETRED (pData [i]),
			RGBA_GETGREEN (pData [i]),
			RGBA_GETBLUE (pData [i]));
	}
	delete [] pData;
	return hbm;
}