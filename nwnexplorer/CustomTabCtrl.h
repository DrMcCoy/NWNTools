#ifndef __CUSTOMTABCTRL_H__
#define __CUSTOMTABCTRL_H__

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CCustomTabCtrl - A base class to help implement
//   Tab Controls with different appearances
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Several improvements by Daniel Bowen (dbowen@es.com).
// Copyright (c) 2001-2002 Bjarke Viksoe.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//
// History (Date/Author/Description):
// ----------------------------------
//
// 2002/06/20: Daniel Bowen
// - SetCurSel -
//   * Added a new optional parameter "bNotify" that allows
//     you to specify whether or not you want the parent
//     to receive the notifications CTCN_SELCHANGING and
//     CTCN_SELCHANGE.
//   * Even if the newly requested selection index is
//     the same index as the previous selection index,
//     go through the whole SetCurSel process.
//     Even though the index is the same, the item
//     might be different (as in the case of
//     InsertItem inserting a new item where the old
//     selection used to be).  EnsureVisible is also called,
//     which in the CTCS_SCROLL case, you really want
//     every time even if the item is the same.
//
//     This is a change from previous versions,
//     where if the index was the same as the current selection,
//     the method returned immediately.
//   
// - DeleteItem -
//   * When bUpdateSelection is true:
//     Now, when you delete the selected item, instead of
//     selecting the 0-index item, it tries to leave the index
//     of the selected item the same.  If the selected item
//     was the last item, the new last item is selected.
//     If the selected item was the only remaining item,
//     the selection is cleared.
//     
//
// 2002/06/13: Daniel Bowen
// - Fix small bug with scroll-repeat when scrolling right.
// 
// 2002/06/12: Daniel Bowen
// - Publish codeproject article.  For history prior
//   to the release of the article, please see the article
//   and the section "Note to previous users"

#ifndef __cplusplus
  #error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
  #error CustomTabCtrl.h requires atlapp.h to be included first
#endif

#ifndef __ATLCTRLS_H__
  #error CustomTabCtrl.h requires atlctrls.h to be included first
#endif

//#ifndef __ATLGDIX_H__
//  #error CustomTabCtrl.h requires atlgdix.h to be included first
//#endif

// There are slightly different dependencies under VC 7 (ATL 7) vs. VC 6 (ATL 3)
#if (_ATL_VER >= 0x0700)
	#if !defined(__ATLCOLL_H__)
		#error CustomTabCtrl.h requires atlcoll.h (under VC 7).
	#endif
	#if !defined(__ATLSTR_H__)
		#error CustomTabCtrl.h requires CString. In VC 7, include atlstr.h
	#endif
	#if defined(__ATLMISC_H__) && !defined(_WTL_NO_CSTRING)
		#error In VC 7, please define _WTL_NO_CSTRING if you include atlmisc.h
	#endif
#else
	#if !defined(_WTL_USE_CSTRING)
		#error CustomTabCtrl.h requires CString. In VC 6, be sure to include atlmisc.h.
	#endif
#endif

#if (_WIN32_IE < 0x0400)
  #error CustomTabCtrl.h requires _WIN32_IE >= 0x0400
#endif

/////////////////////////////////////////////////////////////////////////////
// CMemDC

class CMemDC : public CDC
{
public:
   CDCHandle     m_dc;          // Owner DC
   CBitmap       m_bitmap;      // Offscreen bitmap
   CBitmapHandle m_hOldBitmap;  // Originally selected bitmap
   RECT          m_rc;          // Rectangle of drawing area

   CMemDC(HDC hDC, LPRECT pRect)
   {
      ATLASSERT(hDC!=NULL);
      m_dc = hDC;
      if( pRect!=NULL ) m_rc = *pRect; else m_dc.GetClipBox(&m_rc);

      CreateCompatibleDC(m_dc);
      ::LPtoDP(m_dc, (LPPOINT) &m_rc, sizeof(RECT)/sizeof(POINT));
      m_bitmap.CreateCompatibleBitmap(m_dc, m_rc.right-m_rc.left, m_rc.bottom-m_rc.top);
      m_hOldBitmap = SelectBitmap(m_bitmap);
      ::DPtoLP(m_dc, (LPPOINT) &m_rc, sizeof(RECT)/sizeof(POINT));
      SetWindowOrg(m_rc.left, m_rc.top);
   }
   ~CMemDC()
   {
      // Copy the offscreen bitmap onto the screen.
      m_dc.BitBlt(m_rc.left, m_rc.top, m_rc.right-m_rc.left, m_rc.bottom-m_rc.top,
                  m_hDC, m_rc.left, m_rc.top, SRCCOPY);
      //Swap back the original bitmap.
      SelectBitmap(m_hOldBitmap);
   }
};


// Window styles:
// NOTE: "CTCS" stands for "Custom tab control style"
#define CTCS_SCROLL              0x0001   // TCS_SCROLLOPPOSITE
#define CTCS_BOTTOM              0x0002   // TCS_BOTTOM
//#define CTCS_RIGHT               0x0002   // TCS_RIGHT
//#define CTCS_MULTISELECT         0x0004   // TCS_MULTISELECT
#define CTCS_CLOSEBUTTON         0x0008   // TCS_FLATBUTTONS
//#define CTCS_FORCEICONLEFT       0x0010   // TCS_FORCEICONLEFT
//#define CTCS_FORCELABELLEFT      0x0020   // TCS_FORCELABELLEFT
#define CTCS_HOTTRACK            0x0040   // TCS_HOTTRACK
//#define CTCS_VERTICAL            0x0080   // TCS_VERTICAL
//#define CTCS_TABS                0x0000   // TCS_TABS
//#define CTCS_BUTTONS             0x0100   // TCS_BUTTONS
//#define CTCS_SINGLELINE          0x0000   // TCS_SINGLELINE
//#define CTCS_MULTILINE           0x0200   // TCS_MULTILINE
//#define CTCS_RIGHTJUSTIFY        0x0000   // TCS_RIGHTJUSTIFY
//#define CTCS_FIXEDWIDTH          0x0400   // TCS_FIXEDWIDTH
//#define CTCS_RAGGEDRIGHT         0x0800   // TCS_RAGGEDRIGHT
//#define CTCS_FOCUSONBUTTONDOWN   0x1000   // TCS_FOCUSONBUTTONDOWN
#define CTCS_BOLDSELECTEDTAB     0x2000   // TCS_OWNERDRAWFIXED
#define CTCS_TOOLTIPS            0x4000   // TCS_TOOLTIPS
//#define CTCS_FOCUSNEVER          0x8000   // TCS_FOCUSNEVER

// Notifications:

#define CTCN_FIRST              (0U-550U)           // TCN_FIRST
#define CTCN_LAST               (0U-580U)           // TCN_LAST

#define CTCN_SELCHANGE          (TCN_FIRST - 1)     // TCN_SELCHANGE
#define CTCN_SELCHANGING        (TCN_FIRST - 2)     // TCN_SELCHANGING
//#define CTCN_GETOBJECT          (TCN_FIRST - 3)     // TCN_GETOBJECT
//#define CTCN_FOCUSCHANGE        (TCN_FIRST - 4)     // TCN_FOCUSCHANGE
//#define CTCN_INITIALIZE         (TCN_FIRST - 10)    // obsolete for now
#define CTCN_INSERTITEM         (TCN_FIRST - 11)
#define CTCN_DELETEITEM         (TCN_FIRST - 12)
#define CTCN_MOVEITEM           (TCN_FIRST - 13)
#define CTCN_SWAPITEMPOSITIONS  (TCN_FIRST - 14)
#define CTCN_CLOSE              (TCN_FIRST - 15)

// Hit Test codes
#define CTCHT_NOWHERE            0x0001             // TCHT_NOWHERE
#define CTCHT_ONITEMICON         0x0002             // TCHT_ONITEMICON
#define CTCHT_ONITEMLABEL        0x0004             // TCHT_ONITEMLABEL
#define CTCHT_ONITEM             (CTCHT_ONITEMICON | CTCHT_ONITEMLABEL)
#define CTCHT_ONCLOSEBTN         0x0010
#define CTCHT_ONSCROLLRIGHTBTN   0x0020
#define CTCHT_ONSCROLLLEFTBTN    0x0040

// Number of milliseconds for scroll repeat
#define CTCSR_NONE                0
#define CTCSR_SLOW                100
#define CTCSR_NORMAL              25
#define CTCSR_FAST                10

// Structures
typedef struct tagNMCTCITEM
{
	NMHDR   hdr;
	int     iItem;  // Item Index
	POINT   pt;     // Screen Coordinates
} NMCTCITEM, *LPNMCTCITEM;

typedef struct tagNMCTC2ITEMS
{
	NMHDR   hdr;
	int     iItem1;  // First Item Index
	int     iItem2;  // Second Item Index
	POINT   pt;      // Screen Coordinates
} NMCTC2ITEMS, *LPNMCTC2ITEMS;

typedef struct tagCTCHITTESTINFO
{
	POINT pt;        // Client Coordinates of point to test
	UINT flags;
} CTCHITTESTINFO, *LPCTCHITTESTINFO;

typedef struct tagNMCTCCUSTOMDRAW
{
	NMCUSTOMDRAW nmcd;
	HFONT hFontInactive;
	HFONT hFontSelected;
	HBRUSH hBrushBackground;
	COLORREF clrTextInactive;
	COLORREF clrTextSelected;
	COLORREF clrSelectedTab;
	COLORREF clrBtnFace;
	COLORREF clrBtnShadow;
	COLORREF clrBtnHighlight;
	COLORREF clrBtnText;
	COLORREF clrHighlight;
	COLORREF clrHighlightHotTrack;
} NMCTCCUSTOMDRAW, FAR * LPNMCTCCUSTOMDRAW;

typedef struct tagCTCSETTINGS
{
	signed char iPadding;
	signed char iMargin;
	signed char iSelMargin;
	signed char iIndent;
} CTCSETTINGS;

// Tab Item classes

class CCustomTabItem
{
// Member variables
protected:
	RECT m_rcItem;
	int m_nImage;
	CString m_sText;
	CString m_sToolTip;

public:
	typedef enum FieldFlags
	{
		eCustomTabItem_None = 0x0000,
		eCustomTabItem_Rect = 0x0001,
		eCustomTabItem_Image = 0x0002,
		eCustomTabItem_Text = 0x0004,
		eCustomTabItem_ToolTip = 0x0008,
		//eCustomTabItemResourceerved1 = 0x0010,
		//eCustomTabItemResourceerved2 = 0x0020,
		//eCustomTabItemResourceerved3 = 0x0040,
		eCustomTabItem_All = 0x00FF,
	};

// Constructors/Destructors
public:
	CCustomTabItem() :
		m_nImage(-1)
	{
		::SetRectEmpty(&m_rcItem);
	}

	CCustomTabItem(const CCustomTabItem& rhs)
	{
		*this = rhs;
	}

	virtual ~CCustomTabItem()
	{
	}

