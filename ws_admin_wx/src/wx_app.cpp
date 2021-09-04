//
// Created by arcady on 03.08.2021.
//
#ifdef WX_PRECOMP
#include "../include/wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "../include/wx_app.h"
#include "../include/main.h"

IMPLEMENT_APP(ws_app);

bool ws_app::OnInit()
{
    main_frame* frame = new main_frame(0L);

    frame->Show();

    return true;
}

