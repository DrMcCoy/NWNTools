/***************************************************************************
 *   Copyright (C) 2003 by John S. Brown                                   *
 *   malakh@dragoncat dot net                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

//#include "wxChoice.h"
#include "ItemPropDlg.h"

BEGIN_EVENT_TABLE(ItemPropDlg, wxDialog)
    EVT_BUTTON( wxID_OK, ItemPropDlg::OnOk )
    EVT_CHOICE( ID_Choice, ItemPropDlg::OnChoiceSel )
END_EVENT_TABLE()


ItemPropDlg::ItemPropDlg(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE)
{


    label_1 = new wxStaticText(this, -1, wxT("Action To Take"));
    label_2 = new wxStaticText(this, -1, wxT("Percent Chance"));
        const wxString choice_1_choices[] = {
        wxT("Drop Nothing"),
        wxT("Drop Gold"),
        wxT("Drop Item"),
        wxT("Move To Table")
            };
        const wxString choice_2_choices[] = {
        wxT(""),
            };
        const wxString choice_3_choices[] = {
        wxT("Inherit"),
        wxT("Generic"),
        wxT("Specific"),
            };                        
        const wxString choice_4_choices[] = {
        wxT("Inherit"),
        wxT("Generic"),
        wxT("Specific"),
            };                        
    choice_1 = new wxChoice(this, ID_Choice, wxDefaultPosition, wxDefaultSize, 4, choice_1_choices, 0);
    spin_1 = new wxSpinCtrl(this, -1, wxT("1"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100);
    
    label_3 = new wxStaticText(this, -1, wxT("Number of Die"));
    spin_2 = new wxSpinCtrl(this, -1, wxT("1"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100);
    label_4 = new wxStaticText(this, -1, wxT("Die"));
    spin_3 = new wxSpinCtrl(this, -1, wxT("1"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100);    
    label_5 = new wxStaticText(this, -1, wxT("Multiplier"));
    text_1 = new wxTextCtrl(this, -1, wxT("1.00"));
    
    label_6 = new wxStaticText(this, -1, wxT("Item Blueprint"));
    text_2 = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_LINEWRAP|wxTE_MULTILINE|wxTE_READONLY);
    button_3 = new wxButton(this, -1, wxT("..."));

    label_7 = new wxStaticText(this, -1, wxT("The minimum and maximum values should both be set to 1 for non-stackable items.  For stackables, the count will be randomized in the given range."));
    label_8 = new wxStaticText(this, -1, wxT("Minimum"));
    label_9 = new wxStaticText(this, -1, wxT("Maximum"));
    spin_4 = new wxSpinCtrl(this, -1, wxT("1"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100);    
    spin_5 = new wxSpinCtrl(this, -1, wxT("1"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100);                      
    
    label_10 = new wxStaticText(this, -1, wxT("Table Name"));
    choice_2 = new wxChoice(this, -1, wxDefaultPosition, wxDefaultSize, 0, choice_2_choices, 0);
    label_11 = new wxStaticText(this, -1, wxT("The minimum and maximum specify the number of times this table will be used for generation."));
    label_12 = new wxStaticText(this, -1, wxT("Minimum"));
    label_13 = new wxStaticText(this, -1, wxT("Maximum"));
    spin_6 = new wxSpinCtrl(this, -1, wxT("1"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100);    
    spin_7 = new wxSpinCtrl(this, -1, wxT("1"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100);    
    label_14 = new wxStaticText(this, -1, wxT("Modifier"));
    label_15 = new wxStaticText(this, -1, wxT("Specific"));
    choice_3 = new wxChoice(this, -1, wxDefaultPosition, wxDefaultSize, 3, choice_3_choices, 0);
    choice_4 = new wxChoice(this, -1, wxDefaultPosition, wxDefaultSize, 3, choice_4_choices, 0);
    
    
    
    button_1 = new wxButton(this, wxID_OK, wxT("OK"));
    button_2 = new wxButton(this, wxID_CANCEL, wxT("Cancel"));

    set_properties();
    do_layout();

}

void ItemPropDlg::set_properties()
{

    SetTitle(wxT("Item Proporties"));
    this->SetSize(wxSize(340, 110));
    current=0;
    choice_1->SetSelection(current);
    choice_2->SetSelection(0);
    choice_3->SetSelection(0);
    choice_4->SetSelection(0);
    button_3->SetSize(wxSize(20,20));
    text_2->SetSize(wxSize(175, 22));

    
    


}


void ItemPropDlg::do_layout()
{
 main_sizer = new wxBoxSizer(wxVERTICAL);
    
    gridsizer_top = new wxGridSizer(2, 3, 0, 0);
	    gridsizer_top->Add(label_1, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
	    gridsizer_top->Add(choice_1, 0, wxALIGN_CENTER_VERTICAL, 0);
	    gridsizer_top->Add(20, 20, 0, 0);
	    gridsizer_top->Add(label_2, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
	    gridsizer_top->Add(spin_1, 0, wxALIGN_CENTER_VERTICAL, 0);
	    gridsizer_top->Add(20, 20, 0, 0);
    main_sizer->Add(gridsizer_top, 0, wxEXPAND, 0);
    
    gridsizer_mid1 = new wxGridSizer(3, 3, 0, 0);
        gridsizer_mid1->Add(label_3, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
        gridsizer_mid1->Add(spin_2, 0, wxALIGN_CENTER_VERTICAL, 0);
        gridsizer_mid1->Add(20, 20, 0, 0);
        gridsizer_mid1->Add(label_4, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10); 
        gridsizer_mid1->Add(spin_3, 0, wxALIGN_CENTER_VERTICAL, 0);
        gridsizer_mid1->Add(20, 20, 0, 0);
        gridsizer_mid1->Add(label_5, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10); 
        gridsizer_mid1->Add(text_1, 0, wxALIGN_CENTER_VERTICAL, 0);
        gridsizer_mid1->Add(20, 20, 0, 0);
    main_sizer->Add(gridsizer_mid1, 0, wxEXPAND, 0);

    gridsizer_mid2 = new wxGridSizer(3, 1, 0, 0);
         mid2_sizer1 = new wxGridSizer(1, 3, 0, 0);        
            mid2_sizer1->Add(label_6, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10); 
            mid2_sizer1->Add(text_2, 0, wxALIGN_CENTER_VERTICAL, 0);
            mid2_sizer1->Add(button_3, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 10);
         gridsizer_mid2->Add(mid2_sizer1, 0, wxEXPAND, 0);
         text_sizer = new wxBoxSizer(wxVERTICAL);
            text_sizer->Add(label_7, 1, wxALL|wxEXPAND, 5);                  
         gridsizer_mid2->Add(text_sizer, 1, wxEXPAND, 5);
         mid2_sizer2 = new wxGridSizer(2, 3, 0, 0);
            mid2_sizer2->Add(label_8, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);       
            mid2_sizer2->Add(spin_4, 0, wxALIGN_CENTER_VERTICAL, 0);
            mid2_sizer2->Add(20, 20, 0, 0);
            mid2_sizer2->Add(label_9, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);                   
            mid2_sizer2->Add(spin_5, 0, wxALIGN_CENTER_VERTICAL, 0);
            mid2_sizer2->Add(20, 20, 0, 0);
         gridsizer_mid2->Add(mid2_sizer2, 0, wxEXPAND, 0);   
    main_sizer->Add(gridsizer_mid2, 1, wxEXPAND, 0);
    
    gridsizer_mid3 = new wxGridSizer(3, 1, 0, 0);
         mid3_sizer1 = new wxGridSizer(1, 3, 0, 0);        
            mid3_sizer1->Add(label_10, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10); 
            mid3_sizer1->Add(choice_2, 0, wxALIGN_CENTER_VERTICAL, 0);
            mid3_sizer1->Add(20, 20, 0, 0);
         gridsizer_mid3->Add(mid3_sizer1, 0, wxEXPAND, 0);
         text_sizer2 = new wxBoxSizer(wxVERTICAL);
            text_sizer2->Add(label_11, 1, wxALL|wxEXPAND, 5);                  
         gridsizer_mid3->Add(text_sizer2, 1, wxEXPAND, 5);
         mid3_sizer2 = new wxGridSizer(4, 3, 10, 10); 
            mid3_sizer2->Add(label_12, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
            mid3_sizer2->Add(spin_6, 0, wxALIGN_CENTER_VERTICAL, 0);
            mid3_sizer2->Add(20, 20, 0, 0);
            mid3_sizer2->Add(label_13, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
            mid3_sizer2->Add(spin_7, 0, wxALIGN_CENTER_VERTICAL, 0);
            mid3_sizer2->Add(20, 20, 0, 0);
            mid3_sizer2->Add(label_14, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
            mid3_sizer2->Add(choice_3, 0, wxALIGN_CENTER_VERTICAL, 0);
            mid3_sizer2->Add(20, 20, 0, 0);
            mid3_sizer2->Add(label_15, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
            mid3_sizer2->Add(choice_4, 0, wxALIGN_CENTER_VERTICAL, 0);
            mid3_sizer2->Add(20, 20, 0, 0);
         gridsizer_mid3->Add(mid3_sizer2, 0, wxEXPAND, 0);
    main_sizer->Add(gridsizer_mid3, 1, wxEXPAND, 0);
    
    button_sizer = new wxGridSizer(1, 2, 0, 0);
	 button_sizer->Add(button_1, 0, wxALIGN_BOTTOM|wxALIGN_RIGHT|wxALL, 10);
	 button_sizer->Add(button_2, 0, wxALIGN_BOTTOM|wxALL, 10);
    main_sizer->Add(button_sizer, 0, wxALIGN_BOTTOM|wxEXPAND, 0);

    main_sizer->Show(gridsizer_mid1, false);
    main_sizer->Show(gridsizer_mid2, false);
    main_sizer->Show(gridsizer_mid3, false);
    SetAutoLayout(true);
    SetSizer(main_sizer);
    Layout();
    Centre();

}

wxString ItemPropDlg::GetTblName()
{
  wxString value = choice_1->GetStringSelection();

    return value;
}

void ItemPropDlg::OnOk(wxCommandEvent &event)
{

      EndModal(wxID_OK);
}

void ItemPropDlg::OnChoiceSel(wxCommandEvent &event)
{
   int  choice = choice_1->GetSelection();
  
   if (current==1) { main_sizer->Show(gridsizer_mid1, false); };
   if (current==2) { main_sizer->Show(gridsizer_mid2, false); };
   if (current==3) { main_sizer->Show(gridsizer_mid3, false); };

   if (choice==0) { current=0; this->SetSize(wxSize(340, 110)); Centre(); };
   if (choice==1) { main_sizer->Show(gridsizer_mid1, true); current=1; this->SetSize(wxSize(340, 175)); Centre(); };
   if (choice==2) { main_sizer->Show(gridsizer_mid2, true); current=2; this->SetSize(wxSize(340, 300)); Centre(); };
   if (choice==3) { main_sizer->Show(gridsizer_mid3, true); current=3; this->SetSize(wxSize(340, 350)); Centre(); };

   main_sizer->Layout ();


}