	const CCustomTabItem& operator=(const CCustomTabItem& rhs)
	{
		if(&rhs != this)
		{
			m_rcItem		= rhs.m_rcItem;
			m_nImage		= rhs.m_nImage;
			m_sText			= rhs.m_sText;
			m_sToolTip		= rhs.m_sToolTip;
		}
		return *this;
	}

// Accessors
public:

	RECT GetRect() const
	{
		return m_rcItem;
	}
	LPCRECT GetRectRef() const
	{
		return &m_rcItem;
	}
	bool SetRect(RECT rcItem)
	{
		m_rcItem = rcItem;
		return true;
	}

	int GetImageIndex() const
	{
		return m_nImage;
	}
	bool SetImageIndex(int nImage = -1)
	{
		m_nImage = nImage;
		return true;
	}

	CString GetText() const
	{
		return m_sText;
	}
	LPCTSTR GetTextRef() const
	{
		return (LPCTSTR)m_sText;
	}
	bool SetText(LPCTSTR sNewText)
	{
		m_sText = sNewText;
		return true;
	}

	CString GetToolTip() const
	{
		return m_sToolTip;
	}
	LPCTSTR GetToolTipRef() const
	{
		return (LPCTSTR)m_sToolTip;
	}
	bool SetToolTip(LPCTSTR sNewText)
	{
		m_sToolTip = sNewText;
		return true;
	}
	

// Methods:
public:
	bool UsingImage() const
	{
		return (m_nImage >= 0);
	}
	bool UsingText() const
	{
		return (m_sText.GetLength() > 0);
	}
	bool UsingToolTip() const
	{
		return (m_sToolTip.GetLength() > 0);
	}

	BOOL InflateRect(int dx, int dy)
	{
		return ::InflateRect(&m_rcItem, dx, dy);
	}

	bool MatchItem(CCustomTabItem* pItem, FieldFlags eFlags) const
	{
		bool bMatch = true;
		if(bMatch && (eFlags & eCustomTabItem_Rect) == eCustomTabItem_Rect)
		{
			bMatch = (TRUE == ::EqualRect(&m_rcItem, &pItem->m_rcItem));
		}
		if(bMatch && (eFlags & eCustomTabItem_Image) == eCustomTabItem_Image)
		{
			bMatch = (m_nImage == pItem->m_nImage);
		}
		if(bMatch && (eFlags & eCustomTabItem_Text) == eCustomTabItem_Text)
		{
			bMatch = (m_sText == pItem->m_sText);
		}
		if(bMatch && (eFlags & eCustomTabItem_ToolTip) == eCustomTabItem_ToolTip)
		{
			bMatch = (m_sToolTip == pItem->m_sToolTip);
		}

		if(bMatch)
		{
			*pItem = *this;
		}

		return bMatch;
	}
};

// Derived Tab Item class that supports an HWND identifying a "tab view"
class CTabViewTabItem : public CCustomTabItem
{
protected:
	typedef CCustomTabItem baseClass;

// Member variables (in addition to CCustomTabItem ones)
protected:
	HWND m_hWndTabView;

public:
	typedef enum FieldFlags
	{
		eCustomTabItem_None = 0x0000,
		eCustomTabItem_Rect = 0x0001,
		eCustomTabItem_Text = 0x0002,
		eCustomTabItem_Image = 0x0004,
		eCustomTabItem_ToolTip = 0x0008,
		eCustomTabItem_TabView = 0x0010,
		//eCustomTabItemResourceerved2 = 0x0020,
		//eCustomTabItemResourceerved3 = 0x0040,
		eCustomTabItem_All = 0x00FF,
	};

// Constructors/Destructors
public:
	CTabViewTabItem() :
		m_hWndTabView(NULL)
	{
	}

	CTabViewTabItem(const CTabViewTabItem& rhs)
	{
		*this = rhs;
	}

	virtual ~CTabViewTabItem()
	{
	}

	const CTabViewTabItem& operator=(const CTabViewTabItem& rhs)
	{
		if(&rhs != this)
		{
			m_rcItem		= rhs.m_rcItem;
			m_nImage		= rhs.m_nImage;
			m_sText			= rhs.m_sText;
			m_sToolTip		= rhs.m_sToolTip;
			m_hWndTabView	= rhs.m_hWndTabView;
		}
		return *this;
	}

// Accessors
public:

	HWND GetTabView() const
	{
		return m_hWndTabView;
	}
	bool SetTabView(HWND hWnd = NULL)
	{
		m_hWndTabView = hWnd;
		return true;
	}

// Methods:
public:
	bool UsingTabView() const
	{
		return (m_hWndTabView != NULL);
	}

	bool MatchItem(CTabViewTabItem* pItem, FieldFlags eFlags) const
	{
		bool bMatch = true;
		if(bMatch && (eFlags & eCustomTabItem_Rect) == eCustomTabItem_Rect)
		{
			bMatch = (TRUE == ::EqualRect(&m_rcItem, &pItem->m_rcItem));
		}
		if(bMatch && (eFlags & eCustomTabItem_Image) == eCustomTabItem_Image)
		{
			bMatch = (m_nImage == pItem->m_nImage);
		}
		if(bMatch && (eFlags & eCustomTabItem_Text) == eCustomTabItem_Text)
		{
			bMatch = (m_sText == pItem->m_sText);
		}
		if(bMatch && (eFlags & eCustomTabItem_ToolTip) == eCustomTabItem_ToolTip)
		{
			bMatch = (m_sToolTip == pItem->m_sToolTip);
		}
		if(bMatch && (eFlags & eCustomTabItem_TabView) == eCustomTabItem_TabView)
		{
			bMatch = (m_hWndTabView == pItem->m_hWndTabView);
		}

		if(bMatch)
		{
			*pItem = *this;
		}

		return bMatch;
	}
};

typedef CWinTraits<WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CTCS_TOOLTIPS, 0> CCustomTabCtrlWinTraits;

