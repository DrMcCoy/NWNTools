#ifndef __DOTNET_TABCTRL_H__
#define __DOTNET_TABCTRL_H__

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CDotNetTabCtrl - Tab control derived from CCustomTabCtrl
//    meant to look like the tabs in VS.Net (MDI tabs,
//    solution explorer tabs, etc.)
// CDotNetButtonTabCtrl - Tab control derived from CCustomTabCtrl
//    meant to look like VS.Net view of HTML with the Design/HTML buttons
//  
//
// Written by Daniel Bowen (dbowen@es.com).
// Copyright (c) 2001-2002 Daniel Bowen.
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
// 2002/06/12: Daniel Bowen
// - Publish codeproject article.  For history prior
//   to the release of the article, please see the article
//   and the section "Note to previous users"

#ifndef __CUSTOMTABCTRL_H__
  #error DotNetTabCtrl.h requires CustomTabCtrl.h to be included first
#endif

// NOTE: If you are compiling under VC7, be sure to put the following in
// your precompiled header:
//
//extern "C" const int _fltused = 0;

template<typename T, typename TItem = CCustomTabItem, class TBase = CWindow, class TWinTraits = CCustomTabCtrlWinTraits>
class CDotNetTabCtrlImpl : 
	public CCustomTabCtrl<T, TItem, TBase, TWinTraits>
{
protected:
	typedef CDotNetTabCtrlImpl<T, TItem, TBase, TWinTraits> thisClass;
	typedef CCustomTabCtrl<T, TItem, TBase, TWinTraits> customTabClass;

protected:
	CBrush m_hbrBackground;
	COLORREF m_clrBackground, m_clrInactiveTab;

	signed char m_nFontSizeTextTopOffset;

	const signed char m_nMinWidthToDisplayText;

// Constructor
public:
	CDotNetTabCtrlImpl() :
		m_clrBackground(::GetSysColor(COLOR_APPWORKSPACE)),
		m_clrInactiveTab(::GetSysColor(COLOR_GRAYTEXT)),
		m_nFontSizeTextTopOffset(0),
		m_nMinWidthToDisplayText(12)
	{
	}

// Message Handling
public:
	DECLARE_WND_CLASS(_T("WTL_DotNetTabCtrl"))

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
		CHAIN_MSG_MAP(customTabClass)
	END_MSG_MAP()

	LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DWORD dwStyle = this->GetStyle();

		// Initialize/Reinitialize font
		// Visual Studio.Net seems to use the "icon" font for the tabs
		LOGFONT lfIcon = { 0 };
		::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lfIcon), &lfIcon, 0);

		bool bResetFont = true;
		if( !m_font.IsNull() )
		{
			LOGFONT lf = {0};
			if(m_font.GetLogFont(&lf))
			{
				if(lstrcmpi(lf.lfFaceName, lfIcon.lfFaceName) == 0 &&
					lf.lfHeight == lfIcon.lfHeight)
				{
					bResetFont = false;
				}
			}
		}

		if(bResetFont)
		{
			if(!m_font.IsNull()) m_font.DeleteObject();
			if(!m_fontSel.IsNull()) m_fontSel.DeleteObject();

			HFONT font = m_font.CreateFontIndirect(&lfIcon);
			if(font==NULL)
			{
				m_font.Attach(AtlGetDefaultGuiFont());
			}

			if(CTCS_BOLDSELECTEDTAB == (dwStyle & CTCS_BOLDSELECTEDTAB))
			{
				lfIcon.lfWeight = FW_BOLD;
			}

			font = m_fontSel.CreateFontIndirect(&lfIcon);
			if(font==NULL)
			{
				m_fontSel.Attach(AtlGetDefaultGuiFont());
			}
		}

		// Background brush
		if( !m_hbrBackground.IsNull() ) m_hbrBackground.DeleteObject();
		CWindowDC dcWindow(NULL);
		int nBitsPerPixel = dcWindow.GetDeviceCaps(BITSPIXEL);
		if( nBitsPerPixel > 8 )
		{
			//COLORREF clrBtnHilite = ::GetSysColor(COLOR_BTNHILIGHT);
			//COLORREF clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
			//m_clrBackground =
			//	RGB( GetRValue(clrBtnFace) + ((GetRValue(clrBtnHilite) - GetRValue(clrBtnFace)) / 3 * 2),
			//		GetGValue(clrBtnFace) + ((GetGValue(clrBtnHilite) - GetGValue(clrBtnFace)) / 3 * 2),
			//		GetBValue(clrBtnFace) + ((GetBValue(clrBtnHilite) - GetBValue(clrBtnFace)) / 3 * 2),
			//	);
			//m_hbrBackground.CreateSolidBrush(m_clrBackground);

			COLORREF clrBtnFace = ::GetSysColor(COLOR_BTNFACE);

			// This is a brave attempt to mimic the algorithm that Visual Studio.Net
			// uses to calculate the tab's background color and inactive tab color.
			// The other colors that VS.Net uses seems to be standard ones,
			// but these two colors are calculated.
			BYTE nRed = 0, nGreen = 0, nBlue = 0, nMax = 0;

			// Early experiments seemed to reveal that the background color is dependant
			// on COLOR_BTNFACE.  The following algorithm is just an attempt
			// to match several empirical results.  I tested with 20 variations
			// on COLOR_BTNFACE and kept track of what the tab background became.
			// I then brought the numbers into Excel, and started crunching on the numbers
			// until I came up with a formula that seems to pretty well match.

			nRed = GetRValue(clrBtnFace);
			nGreen = GetGValue(clrBtnFace);
			nBlue = GetBValue(clrBtnFace);

			nMax = (nRed > nGreen) ? ((nRed > nBlue) ? nRed : nBlue) : ((nGreen > nBlue) ? nGreen : nBlue);
			const BYTE nMagicBackgroundOffset = (nMax > (0xFF - 35)) ? (BYTE)(0xFF - nMax) : (BYTE)35;
			if(nMax == 0)
			{
				nRed = (BYTE)(nRed + nMagicBackgroundOffset);
				nGreen = (BYTE)(nGreen + nMagicBackgroundOffset);
				nBlue = (BYTE)(nBlue + nMagicBackgroundOffset);
			}
			else
			{
				nRed = (BYTE)(nRed + (nMagicBackgroundOffset*(nRed/(double)nMax) + 0.5));
				nGreen = (BYTE)(nGreen + (nMagicBackgroundOffset*(nGreen/(double)nMax) + 0.5));
				nBlue = (BYTE)(nBlue + (nMagicBackgroundOffset*(nBlue/(double)nMax) + 0.5));
			}

			m_clrBackground = RGB(nRed, nGreen, nBlue);
			m_hbrBackground.CreateSolidBrush(m_clrBackground);


			// The inactive tab color seems to be calculated in a similar way to
			// the tab background, only instead of lightening BNTFACE, it darkens GRAYTEXT.
			COLORREF clrGrayText = ::GetSysColor(COLOR_GRAYTEXT);

			nRed = GetRValue(clrGrayText);
			nGreen = GetGValue(clrGrayText);
			nBlue = GetBValue(clrGrayText);

			nMax = (nRed > nGreen) ? ((nRed > nBlue) ? nRed : nBlue) : ((nGreen > nBlue) ? nGreen : nBlue);
			const BYTE nMagicInactiveOffset = 43;
			if(nMax != 0)
			{
				if(nRed < nMagicInactiveOffset)
					nRed = (BYTE)(nRed / 2);
				else
					nRed  = (BYTE)(nRed - (nMagicInactiveOffset*(nRed/(double)nMax) + 0.5));

				if(nGreen < nMagicInactiveOffset)
					nGreen = (BYTE)(nGreen / 2);
				else
					nGreen = (BYTE)(nGreen - (nMagicInactiveOffset*(nGreen/(double)nMax) + 0.5));

				if(nBlue < nMagicInactiveOffset)
					nBlue = (BYTE)(nBlue / 2);
				else
					nBlue = (BYTE)(nBlue - (nMagicInactiveOffset*(nBlue/(double)nMax) + 0.5));
			}

			m_clrInactiveTab = RGB(nRed, nGreen, nBlue);
		}
		else
		{
			m_hbrBackground =  CDCHandle::GetHalftoneBrush();
			m_clrInactiveTab = ::GetSysColor(COLOR_GRAYTEXT);
		}

		m_settings.iIndent = 5;
		m_settings.iPadding = 4;
		m_settings.iMargin = 3;
		m_settings.iSelMargin = 3;

		int nHeightLogicalUnits = -lfIcon.lfHeight;
		// In MSDN for "LOGFONT", they give the following formula for calculating
		// the log font height given a point size.
		//long lfHeight = -MulDiv(PointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);

		const int nNominalFontLogicalUnits = 11;	// 8 point Tahoma with 96 DPI
		m_nFontSizeTextTopOffset = (BYTE)((nHeightLogicalUnits - nNominalFontLogicalUnits) / 2);

		T* pT = static_cast<T*>(this);
		pT->UpdateLayout();
		Invalidate();
		return 0;
	}

