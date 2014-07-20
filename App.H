//
// HEADER App 
// App.H
//

//
// Date: 2008-06-18
//


#ifndef APP_HEADER
#define APP_HEADER

#include <stdio.h>

class Kernel;

class App
{

public:
	App();
	~App();
	void OnInit(Kernel* pKernal);
	void OnCleanup();

private:

};

#endif // APP_HEADER


