//
// CLASS App
// App.C
//

//
// Date: 2008-06-18
//


#include "App.H"
#include "Kernel.H"
#include "PreProcessorEx.H"
#include "EigenMethod.H"

App::App()
{}


App::~App()
{}


void App::OnInit(Kernel* pKernal)
{
	pKernal->Register(new ImgScaler());
	pKernal->Register(new Equalizer());
	pKernal->Register(new EigenMethod());
}

void App::OnCleanup()
{}