// Overrides for painting from CCustomTabCtrl
public:

	void InitializeDrawStruct(LPNMCTCCUSTOMDRAW lpNMCustomDraw)
	{
		DWORD dwStyle = this->GetStyle();

		lpNMCustomDraw->hFontInactive = m_font;
		lpNMCustomDraw->hFontSelected = m_fontSel;
		lpNMCustomDraw->hBrushBackground = m_hbrBackground;
		lpNMCustomDraw->clrTextSelected = ::GetSysColor(COLOR_BTNTEXT);
		lpNMCustomDraw->clrTextInactive = m_clrInactiveTab;
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
		CDCHandle dc( lpNMCustomDraw->nmcd.hdc );

		// Set up the text color and background mode
		dc.SetTextColor(lpNMCustomDraw->clrBtnText);
		dc.SetBkMode(TRANSPARENT);

		RECT rcClip = {0};
		dc.GetClipBox(&rcClip);

		if(::EqualRect(&rcClip, &m_rcCloseButton) ||
			::EqualRect(&rcClip, &m_rcScrollRight) ||
			::EqualRect(&rcClip, &m_rcScrollLeft))
		{
			// Paint needed in only "other button" area

			HBRUSH hOldBrush = dc.SelectBrush(lpNMCustomDraw->hBrushBackground);
			dc.PatBlt(rcClip.left, rcClip.top, rcClip.right-rcClip.left, rcClip.bottom-rcClip.top, PATCOPY);
			dc.SelectBrush(hOldBrush);
		}
		else
		{
			// Paint needed in tab item area or more

			// Erase Background
			//  (do it here instead of a handler for WM_ERASEBKGND
			//   so that we can do flicker-free drawing with the help
			//   of COffscreenDrawRect that's in the base class)

			// TODO: Don't "erase" entire client area.
			//  Do a smarter erase of just what needs it

			RECT rc = rcClient;

			HBRUSH hOldBrush = dc.SelectBrush(lpNMCustomDraw->hBrushBackground);
			dc.PatBlt(rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, PATCOPY);
			dc.SelectBrush(hOldBrush);

			// Connect with the client area.
			DWORD dwStyle = this->GetStyle();

			if(CTCS_BOTTOM == (dwStyle & CTCS_BOTTOM))
			{
				rc.bottom = rc.top + 3;
				dc.FillSolidRect(&rc, lpNMCustomDraw->clrBtnFace);

				CPen penText;
				penText.CreatePen(PS_SOLID, 1, lpNMCustomDraw->clrBtnText);
				CPenHandle penOld = dc.SelectPen(penText);

				dc.MoveTo(rc.left, rc.bottom);
				dc.LineTo(rc.right, rc.bottom);

				dc.SelectPen(penOld);
			}
			else
			{
				int nOrigTop = rc.top;
				rc.top = rc.bottom - 2;
				dc.FillSolidRect(&rc, lpNMCustomDraw->clrBtnFace);

				CPen penHilight;
				penHilight.CreatePen(PS_SOLID, 1, lpNMCustomDraw->clrBtnHighlight);
				CPenHandle penOld = dc.SelectPen(penHilight);

				dc.MoveTo(rc.left, rc.top-1);
				dc.LineTo(rc.right, rc.top-1);

				rc.top = nOrigTop;

				CPen penShadow, pen3D;
				penShadow.CreatePen(PS_SOLID, 1, lpNMCustomDraw->clrBtnShadow);
				pen3D.CreatePen(PS_SOLID, 1, lpNMCustomDraw->clrBtnFace);
				dc.SelectPen(penShadow);

				dc.MoveTo(rc.left, rc.bottom);
				dc.LineTo(rc.left, rc.top);
				dc.LineTo(rc.right-1, rc.top);
						
				dc.SelectPen(penHilight);
				dc.LineTo(rc.right-1, rc.bottom);

				dc.SelectPen(pen3D);
				dc.MoveTo(rc.right-2, rc.top+1);
				dc.LineTo(rc.right-2, rc.bottom-2);

				dc.SelectPen(penOld);
			}

		}
	}

	void DoItemPaint(LPNMCTCCUSTOMDRAW lpNMCustomDraw)
	{
		CDCHandle dc( lpNMCustomDraw->nmcd.hdc );
		bool bSelected = (CDIS_SELECTED == (lpNMCustomDraw->nmcd.uItemState & CDIS_SELECTED));
		bool bHot = (CDIS_HOT == (lpNMCustomDraw->nmcd.uItemState & CDIS_HOT));
		// NOTE: lpNMCustomDraw->nmcd.rc is in logical coordinates
		RECT &rc = lpNMCustomDraw->nmcd.rc;
		int nItem = lpNMCustomDraw->nmcd.dwItemSpec;

		DWORD dwStyle = this->GetStyle();

		int nIconVerticalCenter = 0;

		RECT rcTab = rc;
		RECT rcText = rc;

		if(CTCS_BOTTOM == (dwStyle & CTCS_BOTTOM))
		{
			rcTab.top += 3;
			rcTab.bottom -= 2;

			rcText.top = rcTab.top+1 + m_nFontSizeTextTopOffset;
			rcText.bottom = rc.bottom;
			//nIconVerticalCenter = rcTab.top + (rc.bottom - rcTab.top) / 2;
			//nIconVerticalCenter = rcTab.top + rcText.Height() / 2;
			nIconVerticalCenter = (rc.bottom + rc.top) / 2 + rcTab.top / 2;
		}
		else
		{
			rcTab.top += 3;
			rcTab.bottom -= 2;

			rcText.top = rc.top+1 + m_nFontSizeTextTopOffset;
			rcText.bottom = rc.bottom;
			nIconVerticalCenter = (rc.bottom + rc.top) / 2 + rcTab.top / 2;
		}

		if( bSelected )
		{
			// Tab is selected, so paint tab folder

			rcTab.right--;
			dc.FillSolidRect(&rcTab, lpNMCustomDraw->clrSelectedTab);

			CPen penText, penHilight;
			penText.CreatePen(PS_SOLID, 1, lpNMCustomDraw->clrBtnText);
			penHilight.CreatePen(PS_SOLID, 1, lpNMCustomDraw->clrBtnHighlight);

			if(CTCS_BOTTOM == (dwStyle & CTCS_BOTTOM))
			{
				CPenHandle penOld = dc.SelectPen(penText);

				dc.MoveTo(rcTab.right, rcTab.top);
				dc.LineTo(rcTab.right, rcTab.bottom);
				dc.LineTo(rcTab.left, rcTab.bottom);
				dc.SelectPen(penHilight);
				dc.LineTo(rcTab.left, rcTab.top-1);

				dc.SelectPen(penOld);
			}
			else
			{
				CPenHandle penOld = dc.SelectPen(penHilight);

				dc.MoveTo(rcTab.left, rcTab.bottom-1);
				dc.LineTo(rcTab.left, rcTab.top);
				dc.LineTo(rcTab.right, rcTab.top);
				dc.SelectPen(penText);
				dc.LineTo(rcTab.right, rcTab.bottom);

				dc.SelectPen(penOld);
			}
		}
		else
		{
			// Tab is not selected

			// Draw division line on right, unless we're the item
			// on the left of the selected tab
			if(nItem + 1 == m_iCurSel)
			{
				// Item just left of selected tab
			}
			else
			{
				CPen pen;
				pen.CreatePen(PS_SOLID, 1, lpNMCustomDraw->clrBtnShadow);
				CPenHandle penOld = dc.SelectPen(pen);
				if(CTCS_BOTTOM == (dwStyle & CTCS_BOTTOM))
				{
					// Important!  Be sure and keep within "our" tab area horizontally
					dc.MoveTo(rcTab.right-1, rcTab.top + 3);
					dc.LineTo(rcTab.right-1, rcTab.bottom - 1);
				}
				else
				{
					// Important!  Be sure and keep within "our" tab area horizontally
					dc.MoveTo(rcTab.right-1, rcTab.top + 2);
					dc.LineTo(rcTab.right-1, rcTab.bottom - 2);
				}
				dc.SelectPen(penOld);
			}
		}

		TItem* pItem = this->GetItem(nItem);

		HFONT hOldFont = NULL;
		if( bSelected )
		{
			hOldFont = dc.SelectFont(lpNMCustomDraw->hFontSelected);
		}
		else
		{
			hOldFont = dc.SelectFont(lpNMCustomDraw->hFontInactive);
		}

		COLORREF crPrevious = 0;
		if( bHot )
		{
			crPrevious = dc.SetTextColor(lpNMCustomDraw->clrHighlightHotTrack);
		}
		else if( bSelected )
		{
			crPrevious = dc.SetTextColor(lpNMCustomDraw->clrTextSelected);
		}
		else
		{
			crPrevious = dc.SetTextColor(lpNMCustomDraw->clrTextInactive);
		}

		//--------------------------------------------
		// This is how CDotNetTabCtrlImpl interprets padding, margin, etc.:
		//
		//  M - Margin
		//  P - Padding
		//  I - Image
		//  Text - Tab Text
		//
		// With image:
		//     __________________________
		//
		//    | M | I | P | Text | P | M |
		//     --------------------------
		//
		// Without image:
		//     ______________________
		//
		//    | M | P | Text | P | M |
		//     ----------------------

		//rcText.left += (bSelected ? m_settings.iSelMargin : m_settings.iMargin);
		rcText.left += m_settings.iMargin;
		rcText.right -= m_settings.iMargin;
		if (pItem->UsingImage() && !m_imageList.IsNull())
		{
			// Draw the image.
			IMAGEINFO ii = {0};
			int nImageIndex = pItem->GetImageIndex();
			m_imageList.GetImageInfo(nImageIndex, &ii);

			if( (ii.rcImage.right - ii.rcImage.left) < (rcTab.right - rcTab.left) )
			{
				int nImageHalfHeight = (ii.rcImage.bottom - ii.rcImage.top) / 2;
				m_imageList.Draw(dc, nImageIndex, rcText.left, nIconVerticalCenter - nImageHalfHeight + m_nFontSizeTextTopOffset, ILD_NORMAL);
			}

			// Offset on the right of the image.
			rcText.left += (ii.rcImage.right - ii.rcImage.left);
		}

		if (rcText.left + m_nMinWidthToDisplayText < rcText.right)
		{
			::InflateRect(&rcText, -m_settings.iPadding, 0);

			CString sText = pItem->GetText();
			dc.DrawText(sText, sText.GetLength(), &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
		}

		dc.SetTextColor(crPrevious);
		dc.SelectFont(hOldFont);
	}

	void DoPostPaint(RECT /*rcClient*/, LPNMCTCCUSTOMDRAW lpNMCustomDraw)
	{
		DWORD dwStyle = this->GetStyle();

		if(0 == (dwStyle & (CTCS_CLOSEBUTTON | CTCS_SCROLL)))
		{
			return;
		}

		CDCHandle dc( lpNMCustomDraw->nmcd.hdc );

		CPen penButtons;
		penButtons.CreatePen(PS_SOLID, 1, lpNMCustomDraw->clrTextInactive);
		CBrush brushArrow;
		brushArrow.CreateSolidBrush(lpNMCustomDraw->clrTextInactive);

		CPenHandle penOld = dc.SelectPen(penButtons);
		CBrushHandle brushOld = dc.SelectBrush(brushArrow);

		// Close Button
		if(CTCS_CLOSEBUTTON == (dwStyle & CTCS_CLOSEBUTTON))
		{
			RECT rcX = m_rcCloseButton;

			if(ectcMouseDown_CloseButton == (m_dwState & ectcMouseDown))
			{
				if(ectcMouseOver_CloseButton == (m_dwState & ectcMouseOver))
				{
					::OffsetRect(&rcX, 1, 1);
				}
			}

			const int sp = 4;

			dc.MoveTo(rcX.left+sp+ -1, rcX.top+sp);
			dc.LineTo(rcX.right-sp -1, rcX.bottom-sp);
			dc.MoveTo(rcX.left+sp, rcX.top+sp);
			dc.LineTo(rcX.right-sp, rcX.bottom-sp);

			dc.MoveTo(rcX.left+sp -1, rcX.bottom-sp -1);
			dc.LineTo(rcX.right-sp -1, rcX.top+sp -1 );
			dc.MoveTo(rcX.left+sp, rcX.bottom-sp -1);
			dc.LineTo(rcX.right-sp, rcX.top+sp -1);

			if(ectcMouseDown_CloseButton == (m_dwState & ectcMouseDown))
			{
				if(ectcMouseOver_CloseButton == (m_dwState & ectcMouseOver))
				{
					dc.DrawEdge(&m_rcCloseButton, BDR_SUNKENOUTER, BF_RECT);
				}
			}
			else if(ectcHotTrack_CloseButton == (m_dwState & ectcHotTrack))
			{
				dc.DrawEdge(&m_rcCloseButton, BDR_RAISEDINNER, BF_RECT);
			}
		}

		// Scroll Buttons
		if(CTCS_SCROLL == (dwStyle & CTCS_SCROLL))
		{
			RECT rcArrowRight = m_rcScrollRight;
			RECT rcArrowLeft = m_rcScrollLeft;

			if(ectcMouseDown_ScrollRight == (m_dwState & ectcMouseDown))
			{
				if(ectcMouseOver_ScrollRight == (m_dwState & ectcMouseOver))
				{
					if(ectcOverflowRight == (m_dwState & ectcOverflowRight))
					{
						::OffsetRect(&rcArrowRight, 1, 1);
					}
				}
			}
			if(ectcMouseDown_ScrollLeft == (m_dwState & ectcMouseDown))
			{
				if(ectcMouseOver_ScrollLeft == (m_dwState & ectcMouseOver))
				{
					if(ectcOverflowLeft == (m_dwState & ectcOverflowLeft))
					{
						::OffsetRect(&rcArrowLeft, 1, 1);
					}
				}
			}

			const int spRight = 5;
			const int spLeft = 6;

			POINT ptsArrowRight[] = {
				{rcArrowRight.left+spRight, rcArrowRight.top+spRight -2},
				{rcArrowRight.left+spRight, rcArrowRight.bottom-spRight +1},
				{rcArrowRight.right-spRight -1, (rcArrowRight.bottom + m_rcScrollRight.top) / 2},
				{rcArrowRight.left+spRight, rcArrowRight.top+spRight -2}
			};
			if(ectcOverflowRight != (m_dwState & ectcOverflowRight))
			{
				dc.Polyline(ptsArrowRight, 4);
			}
			else
			{
				dc.Polygon(ptsArrowRight, 4);

				if(ectcMouseDown_ScrollRight == (m_dwState & ectcMouseDown))
				{
					if(ectcMouseOver_ScrollRight == (m_dwState & ectcMouseOver))
					{
						dc.DrawEdge(&m_rcScrollRight, BDR_SUNKENOUTER, BF_RECT);
					}
				}
				else if(ectcHotTrack_ScrollRight == (m_dwState & ectcHotTrack))
				{
					dc.DrawEdge(&m_rcScrollRight, BDR_RAISEDINNER, BF_RECT);
				}
			}

			POINT ptsArrowLeft[] = {
				{rcArrowLeft.right-spLeft, rcArrowLeft.top+spLeft -3},
				{rcArrowLeft.right-spLeft, rcArrowLeft.bottom-spLeft +2},
				{rcArrowLeft.left+spLeft -1, (rcArrowLeft.bottom + m_rcScrollLeft.top) / 2},
				{rcArrowLeft.right-spLeft, rcArrowLeft.top+spLeft -3}
			};
			if(ectcOverflowLeft != (m_dwState & ectcOverflowLeft))
			{
				dc.Polyline(ptsArrowLeft, 4);
			}
			else
			{
				dc.Polygon(ptsArrowLeft, 4);

				if(ectcMouseDown_ScrollLeft == (m_dwState & ectcMouseDown))
				{
					if(ectcMouseOver_ScrollLeft == (m_dwState & ectcMouseOver))
					{
						dc.DrawEdge(&m_rcScrollLeft, BDR_SUNKENOUTER, BF_RECT);
					}
				}
				else if(ectcHotTrack_ScrollLeft == (m_dwState & ectcHotTrack))
				{
					dc.DrawEdge(&m_rcScrollLeft, BDR_RAISEDINNER, BF_RECT);
				}
			}

		}

		dc.SelectBrush(brushOld);
		dc.SelectPen(penOld);
	}

// Overrides from CCustomTabCtrl
public:

	void CalcSize_CloseButton(LPRECT prcTabItemArea)
	{
		//int nButtonSizeX = ::GetSystemMetrics(SM_CXSMSIZE);
		//int nButtonSizeY = ::GetSystemMetrics(SM_CYSMSIZE);
		// NOTE: After several tests, VS.Net does NOT depend on
		//  any system metric for the button size, so neither will we.
		int nButtonSizeX = 15;
		int nButtonSizeY = 15;

		if((prcTabItemArea->right - prcTabItemArea->left) < nButtonSizeX)
		{
			::SetRectEmpty(&m_rcCloseButton);
			return;
		}

		m_rcCloseButton = *prcTabItemArea;

		DWORD dwStyle = this->GetStyle();

		if (CTCS_BOTTOM == (dwStyle & CTCS_BOTTOM))
		{
			m_rcCloseButton.top += 3;
			m_rcCloseButton.right -= 3;
		}
		else
		{
			m_rcCloseButton.top += 1;
			m_rcCloseButton.bottom -= 2;
			m_rcCloseButton.right -= 5;
		}
		m_rcCloseButton.top = (m_rcCloseButton.bottom + m_rcCloseButton.top - nButtonSizeY) / 2;
		m_rcCloseButton.bottom = m_rcCloseButton.top + nButtonSizeY;

		m_rcCloseButton.left = m_rcCloseButton.right - (nButtonSizeX);

		m_tooltip.SetToolRect(m_hWnd, ectcToolTip_Close, &m_rcCloseButton);

		// Adjust the tab area
		prcTabItemArea->right = m_rcCloseButton.left;
	}

	void CalcSize_ScrollButtons(LPRECT prcTabItemArea)
	{
		//int nButtonSizeX = ::GetSystemMetrics(SM_CXSMSIZE);
		//int nButtonSizeY = ::GetSystemMetrics(SM_CYSMSIZE);
		// NOTE: After several tests, VS.Net does NOT depend on
		//  any system metric for the button size, so neither will we.
		int nButtonSizeX = 15;
		int nButtonSizeY = 15;

		if((prcTabItemArea->right - prcTabItemArea->left) < nButtonSizeX)
		{
			::SetRectEmpty(&m_rcScrollRight);
			::SetRectEmpty(&m_rcScrollLeft);
			return;
		}

		RECT rcScroll = *prcTabItemArea;

		DWORD dwStyle = this->GetStyle();

		if (CTCS_BOTTOM == (dwStyle & CTCS_BOTTOM))
		{
			rcScroll.top += 3;
			if(0 == (dwStyle & CTCS_CLOSEBUTTON))
			{
				rcScroll.right -= 3;
			}
		}
		else
		{
			rcScroll.top += 1;
			rcScroll.bottom -= 2;
			if(0 == (dwStyle & CTCS_CLOSEBUTTON))
			{
				rcScroll.right -= 5;
			}
		}
		rcScroll.top = (rcScroll.bottom + rcScroll.top - nButtonSizeY) / 2;
		rcScroll.bottom = rcScroll.top + nButtonSizeY;

		m_rcScrollRight = rcScroll;
		m_rcScrollLeft = rcScroll;

		m_rcScrollRight.left = m_rcScrollRight.right - nButtonSizeX;

		m_rcScrollLeft.right = m_rcScrollRight.left;
		m_rcScrollLeft.left = m_rcScrollLeft.right - nButtonSizeX;

		m_tooltip.SetToolRect(m_hWnd, ectcToolTip_ScrollRight, &m_rcScrollRight);
		m_tooltip.SetToolRect(m_hWnd, ectcToolTip_ScrollLeft, &m_rcScrollLeft);

		// Adjust the tab area
		prcTabItemArea->right = m_rcScrollLeft.left;
	}


	void UpdateLayout_Default(RECT rcTabItemArea)
	{
		long nMinInactiveWidth = 0x7FFFFFFF;
		long nMaxInactiveWidth = 0;

		DWORD dwStyle = this->GetStyle();

		CClientDC dc(m_hWnd);
		//HFONT hOldFont = dc.SelectFont(lpNMCustomDraw->hFontInactive);
		HFONT hOldFont = dc.SelectFont(m_font);

		LONG nTabAreaWidth = (rcTabItemArea.right - rcTabItemArea.left);

		RECT rcItem = rcTabItemArea;
		// rcItem.top and rcItem.bottom aren't really going to change

		// Recalculate tab positions and widths
		// See OnItemPrePaint for a discussion of how CDotNetTabCtrlImpl
		//  interprets margin, padding, etc.
		size_t nCount = m_Items.GetCount();
		int xpos = m_settings.iIndent;
		HFONT hRestoreNormalFont = NULL;
		for( size_t i=0; i<nCount; ++i )
		{
			bool bSelected = ((int)i == m_iCurSel);
			if(bSelected)
			{
				//hRestoreNormalFont = dc.SelectFont(lpNMCustomDraw->hFontSelected);
				hRestoreNormalFont = dc.SelectFont(m_fontSel);
			}

			TItem* pItem = m_Items[i];
			rcItem.left = rcItem.right = xpos;
			//rcItem.right += ((bSelected ? m_settings.iSelMargin : m_settings.iMargin));
			rcItem.right += m_settings.iMargin;
			if( pItem->UsingImage() && !m_imageList.IsNull())
			{
				IMAGEINFO ii = {0};
				int nImageIndex = pItem->GetImageIndex();
				m_imageList.GetImageInfo(nImageIndex, &ii);
				rcItem.right += (ii.rcImage.right - ii.rcImage.left);
			}
			if( pItem->UsingText() )
			{
				RECT rcText = {0};
				CString sText = pItem->GetText();
				dc.DrawText(sText, sText.GetLength(), &rcText, DT_SINGLELINE | DT_CALCRECT);
				rcItem.right += (rcText.right - rcText.left) + (m_settings.iPadding * 2);
			}
			rcItem.right += m_settings.iMargin;
			pItem->SetRect(rcItem);
			xpos += (rcItem.right - rcItem.left);

			if(hRestoreNormalFont != NULL)
			{
				dc.SelectFont(hRestoreNormalFont);
				hRestoreNormalFont = NULL;
			}

			if(!bSelected)
			{
				if((rcItem.right - rcItem.left) < nMinInactiveWidth)
				{
					nMinInactiveWidth = (rcItem.right - rcItem.left);
				}
				if((rcItem.right - rcItem.left) > nMaxInactiveWidth)
				{
					nMaxInactiveWidth = (rcItem.right - rcItem.left);
				}
			}
		}
		xpos += m_settings.iIndent;

		if(xpos > nTabAreaWidth && nCount > 0 && m_iCurSel >= 0)
		{
			// Our desired widths are more than the width of the client area.
			// We need to have some or all of the tabs give up some real estate

			// We'll try to let the selected tab have its fully desired width.
			// If it can't, we'll make all the tabs the same width.

			RECT rcSelected = m_Items[m_iCurSel]->GetRect();
			LONG nSelectedWidth = (rcSelected.right - rcSelected.left);

			long cxClientInactiveTabs = nTabAreaWidth - (m_settings.iIndent * 2) - nSelectedWidth;

			double nRatioWithSelectionFullSize = (double) (cxClientInactiveTabs) / (double)(xpos - (m_settings.iIndent * 2) - nSelectedWidth);

			long nInactiveSameSizeWidth = (m_nMinWidthToDisplayText + (m_settings.iMargin*2) + (m_settings.iPadding));

			if(cxClientInactiveTabs > (nInactiveSameSizeWidth * (long)(nCount-1)))
			{
				//  There should be enough room to display the entire contents of
				//  the selected tab plus something for the inactive tabs

				bool bMakeInactiveSameSize = ((nMinInactiveWidth * nRatioWithSelectionFullSize) < nInactiveSameSizeWidth);

				xpos = m_settings.iIndent;
				for(i=0; i<nCount; ++i )
				{
					TItem* pItem = m_Items[i];
					RECT rcItemDesired = pItem->GetRect();
					rcItem.left = rcItem.right = xpos;
					if((int)i == m_iCurSel)
					{
						rcItem.right += (rcItemDesired.right - rcItemDesired.left);
					}
					else
					{
						if(bMakeInactiveSameSize)
						{
							rcItem.right += (long)((cxClientInactiveTabs / (nCount-1)) + 0.5);
						}
						else
						{
							rcItem.right += (long)(((rcItemDesired.right - rcItemDesired.left) * nRatioWithSelectionFullSize) + 0.5);
						}
					}
					pItem->SetRect(rcItem);
					xpos += (rcItem.right-rcItem.left);
				}
			}
			else
			{
				// We're down pretty small, so just make all the tabs the same width
				int cxItem = (nTabAreaWidth - (m_settings.iIndent*2)) / (int)nCount;

				xpos = m_settings.iIndent;

				for(i=0; i<nCount; ++i)
				{
					rcItem.left = rcItem.right = xpos;
					rcItem.right += cxItem;
					m_Items[i]->SetRect(rcItem);
					xpos += (rcItem.right-rcItem.left);
				}
			}
		}

		dc.SelectFont(hOldFont);
	}

	void UpdateLayout_ScrollToFit(RECT rcTabItemArea)
	{
		DWORD dwStyle = this->GetStyle();

		// When we scroll to fit, we ignore what's passed in for the
		// tab item area rect, and use the client rect instead
		RECT rcClient;
		this->GetClientRect(&rcClient);

		CClientDC dc(m_hWnd);
		//HFONT hOldFont = dc.SelectFont(lpNMCustomDraw->hFontInactive);
		HFONT hOldFont = dc.SelectFont(m_font);

		RECT rcItem = rcClient;
		// rcItem.top and rcItem.bottom aren't really going to change

		// Recalculate tab positions and widths
		// See OnItemPrePaint for a discussion of how CDotNetTabCtrlImpl
		//  interprets margin, padding, etc.
		size_t nCount = m_Items.GetCount();
		int xpos = m_settings.iIndent;
		HFONT hRestoreNormalFont = NULL;
		for( size_t i=0; i<nCount; ++i )
		{
			bool bSelected = ((int)i == m_iCurSel);
			if(bSelected)
			{
				//hRestoreNormalFont = dc.SelectFont(lpNMCustomDraw->hFontSelected);
				hRestoreNormalFont = dc.SelectFont(m_fontSel);
			}

			TItem* pItem = m_Items[i];
			rcItem.left = rcItem.right = xpos;
			//rcItem.right += ((bSelected ? m_settings.iSelMargin : m_settings.iMargin));
			rcItem.right += m_settings.iMargin;
			if( pItem->UsingImage() && !m_imageList.IsNull())
			{
				IMAGEINFO ii = {0};
				int nImageIndex = pItem->GetImageIndex();
				m_imageList.GetImageInfo(nImageIndex, &ii);
				rcItem.right += (ii.rcImage.right - ii.rcImage.left);
			}
			if( pItem->UsingText() )
			{
				RECT rcText = {0};
				CString sText = pItem->GetText();
				dc.DrawText(sText, sText.GetLength(), &rcText, DT_SINGLELINE | DT_CALCRECT);
				rcItem.right += (rcText.right - rcText.left) + (m_settings.iPadding * 2);
			}
			rcItem.right += m_settings.iMargin;
			pItem->SetRect(rcItem);
			xpos += (rcItem.right - rcItem.left);

			if(hRestoreNormalFont != NULL)
			{
				dc.SelectFont(hRestoreNormalFont);
				hRestoreNormalFont = NULL;
			}
		}
		xpos += m_settings.iIndent;

		// If we've been scrolled to the left, and resize so
		// there's more client area to the right, adjust the
		// scroll offset accordingly.
		if((xpos + m_iScrollOffset) < rcTabItemArea.right)
		{
			m_iScrollOffset = (rcTabItemArea.right - xpos);
		}

		dc.SelectFont(hOldFont);
	}

};


