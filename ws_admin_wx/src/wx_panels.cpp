//
// Created by arcady on 04.08.2021.
//

#include "../include/wx_panels.h"

// ----------------------------------------------------------------------------
// MySplitterWindow
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(SplitterWindow, wxSplitterWindow)
EVT_SPLITTER_SASH_POS_CHANGED(wxID_ANY, SplitterWindow::OnPositionChanged)
EVT_SPLITTER_SASH_POS_CHANGING(wxID_ANY, SplitterWindow::OnPositionChanging)

EVT_SPLITTER_DCLICK(wxID_ANY, SplitterWindow::OnDClick)

EVT_SPLITTER_UNSPLIT(wxID_ANY, SplitterWindow::OnUnSplitEvent)
wxEND_EVENT_TABLE()

SplitterWindow::SplitterWindow(wxFrame *parent)
: wxSplitterWindow(parent, wxID_ANY,
                   wxDefaultPosition, wxDefaultSize,
                   wxSP_3D | wxSP_LIVE_UPDATE |
                   wxCLIP_CHILDREN /* | wxSP_NO_XP_THEME */ )
{
    m_frame = parent;
}

void SplitterWindow::OnPositionChanged(wxSplitterEvent& event)
{
    wxLogStatus(m_frame, "Position has changed, now = %d (or %d)",
                event.GetSashPosition(), GetSashPosition());

    event.Skip();
}

void SplitterWindow::OnPositionChanging(wxSplitterEvent& event)
{
    wxLogStatus(m_frame, "Position is changing, now = %d (or %d)",
                event.GetSashPosition(), GetSashPosition());

    event.Skip();
}

void SplitterWindow::OnDClick(wxSplitterEvent& event)
{
#if wxUSE_STATUSBAR
    m_frame->SetStatusText("Splitter double clicked", 1);
#endif // wxUSE_STATUSBAR

    event.Skip();
}

void SplitterWindow::OnUnSplitEvent(wxSplitterEvent& event)
{
#if wxUSE_STATUSBAR
    m_frame->SetStatusText("Splitter unsplit", 1);
#endif // wxUSE_STATUSBAR

    event.Skip();
}

// ----------------------------------------------------------------------------
// ScrolledWindow
// ----------------------------------------------------------------------------

ScrolledWindow::ScrolledWindow(wxWindow* parent, bool mirror)
: wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
wxHSCROLL | wxVSCROLL)
{
    m_mirror = mirror;
    SetScrollbars(20, 20, 5, 5);
}

void ScrolledWindow::OnDraw(wxDC& dcOrig)
{
/*    wxMirrorDC dc(dcOrig, m_mirror);

    dc.SetPen(*wxBLACK_PEN);
    dc.DrawLine(0, 0, 100, 200);

    dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
    dc.DrawText("Testing", 50, 50);

    dc.SetPen(*wxRED_PEN);
    dc.SetBrush(*wxGREEN_BRUSH);
    dc.DrawRectangle(120, 120, 100, 80);*/
}
