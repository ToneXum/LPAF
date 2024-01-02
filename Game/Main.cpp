#include "Framework.hpp"
#include <exception>
#include <ostream>
#include <string>

bool OpenAnotherWindow()
{
    if (tsd::MessageBox(0, L"Are you sure about that?", L"Are you sure you want to close this window?", (int)tsd::MBF::TASKMODAL | (int)tsd::MBF::BUTTON_YES_NO | (int)tsd::MBF::ICON_QUESTION) == tsd::MBR::NO)
        return false;

    if (tsd::MessageBox(0, L"Are you sure about that?", L"Are you realy sure?", (int)tsd::MBF::TASKMODAL | (int)tsd::MBF::BUTTON_YES_NO | (int)tsd::MBF::ICON_QUESTION) == tsd::MBR::NO)
        return false;

    if (tsd::MessageBox(0, L"Are you sure about that?", L"Like are you realy realy?", (int)tsd::MBF::TASKMODAL | (int)tsd::MBF::BUTTON_YES_NO | (int)tsd::MBF::ICON_QUESTION) == tsd::MBR::NO)
        return false;

    if (tsd::MessageBox(0, L"Are you sure about that?", L"Like realy realy realy realy realy realy?", (int)tsd::MBF::TASKMODAL | (int)tsd::MBF::BUTTON_YES_NO | (int)tsd::MBF::ICON_QUESTION) == tsd::MBR::NO)
        return false;

    if (tsd::MessageBox(0, L"Are you sure about that?", L"Like realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realyrealy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy realy?", (int)tsd::MBF::TASKMODAL | (int)tsd::MBF::BUTTON_YES_NO | (int)tsd::MBF::ICON_QUESTION) == tsd::MBR::NO)
        return false;

    tsd::MessageBox(0, L"You won", L"Ok fine, Ill close it for you ...", (int)tsd::MBF::TASKMODAL | (int)tsd::MBF::BUTTON_OK | (int)tsd::MBF::ICON_INFO);
    return true;
}

void DoSomething()
{
    if (tsd::MessageBox(0, L"Done", L"The window is now gone, are you happy?", (int)tsd::MBF::ICON_QUESTION | (int)tsd::MBF::BUTTON_YES_NO | (int)tsd::MBF::TASKMODAL) == tsd::MBR::NO)
        tsd::MessageBox(0, L"TF?", L"Whatever ...", (int)tsd::MBF::ICON_QUESTION | (int)tsd::MBF::BUTTON_OK | (int)tsd::MBF::TASKMODAL);
}

// This is a simple test program to try out and find bugs in GAFW
int main()
{
    TSD_CALL(tsd::Initialise(0, 105), true);

    short windowHandle = 0;
    TSD_CALL_RET(windowHandle, tsd::CreateWindow(L"Boring Box", 500, 500), true);

    while (tsd::Running())
    {
        //TSD_CALL(tsd::CreateWindow(L"Heheheha", 500, 500), true);
        // simulate computation
        tsd::Halt(16);
    }

    tsd::Uninitialise();
    return 0;
}