template <class TItem = CCustomTabItem>
class CDotNetButtonTabCtrl : 
	public CCustomTabCtrl<CDotNetButtonTabCtrl, TItem>
{
protected:
	typedef CCustomTabCtrl<CDotNetButtonTabCtrl, TItem> customTabClass;

protected:
	CBrush m_hbrBackground;

	const signed char m_nMinWidthToDisplayText;

// Constructor
public:
	CDotNetButtonTabCtrl() :
		m_nMinWidthToDisplayText(12)
	{
	}

// Message Handling
public:
	DECLARE_WND_CLASS(_T("WTL_DotNetButtonTabCtrl"))

	BEGIN_MSG_MAP(CDotNetButtonTabCtrl)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
		CHAIN_MSG_MAP(customTabClass)
	END_MSG_MAP()

	LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DWORD dwStyle = this->GetStyle();

		// Initialize/Reinitialize font
		// Visual Studio.Net seems to use the "icon" font for the tabs
		LOGFONT lfIcon = { 0 };
		::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lfIcon), &lfIcon, 0);

		bool bResetFont = true;
		if( !m_font.IsNull() )
		{
			LOGFONT lf = {0};
			if(m_font.GetLogFont(&lf))
			{
				if(lstrcmpi(lf.lfFaceName, lfIcon.lfFaceName) == 0 &&
					lf.lfHeight == lfIcon.lfHeight)
				{
					bResetFont = false;
				}
			}
		}

		if(bResetFont)
		{
			if(!m_font.IsNull()) m_font.DeleteObject();
			if(!m_fontSel.IsNull()) m_fontSel.DeleteObject();

			HFONT font = m_font.CreateFontIndirect(&lfIcon);
			if(font==NULL)
			{
				m_font.Attach(AtlGetDefaultGuiFont());
			}

			if(CTCS_BOLDSELECTEDTAB == (dwStyle & CTCS_BOLDSELECTEDTAB))
			{
				lfIcon.lfWeight = FW_BOLD;
			}

			font = m_fontSel.CreateFontIndirect(&lfIcon);
			if(font==NULL)
			{
				m_fontSel.Attach(AtlGetDefaultGuiFont());
			}
		}

		// Background brush
		if( !m_hbrBackground.IsNull() ) m_hbrBackground.DeleteObject();

		m_hbrBackground.CreateSysColorBrush(COLOR_BTNFACE);

		m_settings.iIndent = 5;
		m_settings.iPadding = 4;
		m_settings.iMargin = 3;
		m_settings.iSelMargin = 3;

		//T* pT = static_cast<T*>(this);
		//pT->UpdateLayout();
		this->UpdateLayout();
		Invalidate();
		return 0;
	}