template <class T, class TItem = CCustomTabItem, class TBase = CWindow, class TWinTraits = CCustomTabCtrlWinTraits>
class ATL_NO_VTABLE CCustomTabCtrl : 
	public CWindowImpl< T, TBase, TWinTraits >
{
public:
	// Expose the item type (that's a template parameter to this base class)
	typedef TItem TItem;

protected:
	typedef CWindowImpl< T, TBase, TWinTraits > baseClass;

#if (_ATL_VER < 0x0700)
// With ATL 7, CAtlArray was introduced which is better than
//  CSimpleArray. Among other things, it supports inserting
//  items in any place.  If this code is compiled under ATL 7,
//  we'll use the real CAtlArray.  If this is compiled under ATL 3,
//  we'll use a "fake" CAtlArray where we implement the
//  functionality we're using that the real CAtlArray provides.
//
// Important! This isn't the real ATL 7 CAtlArray.
//  We inherit from CSimpleArray as "protected", so that you
//  can't call its versions of functions (so you have
//  to use the CAtlArray style of functions)

template<typename E>
class CAtlArray : protected CSimpleArray<E>
{
protected:
	typedef CAtlArray thisClass;
	typedef CSimpleArray<E> baseClass;

public:

	//Real CAtlArray:  size_t GetCount() const throw();
	size_t GetCount() const
	{
		return m_nSize;
	}

	//Real CAtlArray:  void InsertAt( size_t iElement, INARGTYPE element, size_t nCount = 1 );
	void InsertAt( size_t nIndex, E& element )
	{
		if(m_nSize == m_nAllocSize)
		{
			E* aT;
			int nNewAllocSize = (m_nAllocSize == 0) ? 1 : (m_nSize * 2);
			aT = (E*)realloc(m_aT, nNewAllocSize * sizeof(E));
			if(aT == NULL)
				return; // FALSE;
			m_nAllocSize = nNewAllocSize;
			m_aT = aT;
		}
		memmove((void*)&m_aT[nIndex+1], (void*)&m_aT[nIndex], (m_nSize - nIndex ) * sizeof(E));
		m_nSize++;
		SetAtIndex(nIndex, element);
		//return TRUE;
	}

	//Real CAtlArray:  void RemoveAt( size_t iElement, size_t nCount = 1 );
	void RemoveAt( size_t nIndex )
	{
		// This is an improvement over CSimpleArray::RemoveAt suggested
		//  by Jim Springfield on the ATL discussion list
		m_aT[nIndex].~E();
		if((int)nIndex != (m_nSize - 1))
		{
			memmove((void*)&m_aT[nIndex], (void*)&m_aT[nIndex + 1], (m_nSize - (nIndex + 1)) * sizeof(E));
		}
		m_nSize--;
		//return TRUE;
	}

	//Real CAtlArray:  const E& operator[]( size_t iElement ) const throw();
	const E& operator[]( size_t iElement ) const
	{
		ATLASSERT( iElement < (size_t)m_nSize );
		return( m_aT[iElement] );
	}

	//Real CAtlArray:  E& operator[]( size_t iElement ) throw();
	E& operator[]( size_t iElement )
	{
		ATLASSERT( iElement < (size_t)m_nSize );
		return( m_aT[iElement] );
	}

};

#endif // (_ATL_VER < 0x0700)

// Member variables
protected:
	int m_iCurSel;
	int m_iHotItem;
	CTCSETTINGS m_settings;
	CAtlArray< TItem* > m_Items;
	UINT m_idDlgCtrl;
	CFont m_font;
	CFont m_fontSel;
	CImageList m_imageList;
	CToolTipCtrl m_tooltip;

	RECT m_rcTabItemArea;
	RECT m_rcScrollLeft;
	RECT m_rcScrollRight;
	RECT m_rcCloseButton;

	int m_iScrollOffset;

	// Flags, internal state, etc.
	//
	//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
	//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
	//  +-------+-------+-------+-------+---+-----------+---------------+
	//  |  FUT  |  MO   |  MD   |  HT   |SR |    SD     |     FLAGS     |
	//  +-------+-------+-------+-------+---+-----------+---------------+
	//
	//  FLAGS - boolean flags
	//  SD - Scroll delta.  The number of pixels to move in a single scroll.
	//       Valid values are 0-63 (the value is bit shifted to/from position).
	//  SR - Scroll repeat speed.  Valid values are no-repeat,
	//       slow repeat, normal repeat and fast repeat
	//  HT - Current hot tracked item (if its a tab, then m_iHotItem is the hot tab item) 
	//  MD - Item under mouse when mouse button down message was sent
	//       but before mouse button up message is sent
	//  MO - Item current under mouse cursor
	//  FUT - Not used at this time, but reserved for the future.
	DWORD m_dwState;

	enum StateBits
	{
		// Flags
		// bits                   = 0x000000ff
		ectcMouseInWindow         = 0x00000001,
		ectcOverflowLeft          = 0x00000002,
		ectcOverflowRight         = 0x00000004,
		//ectcOverflowBottom        = 0x00000002,  // alias for vertical mode
		//ectcOverflowTop           = 0x00000004,  // alias for vertical mode
		//ectcFlag08                = 0x00000008,
		//ectcFlag10                = 0x00000010,
		//ectcFlag20                = 0x00000020,
		//ectcFlag40                = 0x00000040,
		//ectcFlag80                = 0x00000080,

		// Scroll
		// bits                   = 0x0000ff00
		ectcScrollDeltaMask       = 0x00003f00,    //0011 1111
		ectcScrollDeltaShift      = 8,

		// We have to publicly expose these:
		ectcScrollRepeat          = 0x0000c000,    //1100 0000
		//ectcScrollRepeat_None     = 0x00000000,
		//ectcScrollRepeat_Slow     = 0x00004000,    //0100 0000
		//ectcScrollRepeat_Normal   = 0x00008000,    //1000 0000
		//ectcScrollRepeat_Fast     = 0x0000c000,    //1100 0000

		// Hot Tracking
		// bits                   = 0x000f0000
		ectcHotTrack              = 0x000f0000,
		ectcHotTrack_CloseButton  = 0x00010000,
		ectcHotTrack_ScrollRight  = 0x00020000,
		ectcHotTrack_ScrollLeft   = 0x00030000,
		ectcHotTrack_TabItem      = 0x00040000,

		// Mouse Down
		// bits                   = 0x00f00000
		ectcMouseDown             = 0x00f00000,
		ectcMouseDown_CloseButton = 0x00100000,
		ectcMouseDown_ScrollRight = 0x00200000,
		ectcMouseDown_ScrollLeft  = 0x00300000,
		ectcMouseDown_TabItem     = 0x00400000,

		// Mouse Over
		// bits                   = 0x0f000000
		ectcMouseOver             = 0x0f000000,
		ectcMouseOver_CloseButton = 0x01000000,
		ectcMouseOver_ScrollRight = 0x02000000,
		ectcMouseOver_ScrollLeft  = 0x03000000,
		ectcMouseOver_TabItem     = 0x04000000,
	};

	enum ButtonToolTipIDs
	{
		ectcToolTip_Close         = 0xFFFFFFF0,
		ectcToolTip_ScrollRight   = 0xFFFFFFF1,
		ectcToolTip_ScrollLeft    = 0xFFFFFFF2,
	};

	enum TimerIDs
	{
		ectcTimer_ScrollLeft      = 0x00000010,
		ectcTimer_ScrollRight     = 0x00000020,
	};

// Public enumerations
public:
	enum ScrollRepeat
	{
		ectcScrollRepeat_None     = 0x00000000,
		ectcScrollRepeat_Slow     = 0x00004000,
		ectcScrollRepeat_Normal   = 0x00008000,
		ectcScrollRepeat_Fast     = 0x0000c000,
	};

// Constructors
public:
	CCustomTabCtrl() :
		m_iCurSel(-1),
		m_iHotItem(-1),
		m_idDlgCtrl(0),
		m_dwState(0),
		m_iScrollOffset(0)
	{
		::ZeroMemory(&m_settings, sizeof(CTCSETTINGS));

		::SetRectEmpty(&m_rcTabItemArea);
		::SetRectEmpty(&m_rcCloseButton);
		::SetRectEmpty(&m_rcScrollLeft);
		::SetRectEmpty(&m_rcScrollRight);

		m_dwState |=  ((40 << ectcScrollDeltaShift) & ectcScrollDeltaMask);
		m_dwState |=  ectcScrollRepeat_Normal;
	}

// Implementation
protected:

	void InitializeTooltips(void)
	{
		// Be sure InitCommonControlsEx is called before this,
		//  with one of the flags that includes the tooltip control
		m_tooltip.Create(m_hWnd, NULL, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP /* | TTS_BALLOON */, WS_EX_TOOLWINDOW);
		if(m_tooltip.IsWindow())
		{
			m_tooltip.SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0,
				 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

			m_tooltip.SetDelayTime(TTDT_INITIAL, ::GetDoubleClickTime());
			m_tooltip.SetDelayTime(TTDT_AUTOPOP, ::GetDoubleClickTime() * 20);
			m_tooltip.SetDelayTime(TTDT_RESHOW, ::GetDoubleClickTime() / 5);
		}
	}

	void ActivateTooltips(BOOL bActivate = TRUE)
	{
		ATLASSERT(m_tooltip.IsWindow());
		if(m_tooltip.IsWindow())
		{
			m_tooltip.Activate(bActivate);
		}
	}

	void ClearCurrentHotTracking(bool bRedrawEffectedArea = true)
	{
		switch(m_dwState & ectcHotTrack)
		{
		case ectcHotTrack_CloseButton:
			m_dwState &= ~ectcHotTrack;
			if(bRedrawEffectedArea)
			{
				this->InvalidateRect(&m_rcCloseButton);
			}
			break;
		case ectcHotTrack_ScrollRight:
			m_dwState &= ~ectcHotTrack;
			if(bRedrawEffectedArea)
			{
				this->InvalidateRect(&m_rcScrollRight);
			}
			break;
		case ectcHotTrack_ScrollLeft:
			m_dwState &= ~ectcHotTrack;
			if(bRedrawEffectedArea)
			{
				this->InvalidateRect(&m_rcScrollLeft);
			}
			break;
		case ectcHotTrack_TabItem:
			m_dwState &= ~ectcHotTrack;
			m_iHotItem = -1;

			if(bRedrawEffectedArea)
			{
				// In case the derived class actually changes the width
				//  of a hot tracked tab, invalidate the whole tab window
				this->Invalidate();
			}
			break;
		default:
			m_dwState &= ~ectcHotTrack;
			m_iHotItem = -1;
			if(bRedrawEffectedArea)
			{
				this->Invalidate();
			}
			break;
		}
	}

	void ClearCurrentMouseDownTracking(bool bRedrawEffectedArea = true)
	{
		switch(m_dwState & ectcMouseDown)
		{
		case ectcMouseDown_CloseButton:
			m_dwState &= ~ectcMouseDown;
			if(bRedrawEffectedArea)
			{
				this->InvalidateRect(&m_rcCloseButton);
			}
			break;
		case ectcMouseDown_ScrollRight:
			m_dwState &= ~ectcMouseDown;
			if(bRedrawEffectedArea)
			{
				this->InvalidateRect(&m_rcScrollRight);
			}
			break;
		case ectcMouseDown_ScrollLeft:
			m_dwState &= ~ectcMouseDown;
			if(bRedrawEffectedArea)
			{
				this->InvalidateRect(&m_rcScrollLeft);
			}
			break;
		case ectcMouseDown_TabItem:
			m_dwState &= ~ectcMouseDown;

			if(bRedrawEffectedArea)
			{
				// In case the derived class actually changes the width
				//  of a hot tracked tab, invalidate the whole tab window
				this->Invalidate();

				//if(m_iActionItem >= 0 && m_iActionItem < m_Items.GetCount())
				//{
				//	RECT rcItemDP;
				//	this->GetItemRect(m_iActionItem, &rcItemDP);
				//	this->InvalidateRect(rcItemDP);
				//}
			}
			break;
		default:
			m_dwState &= ~ectcMouseDown;
			if(bRedrawEffectedArea)
			{
				this->Invalidate();
			}
			break;
		}
	}

	void ClearCurrentMouseOverTracking(bool bRedrawEffectedArea = true)
	{
		switch(m_dwState & ectcMouseOver)
		{
		case ectcMouseOver_CloseButton:
			m_dwState &= ~ectcMouseOver;
			if(bRedrawEffectedArea)
			{
				this->InvalidateRect(&m_rcCloseButton);
			}
			break;
		case ectcMouseOver_ScrollRight:
			m_dwState &= ~ectcMouseOver;
			if(bRedrawEffectedArea)
			{
				this->InvalidateRect(&m_rcScrollRight);
			}
			break;
		case ectcMouseOver_ScrollLeft:
			m_dwState &= ~ectcMouseOver;
			if(bRedrawEffectedArea)
			{
				this->InvalidateRect(&m_rcScrollLeft);
			}
			break;
		case ectcMouseOver_TabItem:
			m_dwState &= ~ectcMouseOver;

			if(bRedrawEffectedArea)
			{
				// In case the derived class actually changes the width
				//  of a hot tracked tab, invalidate the whole tab window
				this->Invalidate();

				//if(m_iActionItem >= 0 && m_iActionItem < m_Items.GetCount())
				//{
				//	RECT rcItemDP;
				//	this->GetItemRect(m_iActionItem, &rcItemDP);
				//	this->InvalidateRect(rcItemDP);
				//}
			}
			break;
		default:
			m_dwState &= ~ectcMouseOver;
			if(bRedrawEffectedArea)
			{
				this->Invalidate();
			}
			break;
		}
	}

	BOOL DeleteItemInternal(size_t nItem, bool bUpdateSelection = true, bool bNotify = true)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(m_hWnd));
		size_t nOldCount = m_Items.GetCount();
		if( nItem >= nOldCount )
		{
			return FALSE;
		}

		// Send notification
		if(bNotify)
		{
			// Returning TRUE tells us not to delete the item
			NMCTCITEM nmh = {{ m_hWnd, m_idDlgCtrl, CTCN_DELETEITEM }, (int) nItem, {-1,-1}};
			if( TRUE == ::SendMessage(GetParent(), WM_NOTIFY, nmh.hdr.idFrom, (LPARAM)&nmh) )
			{
				// Cancel the attempt
				return FALSE;
			}
		}

		int nPostDeleteSelection = -1;

		// The number of items is changing, so m_iCurSel
		// might be invalid if we don't change it.
		if(nOldCount <= 1)
		{
			// There's only one item left,
			// and its being deleted.
			m_iCurSel = -1;
		}
		else if(bUpdateSelection)
		{
			if( (int)nItem < m_iCurSel )
			{
				// The item being removed is before the current selection.
				// We still want the same item to be selected, but
				// the index needs to be adjusted to account for the missing item
				m_iCurSel--;
				pT->EnsureVisible(m_iCurSel);
				// NOTE: We don't call SetCurSel because we don't want
				//  any of the notifications to go to the parent - because
				//  the selection didn't change, the index just had to be adjusted.
			}
			else if( (int)nItem == m_iCurSel )
			{
				// If the item to be deleted is currently selected,
				//  select something else.
				// NOTE: We've already handled the "we're deleting
				//  the only remaining item" case.
				if(nItem >= (nOldCount-1))
				{
					// The selected item was the last item,
					// and there will still be at least one
					// item after this deletion occurs.
					// Select the item that will be the
					// new last item.
					// We need to do this before the actual
					// deletion so that m_iCurSel
					// will still be valid during the SetCurSel
					// call and the notification handlers of
					// CTCN_SELCHANGING and CTCN_SELCHANGE
					pT->SetCurSel(m_iCurSel-1, false);
				}
				else
				{
					// The selected item was NOT the last item,
					// and there will still be at least one
					// item after this deletion occurs.
					// Force a selection of the item that will
					// have the same index as the selected item being
					// deleted, but do it after the actual deletion.
					//pT->SetCurSel(m_iCurSel);
					nPostDeleteSelection = m_iCurSel;
				}
			}
		}
		else
		{
			if(((int)nItem == m_iCurSel) && (nItem >= (nOldCount-1)))
			{
				// If bUpdateSelection is false,
				// the item being deleted is selected,
				// and the item being deleted is the last
				// item, we need to clear the current selection
				// (setting m_iCurSel to -1)
				// so that our call to UpdateLayout
				// and our paint message handling don't
				// crash and burn with an invalid selected
				// item index. Its likely that the
				// caller is going to SetCurSel right
				// after this call to DeleteItem.
				m_iCurSel = -1;
			}
		}

		// Remove tooltip and item

		// The way we implement tooltips is to have as many
		// "tools" as there are tabs.  The relationship of
		// tool ID => tab index is:
		// tool ID = tab index + 1	(to avoid 0 as an ID)
		//
		// We supply the RECT and text elsewhere.
		if(m_tooltip.IsWindow())
		{
			m_tooltip.DelTool(m_hWnd, (UINT) m_Items.GetCount());
		}

		TItem* pItem = m_Items[nItem];
		m_Items.RemoveAt(nItem);

		pT->DeleteItem(pItem);

		if(nPostDeleteSelection >= 0)
		{
			pT->SetCurSel(nPostDeleteSelection, false);
		}
		return TRUE;
	}

