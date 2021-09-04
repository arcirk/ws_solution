//
// Created by arcady on 03.08.2021.
//

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "../include/wx_frame.h"

#include "../res/icon1.xpm"
#include "../res/icon2.xpm"
#include "../res/icon3.xpm"
#include "../res/icon4.xpm"
#include "../res/icon5.xpm"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( Frame, wxFrame )
EVT_SIZE(Frame::OnSize)
EVT_CLOSE( Frame::_wxFB_OnClose )
EVT_MENU( idMenuQuit, Frame::_wxFB_OnQuit )
EVT_MENU( idMenuAbout, Frame::_wxFB_OnAbout )
EVT_MENU( idAddServer, Frame::_wxFB_OnAddServer)
END_EVENT_TABLE()

Frame::Frame( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxFrame( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    this->SetSize(800, 600);

    menuBar = new wxMenuBar(0 );
    wxMenu* fileMenu;
    fileMenu = new wxMenu();
    
    wxMenuItem* menuAddServer = new wxMenuItem(fileMenu, idAddServer, wxString(wxT("&Добавить сервер")), wxT("Добавить сервер"), wxITEM_NORMAL);
    fileMenu->Append(menuAddServer);
    wxMenuItem* menuFileQuit = new wxMenuItem( fileMenu, idMenuQuit, wxString( wxT("&Выход") ) + wxT('\t') + wxT("Alt+F4"), wxT("Выйти из приложения"), wxITEM_NORMAL );
    fileMenu->Append( menuFileQuit );
    menuBar->Append(fileMenu, wxT("&Файл") );

    wxMenu* helpMenu;
    helpMenu = new wxMenu();
    wxMenuItem* menuHelpAbout = new wxMenuItem( helpMenu, idMenuAbout, wxString( wxT("&О программе") ) + wxT('\t') + wxT("F1"), wxT("Показать диалог о программе"), wxITEM_NORMAL );
    helpMenu->Append( menuHelpAbout );
    menuBar->Append(helpMenu, wxT("&Справка") );
    this->SetMenuBar(menuBar );

    m_splitter = new SplitterWindow(this);

    // If you use non-zero gravity you must initialize the splitter with its
    // correct initial size, otherwise it will change the sash position by a
    // huge amount when it's resized from its initial default size to its real
    // size when the frame lays it out. This wouldn't be necessary if default
    // zero gravity were used (although it would do no harm neither).
    m_splitter->SetSize(GetClientSize());
    m_splitter->SetSashGravity(1.0);

    long _style = wxTR_DEFAULT_STYLE |
#ifndef NO_VARIABLE_HEIGHT
        wxTR_HAS_VARIABLE_ROW_HEIGHT 
#endif
        ; //|wxTR_EDIT_LABELS

    CreateTree(_style | wxSUNKEN_BORDER);
    CreateList();

    m_splitter->SplitVertically(m_treeCtrl, m_listCtrl, 200);

    statusBar = this->CreateStatusBar( 2, wxST_SIZEGRIP, wxID_ANY );
}

void Frame::CreateTree(long _style)
{

    m_treeCtrl = new wxTreeCtrl(m_splitter, -1,
        wxDefaultPosition, wxDefaultSize,
        _style);

    CreateImageList(16);

    m_treeCtrl->AddRoot("Console root", TreeCtrlIcon_Folder);


    //Resize();
}

void Frame::CreateList()
{

    m_listCtrl = new wxListCtrl(m_splitter, -1,
        wxDefaultPosition, wxDefaultSize,
        wxLC_LIST | wxBORDER_THEME | wxLC_EDIT_LABELS);
    //GetMenuBar()->Enable(TreeTest_SelectRoot, !(style & wxTR_HIDE_ROOT));

    //Resize();
}

void Frame::Resize()
{
    //wxSize size = m_left->GetSize();
    //m_treeCtrl->SetSize(0, 0, size.x, size.y
    //    ); //*2 / 3
//#if wxUSE_LOG
    //m_textCtrl->SetSize(0, 2 * size.y / 3, size.x, size.y / 3
//#endif
//    );
}

void Frame::OnSize(wxSizeEvent& event)
{
    if (m_treeCtrl )
    {
        //Resize();
    }

    event.Skip();
}

void Frame::CreateImageList(int size)
{
    if (size == -1)
    {
        m_treeCtrl->SetImageList(NULL);
        return;
    }
//    if (size == 0)
//        size = m_imageSize;
//    else
//        m_imageSize = size;

    // Make an image list containing small icons
    wxImageList* images = new wxImageList(size, size, true);

    // should correspond to TreeCtrlIcon_xxx enum
    wxBusyCursor wait;
    wxIcon icons[5];

    //if (m_alternateImages)
    //{
        icons[TreeCtrlIcon_File] = wxIcon(icon1_xpm);
        icons[TreeCtrlIcon_FileSelected] = wxIcon(icon2_xpm);
        icons[TreeCtrlIcon_Folder] = wxIcon(icon3_xpm);
        icons[TreeCtrlIcon_FolderSelected] = wxIcon(icon4_xpm);
        icons[TreeCtrlIcon_FolderOpened] = wxIcon(icon5_xpm);
    //}
    //else
    //{
    //    wxSize iconSize(size, size);

    //    icons[TreeCtrlIcon_File] =
    //        icons[TreeCtrlIcon_FileSelected] = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_LIST, iconSize);
    //    icons[TreeCtrlIcon_Folder] =
    //        icons[TreeCtrlIcon_FolderSelected] =
    //        icons[TreeCtrlIcon_FolderOpened] = wxArtProvider::GetIcon(wxART_FOLDER, wxART_LIST, iconSize);
    //}

    for (size_t i = 0; i < WXSIZEOF(icons); i++)
    {
        int sizeOrig = icons[0].GetWidth();
        if (size == sizeOrig)
        {
            images->Add(icons[i]);
        }
        else
        {
            images->Add(wxBitmap(wxBitmap(icons[i]).ConvertToImage().Rescale(size, size)));
        }
    }

    m_treeCtrl->AssignImageList(images);
}