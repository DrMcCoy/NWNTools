

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/spinctrl.h>


#ifndef ITEMPROPDLG_H
#define ITEMPROPDLG_H

enum
{

};


class ItemPropDlg: public wxDialog {
public:



    ItemPropDlg(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);

private:

    void set_properties();
    void do_layout();


    void OnOk(wxCommandEvent &event);


protected:

    wxStaticText* label_1;
    wxStaticText* label_2;
    wxComboBox* combo_1;
    wxSpinCtrl* spin_1;
    wxButton* button_1;
    wxButton* button_2;
    wxBoxSizer* main_sizer;
    wxGridSizer* gridsizer_top;
    wxBoxSizer* button_sizer;




private:
	DECLARE_EVENT_TABLE()

};



#endif // ITEMPROPDLG_H
