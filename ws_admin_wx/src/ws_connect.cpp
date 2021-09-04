#include "../include/wx_connect.h"

wxBEGIN_EVENT_TABLE(ConnectDlg, wxDialog)
EVT_CLOSE(ConnectDlg::OnClose)
wxEND_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////
inline void wxModifyStyle(wxWindow* wnd, long remove, long add)
{
    // Copied from http://wxforum.shadonet.com/viewtopic.php?t=17926
    long dw = wnd->GetWindowStyleFlag();
    dw &= ~remove;
    dw |= add;
    wnd->SetWindowStyleFlag(dw);
}

inline void wxTextCtrl_SetPasswordFlag(wxTextCtrl* win, bool active)
{
#ifdef __WXMSW__
    HWND hwnd = (HWND)win->GetHandle();
#if wxUSE_UNICODE
    static wchar_t password_char = 0x25CF;
    static bool initialized = false;
    if (win->HasFlag(wxTE_PASSWORD) && !initialized)
    {
        password_char = (wchar_t)SendMessage(hwnd, EM_GETPASSWORDCHAR, 0, 0);
        initialized = true;
    }
#else
    const char password_char = '*';
#endif
    SendMessage(hwnd, EM_SETPASSWORDCHAR, active ? password_char : 0, 0);
#endif
    wxModifyStyle(win, active ? 0 : wxTE_PASSWORD, active ? wxTE_PASSWORD : 0);
    win->Refresh();
}

ConnectDlg::ConnectDlg(wxWindow* parent, srv_info *info, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
    srvInfo = info;

	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* main_sizer;
    main_sizer = new wxBoxSizer(wxVERTICAL);

	const int border = wxSizerFlags::GetDefaultBorder();
//	wxFlexGridSizer* grid_sizer = new wxFlexGridSizer(2, wxSize(border, border));

    wxGridSizer* grid_sizer;
    grid_sizer = new wxGridSizer(4, 2, 0, 0);

	m_staticServerName = new wxStaticText(this, wxID_ANY, wxT("Сервер:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticServerName->Wrap(-1);
	grid_sizer->Add(m_staticServerName, 0, wxALL, 5);

	m_textCtrlServer = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	grid_sizer->Add(m_textCtrlServer, 0, wxALL | wxEXPAND, 5);

	m_staticTextPort = new wxStaticText(this, wxID_ANY, wxT("Порт:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticTextPort->Wrap(-1);
	grid_sizer->Add(m_staticTextPort, 0, wxALL, 5);

	m_textCtrlPort = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	grid_sizer->Add(m_textCtrlPort, 0, wxALL | wxEXPAND, 5);

	m_staticUser = new wxStaticText(this, wxID_ANY, wxT("Пользователь:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticUser->Wrap(-1);
	grid_sizer->Add(m_staticUser, 0, wxALL, 5);

	m_textCtrlUserName = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_textCtrlUserName->SetToolTip(wxT("Введите имя пользователя"));

	grid_sizer->Add(m_textCtrlUserName, 0, wxALL | wxEXPAND, 5);

	m_staticPassword = new wxStaticText(this, wxID_ANY, wxT("Пароль:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticPassword->Wrap(-1);
	grid_sizer->Add(m_staticPassword, 0, wxALL, 5);

	m_textCtrlPassword = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_textCtrlPassword->SetToolTip(wxT("Введите пароль"));

	grid_sizer->Add(m_textCtrlPassword, 0, wxALL | wxEXPAND, 5);


	main_sizer->Add(grid_sizer, 1, wxEXPAND, 5);

/*	wxBoxSizer* bSizerSep;
	bSizerSep = new wxBoxSizer(wxVERTICAL);

	m_separator = new wxStaticLine(this, wxID_ANY, wxPoint(-1, -1), wxSize(-1, -1), wxLI_HORIZONTAL);
	bSizerSep->Add(m_separator, 0, wxEXPAND | wxALL, 5);


	main_sizer->Add(bSizerSep, 1, wxEXPAND | wxTOP, 5);

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxHORIZONTAL);

	m_btnOk = new wxButton(this, wxID_ANY, wxT("&ОК"), wxDefaultPosition, wxDefaultSize, 0);

	m_btnOk->SetBitmapPosition(wxRIGHT);
	bSizer4->Add(m_btnOk, 0, wxALL, 5);

	m_btnCancel = new wxButton(this, wxID_ANY, wxT("&Отмена"), wxDefaultPosition, wxDefaultSize, 0);

	m_btnCancel->SetBitmapPosition(wxRIGHT);
	bSizer4->Add(m_btnCancel, 0, wxALL, 5);*/


	//main_sizer->Add(bSizer4, 1, wxALIGN_RIGHT, 5);


	wxSizer* button_sizer = CreateSeparatedButtonSizer(wxCANCEL | wxOK );
	if (button_sizer)
	    main_sizer->Add(button_sizer, wxSizerFlags().Expand().Border());

	//SetSizerAndFit(main_sizer);


	this->SetSizer(main_sizer);
	this->Layout();

	this->Centre(wxBOTH);

	wxTextCtrl_SetPasswordFlag(m_textCtrlPassword, true);

	// Connect Events
	//m_btnOk->Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(ConnectDlg::onOkClick), NULL, this);
}

ConnectDlg::~ConnectDlg()
{
	// Disconnect Events
	//m_btnOk->Disconnect(wxEVT_RIGHT_UP, wxMouseEventHandler(ConnectDlg::onOkClick), NULL, this);



}

void ConnectDlg::OnClose(wxCloseEvent& event)
{

    if ( event.CanVeto() )
    {
//        wxMessageBox("Use the menu item to close this dialog",
//                     "Modeless dialog",
//                     wxOK | wxICON_INFORMATION, this);

        event.Veto();
    }
}

void ConnectDlg::updateSettings() {
    if (srvInfo){
        srvInfo->host = m_textCtrlServer->GetValue();
        srvInfo->port = m_textCtrlPort->GetValue();
    }
}