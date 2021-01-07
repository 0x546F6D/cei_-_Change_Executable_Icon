// # cei - Change Executable Icon

// Change Executable Icon for command line:
//   - Based on https://github.com/fatrolls/IconChanger
//   - Only the 1st image of the .ico file is added to the .exe
//   - Tested on Windows 10 Pro, build 19041.685, locale en-US

// Compiled with MSYS2/MinGW-w64:
//	$ gcc -o cei cei.c -s -O3 -Wl,--gc-sections -fno-asynchronous-unwind-tables -nostartfiles --entry=cei

// Usage:
//	> cei PATH\TO\ICO PATH\TO\EXE [PATH\TO\NEWEXEWITHICO]
//   - If [PATH\TO\NEWEXEWITHICO] is ommitted, cei.exe edits the original .exe located at PATH\TO\EXE
//   - If [PATH\TO\NEWEXEWITHICO] is passed, cei.exe makes a copy of the original .exe to PATH\TO\EXEWITHICO, and edits this copy

// Notes:
//   - !! Some executables wont work anymore after changing their icon.. !!
//   - ..So make a backup beforehand and/or pass [PATH\TO\NEWEXEWITHICO] as a 3rd argument

#include <windows.h>
#include <stdint.h>
// #include <stdio.h>

// ----------------------- Project Functions Prototype ------------------------ //
void CommandLineToArgvA(char* cpCmdLine, char** cpaArgs);						// Get arguments from command line.. just a personal preference for char* instead of the wchar_t*/LPWSTR type provided by "CommandLineToArgvW()"
void WriteToConsoleA(char* cpMsg);												// "Write to Console A" function to save >20KB compared to printf and <stdio.h>

// --------------------------- Functions Prototype ---------------------------- //
int access(const char* path, int mode);											// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/access-waccess?view=msvc-160
// void* __stdcall GetStdHandle(int32_t nStdHandle);							// https://docs.microsoft.com/en-us/windows/console/getstdhandle
// void* GetCommandLineA();														// https://docs.microsoft.com/en-us/windows/win32/api/processenv/nf-processenv-getcommandlinea
// size_t strlen(const char *str);												// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/strlen-wcslen-mbslen-mbslen-l-mbstrlen-mbstrlen-l?view=msvc-160
// int __stdcall WriteConsoleA(void* hConsoleOutput, const char* lpBuffer,int32_t nNumberOfCharsToWrite, unsigned long* lpNumberOfCharsWritten,void* lpReserved);  // https://docs.microsoft.com/en-us/windows/console/writeconsole
// int CopyFileA(const char* lpExistingFileName, const char* lpNewFileName, int bFailIfExists);  // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-copyfilea
// void* malloc(size_t size);													// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/malloc?view=msvc-160
// void* CreateFileA(const char* lpFileName, unsigned long dwDesiredAccess, unsigned long dwShareMode, void* lpSecurityAttributes, unsigned long dwCreationDisposition, unsigned long dwFlagsAndAttributes, void* hTemplateFile);  // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
// int ReadFile(void* hFile, void* lpBuffer, unsigned long nNumberOfBytesToRead, unsigned long* lpNumberOfBytesRead, void* lpOverlapped);  // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile
// unsigned long SetFilePointer(void* hFile, long lDistanceToMove, long* lpDistanceToMoveHigh, unsigned long dwMoveMethod);  // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-setfilepointer
// int CloseHandle(void* hObject);												// https://docs.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
// void* BeginUpdateResourceA(const char* pFileName, int bDeleteExistingResources);  // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-beginupdateresourcea
// int UpdateResourceA(void* hUpdate, const char* lpType, const char* lpName,int16_t wLanguage, void* lpData, int32_t cb);  // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-updateresourcea
// int EndUpdateResourceA(void* hUpdate, int fDiscard);							// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-endupdateresourcea
// void ExitProcess(unsigned int uExitCode);									// https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-exitprocess

// ------------------------------ Windows Stuffs ------------------------------ //
// #define MAKEINTRESOURCEA(r)	((unsigned long)(unsigned short) r)				// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-makeintresourcea
// #define MAKELANGID(p,s) ((((unsigned long)(s)) << 10) | (unsigned long)(p))	// https://docs.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-makelangid

// ----------------------------- Global Variables ----------------------------- //
void* __stdcall vp_ConsOut;

