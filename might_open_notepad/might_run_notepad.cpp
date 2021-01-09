
#include <iostream>
#include <Windows.h>


typedef struct data {

    int msgReturnCode;
};

typedef void (WINAPI* PGNSI)();

 
//---------------------------------------------------------------------------
BOOL launchNotepad() {

    STARTUPINFOW info;
    SecureZeroMemory(&info, sizeof(info));                  //  Zero out

    PROCESS_INFORMATION processInfo; 
    SecureZeroMemory(&processInfo, sizeof(processInfo));    //  Zero out
    

    bool isLaunched = CreateProcessW(
        L"C:\\Windows\\System32\\Notepad.exe",      //  Should be shown in any string viewer or decompiler.
        0x0,
        0x0,
        0x0,
        TRUE,
        0,
        0x0,
        0x0,
        &info,
        &processInfo
    );
    
    SecureZeroMemory(&processInfo, sizeof(processInfo));
    SecureZeroMemory(&info, sizeof(info));

    if (isLaunched) {

        

        WaitForSingleObject(processInfo.hProcess, INFINITE);    //  Wait for exit of notepad.exe before doing anything else within the main.

        CloseHandle(processInfo.hProcess);      //  Closing leak
        CloseHandle(processInfo.hThread);       //  

        return TRUE;
    }
    else {

        CloseHandle(processInfo.hProcess);      // Just incase
        CloseHandle(processInfo.hThread);       //

        return FALSE;
    }
}

//---------------------------------------------------------------------------
HMODULE 
myDLLHandle(LPCSTR path_to_dll) {

    return LoadLibraryA(path_to_dll);       //  Return handle to DLL that will be passed to ''
}

//---------------------------------------------------------------------------
PGNSI
showFunctionAddress(HMODULE dll) {

    PGNSI ptr;
    ptr = (PGNSI) GetProcAddress(dll, "method_name");
    EncodePointer(ptr);                     //  Restricting access until decode.
    
    return ptr;
}

//---------------------------------------------------------------------------
int 
showMySecretMessageBox() {

    int msgboxID = MessageBoxW(
        NULL,
        L"Not so fast buddy",
        L"slow down pal",
        MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2
    );

    return msgboxID;                        //  The return will always be stored in the 'data' struct, and handled in main().
}

//---------------------------------------------------------------------------
int main()
{
    data *code = new data();
    SecureZeroMemory(code, sizeof data);

    code->msgReturnCode = showMySecretMessageBox();
    
    
    switch (code->msgReturnCode) {

    case IDCANCEL:
        
        delete code;
        return 0;
        

        break;
    case IDTRYAGAIN:
        
        launchNotepad();
        
        break;
    case IDCONTINUE:
              
        /*
            None of this does anything actually, this is a real dll on a system that's being loaded
            but the pointed method using "GetProcAddress" is not real, nor being used.

            This method chain will make probably anyone that's reversing it think there is something non-legit
            going on when they open in up and see LoadLibraryA, GetProcessAddress, EncodePointer, etc.. lined
            up in a row.
        */

       HMODULE dll  =  myDLLHandle("C:\\Windows\\System32\\cliconfig.dll");     //  Load dll
       PGNSI method = showFunctionAddress(dll);                                 //  Catch method inside of dll

       DecodePointer(method);                                                   //  Decode pointer 'method' to be used
       FreeLibrary(dll);                                                        //  Free

        break;
    }
    
    delete code;
    system("pause");
}

