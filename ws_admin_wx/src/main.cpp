/***************************************************************
 * Name:      testMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    arcady ()
 * Created:   2021-08-03
 * Copyright: arcady ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "../include/main.h"
#include "../include/wx_addserver.h"
#include "../include/wx_connect.h"



//helper functions
enum wxBuildInoFormat {
    short_f, long_f };

wxString wxBuildInfo(wxBuildInoFormat format)
{
    wxString wxBuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxBuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxBuild << _T("-Mac");
#elif defined(__UNIX__)
        wxBuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxBuild << _T("-Unicode build");
#else
        wxBuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxBuild;
}



main_frame::main_frame(wxFrame *frame)
: Frame(frame)
{
#if wxUSE_STATUSBAR
    statusBar->SetStatusText(_("Hello admin console!"), 0);
    statusBar->SetStatusText(wxBuildInfo(short_f), 1);
#endif

    //setlocale(LC_ALL, "Russian");

    conf = new settings();
    conf->get_settings();
}

main_frame::~main_frame(){
    conf->save_settings();
}


void main_frame::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void main_frame::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void main_frame::OnAbout(wxCommandEvent &event)
{
    wxString msg = wxBuildInfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void main_frame::OnAddServer(wxCommandEvent& event)
{
    auto * info = new srv_info();

    auto *dlg = new ConnectDlg(this, info);

    //addServerDlg dlg(this);
    int result = dlg->ShowModal();

    if (result == wxID_OK){
        std::cout << "result::OK" << std::endl;
        dlg->updateSettings();
    }


    dlg->Destroy();
}