// --------------------------- entry point function --------------------------- //
void cei() {
	vp_ConsOut = GetStdHandle(-11);
// Get arguments from command line
	const int iNbArgs = 3;														// number of expected arguments
	char*	cpaArgs[iNbArgs+1];													// 1st "argument" isnt really one: it's this program path
	char*	cpCmdLine = GetCommandLineA();
	CommandLineToArgvA(cpCmdLine, cpaArgs);										// Get arguments from command line
// Check that enough arguments were passed
	if(!cpaArgs[2]) {
		WriteToConsoleA("\nERROR_BAD_ARGUMENTS: Arguments missing\n");
		WriteToConsoleA("\nUsage: > cei PATH\\TO\\ICO PATH\\TO\\EXE [PATH\\TO\\NEWEXEWITHICO]\n");
		ExitProcess(0xA0); }													// 0xA0 = ERROR_BAD_ARGUMENTS	
// Check if 1st and 2nd arguments are a path to a file that exists
	for (int ct=1; ct<=2; ct++) {
		if(access(cpaArgs[ct], 0) < 0 ) {
			WriteToConsoleA("\nERROR_FILE_NOT_FOUND: \""); WriteToConsoleA(cpaArgs[ct]); WriteToConsoleA("\"\n");
		ExitProcess(0x2); } }													// 0x2 = ERROR_FILE_NOT_FOUND
// Check if a 3rd argument has been passed to backup the original exe
	if(cpaArgs[3]) {
		CopyFileA(cpaArgs[2], cpaArgs[3], 0);
		cpaArgs[2] = cpaArgs[3]; }
// read 1st 22 bytes from ico file: https://en.wikipedia.org/wiki/ICO_(file_format)
	void*	vpIcoFile	= CreateFileA(cpaArgs[1], 0x80000000, 0, NULL, 3, 0, NULL);
	void*	vpIcoInfo	= malloc(22);
	int64_t vpmIcoInfo	= (int64_t)vpIcoInfo;
	unsigned long* ulpBytRead = 0;
	ReadFile(vpIcoFile, vpIcoInfo, 22, ulpBytRead, NULL);
// printf("\n");
// printf("i16Reserved:  %x\n",	*(int16_t*)(vpmIcoInfo));
// printf("i16Type:      %x\n",	*(int16_t*)(vpmIcoInfo + 2));
// printf("i16NbImg:     %x\n",	*(int16_t*)(vpmIcoInfo + 4));
// printf("i8Width:      %x\n",	* (int8_t*)(vpmIcoInfo + 6));
// printf("i8Height:     %x\n",	* (int8_t*)(vpmIcoInfo + 7));
// printf("i8NbCol:      %x\n",	* (int8_t*)(vpmIcoInfo + 8));
// printf("i8Reserved:   %x\n",	* (int8_t*)(vpmIcoInfo + 9));
// printf("i16ColPlanes: %x\n",	*(int16_t*)(vpmIcoInfo + 10));
// printf("i16BitPerPx:  %x\n",	*(int16_t*)(vpmIcoInfo + 12));
// printf("i32ImgBytSiz: %x\n",	*(int32_t*)(vpmIcoInfo + 14));
// printf("i32ImgOffset: %x\n",	*(int32_t*)(vpmIcoInfo + 18));

// Check if 1st 2 WORDs of .ico file are valid
	if (*(int16_t*)vpmIcoInfo != 0 || *(int16_t*)(vpmIcoInfo + 2) != 1 ){		// ICONDIR->Reserved = 0 && ICONDIR->Type = 1
		WriteToConsoleA("\nERROR_BAD_ARGUMENTS: \""); WriteToConsoleA(cpaArgs[1]); WriteToConsoleA("\" is not a valid .ico file\n");
		CloseHandle(vpIcoFile);
		ExitProcess(0xA0); }													// 0xA0 = ERROR_BAD_ARGUMENTS	
// Get .ico 1st image + info
	int32_t	i32ImgOffset = *(int32_t*)(vpmIcoInfo + 18);						// Get 1st image offset: ICONDIRENTRY[1]->i32ImgOffset
	SetFilePointer(vpIcoFile, i32ImgOffset, NULL, 0);
	int32_t	i32ImgBytSiz = *(int32_t*)(vpmIcoInfo + 14);						// Get 1st image size in byte: ICONDIRENTRY[1]->i32ImgBytSiz
	void*	vpRes = malloc(i32ImgBytSiz);
	ReadFile(vpIcoFile, vpRes, i32ImgBytSiz, ulpBytRead, NULL);
	CloseHandle(vpIcoFile);
	*(int16_t*)(vpmIcoInfo + 4) = 1;											// Set ICONDIR->NbImg = 1: only 1st image is used
	*(int16_t*)(vpmIcoInfo + 18) = 1;											// Replace ICONDIRENTRY[1]->i32ImgOffset with GRPICONDIRENTRY[1]->OrdinalName = 1
// Update .exe ressource
	void*	vpUpdRes = BeginUpdateResourceA(cpaArgs[2], 0);
	UpdateResourceA(vpUpdRes, (char*)3, (char*)1, 0, vpRes, i32ImgBytSiz);		// (char*)3  = MAKEINTRESOURCE(3)  = RT_ICON, 0 = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL)
	UpdateResourceA(vpUpdRes, (char*)14, (char*)1, 0, vpIcoInfo, 20);			// (char*)14 = MAKEINTRESOURCE(14) = RT_GROUP_ICON
	EndUpdateResourceA(vpUpdRes, 0);
// Done
	ExitProcess(0);
}

// -------------------- Get arguments from command line A --------------------- //
// Notes:
//	- Personal preference for char* instead of the wchar_t* provided by "CommandLineToArgvW()"
//	- Works with double quoted arguments containing escaped quotes: "Such as this \"Double Quoted\" Argument with \"Escaped Quotes\""
void CommandLineToArgvA(char* cpCmdLine, char** cpaArgs) {
	char	cEnd;
	while (*cpCmdLine) {
		while (*cpCmdLine && *cpCmdLine == ' ') cpCmdLine++;					// Trim white-spaces before the argument
		cEnd = ' ';																// end of argument is defined as white-space..
		if (*cpCmdLine == '\"') { cEnd = '\"'; cpCmdLine++; }					// ..or as a double quote if argument is between double quotes
		*cpaArgs = cpCmdLine;													// Save argument pointer
		while (*cpCmdLine && (*cpCmdLine != cEnd || (cEnd == '\"' && *(cpCmdLine-1) == '\\'))) cpCmdLine++;  // Find end of argument ' ' or '\"', while skipping '\\\"' if cEnd = '\"'
		*cpCmdLine = 0;	cpCmdLine++;
		cpaArgs++; }
}

// --------------------------- "Write to Console A" --------------------------- //
// Note: Saves >20KB compared to printf and <stdio.h>
void WriteToConsoleA(char* cpMsg) {
	WriteConsoleA(vp_ConsOut, cpMsg, strlen(cpMsg), NULL, NULL);
}