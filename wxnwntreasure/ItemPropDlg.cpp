

#include "ItemPropDlg.h"

BEGIN_EVENT_TABLE(ItemPropDlg, wxDialog)
    EVT_BUTTON( wxID_OK, ItemPropDlg::OnOk )
END_EVENT_TABLE()


ItemPropDlg::ItemPropDlg(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE)
{

    label_1 = new wxStaticText(this, -1, "Action To Take");
    label_2 = new wxStaticText(this, -1, "Percent Chance");
    const wxString combo_1_choices[] = {
        "Drop Nothing",
        "Drop Gold",
        "Drop Item",
        "Move To Table"
    };
    combo_1 = new wxComboBox(this, -1, "", wxDefaultPosition, wxDefaultSize, 4, combo_1_choices, wxCB_DROPDOWN);
    spin_1 = new wxSpinCtrl(this, -1, "1", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100);
    button_1 = new wxButton(this, wxID_OK, "OK");
    button_2 = new wxButton(this, wxID_CANCEL, "Cancel");

    set_properties();
    do_layout();

}


void ItemPropDlg::set_properties()
{

    SetTitle("Item Proporties");
    combo_1->SetSelection(0);



}


void ItemPropDlg::do_layout()
{
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);

    wxGridSizer* gridsizer_top = new wxGridSizer(2, 2, 0, 0);
	gridsizer_top->Add(label_1, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
	gridsizer_top->Add(combo_1, 0, wxALIGN_CENTER_VERTICAL, 0);
	gridsizer_top->Add(label_2, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
	gridsizer_top->Add(spin_1, 0, wxALIGN_CENTER_VERTICAL, 0);
    main_sizer->Add(gridsizer_top, 1, wxEXPAND, 0);

    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
	button_sizer->Add(button_1, 0, wxALIGN_RIGHT, 0);
	button_sizer->Add(button_2, 0, 0, 0);
    main_sizer->Add(button_sizer, 1, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(main_sizer);
    Layout();
    Centre();

}

void ItemPropDlg::OnOk(wxCommandEvent &event)
{
       main_sizer->Show(gridsizer_top, 1);
       Layout();
}

