//
// Created by arcady on 03.08.2021.
//

#ifndef WS_SOLUTION_MAIN_H
#define WS_SOLUTION_MAIN_H

#include "wx_app.h"
#include "wx_frame.h"
#include "settings.h"

class main_frame: public Frame
{
public:
    explicit main_frame(wxFrame *frame);
    ~main_frame() override;
private:
    virtual void OnClose(wxCloseEvent& event);
    virtual void OnQuit(wxCommandEvent& event);
    virtual void OnAbout(wxCommandEvent& event);
    virtual void OnAddServer(wxCommandEvent& event);

    settings *conf;
};
#endif //WS_SOLUTION_MAIN_H
