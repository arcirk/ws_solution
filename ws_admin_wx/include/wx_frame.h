//
// Created by arcady on 03.08.2021.
//

#ifndef WS_SOLUTION_FRAME_H
#define WS_SOLUTION_FRAME_H
#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#include <wx/menu.h>
#include "wx/imaglist.h"
#include "wx/treectrl.h"
#include "wx/listctrl.h"
#include "wx_panels.h"


///////////////////////////////////////////////////////////////////////////

#define idMenuQuit 1000
#define idMenuAbout 1001
#define idAddServer 1002

    enum
    {
        TreeCtrlIcon_File,
        TreeCtrlIcon_FileSelected,
        TreeCtrlIcon_Folder,
        TreeCtrlIcon_FolderSelected,
        TreeCtrlIcon_FolderOpened
    };
///////////////////////////////////////////////////////////////////////////////
/// Class GUIFrame
///////////////////////////////////////////////////////////////////////////////
class Frame : public wxFrame
{
DECLARE_EVENT_TABLE()
private:


    // Private event handlers
    void _wxFB_OnClose( wxCloseEvent& event ){ OnClose( event ); }
    void _wxFB_OnQuit( wxCommandEvent& event ){ OnQuit( event ); }
    void _wxFB_OnAbout( wxCommandEvent& event ){ OnAbout( event ); }
    void _wxFB_OnAddServer(wxCommandEvent& event) { OnAddServer(event); }
    
    void CreateImageList(int size);

protected:
    wxMenuBar* menuBar;
    wxStatusBar* statusBar;

    wxWindow *m_left, *m_right;
    wxSplitterWindow* m_splitter;

    wxTreeCtrl* m_treeCtrl;
    wxListCtrl* m_listCtrl;
    wxImageList* m_imageList;


    // Virtual event handlers, override them in your derived class
    virtual void OnClose( wxCloseEvent& event ){ event.Skip(); }
    virtual void OnQuit( wxCommandEvent& event ){ event.Skip(); }
    virtual void OnAbout( wxCommandEvent& event ){ event.Skip(); }
    virtual void OnAddServer( wxCommandEvent& event ){ event.Skip(); }


public:
    Frame( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("WebSocketServer: Консоль администрирования"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 481,466 ), int style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
    
    void OnSize(wxSizeEvent& event);

    void CreateTree(long style);
    void CreateList();
    void Resize();
};

#endif //WS_SOLUTION_FRAME_H