// Message Handling
public:
	// Your derived class should use DECLARE_WND_CLASS or DECLARE_WND_SUPERCLASS, etc.
	//DECLARE_WND_CLASS(_T("WTL_CustomTabCtrl"))

	BEGIN_MSG_MAP(CCustomTabCtrl)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDoubleClick)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtonUp)
		MESSAGE_HANDLER(WM_RBUTTONDBLCLK, OnRButtonDoubleClick)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
		MESSAGE_HANDLER(WM_GETDLGCODE, OnGetDlgCode)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_GETFONT, OnGetFont)
		MESSAGE_HANDLER(WM_SETFONT, OnSetFont)
		MESSAGE_HANDLER(WM_STYLECHANGED, OnStyleChanged)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		NOTIFY_CODE_HANDLER(TTN_GETDISPINFO, OnGetToolTipInfo)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		if( wParam != NULL )
		{
			CMemDC memdc((HDC)wParam, NULL);
			pT->DoPaint(memdc.m_hDC, memdc.m_rc);
		}
		else
		{
			CPaintDC dc(pT->m_hWnd);
			CMemDC memdc(dc.m_hDC, &dc.m_ps.rcPaint);
			pT->DoPaint(memdc.m_hDC, dc.m_ps.rcPaint);
		}
		return 0;
	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1; // handled; no need to erase background; do it in DoPaint();
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		LRESULT lRes = DefWindowProc();
		T* pT = static_cast<T*>(this);
		pT->Initialize();
		return lRes;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		DWORD dwStyle = this->GetStyle();

		if(m_tooltip.IsWindow())
		{
			if(CTCS_SCROLL == (dwStyle & CTCS_SCROLL))
			{
				m_tooltip.DelTool(m_hWnd, ectcToolTip_ScrollRight);
				m_tooltip.DelTool(m_hWnd, ectcToolTip_ScrollLeft);
			}

			if(CTCS_CLOSEBUTTON == (dwStyle & CTCS_CLOSEBUTTON))
			{
				m_tooltip.DelTool(m_hWnd, ectcToolTip_Close);
			}
		}

		DeleteAllItems();

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		pT->UpdateLayout();
		Invalidate();
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->UpdateLayout();
		Invalidate();
		return 0;
	}

	LRESULT OnGetDlgCode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		return DefWindowProc(uMsg, wParam, lParam) | DLGC_WANTARROWS;
	}

	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if(m_tooltip.IsWindow())
		{
			MSG msg = { m_hWnd, uMsg, wParam, lParam };
			m_tooltip.RelayEvent(&msg);
		}
		return 1;
	}

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		DWORD dwStyle = this->GetStyle();

		T* pT = static_cast<T*>(this);
		if(ectcMouseInWindow != (m_dwState & ectcMouseInWindow))
		{
			TRACKMOUSEEVENT tme = { 0 };
			tme.cbSize = sizeof(tme);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = m_hWnd;
			if( _TrackMouseEvent(&tme) )
			{
				m_dwState |= ectcMouseInWindow;

				pT->UpdateLayout();
				Invalidate();

				// "OnMouseEnter"
				//...
			}
		}

		if(ectcMouseInWindow == (m_dwState & ectcMouseInWindow))
		{
			// hit test
			POINT ptMouse = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

			if(::PtInRect(&m_rcCloseButton, ptMouse))
			{
				if( ectcMouseOver_CloseButton != (m_dwState & ectcMouseOver))
				{
					ClearCurrentMouseOverTracking(true);
					m_dwState |= ectcMouseOver_CloseButton;

					if(ectcMouseDown_CloseButton == (m_dwState & ectcMouseDown))
					{
						this->InvalidateRect(&m_rcCloseButton);
					}
				}
				else if( 0 == (m_dwState & ectcMouseDown) &&
					ectcHotTrack_CloseButton != (m_dwState & ectcHotTrack))
				{
					ClearCurrentHotTracking(true);
					m_dwState |= ectcHotTrack_CloseButton;
					this->InvalidateRect(&m_rcCloseButton);
				}
			}
			else
			{
				if(ectcMouseOver_CloseButton == (m_dwState & ectcMouseOver))
				{
					ClearCurrentMouseOverTracking(true);
				}
				if(ectcHotTrack_CloseButton == (m_dwState & ectcHotTrack))
				{
					ClearCurrentHotTracking(true);
				}
			}

			if(::PtInRect(&m_rcScrollRight, ptMouse))
			{
				if( ectcMouseOver_ScrollRight != (m_dwState & ectcMouseOver))
				{
					ClearCurrentMouseOverTracking(true);
					m_dwState |= ectcMouseOver_ScrollRight;

					if(ectcMouseDown_ScrollRight == (m_dwState & ectcMouseDown))
					{
						this->InvalidateRect(&m_rcScrollRight);
					}
				}
				else if(0 == (m_dwState & ectcMouseDown) &&
					ectcHotTrack_ScrollRight != (m_dwState & ectcHotTrack))
				{
					ClearCurrentHotTracking(true);
					m_dwState |= ectcHotTrack_ScrollRight;
					this->InvalidateRect(&m_rcScrollRight);
				}
			}
			else
			{
				if(ectcMouseOver_ScrollRight == (m_dwState & ectcMouseOver))
				{
					ClearCurrentMouseOverTracking(true);
				}
				if(ectcHotTrack_ScrollRight == (m_dwState & ectcHotTrack))
				{
					ClearCurrentHotTracking(true);
				}
			}

			if(::PtInRect(&m_rcScrollLeft, ptMouse))
			{
				if( ectcMouseOver_ScrollLeft != (m_dwState & ectcMouseOver))
				{
					ClearCurrentMouseOverTracking(true);
					m_dwState |= ectcMouseOver_ScrollLeft;

					if(ectcMouseDown_ScrollLeft == (m_dwState & ectcMouseDown))
					{
						this->InvalidateRect(&m_rcScrollLeft);
					}
				}
				else if(0 == (m_dwState & ectcMouseDown) &&
					ectcHotTrack_ScrollLeft != (m_dwState & ectcHotTrack))
				{
					ClearCurrentHotTracking(true);
					m_dwState |= ectcHotTrack_ScrollLeft;
					this->InvalidateRect(&m_rcScrollLeft);
				}
			}
			else
			{
				if(ectcMouseOver_ScrollLeft == (m_dwState & ectcMouseOver))
				{
					ClearCurrentMouseOverTracking(true);
				}
				if(ectcHotTrack_ScrollLeft == (m_dwState & ectcHotTrack))
				{
					ClearCurrentHotTracking(true);
				}
			}

			if(::PtInRect(&m_rcTabItemArea, ptMouse))
			{
				if( ectcMouseOver_TabItem != (m_dwState & ectcMouseOver))
				{
					ClearCurrentMouseOverTracking(true);
					m_dwState |= ectcMouseOver_TabItem;

					// Not needed for simple hot tracking:
					//if(ectcMouseDown_TabItem == (m_dwState & ectcMouseDown))
					//{
					//	this->InvalidateRect(&m_rcTabItemArea);
					//}
				}
				else if( CTCS_HOTTRACK == (dwStyle & CTCS_HOTTRACK) )
					// && ectcHotTrack_TabItem != (m_dwState & ectcHotTrack))
				{
					ClearCurrentHotTracking(true);

					// TODO: The current HitTest code isn't very efficient.
					//        See if it can be improved.
					CTCHITTESTINFO tchti = { 0 };
					tchti.pt = ptMouse;
					int nIndex = pT->HitTest(&tchti);
					if(nIndex >= 0)
					{
						m_iHotItem = nIndex;

						m_dwState |= ectcHotTrack_TabItem;
						RECT rcItem;
						this->GetItemRect(nIndex, &rcItem);
						this->InvalidateRect(&rcItem);
					}
				}
			}
			else
			{
				if(ectcMouseOver_TabItem == (m_dwState & ectcMouseOver))
				{
					ClearCurrentMouseOverTracking(true);
				}
				if( CTCS_HOTTRACK == (dwStyle & CTCS_HOTTRACK) &&
					ectcHotTrack_TabItem == (m_dwState & ectcHotTrack))
				{
					ClearCurrentHotTracking(true);
				}
			}
		}

		return 1;
	}

	LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		m_dwState &= ~ectcMouseInWindow;
		ClearCurrentHotTracking(false);
		ClearCurrentMouseOverTracking(false);

		T* pT = static_cast<T*>(this);
		pT->UpdateLayout();
		Invalidate();

		return 0;
	}

	LRESULT OnCaptureChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		ClearCurrentMouseDownTracking(false);
		return 0;
	}

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		POINT ptMouse = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		if(::PtInRect(&m_rcCloseButton, ptMouse))
		{
			m_dwState |= (ectcMouseDown_CloseButton | ectcMouseOver_CloseButton);
			this->InvalidateRect(&m_rcCloseButton);
			this->SetCapture();
		}
		else if(::PtInRect(&m_rcScrollRight, ptMouse))
		{
			m_dwState |= (ectcMouseDown_ScrollRight | ectcMouseOver_ScrollRight);
			if(ectcOverflowRight == (m_dwState & ectcOverflowRight))
			{
				int nScrollSpeed = 0;
				switch(m_dwState & ectcScrollRepeat)
				{
				case ectcScrollRepeat_Slow:
					nScrollSpeed = CTCSR_SLOW;
					break;
				case ectcScrollRepeat_Normal:
					nScrollSpeed = CTCSR_NORMAL;
					break;
				case ectcScrollRepeat_Fast:
					nScrollSpeed = CTCSR_FAST;
					break;
				case ectcScrollRepeat_None:
				default:
					nScrollSpeed = CTCSR_NONE;
					break;
				}

				pT->ScrollRight(true);
				this->SetTimer(ectcTimer_ScrollRight, nScrollSpeed);
			}
			this->SetCapture();
		}
		else if(::PtInRect(&m_rcScrollLeft, ptMouse))
		{
			m_dwState |= (ectcMouseDown_ScrollLeft | ectcMouseOver_ScrollLeft);
			if(ectcOverflowLeft == (m_dwState & ectcOverflowLeft))
			{
				int nScrollSpeed = 0;
				switch(m_dwState & ectcScrollRepeat)
				{
				case ectcScrollRepeat_Slow:
					nScrollSpeed = CTCSR_SLOW;
					break;
				case ectcScrollRepeat_Normal:
					nScrollSpeed = CTCSR_NORMAL;
					break;
				case ectcScrollRepeat_Fast:
					nScrollSpeed = CTCSR_FAST;
					break;
				case ectcScrollRepeat_None:
				default:
					nScrollSpeed = CTCSR_NONE;
					break;
				}

				pT->ScrollLeft(true);
				this->SetTimer(ectcTimer_ScrollLeft, nScrollSpeed);
			}
			this->SetCapture();
		}
		else
		{
			// Search for a tab
			CTCHITTESTINFO tchti = { 0 };
			tchti.pt = ptMouse;
			int nIndex = pT->HitTest(&tchti);

			NMCTCITEM nmh = {{ m_hWnd, m_idDlgCtrl, NM_CLICK }, 
				nIndex, {GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam)}};
			if(FALSE == ::SendMessage(GetParent(), WM_NOTIFY, nmh.hdr.idFrom, (LPARAM)&nmh))
			{
				// returning FALSE let's us do our default handling
				if( nIndex!=-1 )
				{
					pT->SetFocus();
					pT->SetCurSel(nIndex, true);
				}
			}
		}

		return 0;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if(m_hWnd == ::GetCapture())
		{
			// Before we release the capture, remember what the state was
			// (in WM_CAPTURECHANGED we ClearCurrentMouseDownTracking)
			DWORD dwState = m_dwState;

			::ReleaseCapture();

			if(ectcMouseDown_CloseButton == (dwState & ectcMouseDown) &&
				ectcMouseOver_CloseButton == (dwState & ectcMouseOver))
			{
				// Close Button
				NMCTCITEM nmh = {{ m_hWnd, m_idDlgCtrl, CTCN_CLOSE },
					this->GetCurSel(), {GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam)}};
				::SendMessage(GetParent(), WM_NOTIFY, nmh.hdr.idFrom, (LPARAM)&nmh);
			}

			T* pT = static_cast<T*>(this);
			pT->UpdateLayout();
			Invalidate();
		}
		return 0;
	}

	LRESULT OnLButtonDoubleClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		// Search for a tab
		T* pT = static_cast<T*>(this);
		CTCHITTESTINFO tchti = { 0 };
		tchti.pt.x = GET_X_LPARAM(lParam);
		tchti.pt.y = GET_Y_LPARAM(lParam);
		int nIndex = pT->HitTest(&tchti);

		// returning TRUE tells us not to do our default handling
		NMCTCITEM nmh = {{ m_hWnd, m_idDlgCtrl, NM_DBLCLK }, 
			nIndex, {GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam)}};
		if(FALSE == ::SendMessage(GetParent(), WM_NOTIFY, nmh.hdr.idFrom, (LPARAM)&nmh))
		{
			// returning FALSE let's us do our default handling
			if( nIndex!=-1 )
			{
				//pT->SetFocus();
				//pT->SetCurSel(nIndex);
			}
		}

		return 0;
	}

	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		// Search for a tab
		T* pT = static_cast<T*>(this);
		CTCHITTESTINFO tchti = { 0 };
		tchti.pt.x = GET_X_LPARAM(lParam);
		tchti.pt.y = GET_Y_LPARAM(lParam);
		int nIndex = pT->HitTest(&tchti);

		// returning TRUE tells us not to do our default handling
		NMCTCITEM nmh = {{ m_hWnd, m_idDlgCtrl, NM_RCLICK }, nIndex, 
			{GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam)}};
		if(FALSE == ::SendMessage(GetParent(), WM_NOTIFY, nmh.hdr.idFrom, (LPARAM)&nmh))
		{
			// returning FALSE let's us do our default handling
			if( nIndex!=-1 )
			{
				pT->SetFocus();
				pT->SetCurSel(nIndex, true);
			}
		}

		return 0;
	}

	LRESULT OnRButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnRButtonDoubleClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		// Search for a tab
		T* pT = static_cast<T*>(this);
		CTCHITTESTINFO tchti = { 0 };
		tchti.pt.x = GET_X_LPARAM(lParam);
		tchti.pt.y = GET_Y_LPARAM(lParam);
		int nIndex = pT->HitTest(&tchti);

		// returning TRUE tells us not to do our default handling
		NMCTCITEM nmh = {{ m_hWnd, m_idDlgCtrl, NM_RDBLCLK }, 
			nIndex, {GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam)}};
		if(FALSE == ::SendMessage(GetParent(), WM_NOTIFY, nmh.hdr.idFrom, (LPARAM)&nmh))
		{
			// returning FALSE let's us do our default handling
			if( nIndex!=-1 )
			{
				//pT->SetFocus();
				//pT->SetCurSel(nIndex);
			}
		}

		return 0;
	}

	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		switch( wParam )
		{
		case VK_LEFT:
			if( m_iCurSel>0 )
			{
				pT->SetCurSel(m_iCurSel-1, true);
			}
			return 0;
		case VK_RIGHT:
			if( m_iCurSel<(int)m_Items.GetCount()-1 )
			{
				pT->SetCurSel(m_iCurSel+1, true);
			}
			return 0;
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnGetFont(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// DDB: 2002/04/22
		// The code was doing GetFont and SetFont, but wasn't actually
		//  properly dealing with implementing it if the window
		//  was not a subclassed static control.
		return (LRESULT)(HFONT)m_font;
	}

	LRESULT OnSetFont(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		// DDB: 2002/04/22
		// The code was doing GetFont and SetFont, but wasn't actually
		//  properly dealing with implementing it if the window
		//  was not a subclassed static control.
		//
		// Also, we're managing the lifetime of our font
		//  (i.e., we're going to DeleteObject it in our destructor),
		//  so if someone calls SetFont, keep a copy of the
		//  font instead of just "pointing" to it
		LOGFONT lfCopy = {0};
		::GetObject((HFONT)wParam, sizeof(LOGFONT), &lfCopy);

		if(!m_font.IsNull()) m_font.DeleteObject();

		m_font.CreateFontIndirect(&lfCopy);

		if(LOWORD(lParam))
		{
			Invalidate();
		}

		return 0;
	}

	LRESULT OnStyleChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(wParam == GWL_STYLE)
		{
			LPSTYLESTRUCT pStyles = (LPSTYLESTRUCT)lParam;
			if(pStyles)
			{
				T* pT = static_cast<T*>(this);

				// Tooltips
				if((((pStyles->styleOld) & CTCS_TOOLTIPS) != CTCS_TOOLTIPS) &&
					(((pStyles->styleNew) & CTCS_TOOLTIPS) == CTCS_TOOLTIPS))
				{
					this->ActivateTooltips(TRUE);
				}
				else if((((pStyles->styleOld) & CTCS_TOOLTIPS) == CTCS_TOOLTIPS) &&
					(((pStyles->styleNew) & CTCS_TOOLTIPS) != CTCS_TOOLTIPS))
				{
					this->ActivateTooltips(FALSE);
				}

				// Scroll to fit
				if((((pStyles->styleOld) & CTCS_SCROLL) != CTCS_SCROLL) &&
					(((pStyles->styleNew) & CTCS_SCROLL) == CTCS_SCROLL))
				{
					m_tooltip.AddTool(m_hWnd, _T("Scroll Right"), &rcDefault, ectcToolTip_ScrollRight);
					m_tooltip.AddTool(m_hWnd, _T("Scroll Left"), &rcDefault, ectcToolTip_ScrollLeft);

					//pT->UpdateLayout();
					//Invalidate();
				}
				else if((((pStyles->styleOld) & CTCS_SCROLL) == CTCS_SCROLL) &&
					(((pStyles->styleNew) & CTCS_SCROLL) != CTCS_SCROLL))
				{
					m_tooltip.DelTool(m_hWnd, ectcToolTip_ScrollRight);
					m_tooltip.DelTool(m_hWnd, ectcToolTip_ScrollLeft);

					m_iScrollOffset = 0;
					//pT->UpdateLayout();
					//Invalidate();
				}

				// Close Button
				if((((pStyles->styleOld) & CTCS_CLOSEBUTTON) != CTCS_CLOSEBUTTON) &&
					(((pStyles->styleNew) & CTCS_CLOSEBUTTON) == CTCS_CLOSEBUTTON))
				{
					m_tooltip.AddTool(m_hWnd, _T("Close"), &rcDefault, ectcToolTip_Close);

					//pT->UpdateLayout();
					//Invalidate();
				}
				else if((((pStyles->styleOld) & CTCS_CLOSEBUTTON) == CTCS_CLOSEBUTTON) &&
					(((pStyles->styleNew) & CTCS_CLOSEBUTTON) != CTCS_CLOSEBUTTON))
				{
					m_tooltip.DelTool(m_hWnd, ectcToolTip_Close);

					//pT->UpdateLayout();
					//Invalidate();
				}

				pT->UpdateLayout();
				Invalidate();
			}
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		switch(wParam)
		{
		case ectcTimer_ScrollRight:
			if(ectcMouseDown_ScrollRight == (m_dwState & ectcMouseDown) &&
				ectcOverflowRight == (m_dwState & ectcOverflowRight))
			{
				if(ectcMouseOver_ScrollRight == (m_dwState & ectcMouseOver))
				{
					T* pT = static_cast<T*>(this);
					pT->ScrollRight(true);

					if(ectcScrollRepeat_None == (m_dwState & ectcScrollRepeat))
					{
						this->KillTimer(ectcTimer_ScrollRight);
					}
				}
			}
			else
			{
				this->KillTimer(ectcTimer_ScrollRight);
			}
			break;
		case ectcTimer_ScrollLeft:
			if(ectcMouseDown_ScrollLeft == (m_dwState & ectcMouseDown) &&
				ectcOverflowLeft == (m_dwState & ectcOverflowLeft))
			{
				if(ectcMouseOver_ScrollLeft == (m_dwState & ectcMouseOver))
				{
					T* pT = static_cast<T*>(this);
					pT->ScrollLeft(true);

					if(ectcScrollRepeat_None == (m_dwState & ectcScrollRepeat))
					{
						this->KillTimer(ectcTimer_ScrollLeft);
					}
				}
			}
			else
			{
				this->KillTimer(ectcTimer_ScrollLeft);
			}
			break;
		default:
			bHandled = FALSE;
			break;
		}
		return 0;
	}

	LRESULT OnGetToolTipInfo(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
	{
		LPNMTTDISPINFO pToolTipInfo = (LPNMTTDISPINFO)pnmh;
		if(pToolTipInfo)
		{
			// The way we implement tooltips for tab items
			// is to have as many "tools" as there are tabs.
			// The relationship of tool ID => tab index is:
			// tool ID = tab index + 1	(to avoid 0 as an ID)
			//
			// We supply the RECT elsewhere and the text here
			UINT id = pToolTipInfo->hdr.idFrom;
			if(id > 0 && id <= m_Items.GetCount())
			{
				TItem* pItem = m_Items[id-1];
				if(pItem)
				{
					if(pItem->UsingToolTip())
					{
						pToolTipInfo->lpszText = const_cast<LPTSTR>(pItem->GetToolTipRef());
					}
					else if(pItem->UsingText())
					{
						pToolTipInfo->lpszText = const_cast<LPTSTR>(pItem->GetTextRef());
					}
				}
			}
		}

		return 0;
	}

// Overridables
public:

	void Initialize(void)
	{
		ATLASSERT(::IsWindow(m_hWnd));

		m_idDlgCtrl = GetDlgCtrlID();

		this->SendMessage(WM_SETTINGCHANGE, 0, 0);

		this->InitializeTooltips();

		// NOTE: you can change the style at any time
		//  for a number of the cool tab control styles
		//  (tool tips, close button, scroll buttons, etc.)
		DWORD dwStyle = this->GetStyle();

		this->ActivateTooltips(CTCS_TOOLTIPS == (dwStyle & CTCS_TOOLTIPS));

		if(CTCS_SCROLL == (dwStyle & CTCS_SCROLL))
		{
			m_tooltip.AddTool(m_hWnd, _T("Scroll Right"), &rcDefault, ectcToolTip_ScrollRight);
			m_tooltip.AddTool(m_hWnd, _T("Scroll Left"), &rcDefault, ectcToolTip_ScrollLeft);
		}

		if(CTCS_CLOSEBUTTON == (dwStyle & CTCS_CLOSEBUTTON))
		{
			m_tooltip.AddTool(m_hWnd, _T("Close"), &rcDefault, ectcToolTip_Close);
		}
	}

	TItem* CreateNewItem(void* pInitData = NULL)
	{
		pInitData; // avoid level 4 warning

#if defined (_CPPUNWIND) & (defined(_ATL_EXCEPTIONS) | defined(_AFX))
		TItem* pNewItem = NULL;
		try { pNewItem = new TItem; }
		catch (...) { ATLTRACE(_T("!! Exception thrown in CCustomTabCtrl::CreateNewItem\r\n")); }
#else
		TItem* pNewItem = new TItem;
#endif
		return pNewItem;
	}

	void DeleteItem(TItem* pOldItem)
	{
#if defined (_CPPUNWIND) & (defined(_ATL_EXCEPTIONS) | defined(_AFX))
		try { delete pOldItem; }
		catch (...) { ATLTRACE(_T("!! Exception thrown in CCustomTabCtrl::DeleteItem\r\n")); }
#else
		delete pOldItem;
#endif
	}

	void UpdateLayout(void)
	{
		if(!m_hWnd || !::IsWindow(m_hWnd))
		{
			return;
		}

		this->GetClientRect(&m_rcTabItemArea);

		T* pT = static_cast<T*>(this);

		DWORD dwStyle = this->GetStyle();

		if(CTCS_CLOSEBUTTON == (dwStyle & CTCS_CLOSEBUTTON))
		{
			pT->CalcSize_CloseButton(&m_rcTabItemArea);
		}

		if(CTCS_SCROLL == (dwStyle & CTCS_SCROLL))
		{
			pT->CalcSize_ScrollButtons(&m_rcTabItemArea);
			pT->UpdateLayout_ScrollToFit(m_rcTabItemArea);
			pT->UpdateScrollOverflowStatus();
		}
		else
		{
			pT->UpdateLayout_Default(m_rcTabItemArea);
		}

		pT->UpdateTabItemTooltipRects();
	}

	void CalcSize_CloseButton(LPRECT prcTabItemArea)
	{
	}

	void CalcSize_ScrollButtons(LPRECT prcTabItemArea)
	{
	}

	void UpdateLayout_Default(RECT rcTabItemArea)
	{
		UpdateLayout_ScrollToFit(rcTabItemArea);
	}

	void UpdateLayout_ScrollToFit(RECT rcTabItemArea)
	{
		CClientDC dc(m_hWnd);
		HFONT hOldFont = dc.SelectFont(this->GetFont());    

		int height = rcTabItemArea.bottom-rcTabItemArea.top;

		// Reposition tabs
		size_t nCount = m_Items.GetCount();
		int xpos = m_settings.iIndent;
		for( size_t i=0; i<nCount; ++i )
		{
			TItem* pItem = m_Items[i];
			RECT rc = {xpos, 0, xpos, height};
			if( pItem->UsingText() )
			{
				RECT rcText = { 0 };
				CString sText = pItem->GetText();
				dc.DrawText(sText, sText.GetLength(), &rcText, DT_SINGLELINE | DT_CALCRECT);
				rc.right += (rcText.right-rcText.left) + (m_settings.iPadding*2);
			}
			pItem->SetRect(rc);
			xpos += (rc.right-rc.left) + m_settings.iMargin;
		}
		if( m_iCurSel >=0 && (size_t)m_iCurSel < nCount )
		{
			m_Items[m_iCurSel]->InflateRect(m_settings.iSelMargin, 0);
		}

		dc.SelectFont(hOldFont);
	}

	void UpdateTabItemTooltipRects(void)
	{
		// The way we implement tooltips for tab items
		// is to have as many "tools" as there are tabs.
		// The relationship of tool ID => tab index is:
		// tool ID = tab index + 1	(to avoid 0 as an ID)
		//
		// We supply the RECT here and text elsewhere.
		if(m_tooltip.IsWindow())
		{
			size_t i = 0;
			size_t nCount = m_Items.GetCount();
			RECT rcIntersect;
			for(i=0; i<nCount; ++i )
			{
				RECT rcItemDP;
				this->GetItemRect(i, &rcItemDP);
				::IntersectRect(&rcIntersect, &rcItemDP, &m_rcTabItemArea);

				// NOTE: Even if IntersectRect determines the rectangles
				// don't intersect at all, we still need
				// to update the tool rect, or we'll get the wrong
				// tooltip in some cases.
				m_tooltip.SetToolRect(m_hWnd, (UINT) (i+1), &rcIntersect);
			}
		}
	}

	void ScrollLeft(bool bForceRedraw = true)
	{
		m_iScrollOffset += ((m_dwState & ectcScrollDeltaMask) >> ectcScrollDeltaShift);

		T* pT = static_cast<T*>(this);
		pT->UpdateLayout();
		pT->UpdateScrollOverflowStatus();
		pT->UpdateTabItemTooltipRects();
		if(bForceRedraw)
		{
			Invalidate();
			// If something a little more forceful is needed:
			//::RedrawWindow(m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}
	}

	void ScrollRight(bool bForceRedraw = true)
	{
		m_iScrollOffset -= ((m_dwState & ectcScrollDeltaMask) >> ectcScrollDeltaShift);

		T* pT = static_cast<T*>(this);
		pT->UpdateLayout();
		pT->UpdateScrollOverflowStatus();
		pT->UpdateTabItemTooltipRects();
		if(bForceRedraw)
		{
			Invalidate();
			// If something a little more forceful is needed:
			//::RedrawWindow(m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}
	}

	void UpdateScrollOverflowStatus(void)
	{
		// Check for overflow left
		if(m_iScrollOffset >= 0)
		{
			m_iScrollOffset = 0;
			m_dwState &= ~ectcOverflowLeft;
		}
		else
		{
			m_dwState |= ectcOverflowLeft;
		}

		// Check for overflow right
		m_dwState &= ~ectcOverflowRight;

		size_t nCount = m_Items.GetCount();
		if(nCount > 0)
		{
			// Check last item
			RECT rcItemDP;
			this->GetItemRect(nCount-1, &rcItemDP);
			if(rcItemDP.right > m_rcTabItemArea.right)
			{
				m_dwState |= ectcOverflowRight;
			}
		}
	}

	void DoPaint(CDCHandle dc, RECT &rcClip)
	{
		// Save current DC selections
		int nSave = dc.SaveDC();
		ATLASSERT(nSave!=0);

		DWORD dwStyle = this->GetStyle();

		// Make sure we don't paint outside client area (possible with paint dc)
		RECT rcClient;
		GetClientRect(&rcClient);
		dc.IntersectClipRect(&rcClient);


		// Prepare DC
		dc.SelectFont(this->GetFont());

		T* pT = static_cast<T*>(this);
		LRESULT lResCustom;
		NMCTCCUSTOMDRAW nmc = { 0 };
		LPNMCUSTOMDRAW pnmcd= &(nmc.nmcd);
		pnmcd->hdr.hwndFrom = m_hWnd;
		pnmcd->hdr.idFrom = m_idDlgCtrl;
		pnmcd->hdr.code = NM_CUSTOMDRAW;
		pnmcd->hdc = dc;
		pnmcd->uItemState = 0;

		pT->InitializeDrawStruct(&nmc);

		pnmcd->dwDrawStage = CDDS_PREPAINT;
		lResCustom = ::SendMessage(GetParent(), WM_NOTIFY, pnmcd->hdr.idFrom, (LPARAM)&nmc);

		if( CDRF_SKIPDEFAULT != (lResCustom & CDRF_SKIPDEFAULT) )
		{
			pT->DoPrePaint(rcClient, &nmc);
		}

		if( lResCustom==CDRF_DODEFAULT || CDRF_NOTIFYITEMDRAW==(lResCustom & CDRF_NOTIFYITEMDRAW) )
		{
			POINT ptPreviousViewport = {0};
			HRGN hRgnClip = NULL;

			bool bScrollStyle = (CTCS_SCROLL == (dwStyle & CTCS_SCROLL));
			bool bHotTrackStyle = (CTCS_HOTTRACK == (dwStyle & CTCS_HOTTRACK));

			if(bScrollStyle)
			{
				// Remember clip region before we modify it
				//  to only include the tab item area
				// NOTE: GetClipRgn expects an already created
				//  region, so we create one with an empty rectangle.
				hRgnClip = ::CreateRectRgn(0,0,0,0);
				::GetClipRgn(dc, hRgnClip);

				dc.IntersectClipRect(&m_rcTabItemArea);

				dc.SetViewportOrg(m_iScrollOffset, 0, &ptPreviousViewport);
			}

			size_t nCount = m_Items.GetCount();
			// Draw the list items, except the selected one. It is drawn last
			// so it can cover the tabs below it.
			RECT rcIntersect;
			for( size_t i=0; i<nCount; ++i )
			{
				if( (int)i!=m_iCurSel )
				{
					RECT rcItemLP, rcItemDP;
					rcItemLP = m_Items[i]->GetRect();

					::CopyRect(&rcItemDP, &rcItemLP);
					::OffsetRect(&rcItemDP, m_iScrollOffset, 0);

					if( ::IntersectRect(&rcIntersect, &rcItemDP, &rcClip) )
					{
						pnmcd->dwItemSpec = i;
						pnmcd->uItemState = 0;
						if(bHotTrackStyle && ((DWORD)m_iHotItem == i))
						{
							pnmcd->uItemState |= CDIS_HOT;
						}
						pnmcd->rc = rcItemLP;
						pT->ProcessItem(lResCustom, &nmc);
					}
				}
			}
			if( m_iCurSel >=0 && (size_t)m_iCurSel < nCount )
			{
				RECT rcItemLP, rcItemDP;
				rcItemLP = m_Items[m_iCurSel]->GetRect();

				::CopyRect(&rcItemDP, &rcItemLP);
				::OffsetRect(&rcItemDP, m_iScrollOffset, 0);

				if( ::IntersectRect(&rcIntersect, &rcItemDP, &rcClip) )
				{
					pnmcd->dwItemSpec = m_iCurSel;
					pnmcd->uItemState = CDIS_SELECTED;
					if(bHotTrackStyle && (m_iHotItem == m_iCurSel))
					{
						pnmcd->uItemState |= CDIS_HOT;
					}
					pnmcd->rc = rcItemLP;
					pT->ProcessItem(lResCustom, &nmc);
				}
			}
			pnmcd->uItemState = 0;
			pnmcd->dwItemSpec = 0;

			if(bScrollStyle)
			{
				dc.SetViewportOrg(ptPreviousViewport);

				dc.SelectClipRgn(hRgnClip);
				::DeleteObject(hRgnClip);
			}
		}

		if( CDRF_SKIPDEFAULT != (lResCustom & CDRF_SKIPDEFAULT) )
		{
			pT->DoPostPaint(rcClient, &nmc);
		}
		
		if( CDRF_NOTIFYPOSTPAINT == (lResCustom & CDRF_NOTIFYPOSTPAINT) )
		{
			pnmcd->dwDrawStage = CDDS_POSTPAINT;
			pnmcd->dwItemSpec = 0;
			pnmcd->uItemState = 0;
			pnmcd->rc = rcClient;
			::SendMessage(GetParent(), WM_NOTIFY, pnmcd->hdr.idFrom, (LPARAM)&nmc);
		}

		dc.RestoreDC(nSave);
	}

	void ProcessItem(LRESULT lResCustom, LPNMCTCCUSTOMDRAW lpNMCustomDraw)
	{
		LRESULT lResItem = CDRF_DODEFAULT;
		if( CDRF_NOTIFYITEMDRAW == (lResCustom & CDRF_NOTIFYITEMDRAW) )
		{
			lpNMCustomDraw->nmcd.dwDrawStage = CDDS_ITEMPREPAINT;
			lResItem = ::SendMessage(GetParent(), WM_NOTIFY, lpNMCustomDraw->nmcd.hdr.idFrom, (LPARAM)lpNMCustomDraw);
		}
		if( CDRF_SKIPDEFAULT != (lResCustom & CDRF_SKIPDEFAULT) )
		{
			// Do default item-drawing
			T* pT = static_cast<T*>(this);
			pT->DoItemPaint(lpNMCustomDraw);
		}
		if( CDRF_NOTIFYITEMDRAW == (lResCustom & CDRF_NOTIFYITEMDRAW) &&
			CDRF_NOTIFYPOSTPAINT == (lResItem & CDRF_NOTIFYPOSTPAINT))
		{
			lpNMCustomDraw->nmcd.dwDrawStage = CDDS_ITEMPOSTPAINT;
			::SendMessage(GetParent(), WM_NOTIFY, lpNMCustomDraw->nmcd.hdr.idFrom, (LPARAM)lpNMCustomDraw);
		}
	}

	void InitializeDrawStruct(LPNMCTCCUSTOMDRAW lpNMCustomDraw)
	{
		DWORD dwStyle = this->GetStyle();

		lpNMCustomDraw->hFontInactive = m_font;
		lpNMCustomDraw->hFontSelected = m_fontSel;
		lpNMCustomDraw->hBrushBackground = ::GetSysColorBrush(COLOR_BTNFACE);
		lpNMCustomDraw->clrTextSelected = ::GetSysColor(COLOR_BTNTEXT);
		lpNMCustomDraw->clrTextInactive = ::GetSysColor(COLOR_BTNTEXT);
		lpNMCustomDraw->clrSelectedTab = ::GetSysColor(COLOR_BTNFACE);
		lpNMCustomDraw->clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
		lpNMCustomDraw->clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);
		lpNMCustomDraw->clrBtnHighlight = ::GetSysColor(COLOR_BTNHIGHLIGHT);
		lpNMCustomDraw->clrBtnText = ::GetSysColor(COLOR_BTNTEXT);
		lpNMCustomDraw->clrHighlight = ::GetSysColor(COLOR_HIGHLIGHT);
#if WINVER >= 0x0500 || _WIN32_WINNT >= 0x0500
		lpNMCustomDraw->clrHighlightHotTrack = ::GetSysColor(COLOR_HOTLIGHT);
#else
		lpNMCustomDraw->clrHighlightHotTrack = ::GetSysColor(COLOR_HIGHLIGHT);
#endif
	}

	void DoPrePaint(RECT rcClient, LPNMCTCCUSTOMDRAW lpNMCustomDraw)
	{
		// "Erase Background"
		// NOTE: Your derived class might be able to do a
		//  better job of erasing only the necessary area
		//  (using the clip box, etc.)
		CDCHandle dc(lpNMCustomDraw->nmcd.hdc);

		HBRUSH hOldBrush = dc.SelectBrush(lpNMCustomDraw->hBrushBackground);
		dc.PatBlt(rcClient.left, rcClient.top, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top, PATCOPY);
		dc.SelectBrush(hOldBrush);

		dc.SetTextColor(lpNMCustomDraw->clrTextInactive);
		dc.SetBkMode(TRANSPARENT);
	}

	void DoItemPaint(LPNMCTCCUSTOMDRAW /*lpNMCustomDraw*/)
	{
	}

	void DoPostPaint(RECT /*rcClient*/, LPNMCTCCUSTOMDRAW /*lpNMCustomDraw*/)
	{
		// In your derived verion, paint the scroll buttons if CTCS_SCROLL
		// is set and the close button if CTCS_CLOSEBUTTON is set
		// (if you want to support these)
	}

// Operations
public:

	BOOL SubclassWindow(HWND hWnd)
	{
		ATLASSERT(m_hWnd == NULL);
		ATLASSERT(::IsWindow(hWnd));
		BOOL bRet = baseClass::SubclassWindow(hWnd);
		if( bRet )
		{
			T* pT = static_cast<T*>(this);
			pT->Initialize();
		}
		return bRet;
	}

	CImageList SetImageList(HIMAGELIST hImageList)
	{
		CImageList& imageListOld = m_imageList;
		m_imageList = hImageList;
		return imageListOld;
	}
	CImageList& GetImageList() const
	{
		return m_imageList;
	}

	CToolTipCtrl GetTooltips() const
	{
		return m_tooltip;
	}
	//void SetTooltips(HWND hWndToolTip)
	//{
	//	ATLASSERT(::IsWindow(m_hWnd));
	//	::SendMessage(m_hWnd, TCM_SETTOOLTIPS, (WPARAM)hWndToolTip, 0L);
	//}

	bool SetScrollDelta(UINT nDelta)
	{
		if(nDelta > (ectcScrollDeltaMask >> ectcScrollDeltaShift))
		{
			return false;
		}
		m_dwState |=  ((nDelta << ectcScrollDeltaShift) & ectcScrollDeltaMask);

		return true;
	}

	UINT GetScrollDelta(void)
	{
		return ((m_dwState & ectcScrollDeltaMask) >> ectcScrollDeltaShift);
	}

	bool SetScrollRepeat(ScrollRepeat ectcNewScrollRepeat = ectcScrollRepeat_Normal)
	{
		m_dwState &= ~ectcScrollRepeat;
		m_dwState |= (ectcNewScrollRepeat & ectcScrollRepeat);

		return true;
	}

	ScrollRepeat GetScrollRepeat(void)
	{
		return (m_dwState & ectcScrollRepeat);
	}

	int InsertItem(int nItem, LPCTSTR sText = NULL, int nImage = -1, LPCTSTR sToolTip = NULL, bool bSelectItem = false)
	{
		T* pT = static_cast<T*>(this);
		TItem* pItem = pT->CreateNewItem();
		if(pItem)
		{
			pItem->SetText(sText);
			pItem->SetImageIndex(nImage);
			pItem->SetToolTip(sToolTip);

			return InsertItem(nItem, pItem, bSelectItem);
		}
		return -1;
	}

	int InsertItem(int nItem, TItem* pItem, bool bSelectItem = false)
	{
		T* pT = static_cast<T*>(this);

		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(pItem);
		ATLASSERT(nItem >=0 && nItem <= (int)m_Items.GetCount());
		if(!::IsWindow(m_hWnd) || pItem == NULL)
		{
			return -1;
		}

		size_t nOldCount = m_Items.GetCount();

		if( nItem < 0 || nItem > (int)nOldCount )
		{
			nItem = (int) nOldCount;
		}

		m_Items.InsertAt((size_t)nItem, pItem);

		size_t nNewCount = m_Items.GetCount();

		// Send notification
		NMCTCITEM nmh = {{ m_hWnd, m_idDlgCtrl, CTCN_INSERTITEM }, nItem, {-1,-1}};
		::SendMessage(GetParent(), WM_NOTIFY, nmh.hdr.idFrom, (LPARAM)&nmh);
		// Select if first tab
		if( nNewCount==1 )
		{
			pT->SetCurSel(0, false);
		}
		else if(bSelectItem)
		{
			pT->SetCurSel(nItem, false);
		}

		// The way we implement tooltips is to have as many
		// "tools" as there are tabs.  The relationship of
		// tool ID => tab index is:
		// tool ID = tab index + 1	(to avoid 0 as an ID)
		//
		// We supply the RECT and text elsewhere.
		if(m_tooltip.IsWindow())
		{
			m_tooltip.AddTool(m_hWnd, LPSTR_TEXTCALLBACK, &rcDefault, (UINT) nNewCount);
		}

		pT->UpdateLayout();
		Invalidate();
		return nItem;
	}

	BOOL MoveItem(size_t nFromIndex, size_t nToIndex, bool bUpdateSelection = true, bool bNotify = true)
	{
		T* pT = static_cast<T*>(this);

		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(nFromIndex < m_Items.GetCount());
		ATLASSERT(nToIndex < m_Items.GetCount());

		if(!::IsWindow(m_hWnd) || nFromIndex >= m_Items.GetCount() || nToIndex >= m_Items.GetCount())
		{
			return FALSE;
		}

		TItem* pFromItem = m_Items[nFromIndex];
		m_Items.RemoveAt(nFromIndex);
		m_Items.InsertAt(nToIndex, pFromItem);

		// The number of items is staying the same, so m_iCurSel
		// won't be invalid whether it gets updated or not
		if(bUpdateSelection)
		{
			if(nFromIndex == m_iCurSel)
			{
				pT->SetCurSel(nToIndex, false);
			}
		}

		if(bNotify)
		{
			NMCTC2ITEMS nmh = {{ m_hWnd, m_idDlgCtrl, CTCN_MOVEITEM }, nFromIndex, nToIndex, {-1,-1}};
			::SendMessage(GetParent(), WM_NOTIFY, nmh.hdr.idFrom, (LPARAM)&nmh);
		}

		return TRUE;
	}

	BOOL SwapItemPositions(size_t nFromIndex, size_t nToIndex, bool bUpdateSelection = true, bool bNotify = true)
	{
		T* pT = static_cast<T*>(this);

		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(nFromIndex < m_Items.GetCount());
		ATLASSERT(nToIndex < m_Items.GetCount());

		if(!::IsWindow(m_hWnd) || nFromIndex >= m_Items.GetCount() || nToIndex >= m_Items.GetCount())
		{
			return FALSE;
		}

		TItem* pFromItem = m_Items[nFromIndex];
		TItem* pToItem = m_Items[nToIndex];
		m_Items[nFromIndex] = pToItem;
		m_Items[nToIndex] = pFromItem;

		// The number of items is staying the same, so m_iCurSel
		// won't be invalid whether it gets updated or not
		if(bUpdateSelection)
		{
			if(nFromIndex == m_iCurSel)
			{
				pT->SetCurSel(nToIndex, false);
			}
			else if(nToIndex == m_iCurSel)
			{
				pT->SetCurSel(nFromIndex, false);
			}
		}

		if(bNotify)
		{
			NMCTC2ITEMS nmh = {{ m_hWnd, m_idDlgCtrl, CTCN_SWAPITEMPOSITIONS }, nFromIndex, nToIndex, {-1,-1}};
			::SendMessage(GetParent(), WM_NOTIFY, nmh.hdr.idFrom, (LPARAM)&nmh);
		}

		return TRUE;
	}

	BOOL DeleteItem(size_t nItem, bool bUpdateSelection = true, bool bNotify = true)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(m_hWnd));

		if (!DeleteItemInternal (nItem, bUpdateSelection, bNotify))
			return FALSE;

		// Repaint
		pT->UpdateLayout();
		Invalidate();
		return TRUE;
	}

	BOOL DeleteAllItems()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		m_iCurSel = -1;
		while( GetItemCount()>0 ) DeleteItemInternal(0U, false, true); // Slooow!!!
		InvalidateRect (NULL);
		return TRUE;
	}

	// NOTE: Now, instead of calling SetItem, call
	//   GetItem to get the pointer to the TItem, update
	//   what you want, then call UpdateLayout and Invalidate
	//   if appropriate.
	/*
	BOOL SetItem(int nItem, TItem* pItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(!::IsBadReadPtr(pItem,sizeof(TItem)));
		CHECK_ITEM(nItem);

		// Copy caller's data to the internal structure
		TItem* pItemT = m_Items[nItem];
		UINT mask = pItem->mask;
		if( mask & TCIF_TEXT )
		{
			pItemT->SetText(pItem->pszText);
		}
		// PBI: Added support for ImageList.
		if( mask & TCIF_IMAGE )
		{
			pItemT->iImage = pItem->iImage;
			pItemT->mask |= TCIF_IMAGE;
		}
		if( mask & TCIF_PARAM )
		{
			pItemT->lParam = pItem->lParam;
			pItemT->mask |= TCIF_PARAM;
		}

		// Repaint control
		T* pT = static_cast<T*>(this);
		pT->UpdateLayout();
		Invalidate();
		return TRUE;
	}
	*/

	TItem* GetItem(size_t nItem) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(nItem>=0 && nItem<(int)m_Items.GetCount());
		if( nItem < 0 || nItem >= (int)m_Items.GetCount() )
		{
			return NULL;
		}

		return m_Items[nItem];
	}

	int SetCurSel(size_t nItem, bool bNotify = false)
	{
		T* pT = static_cast<T*>(this);
		ATLASSERT(::IsWindow(m_hWnd));

		// NEW (DDB):
		// Even if the newly requested selection index is
		// the same index as the previous selection index,
		// the item might be different (as in the case of
		// InsertItem inserting a new item where the old
		// selection used to be).  We also call EnsureVisible
		// and UpdateLayout in this method, which we will want
		// called even if it is the same item
		//
		// OLD:
		// // Selecting same tab?  If so, we won't go through all the notifications
		// if( (int)nItem==m_iCurSel ) return m_iCurSel;

		if( nItem >= m_Items.GetCount() )
		{
			nItem = m_iCurSel;
		}

		int iOldSel = m_iCurSel;
		// Send notification
		NMCTC2ITEMS nmh = {{ m_hWnd, m_idDlgCtrl, CTCN_SELCHANGING }, iOldSel, (int) nItem, {-1,-1}};
		if(bNotify)
		{
			if( TRUE == ::SendMessage(GetParent(), WM_NOTIFY, nmh.hdr.idFrom, (LPARAM)&nmh) )
			{
				// Cancel the attempt
				return -1;
			}
		}

		// Change tab
		m_iCurSel = (int) nItem;

		pT->EnsureVisible(m_iCurSel);

		// Recalc new layout and redraw
		pT->UpdateLayout();
		Invalidate();

		if(bNotify)
		{
			// Send notification
			nmh.hdr.code = CTCN_SELCHANGE;
			::SendMessage(GetParent(), WM_NOTIFY, nmh.hdr.idFrom, (LPARAM)&nmh);
		}
		return iOldSel;
	}

	int GetCurSel() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return m_iCurSel;
	}

	int GetItemCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)m_Items.GetCount();
	}

	int HitTest(LPCTCHITTESTINFO pHitTestInfo) const
	{
		ATLASSERT(!::IsBadWritePtr(pHitTestInfo,sizeof(CTCHITTESTINFO)));
		pHitTestInfo->flags = CTCHT_NOWHERE;
		if(::PtInRect(&m_rcTabItemArea, pHitTestInfo->pt))
		{
			// TODO: Do a smarter search.  Currently,
			//  the tabs are always going to be left to right.
			//  Use this knowledge to do a better spacial search.

			RECT rcItemDP;
			size_t nCount = m_Items.GetCount();
			for( size_t i=0; i<nCount; ++i )
			{
				// NOTE: GetItemRect accounts for any scroll
				this->GetItemRect(i, &rcItemDP);

				if( ::PtInRect(&rcItemDP, pHitTestInfo->pt) )
				{
					// TODO: check for ONITEMLABEL, ONITEMICON
					pHitTestInfo->flags = CTCHT_ONITEM;
					return (int) i;
				}
			}
		}
		else
		{
			if(::PtInRect(&m_rcCloseButton, pHitTestInfo->pt))
			{
				pHitTestInfo->flags = CTCHT_ONCLOSEBTN;
			}
			else if(::PtInRect(&m_rcScrollRight, pHitTestInfo->pt))
			{
				pHitTestInfo->flags = CTCHT_ONSCROLLRIGHTBTN;
			}
			else if(::PtInRect(&m_rcScrollLeft, pHitTestInfo->pt))
			{
				pHitTestInfo->flags = CTCHT_ONSCROLLLEFTBTN;
			}
		}
		return -1;
	}

	bool EnsureVisible(int nItem, bool bPartialOK = false, bool bRecalcAndRedraw = false)
	{
		bool bAdjusted = false;

		// Adjust scroll offset so that item is visible
		if(0 != (m_dwState & (ectcOverflowLeft|ectcOverflowRight)))
		{
			if(nItem < 0 || nItem >= (int)m_Items.GetCount())
			{
				return false;
			}

			// TODO: Depend on some system metric for this value
			int nScrollToViewPadding = 20;

			RECT rcItemDP;
			this->GetItemRect(nItem, &rcItemDP);
			if(rcItemDP.left < m_rcTabItemArea.left)
			{
				if(!bPartialOK || (rcItemDP.right < m_rcTabItemArea.left))
				{
					m_iScrollOffset += (m_rcTabItemArea.left-rcItemDP.left) + nScrollToViewPadding;
					bAdjusted = true;
				}
			}
			else if(rcItemDP.right > m_rcTabItemArea.right)
			{
				if(!bPartialOK || (rcItemDP.left > m_rcTabItemArea.right))
				{
					m_iScrollOffset -= (rcItemDP.right-m_rcTabItemArea.right) + nScrollToViewPadding;
					bAdjusted = true;
				}
			}

			// Note: UpdateLayout should call UpdateScrollOverflowStatus which
			//  will catch m_iScrollOffset being scrolled too far either direction
		}

		if(bAdjusted && bRecalcAndRedraw)
		{
			T* pT = static_cast<T*>(this);
			if(bRecalcAndRedraw)
			{
				pT->UpdateLayout();
				Invalidate();
			}
			else
			{
				pT->UpdateScrollOverflowStatus();
				pT->UpdateTabItemTooltipRects();
			}
		}

		return true;
	}

	int GetRowCount() const
	{
		return 1;
	}

	DWORD SetItemSize(size_t nItem, int cx, int cy)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(nItem>=0 && nItem<(int)m_Items.GetCount());
		if( nItem < 0 || nItem >= (int)m_Items.GetCount() )
		{
			return 0;
		}

		// TODO: Review this method.  It seems that all the tabs
		//  after the one being set would have the wrong RECT.
		//  (unless the caller is iterating through all of the items)

		TItem* pItem = m_Items[nItem];

		RECT rcOld = pItem->GetRect();
		RECT rcNew = { rcOld.left, rcOld.top, rcOld.left + cx, rcOld.top cy };
		pItem->SetRect(rcNew);
		T* pT = static_cast<T*>(this);
		pT->UpdateLayout();
		Invalidate();
		return MAKELONG(rcOld.right-rcOld.left, rcOld.bottom-rcOld.top);
	}

	BOOL GetItemRect(size_t nItem, RECT *prcItem) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(prcItem);
		if( prcItem==NULL ) return FALSE;
		if( nItem >= m_Items.GetCount() )
		{
			::SetRectEmpty(prcItem);
			return FALSE;
		}
		*prcItem = m_Items[nItem]->GetRect();

		// Adjust for any scroll, so that the caller
		// gets the RECT in device coordinates
		// instead of logical coordinates
		::OffsetRect(prcItem, m_iScrollOffset, 0);

		return TRUE;
	}

	void SetPadding(int iPadding) 
	{ 
		m_iPadding = iPadding; 
		T* pT = static_cast<T*>(this);
		pT->UpdateLayout();
		Invalidate();
	};

	// FindItem:    Find the next tab item matching the search criteria
	//              The functions is meant to mimic how
	//              CListViewCtrl::FindItem and LVM_FINDITEM work,
	//              since there are no comparable messages or functions
	//              for a tab control
	//
	//  pFindInfo should specify a mask of things to check for,
	//   and have the corresponding fields set (used in the check).
	//   For example, set the mask to TCIF_PARAM and set .lParam to
	//   what you want to find.
	//  If nStart is -1, the search begins from the beginning.
	//   If nStart is not -1, the search begins with the item
	//   just after nStart (like with LVM_FINDITEM).
	//  If a matching item is found, its index is returned.
	//   Otherwise -1 is returned.
	int FindItem(TItem* pFindItem, TItem::FieldFlags eFlags, int nStart = -1) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		if(nStart < 0)
		{
			nStart = -1;
		}

		// Find the next item matching the criteria specified
		size_t nCount = m_Items.GetCount();
		for( size_t i=(nStart+1); i < nCount; ++i )
		{
			if(m_Items[i]->MatchItem(pFindItem, eFlags))
			{
				return i;
			}
		}

		return -1;
	}

};

#endif // __CUSTOMTABCTRL_H__

