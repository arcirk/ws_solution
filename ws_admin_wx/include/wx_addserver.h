#pragma once

#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

class addServerDlg : public wxDialog
{
public:
    addServerDlg(wxWindow* parent);

private:

    wxTextCtrl* m_pwdText;

    //wxDECLARE_EVENT_TABLE();
    //void _wxFB_ShowModal(){ OnClose(); }
};
