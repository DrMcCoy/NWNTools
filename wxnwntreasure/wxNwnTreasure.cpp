// -*- C++ -*- generated by wxGlade 0.2.3 on Tue Aug 12 16:48:40 2003

#include <wx/wx.h>
#include <wx/image.h>
// begin wxGlade: ::dependencies
#include <wx/treectrl.h>
// end wxGlade


#include "wxNwnTreasure.h"

#include "fileopen.xpm"


BEGIN_EVENT_TABLE(NwnTFrame, wxFrame)
	EVT_MENU(ID_Open, NwnTFrame::OnOpen)
END_EVENT_TABLE()

const wxChar *open_help = wxT("Open a CHM book.");

NwnTFrame::NwnTFrame(wxWindow* parent, int id, const char* title, const wxPoint& pos, const wxSize& size, long style):
    wxFrame(parent, id, title, pos, size, wxDEFAULT_FRAME_STYLE)
{
    // begin wxGlade: NwnTFrame::NwnTFrame
    menubar = new wxMenuBar();
    SetMenuBar(menubar);
    wxMenu* wxglade_tmp_menu_1 = new wxMenu();
    wxglade_tmp_menu_1->Append(wxNewId(), "New", "");
    wxglade_tmp_menu_1->Append(wxNewId(), "Open", "");
    wxglade_tmp_menu_1->Append(wxNewId(), "Save", "");
    wxglade_tmp_menu_1->Append(wxNewId(), "Save As...", "");
    wxglade_tmp_menu_1->AppendSeparator();
    wxglade_tmp_menu_1->Append(wxNewId(), "Export Script...", "");
    wxglade_tmp_menu_1->AppendSeparator();
    wxMenu* wxglade_tmp_menu_1_sub = new wxMenu();
    wxglade_tmp_menu_1_sub->Append(wxNewId(), "<Empty>", "");
    wxglade_tmp_menu_1->Append(wxNewId(), "Recent Files", wxglade_tmp_menu_1_sub, "");
    wxglade_tmp_menu_1->AppendSeparator();
    wxglade_tmp_menu_1->Append(wxNewId(), "Exit", "");
    menubar->Append(wxglade_tmp_menu_1, "New");
    wxMenu* wxglade_tmp_menu_2 = new wxMenu();
    wxglade_tmp_menu_2->Append(wxNewId(), "Insert New In...", "");
    wxglade_tmp_menu_2->Append(wxNewId(), "Insert New After...", "");
    wxglade_tmp_menu_2->Append(wxNewId(), "Insert Table...", "");
    wxglade_tmp_menu_2->AppendSeparator();
    wxglade_tmp_menu_2->Append(wxNewId(), "Proporties...", "");
    wxglade_tmp_menu_2->Append(wxNewId(), "Table Proporties...", "");
    wxglade_tmp_menu_2->AppendSeparator();
    wxglade_tmp_menu_2->Append(wxNewId(), "Move Up", "");
    wxglade_tmp_menu_2->Append(wxNewId(), "Move Down", "");
    wxglade_tmp_menu_2->AppendSeparator();
    wxglade_tmp_menu_2->Append(wxNewId(), "Delete", "");
    menubar->Append(wxglade_tmp_menu_2, "Edit");
    wxMenu* wxglade_tmp_menu_3 = new wxMenu();
    wxglade_tmp_menu_3->Append(wxNewId(), "About", "");
    menubar->Append(wxglade_tmp_menu_3, "Help");
    statusbar = CreateStatusBar(1);



  _tb = CreateToolBar(wxTB_FLAT | wxTB_DOCKABLE | wxTB_HORIZONTAL | wxTB_TEXT);

  _tb->AddTool(ID_Open, wxT("Open"), wxBitmap(fileopen_xpm), "Open...");

  _tb->Realize();

    tree_ctrl_1 = new wxTreeCtrl(this, -1, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxSUNKEN_BORDER);

    set_properties();
    do_layout();
    // end wxGlade
}


void NwnTFrame::set_properties()
{
    // begin wxGlade: NwnTFrame::set_properties
    SetTitle("NWN Treasure Editor (OK Edition)");
    SetSize(wxSize(823, 561));
    int statusbar_widths[] = { -1 };
    statusbar->SetStatusWidths(1, statusbar_widths);
    const wxString statusbar_fields[] = {
    	"Welcome to NWN"
     };

    for(int i = 0; i < statusbar->GetFieldsCount(); ++i) {
        statusbar->SetStatusText(statusbar_fields[i], i);
    }
    // end wxGlade
}


void NwnTFrame::do_layout()
{
    // begin wxGlade: NwnTFrame::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    sizer_1->Add(tree_ctrl_1, 1, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(sizer_1);
    Layout();
    // end wxGlade
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
//    wxInitAllImageHandlers();
    NwnTFrame* frame_1 = new NwnTFrame(0, -1, "");
    SetTopWindow(frame_1);
    frame_1->Show();
    return true;
}

