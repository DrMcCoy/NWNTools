/***************************************************************************
 *   Copyright (C) 2003 by John S. Brown                                   *
 *   malakh@dragoncat dot net                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/treectrl.h>



#include "wxNwnTreasure.h"

#include "new.xpm"
#include "fileopen.xpm"
#include "save.xpm"
#include "encounter.xpm"

#include "none.xpm"
#include "table.xpm"
#include "gold.xpm"
#include "tableprop.xpm"
#include "up.xpm"
#include "down.xpm"
#include "exit.xpm"



BEGIN_EVENT_TABLE(NwnTFrame, wxFrame)
	EVT_MENU(ID_New, NwnTFrame::OnOpen)
	EVT_MENU(ID_Open, NwnTFrame::OnOpen)
	EVT_MENU(ID_Save, NwnTFrame::OnOpen)
END_EVENT_TABLE()

const wxChar *open_help = wxT("Open a file.");

NwnTFrame::NwnTFrame(wxWindow* parent, int id, const char* title, const wxPoint& pos, const wxSize& size, long style):
    wxFrame(parent, id, title, pos, size, wxDEFAULT_FRAME_STYLE)
{

    _mb = new wxMenuBar();
    SetMenuBar(_mb);
    wxMenu* _mFile = new wxMenu();
    _mFile->Append(ID_New, "New", "");
    _mFile->Append(ID_Open, "Open", "");
    _mFile->Append(ID_Save, "Save", "");
    _mFile->Append(wxNewId(), "Save As...", "");
    _mFile->AppendSeparator();
    _mFile->Append(wxNewId(), "Export Script...", "");
    _mFile->AppendSeparator();
    wxMenu* _mFile_Recent = new wxMenu();
    _mFile_Recent->Append(wxNewId(), "<Empty>", "");
    _mFile->Append(wxNewId(), "Recent Files", _mFile_Recent, "");
    _mFile->AppendSeparator();
    _mFile->Append(wxNewId(), "Exit", "");
    _mb->Append(_mFile, "File");
    wxMenu* _mEdit = new wxMenu();
    _mEdit->Append(wxNewId(), "Insert New In...", "");
    _mEdit->Append(wxNewId(), "Insert New After...", "");
    _mEdit->Append(wxNewId(), "Insert Table...", "");
    _mEdit->AppendSeparator();
    _mEdit->Append(wxNewId(), "Proporties...", "");
    _mEdit->Append(wxNewId(), "Table Proporties...", "");
    _mEdit->AppendSeparator();
    _mEdit->Append(wxNewId(), "Move Up", "");
    _mEdit->Append(wxNewId(), "Move Down", "");
    _mEdit->AppendSeparator();
    _mEdit->Append(wxNewId(), "Delete", "");
    _mb->Append(_mEdit, "Edit");
    wxMenu* _mHelp = new wxMenu();
    _mHelp->Append(wxNewId(), "About", "");
	_mb->Append(_mHelp, "Help");
	_sb = CreateStatusBar(1);



	_tb = CreateToolBar(wxTB_TEXT | wxTB_FLAT | wxTB_DOCKABLE, -1, "Toolbar");

	_tb->AddTool(ID_New, wxT("New"), wxBitmap(new_xpm), "New...");
	_tb->AddTool(ID_Open, wxT("Open"), wxBitmap(fileopen_xpm), "Open...");
	_tb->AddTool(ID_Save, wxT("Save"), wxBitmap(save_xpm), "Save...");
	_tb->AddSeparator();
	_tb->AddTool(ID_Save, wxT("Create in"), wxBitmap(none_xpm), "Create in...");
	_tb->AddTool(ID_Save, wxT("After"), wxBitmap(none_xpm), "After...");
	_tb->AddTool(ID_Save, wxT("Table"), wxBitmap(table_xpm), "Table...");
	_tb->AddSeparator();
	_tb->AddTool(ID_Save, wxT("Prop."), wxBitmap(tableprop_xpm), "Proporties...");
	_tb->AddTool(ID_Save, wxT("Table Prop."), wxBitmap(tableprop_xpm), "Table Proporties...");
	_tb->AddSeparator();
	_tb->AddTool(ID_Save, wxT("Delete"), wxBitmap(none_xpm), "Delete...");
	_tb->AddSeparator();
	_tb->AddTool(ID_Save, wxT("Up"), wxBitmap(up_xpm), "Up...");
	_tb->AddTool(ID_Save, wxT("Down"), wxBitmap(down_xpm), "Down...");
	_tb->AddSeparator();
	_tb->AddTool(ID_Save, wxT("Exit"), wxBitmap(exit_xpm), "Exit...");

	_tb->Realize();

	_tc1 = new wxTreeCtrl(this, -1, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxSUNKEN_BORDER);

	set_properties();
	do_layout();

}


void NwnTFrame::set_properties()
{

    SetTitle("NWN Treasure Editor (OK Edition)");
    SetSize(wxSize(823, 561));
    int statusbar_widths[] = { -1 };
    _sb->SetStatusWidths(1, statusbar_widths);
    const wxString statusbar_fields[] = {
    	"Welcome to NWN"
     };

    for(int i = 0; i < _sb->GetFieldsCount(); ++i) {
        _sb->SetStatusText(statusbar_fields[i], i);
    }

}


void NwnTFrame::do_layout()
{

    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    sizer_1->Add(_tc1, 1, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(sizer_1);
    Layout();

}

void NwnTFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{

}


class wxNwnTreasure: public wxApp {
public:
    bool OnInit();
};




IMPLEMENT_APP(wxNwnTreasure)

bool wxNwnTreasure::OnInit()
{

    NwnTFrame* frame_1 = new NwnTFrame(0, -1, "");
    SetTopWindow(frame_1);
    frame_1->Show();
    return true;
}

