#ifndef HDVS_H
#define HDVS_H

#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/menu.h>

namespace hdvs {
class hdvs : public wxFrame
{
public:
    hdvs();

private:

    wxPanel* m_Panel;
    wxSplitterWindow* m_Splitter;

    wxMenuBar* m_MenuBar;

    // temp
    wxPanel *m_P1;
    wxPanel *m_P2;
};

}

#endif
