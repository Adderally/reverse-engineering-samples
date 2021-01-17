

#include <iostream>
#include <thread>
#include <Windows.h>





enum DBG_CODES 
{
	DBG_PED = 0x0001,	//	Debugger started process, in PED block.
	DBG_FND = 0x0002	//	Found debugger.
};





//---------------------------------------------------------------------------
void reportError(DWORD err) {


	if (err != 0) {

		LPSTR returnMessageBuff = nullptr;

		size_t size = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			err,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&returnMessageBuff,
			0,
			NULL
		);

		std::string errorMessage(returnMessageBuff, size);
		LocalFree(returnMessageBuff);

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
		std::cout << "\n[System]\t";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		std::cout << errorMessage << std::endl;
	}
}


//---------------------------------------------------------------------------
void
DEBUG_OUTPUT(char* msg) {

	MessageBoxA(NULL, msg, "DEBUGGER FOUND!", 0);
}




DWORD WINAPI
checkIfDbger(void* param)
{
	while (1)
	{
		if (IsDebuggerPresent()) 
		{
			DEBUG_OUTPUT((char*)"Debugger attached!");
			return DBG_FND;
		}
		else 
		{
			DWORD rtn_code = ::GetLastError();
			reportError(rtn_code);
		}
			

		Sleep(2000);
	}
}


//---------------------------------------------------------------------------
bool 
startedUnderDbger(void) {

	BOOL found = FALSE;

	_asm	//	Checking the PEB for debug flags
			//	Another way of checking of the application was ran with a debugger!

	{
		xor eax, eax;			
		mov eax, fs: [0x30] ;	//	OffSet to PEB
		mov eax, [eax + 0x02];	//  +2 
		and eax, 0x000000FF;	
		mov found, eax;			
	}

	return found;
}



//---------------------------------------------------------------------------
int main()
{
	
	if (startedUnderDbger())
	{
		DEBUG_OUTPUT((char*)"Started under a debugger!");
		return DBG_PED;		
	}
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
	std::cout << "\n[System]\t";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	std::cout << "Did not detect a debugger startup enviroment!\n" << std::endl;

	DWORD err;
	

	for (int i = 0; i <= 100; i++) 
	{
		std::cout << i << std::endl;

		if (i == 30)
		{
			
			
			

			HANDLE start_CheckForDbger = CreateThread(	//	At 30 we launch a thread that will search the process for a debugger.
														//	This is a continuous search. Refer: checkIfDbger()
				NULL,
				0,
				checkIfDbger,
				NULL,
				0,
				NULL
			);

			if (start_CheckForDbger == NULL) 
			{
				err = ::GetLastError();
				reportError(err);
			}



			/* Handle how you wish ... */


		}

		Sleep(1250);
	}
}


