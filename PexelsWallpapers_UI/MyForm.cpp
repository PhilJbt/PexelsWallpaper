#include "MyForm.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThreadAttribute]
void Main(array<String^>^ args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	PexelsWallpapers_UI::MyForm form;
		//form.BackColor = System::Drawing::Color::White;
		//form.TransparencyKey = System::Drawing::Color::Transparent;
		//form.TopMost = true;

	Application::Run(%form);
}