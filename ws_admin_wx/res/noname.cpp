///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "noname.h"
#include "../include/wx_connect.h"


///////////////////////////////////////////////////////////////////////////

ConnectDlg::ConnectDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	wxGridSizer* gSizer3;
	gSizer3 = new wxGridSizer( 3, 2, 0, 0 );

	m_staticServerName = new wxStaticText( this, wxID_ANY, wxT("Сервер:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticServerName->Wrap( -1 );
	gSizer3->Add( m_staticServerName, 0, wxALL, 5 );

	m_textCtrlServer = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_textCtrlServer, 0, wxALL|wxEXPAND, 5 );

	m_staticTextPort = new wxStaticText( this, wxID_ANY, wxT("Порт:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextPort->Wrap( -1 );
	gSizer3->Add( m_staticTextPort, 0, wxALL, 5 );

	m_textCtrlPort = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_textCtrlPort, 0, wxALL|wxEXPAND, 5 );

	m_staticUser = new wxStaticText( this, wxID_ANY, wxT("Пользователь:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticUser->Wrap( -1 );
	gSizer3->Add( m_staticUser, 0, wxALL, 5 );

	m_textCtrlUserName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrlUserName->SetToolTip( wxT("Введите имя пользователя") );

	gSizer3->Add( m_textCtrlUserName, 0, wxALL|wxEXPAND, 5 );

	m_staticPassword = new wxStaticText( this, wxID_ANY, wxT("Пароль:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticPassword->Wrap( -1 );
	gSizer3->Add( m_staticPassword, 0, wxALL, 5 );

	m_textCtrlPassword = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrlPassword->SetToolTip( wxT("Введите пароль") );

	gSizer3->Add( m_textCtrlPassword, 0, wxALL|wxEXPAND, 5 );


	bSizer2->Add( gSizer3, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizerSep;
	bSizerSep = new wxBoxSizer( wxVERTICAL );

	m_separator = new wxStaticLine( this, wxID_ANY, wxPoint( -1,-1 ), wxSize( -1,-1 ), wxLI_HORIZONTAL );
	bSizerSep->Add( m_separator, 0, wxEXPAND | wxALL, 5 );


	bSizer2->Add( bSizerSep, 1, wxEXPAND|wxTOP, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	m_btnOk = new wxButton( this, wxID_ANY, wxT("&ОК"), wxDefaultPosition, wxDefaultSize, 0 );

	m_btnOk->SetBitmapPosition( wxRIGHT );
	bSizer4->Add( m_btnOk, 0, wxALL, 5 );

	m_btnCancel = new wxButton( this, wxID_ANY, wxT("&Отмена"), wxDefaultPosition, wxDefaultSize, 0 );

	m_btnCancel->SetBitmapPosition( wxRIGHT );
	bSizer4->Add( m_btnCancel, 0, wxALL, 5 );


	bSizer2->Add( bSizer4, 1, wxALIGN_RIGHT, 5 );


	this->SetSizer( bSizer2 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_btnOk->Connect( wxEVT_RIGHT_UP, wxMouseEventHandler( ConnectDlg::onOkClick ), NULL, this );
}

ConnectDlg::~ConnectDlg()
{
	// Disconnect Events
	m_btnOk->Disconnect( wxEVT_RIGHT_UP, wxMouseEventHandler( ConnectDlg::onOkClick ), NULL, this );

}