// Overrides for painting from CCustomTabCtrl
public:

	void InitializeDrawStruct(LPNMCTCCUSTOMDRAW lpNMCustomDraw)
	{
		DWORD dwStyle = this->GetStyle();

		lpNMCustomDraw->hFontInactive = m_font;
		lpNMCustomDraw->hFontSelected = m_fontSel;
		lpNMCustomDraw->hBrushBackground = m_hbrBackground;
		lpNMCustomDraw->clrTextSelected = ::GetSysColor(COLOR_BTNTEXT);
		lpNMCustomDraw->clrTextInactive = ::GetSysColor(COLOR_BTNTEXT);
		lpNMCustomDraw->clrSelectedTab = ::GetSysColor(COLOR_WINDOW);
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
		CDCHandle dc( lpNMCustomDraw->nmcd.hdc );

		// Set up the text color and background mode
		dc.SetTextColor(lpNMCustomDraw->clrBtnText);
		dc.SetBkMode(TRANSPARENT);

		// Erase Background
		//  (do it here instead of a handler for WM_ERASEBKGND
		//   so that we can do flicker-free drawing with the help
		//   of COffscreenDrawRect that's in the base class)
		// Note: Because the "erase" part is very simple, and only coloring
		//  it with the background color, we can do a smarter erase.
		//  Instead of erasing the whole client area (which might be clipped),
		//  We'll just ask the HDC for the clip box.

		RECT rc = {0};
		//GetClientRect(&rc);
		dc.GetClipBox(&rc);


		HBRUSH hOldBrush = dc.SelectBrush(lpNMCustomDraw->hBrushBackground);
		dc.PatBlt(rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, PATCOPY);
		dc.SelectBrush(hOldBrush);

	}

	void DoItemPaint(LPNMCTCCUSTOMDRAW lpNMCustomDraw)
	{
		CDCHandle dc( lpNMCustomDraw->nmcd.hdc );
		bool bSelected = (CDIS_SELECTED == (lpNMCustomDraw->nmcd.uItemState & CDIS_SELECTED));
		bool bHot = (CDIS_HOT == (lpNMCustomDraw->nmcd.uItemState & CDIS_HOT));
		// NOTE: lpNMCustomDraw->nmcd.rc is in logical coordinates
		RECT &rc = lpNMCustomDraw->nmcd.rc;
		int nItem = lpNMCustomDraw->nmcd.dwItemSpec;

		DWORD dwStyle = this->GetStyle();

		int nIconVerticalCenter = (rc.bottom + rc.top) / 2;

		RECT rcTab = rc;
		RECT rcText = rc;

		rcTab.top += 3;
		rcTab.bottom -= 3;

		if( bSelected )
		{
			// Tab is selected, so paint as select

			CPen penOutline;
			penOutline.CreatePen(PS_SOLID, 1, lpNMCustomDraw->clrHighlight);

			CBrush brushSelected;
			brushSelected.CreateSolidBrush(lpNMCustomDraw->clrSelectedTab);

			HPEN hOldPen = dc.SelectPen(penOutline);
			HBRUSH hOldBrush = dc.SelectBrush(brushSelected);

			dc.Rectangle(&rcTab);

			dc.SelectPen(hOldPen);
			dc.SelectBrush(hOldBrush);
		}
		else
		{
			// Tab is not selected

			// Draw division line on right, unless we're the item
			// on the left of the selected tab
			if(nItem + 1 == m_iCurSel)
			{
				// Item just left of selected tab
			}
			else
			{
				CPen pen;
				pen.CreatePen(PS_SOLID, 1, lpNMCustomDraw->clrBtnShadow);
				CPenHandle penOld = dc.SelectPen(pen);
				if(CTCS_BOTTOM == (dwStyle & CTCS_BOTTOM))
				{
					// Important!  Be sure and keep within "our" tab area horizontally
					dc.MoveTo(rcTab.right-1, rcTab.top + 3);
					dc.LineTo(rcTab.right-1, rcTab.bottom - 1);
				}
				else
				{
					// Important!  Be sure and keep within "our" tab area horizontally
					dc.MoveTo(rcTab.right-1, rcTab.top + 2);
					dc.LineTo(rcTab.right-1, rcTab.bottom - 2);
				}
				dc.SelectPen(penOld);
			}
		}

		customTabClass::TItem* pItem = this->GetItem(nItem);

		HFONT hOldFont = NULL;
		if( bSelected )
		{
			hOldFont = dc.SelectFont(lpNMCustomDraw->hFontSelected);
		}
		else
		{
			hOldFont = dc.SelectFont(lpNMCustomDraw->hFontInactive);
		}
		COLORREF crPrevious = 0;
		if( bHot )
		{
			crPrevious = dc.SetTextColor(lpNMCustomDraw->clrHighlightHotTrack);
		}
		else if( bSelected )
		{
			crPrevious = dc.SetTextColor(lpNMCustomDraw->clrTextSelected);
		}
		else
		{
			crPrevious = dc.SetTextColor(lpNMCustomDraw->clrTextInactive);
		}


		//--------------------------------------------
		// This is how CDotNetButtonTabCtrl interprets padding, margin, etc.:
		//
		//  M - Margin
		//  P - Padding
		//  I - Image
		//  Text - Tab Text
		//
		// With image:
		//     __________________________
		//
		//    | M | I | P | Text | P | M |
		//     --------------------------
		//
		// Without image:
		//     ______________________
		//
		//    | M | P | Text | P | M |
		//     ----------------------

		//rcText.left += (bSelected ? m_settings.iSelMargin : m_settings.iMargin);
		rcText.left += m_settings.iMargin;
		rcText.right -= m_settings.iMargin;
		if (pItem->UsingImage() && !m_imageList.IsNull())
		{
			// Draw the image.
			IMAGEINFO ii = {0};
			int nImageIndex = pItem->GetImageIndex();
			m_imageList.GetImageInfo(nImageIndex, &ii);

			if( (ii.rcImage.right - ii.rcImage.left) < (rcTab.right - rcTab.left) )
			{
				int nImageHalfHeight = (ii.rcImage.bottom - ii.rcImage.top) / 2;
				m_imageList.Draw(dc, nImageIndex, rcText.left, nIconVerticalCenter - nImageHalfHeight, ILD_NORMAL);
			}

			// Offset on the right of the image.
			rcText.left += (ii.rcImage.right - ii.rcImage.left);
		}

		if (rcText.left + m_nMinWidthToDisplayText < rcText.right)
		{
			::InflateRect(&rcText, -m_settings.iPadding, 0);

			CString sText = pItem->GetText();
			dc.DrawText(sText, sText.GetLength(), &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_PATH_ELLIPSIS);
		}

		dc.SetTextColor(crPrevious);
		dc.SelectFont(hOldFont);
	}


