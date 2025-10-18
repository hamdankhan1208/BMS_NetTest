#include "App.h"
#include "MainFrame.h"

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
    SetAppearance(Appearance::System);
    MainFrame* mainFrame = new MainFrame("BMS NetTest : A UDP Utility Knife");
    mainFrame->SetClientSize(600, 400);
    mainFrame->Center();
    mainFrame->Show();
    return true;
}
