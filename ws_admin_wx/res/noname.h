///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ConnectDlg
///////////////////////////////////////////////////////////////////////////////
class ConnectDlg : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticServerName;
		wxTextCtrl* m_textCtrlServer;
		wxStaticText* m_staticTextPort;
		wxTextCtrl* m_textCtrlPort;
		wxStaticText* m_staticUser;
		wxTextCtrl* m_textCtrlUserName;
		wxStaticText* m_staticPassword;
		wxTextCtrl* m_textCtrlPassword;
		wxStaticLine* m_separator;
		wxButton* m_btnOk;
		wxButton* m_btnCancel;

		// Virtual event handlers, overide them in your derived class
		virtual void onOkClick( wxMouseEvent& event ) { event.Skip(); }


	public:

		ConnectDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Добавить сервер"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 343,224 ), long style = wxDEFAULT_DIALOG_STYLE );
		~ConnectDlg();

};

