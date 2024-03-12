#include "HDVS/hdvs.h"

#define CLIENT_WIDTH 640
#define CLIENT_HEIGHT 480

namespace hdvs {

hdvs::hdvs():
    wxFrame(nullptr, wxID_ANY, "HDVS")
{
    SetClientSize(CLIENT_WIDTH, CLIENT_HEIGHT);

    m_Panel = new wxPanel(this);

    m_Splitter = new wxSplitterWindow(m_Panel, wxID_ANY, {0, 0}, {CLIENT_WIDTH, CLIENT_HEIGHT});

    m_P1 = new wxPanel(m_Splitter);
    m_P1->SetBackgroundColour(wxTheColourDatabase->Find("Red"));

    m_P2 = new wxPanel(m_Splitter);
    m_P2->SetBackgroundColour(wxTheColourDatabase->Find("Green"));

    m_Splitter->SetSplitMode(wxSPLIT_VERTICAL);
    m_Splitter->SplitVertically(m_P1, m_P2);

    m_MenuBar = new wxMenuBar();
    SetMenuBar(m_MenuBar);

    CreateStatusBar();
    SetStatusText("Idle");
}

}
