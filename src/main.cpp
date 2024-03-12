#include "HDVS/hdvs.h"

#include <wx/wx.h>

class MyApp : public wxApp
{
public:
    bool OnInit() override
    {
        hdvs::hdvs* frame = new hdvs::hdvs();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
