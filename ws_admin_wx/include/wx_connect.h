#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/dialog.h>

#include "settings.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ConnectDlg
///////////////////////////////////////////////////////////////////////////////
class ConnectDlg : public wxDialog
{

protected:
	wxStaticText* m_staticServerName;
	wxTextCtrl* m_textCtrlServer;
	wxStaticText* m_staticTextPort;
	wxTextCtrl* m_textCtrlPort;
	wxStaticText* m_staticUser;
	wxTextCtrl* m_textCtrlUserName;
	wxStaticText* m_staticPassword;
	wxTextCtrl* m_textCtrlPassword;
	//wxStaticLine* m_separator;
	//wxButton* m_btnOk;
	//wxButton* m_btnCancel;

	// Virtual event handlers, overide them in your derived class
	virtual void onOkClick(wxMouseEvent& event) { event.Skip(); }


public:

    ConnectDlg(wxWindow* parent, srv_info *info, wxWindowID id = wxID_ANY, const wxString& title = wxT("Добавить сервер"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(319, 224), long style = wxDEFAULT_DIALOG_STYLE);

	~ConnectDlg();

	void OnClose(wxCloseEvent& event);

	void updateSettings();

private:
    srv_info *srvInfo;
    wxDECLARE_EVENT_TABLE();

};