// Overrides from CCustomTabCtrl
public:

	void UpdateLayout_Default(RECT rcTabItemArea)
	{
		long nMinInactiveWidth = 0x7FFFFFFF;
		long nMaxInactiveWidth = 0;

		DWORD dwStyle = this->GetStyle();

		CClientDC dc(m_hWnd);
		//HFONT hOldFont = dc.SelectFont(lpNMCustomDraw->hFontInactive);
		HFONT hOldFont = dc.SelectFont(m_font);

		LONG nTabAreaWidth = (rcTabItemArea.right - rcTabItemArea.left);

		RECT rcItem = rcTabItemArea;
		// rcItem.top and rcItem.bottom aren't really going to change

		// Recalculate tab positions and widths
		// See OnItemPrePaint for a discussion of how CDotNetButtonTabCtrl
		//  interprets margin, padding, etc.
		size_t nCount = m_Items.GetCount();
		int xpos = m_settings.iIndent;
		HFONT hRestoreNormalFont = NULL;
		for( size_t i=0; i<nCount; ++i )
		{
			bool bSelected = ((int)i == m_iCurSel);
			if(bSelected)
			{
				//hRestoreNormalFont = dc.SelectFont(lpNMCustomDraw->hFontSelected);
				hRestoreNormalFont = dc.SelectFont(m_fontSel);
			}

			customTabClass::TItem* pItem = m_Items[i];
			rcItem.left = rcItem.right = xpos;
			//rcItem.right += ((bSelected ? m_settings.iSelMargin : m_settings.iMargin));
			rcItem.right += m_settings.iMargin;
			if( pItem->UsingImage() && !m_imageList.IsNull())
			{
				IMAGEINFO ii = {0};
				int nImageIndex = pItem->GetImageIndex();
				m_imageList.GetImageInfo(nImageIndex, &ii);
				rcItem.right += (ii.rcImage.right - ii.rcImage.left);
			}
			if( pItem->UsingText() )
			{
				RECT rcText = {0};
				CString sText = pItem->GetText();
				dc.DrawText(sText, sText.GetLength(), &rcText, DT_SINGLELINE | DT_CALCRECT);
				rcItem.right += (rcText.right - rcText.left) + (m_settings.iPadding * 2);
			}
			rcItem.right += m_settings.iMargin;
			pItem->SetRect(rcItem);
			xpos += (rcItem.right - rcItem.left);

			if(hRestoreNormalFont != NULL)
			{
				dc.SelectFont(hRestoreNormalFont);
				hRestoreNormalFont = NULL;
			}

			if(!bSelected)
			{
				if((rcItem.right - rcItem.left) < nMinInactiveWidth)
				{
					nMinInactiveWidth = (rcItem.right - rcItem.left);
				}
				if((rcItem.right - rcItem.left) > nMaxInactiveWidth)
				{
					nMaxInactiveWidth = (rcItem.right - rcItem.left);
				}
			}
		}
		xpos += m_settings.iIndent;

		if(xpos > nTabAreaWidth && nCount > 0 && m_iCurSel >= 0)
		{
			// Our desired widths are more than the width of the client area.
			// We need to have some or all of the tabs give up some real estate

			// We'll try to let the selected tab have its fully desired width.
			// If it can't, we'll make all the tabs the same width.

			RECT rcSelected = m_Items[m_iCurSel]->GetRect();
			LONG nSelectedWidth = (rcSelected.right - rcSelected.left);

			long cxClientInactiveTabs = nTabAreaWidth - (m_settings.iIndent * 2) - nSelectedWidth;

			double nRatioWithSelectionFullSize = (double) (cxClientInactiveTabs) / (double)(xpos - (m_settings.iIndent * 2) - nSelectedWidth);

			long nInactiveSameSizeWidth = (m_nMinWidthToDisplayText + (m_settings.iMargin*2) + (m_settings.iPadding));

			if(cxClientInactiveTabs > (nInactiveSameSizeWidth * (long)(nCount-1)))
			{
				//  There should be enough room to display the entire contents of
				//  the selected tab plus something for the inactive tabs

				bool bMakeInactiveSameSize = ((nMinInactiveWidth * nRatioWithSelectionFullSize) < nInactiveSameSizeWidth);

				xpos = m_settings.iIndent;
				for(i=0; i<nCount; ++i )
				{
					customTabClass::TItem* pItem = m_Items[i];
					RECT rcItemDesired = pItem->GetRect();
					rcItem.left = rcItem.right = xpos;
					if((int)i == m_iCurSel)
					{
						rcItem.right += (rcItemDesired.right - rcItemDesired.left);
					}
					else
					{
						if(bMakeInactiveSameSize)
						{
							rcItem.right += (long)((cxClientInactiveTabs / (nCount-1)) + 0.5);
						}
						else
						{
							rcItem.right += (long)(((rcItemDesired.right - rcItemDesired.left) * nRatioWithSelectionFullSize) + 0.5);
						}
					}
					pItem->SetRect(rcItem);
					xpos += (rcItem.right-rcItem.left);
				}
			}
			else
			{
				// We're down pretty small, so just make all the tabs the same width
				int cxItem = (nTabAreaWidth - (m_settings.iIndent*2)) / (int)nCount;

				xpos = m_settings.iIndent;

				for(i=0; i<nCount; ++i)
				{
					rcItem.left = rcItem.right = xpos;
					rcItem.right += cxItem;
					m_Items[i]->SetRect(rcItem);
					xpos += (rcItem.right-rcItem.left);
				}
			}
		}

		dc.SelectFont(hOldFont);
	}

	void UpdateLayout_ScrollToFit(RECT rcTabItemArea)
	{
	}

};

template <class TItem = CCustomTabItem>
class CDotNetTabCtrl :
	public CDotNetTabCtrlImpl<CDotNetTabCtrl, TItem>
{
protected:
	typedef CDotNetTabCtrl thisClass;
	typedef CDotNetTabCtrlImpl<CDotNetTabCtrl, TItem> baseClass;

// Constructors:
public:
	CDotNetTabCtrl()
	{
	}

public:

	DECLARE_WND_CLASS(_T("WTL_DotNetTabCtrl"))  

	//We have nothing special to add.
	//BEGIN_MSG_MAP(thisClass)
	//	CHAIN_MSG_MAP(baseClass)
	//END_MSG_MAP()
};


#endif // __DOTNET_TABCTRL_H__

