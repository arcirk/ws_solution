//
// Created by arcady on 09.08.2021.
//

#include "../include/wx_addserver.h"

#define ID_CATCH_LISTBOX_DCLICK 100
#define ID_LISTBOX              101
#define ID_DISABLE_OK           102
#define ID_DISABLE_CANCEL       103

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

addServerDlg::addServerDlg(wxWindow *parent)
: wxDialog(parent, wxID_ANY, wxString("Добавить сервер"))
{
    setlocale(LC_ALL, "Russian");

	wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);

	const int border = wxSizerFlags::GetDefaultBorder();
	wxFlexGridSizer* grid_sizer = new wxFlexGridSizer(2, wxSize(border, border));

    
    //grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Адрес сервера:"),
    //                wxSizerFlags().CentreVertical());
    grid_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Сервер:"), wxDefaultPosition, wxDefaultSize, 0));
    grid_sizer->Add(new wxTextCtrl(this, wxID_ANY, "0.0.0.0"),
        wxSizerFlags().Expand().CentreVertical());
    grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Порт:"),
        wxSizerFlags().CentreVertical());
    grid_sizer->Add(new wxTextCtrl(this, wxID_ANY, "8080"),
        wxSizerFlags().Expand().CentreVertical());
    grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Пользователь:"),
        wxSizerFlags().CentreVertical());
    grid_sizer->Add(new wxTextCtrl(this, wxID_ANY, "admin"),
        wxSizerFlags().Expand().CentreVertical());
    grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Пароль:"),
        wxSizerFlags().CentreVertical());
    
    m_pwdText = new wxTextCtrl(this, wxID_ANY, "");

    grid_sizer->Add(m_pwdText,
        wxSizerFlags().Expand().CentreVertical());


    main_sizer->Add(grid_sizer, wxSizerFlags().DoubleBorder());

    wxSizer* button_sizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
    if (button_sizer)
        main_sizer->Add(button_sizer, wxSizerFlags().Expand().Border());

    SetSizerAndFit(main_sizer);

    wxTextCtrl_SetPasswordFlag(m_pwdText, true);
}