#include "App.h"
#include "GUI.h"
#include <wx/wx.h>

wxIMPLEMENT_APP(App);

bool App::OnInit() {

	GUI* Frame = new GUI("NDVI APPLICATION");
	Frame->SetClientSize(800, 600);
	Frame->Center();
	Frame->Show();
	
	return true;
}
