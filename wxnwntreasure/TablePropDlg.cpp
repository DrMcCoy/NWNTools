/***************************************************************************
 *   Copyright (C) 2003 by John S. Brown                                   *
 *   malakh@dragoncat dot net                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "TablePropDlg.h"

BEGIN_EVENT_TABLE(TblPropDialog, wxDialog)
    EVT_BUTTON( wxID_OK, TblPropDialog::OnOk )
    EVT_CHECKBOX( ID_GlblCheckbox, TblPropDialog::OnGlblCheck )
END_EVENT_TABLE()

TblPropDialog::TblPropDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE)
{
   
    label_TblName = new wxStaticText(this, -1, "Table Name");
    text_TblName = new wxTextCtrl(this, -1, "");
    label_DMode = new wxStaticText(this, -1, "Decision Mode");
    const wxString combo_box_1_choices[] = {
        "Pick One Of",
        "Pick Multiple From",
        "Pick By Racial Type",
        "Pick By Level Range",
        "Pick By Random Class",
        "Pick By Specific",
        "Pick By Weapon Focus",
        "Pick By Armor Proficiency",
        "Pick By Class"
    };
    combo_box_1 = new wxComboBox(this, -1, "", wxDefaultPosition, wxDefaultSize, 9, combo_box_1_choices, wxCB_DROPDOWN);
    checkbox_1 = new wxCheckBox(this, ID_GlblCheckbox, "Global Table - Global tables will be made ");
    label_1 = new wxStaticText(this, -1, "available as starting points.");
    label_TblNum = new wxStaticText(this, -1, "Table Number");
    spin_ctrl_1 = new wxSpinCtrl(this, -1, "100", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100);
    button_1 = new wxButton(this, wxID_OK, "OK");
    button_2 = new wxButton(this, wxID_CANCEL, "Cancel");

    set_properties();
    do_layout();

}



void TblPropDialog::set_properties()
{

    SetTitle("Table Proporties");
    SetSize(wxSize(300, 263));
    combo_box_1->SetSelection(0);
    label_TblNum->Disable();
    spin_ctrl_1->Disable();

}


void TblPropDialog::do_layout()
{
 
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxGridSizer* grid_sizer_2 = new wxGridSizer(2, 2, 0, 0);
    wxBoxSizer* sizer_2 = new wxBoxSizer(wxVERTICAL);
    wxGridSizer* grid_sizer_1 = new wxGridSizer(2, 2, 0, 0);
    grid_sizer_1->Add(label_TblName, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
    grid_sizer_1->Add(text_TblName, 0, wxTOP, 5);
    grid_sizer_1->Add(label_DMode, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
    grid_sizer_1->Add(combo_box_1, 0, wxALIGN_CENTER_VERTICAL, 10);
    sizer_1->Add(grid_sizer_1, 1, wxEXPAND, 0);
    sizer_2->Add(checkbox_1, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL, 10);
    sizer_2->Add(label_1, 0, wxLEFT, 60);
    sizer_1->Add(sizer_2, 1, wxALL|wxEXPAND, 20);
    grid_sizer_2->Add(label_TblNum, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
    grid_sizer_2->Add(spin_ctrl_1, 0, wxALIGN_CENTER_VERTICAL, 0);
    grid_sizer_2->Add(button_1, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 10);
    grid_sizer_2->Add(button_2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 10);
    sizer_1->Add(grid_sizer_2, 1, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(sizer_1);
    Layout();
    Centre();

}

void TblPropDialog::OnOk(wxCommandEvent &event)
{
    event.Skip();
}


void TblPropDialog::OnGlblCheck(wxCommandEvent &event)
{

    if (checkbox_1->GetValue()==true) {
        label_TblNum->Enable(true);
        spin_ctrl_1->Enable(true);
    }
    else {
         label_TblNum->Disable();
         spin_ctrl_1->Disable();
    }
}










