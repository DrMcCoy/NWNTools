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
//#include <wx/image.h>
#include <wx/treectrl.h>



#include "wxNwnTreasure.h"
#include "TablePropDlg.h"
#include "ItemPropDlg.h"



//#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)

#include "new.xpm"
#include "fileopen.xpm"
#include "save.xpm"
//#include "encounter.xpm"
#include "none.xpm"
#include "table.xpm"
#include "gold.xpm"
#include "after.xpm"
#include "prop.xpm"
#include "tableprop.xpm"
#include "up.xpm"
#include "down.xpm"
#include "helpicon.xpm"
#include "exit.xpm"

//#endif

BEGIN_EVENT_TABLE(NwnTFrame, wxFrame)
	EVT_MENU(ID_New, NwnTFrame::OnNew)
	EVT_MENU(ID_Open, NwnTFrame::OnOpen)
	EVT_MENU(ID_Save, NwnTFrame::OnSave)
	EVT_MENU(ID_Create, NwnTFrame::OnCreate)
	EVT_MENU(ID_After, NwnTFrame::OnAfter)
	EVT_MENU(ID_Table, NwnTFrame::OnTable)
	EVT_MENU(ID_Prop, NwnTFrame::OnProp)
	EVT_MENU(ID_TProp, NwnTFrame::OnTProp)
	EVT_MENU(ID_Delete, NwnTFrame::OnDelete)
	EVT_MENU(ID_Up, NwnTFrame::OnUp)
	EVT_MENU(ID_Down, NwnTFrame::OnDown)
	EVT_MENU(ID_About, NwnTFrame::OnAbout)
	EVT_MENU(ID_Exit, NwnTFrame::OnExit)
END_EVENT_TABLE()

//const wxChar *open_help = wxT("Open a file.");

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
        _mFile_Recent->Append(ID_Empty, "<Empty>", "");
                _mFile_Recent->Enable(ID_Empty, 0);
        _mFile->Append(wxNewId(), "Recent Files", _mFile_Recent, "");
        _mFile->AppendSeparator();
        _mFile->Append(ID_Exit, "Exit", "");
    _mb->Append(_mFile, "File");

    wxMenu* _mEdit = new wxMenu();
        _mEdit->Append(ID_Create, "Insert New In...", "");
        _mEdit->Append(ID_After, "Insert New After...", "");
        _mEdit->Append(ID_Table, "Insert Table...", "");
        _mEdit->AppendSeparator();
    _mEdit->Append(ID_Prop, "Proporties...", "");
    _mEdit->Append(ID_TProp, "Table Proporties...", "");
    _mEdit->AppendSeparator();
    _mEdit->Append(ID_Up, "Move Up", "");
    _mEdit->Append(ID_Down, "Move Down", "");
    _mEdit->AppendSeparator();
    _mEdit->Append(ID_Delete, "Delete", "");
    _mb->Append(_mEdit, "Edit");
    wxMenu* _mHelp = new wxMenu();
    _mHelp->Append(ID_About, "About", "");
	_mb->Append(_mHelp, "Help");
	_sb = CreateStatusBar(1);



	_tb = CreateToolBar(wxTB_TEXT | wxTB_FLAT | wxTB_DOCKABLE, -1, "Toolbar");

	_tb->AddTool(ID_New, wxT("New"), wxBitmap(new_xpm), "New...");
	_tb->AddTool(ID_Open, wxT("Open"), wxBitmap(fileopen_xpm), "Open...");
	_tb->AddTool(ID_Save, wxT("Save"), wxBitmap(save_xpm), "Save...");
	_tb->AddSeparator();
	_tb->AddTool(ID_Create, wxT("Create in"), wxBitmap(gold_xpm), "Create in...");
	_tb->EnableTool(ID_Create, 0);
	_tb->AddTool(ID_After, wxT("After"), wxBitmap(after_xpm), "After...");
	_tb->EnableTool(ID_After, 0);
	_tb->AddTool(ID_Table, wxT("Table"), wxBitmap(table_xpm), "Table...");
	_tb->AddSeparator();
	_tb->AddTool(ID_Prop, wxT("Prop."), wxBitmap(prop_xpm), "Proporties...");
	_tb->AddTool(ID_TProp, wxT("Table Prop."), wxBitmap(tableprop_xpm), "Table Proporties...");
	_tb->AddSeparator();
	_tb->AddTool(ID_Delete, wxT("Delete"), wxBitmap(none_xpm), "Delete...");
	_tb->AddSeparator();
	_tb->AddTool(ID_Up, wxT("Up"), wxBitmap(up_xpm), "Up...");
	_tb->AddTool(ID_Down, wxT("Down"), wxBitmap(down_xpm), "Down...");
	_tb->AddSeparator();
	_tb->AddTool(ID_About, wxT("About"), wxBitmap(helpicon_xpm), "About...");
	_tb->AddTool(ID_Exit, wxT("Exit"), wxBitmap(exit_xpm), "Exit...");

	_tb->Realize();

	_tc1 = new wxTreeCtrl(this, -1, wxDefaultPosition, wxDefaultSize, 
            wxTR_LINES_AT_ROOT|wxTR_HIDE_ROOT|wxTR_HAS_BUTTONS|wxSUNKEN_BORDER);



    _tc1->AddRoot(wxT("Tables"));

    _tc1_b1 = _tc1->AppendItem(_tc1->GetRootItem(), wxT("Treasure Table")
             );


    _tc1_b2 = _tc1->AppendItem(_tc1->GetRootItem(), wxT("Encounter Table")
            /*, wxBitmap(encounter_xpm) */ );
    _tc1_b3 = _tc1->AppendItem(_tc1->GetRootItem(), wxT("Placeable Table")
            /*, wxBitmap(encounter_xpm) */ );
    _tc1_b4 =  _tc1->AppendItem(_tc1->GetRootItem(), wxT("Profile Table")
            /*, MyTreeCtrl::TreeCtrlIcon_File */ );
	_tc1->AppendItem(_tc1_b4, wxT("Profile"));
	set_properties();
	do_layout();

}


