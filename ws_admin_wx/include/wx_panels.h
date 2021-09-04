//
// Created by arcady on 04.08.2021.
//

#ifndef WS_SOLUTION_WX_PANELS_H
#define WS_SOLUTION_WX_PANELS_H

#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/sizer.h>


class PanelTree : public wxPanel
{
public:
    PanelTree(wxPanel *parent);

    void OnPlus(wxCommandEvent & event);
    void OnMinus(wxCommandEvent & event);


};

class PanelList : public wxPanel
{
public:
    PanelList(wxPanel *parent);

    void OnSetText(wxCommandEvent & event);

    wxStaticText *m_text;

};

class ScrolledWindow: public wxScrolledWindow
{
public:
    ScrolledWindow(wxWindow* parent, bool mirror);
    virtual ~ScrolledWindow(){}

    virtual void OnDraw(wxDC& dc) wxOVERRIDE;

private:
    bool m_mirror;

    wxDECLARE_NO_COPY_CLASS(ScrolledWindow);
};

class SplitterWindow : public wxSplitterWindow
{
public:
    explicit SplitterWindow(wxFrame *parent);

    // event handlers
    void OnPositionChanged(wxSplitterEvent& event);
    void OnPositionChanging(wxSplitterEvent& event);
    void OnDClick(wxSplitterEvent& event);
    void OnUnSplitEvent(wxSplitterEvent& event);

private:
    wxFrame *m_frame;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(SplitterWindow);
};
#endif //WS_SOLUTION_WX_PANELS_H