void NwnTFrame::set_properties()
{

    SetTitle("NWN Treasure Editor (OK Edition)");
    SetSize(wxSize(1050, 600));
    int statusbar_widths[] = { -1 };
    _sb->SetStatusWidths(1, statusbar_widths);
    const wxString statusbar_fields[] = {
    	"Welcome to NWN Treasure Editor (OK Edition)"
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

void NwnTFrame::OnNew(wxCommandEvent& WXUNUSED(event))
{

    wxString msg;
	msg.Printf( _T("Insert new file code here."));
	wxMessageBox(msg, _T("New placeholder"), wxOK | wxICON_INFORMATION, this);

}

void NwnTFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
	wxString msg;
	msg.Printf( _T("Insert file open code here."));
	wxMessageBox(msg, _T("Open placeholder"), wxOK | wxICON_INFORMATION, this);
}

void NwnTFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
	wxString msg;
	msg.Printf( _T("Insert file save code here."));
	wxMessageBox(msg, _T("Save placeholder"), wxOK | wxICON_INFORMATION, this);
}

void NwnTFrame::OnCreate(wxCommandEvent& WXUNUSED(event))
{
	wxString msg;
	msg.Printf( _T("Insert Create In code here."));
	wxMessageBox(msg, _T("Create In placeholder"), wxOK | wxICON_INFORMATION, this);
}

void NwnTFrame::OnAfter(wxCommandEvent& WXUNUSED(event))
{
	wxString msg;
	msg.Printf( _T("Insert After code here."));
	wxMessageBox(msg, _T("After placeholder"), wxOK | wxICON_INFORMATION, this);
}

void NwnTFrame::OnTable(wxCommandEvent& WXUNUSED(event))
{
	wxString msg;
	msg.Printf( _T("Insert Table code here."));
	wxMessageBox(msg, _T("Table placeholder"), wxOK | wxICON_INFORMATION, this);
}

void NwnTFrame::OnProp(wxCommandEvent& WXUNUSED(event))
{
  ItemPropDlg
   IPropDlg ( this, -1,
                 this->GetTitle(),
                 wxPoint(300,200),
                 wxSize(250, 300),
                 wxRESIZE_BORDER |  wxDEFAULT_DIALOG_STYLE
               );

    IPropDlg.ShowModal();

}

void NwnTFrame::OnTProp(wxCommandEvent& WXUNUSED(event))
{

  TblPropDialog
   TPropDlg ( this, -1,
                 this->GetTitle(),
                 wxPoint(100,100),
                 wxSize(250, 300),
                 wxRESIZE_BORDER |  wxDEFAULT_DIALOG_STYLE
               );

    TPropDlg.ShowModal();

}

void NwnTFrame::OnDelete(wxCommandEvent& WXUNUSED(event))
{
	wxString msg;
	msg.Printf( _T("Insert Delete code here."));
	wxMessageBox(msg, _T("Delete placeholder"), wxOK | wxICON_INFORMATION, this);
}

void NwnTFrame::OnUp(wxCommandEvent& WXUNUSED(event))
{
	wxString msg;
	msg.Printf( _T("Insert Up code here."));
	wxMessageBox(msg, _T("Up placeholder"), wxOK | wxICON_INFORMATION, this);
}

void NwnTFrame::OnDown(wxCommandEvent& WXUNUSED(event))
{
	wxString msg;
	msg.Printf( _T("Insert Down code here."));
	wxMessageBox(msg, _T("Down placeholder"), wxOK | wxICON_INFORMATION, this);
}

void NwnTFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("Nwn Treasure Editor (OK Edition)\n")
                _T("Using %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About wxNwnTreasure"), wxOK | wxICON_INFORMATION, this);
}

void NwnTFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close

    Close(TRUE);
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

