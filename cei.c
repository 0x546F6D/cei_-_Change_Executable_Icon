// # cei - Change Executable Icon

// Change Executable Icon for command line, in C:
//   - Based on https://github.com/fatrolls/IconChanger
//   - Only the 1st image of the .ico file is added to the .exe
//   - Tested on Windows 10 Pro, build 19041.685, locale en-US

// Compiled with MSYS2/MinGW-w64:
//   $ gcc -o cei cei.c -s -O3 -Wl,--gc-sections -fno-asynchronous-unwind-tables -nostartfiles --entry=cei

// Usage:
//   > cei PATH\TO\ICO PATH\TO\EXE [PATH\TO\NEWEXEWITHICO]
//   - If [PATH\TO\NEWEXEWITHICO] is ommitted, cei.exe edits the original .exe located at PATH\TO\EXE
//   - If [PATH\TO\NEWEXEWITHICO] is passed, cei.exe makes a copy of the original .exe to PATH\TO\EXEWITHICO, and edits this copy

// Notes:
//   - !! Some executables wont work anymore after changing their icon.. !!
//   - ..So make a backup beforehand and/or pass [PATH\TO\NEWEXEWITHICO] as a 3rd argument

#include <windows.h>
#include <stdint.h>
// #include <stdio.h>

// -------------------- Project Function Prototypes --------------------
void GetCommandLineArgvA(char* pCommandLine, char** aArgs);															// Get arguments from command line.. just a personal preference for char* instead of the wchar_t*/LPWSTR type provided by "CommandLineToArgvW()"
void WriteToConsoleA(char* lpMsg);																					// "Write to Console A" function to save >20KB compared to printf and <stdio.h>

// -------------------- C Function Prototypes --------------------
int _access(const char* path, int mode);																			// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/access-waccess?view=msvc-160
// void* __stdcall GetStdHandle(int32_t nStdHandle);																// https://docs.microsoft.com/en-us/windows/console/getstdhandle
// void* GetCommandLineA();																							// https://docs.microsoft.com/en-us/windows/win32/api/processenv/nf-processenv-getcommandlinea
// size_t strlen(const char *str);																					// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/strlen-wcslen-mbslen-mbslen-l-mbstrlen-mbstrlen-l?view=msvc-160
// int __stdcall WriteConsoleA(void* hConsoleOutput, const char* lpBuffer,int32_t nNumberOfCharsToWrite, unsigned long* lpNumberOfCharsWritten,void* lpReserved);	// https://docs.microsoft.com/en-us/windows/console/writeconsole
// int CopyFileA(const char* lpExistingFileName, const char* lpNewFileName, int bFailIfExists);						// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-copyfilea
// void* malloc(size_t size);																						// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/malloc?view=msvc-160
// void* CreateFileA(const char* lpFileName, unsigned long dwDesiredAccess, unsigned long dwShareMode, void* lpSecurityAttributes, unsigned long dwCreationDisposition, unsigned long dwFlagsAndAttributes, void* hTemplateFile);	// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
// int ReadFile(void* hFile, void* lpBuffer, unsigned long nNumberOfBytesToRead, unsigned long* lpNumberOfBytesRead, void* lpOverlapped);	// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile
// unsigned long SetFilePointer(void* hFile, long lDistanceToMove, long* lpDistanceToMoveHigh, unsigned long dwMoveMethod);	// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-setfilepointer
// int CloseHandle(void* hObject);																					// https://docs.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
// void* BeginUpdateResourceA(const char* pFileName, int bDeleteExistingResources);									// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-beginupdateresourcea
// int UpdateResourceA(void* hUpdate, const char* lpType, const char* lpName,int16_t wLanguage, void* lpData, int32_t cb);	// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-updateresourcea
// int EndUpdateResourceA(void* hUpdate, int fDiscard);																// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-endupdateresourcea
// void ExitProcess(uint32_t uExitCode);																			// https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-exitprocess

// -------------------- C Macro --------------------
// #define MAKEINTRESOURCEA(r)	((unsigned long)(unsigned short) r)													// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-makeintresourcea
// #define MAKELANGID(p,s) ((((WORD)(s)) << 10) | (WORD)(p))														// https://docs.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-makelangid

// -------------------- Global Variables --------------------
void* __stdcall hdConsoleOut;

void cei() {
	hdConsoleOut = GetStdHandle(-11);
// Get arguments from command line
	char* pCommandLine = GetCommandLineA();
	const int nbArgs = 3;																							// number of expected arguments
	char* aArgs[nbArgs+2];																							// 1st "argument" isnt really one: it's this program path
	GetCommandLineArgvA(pCommandLine, aArgs);																		// Get arguments from command line
// Check that enough arguments were passed
	if(!aArgs[2]) {
		WriteToConsoleA("\nERROR_BAD_ARGUMENTS: Arguments missing\n");
		WriteToConsoleA("\nUsage: > cei PATH\\TO\\ICO PATH\\TO\\EXE [PATH\\TO\\EXEWITHICO]\n");
		ExitProcess(0xA0); } // ERROR_BAD_ARGUMENTS
// Check if 1st and 2nd arguments are a path to a file that exists
	for (int ct=1; ct<=2; ct++) {
		if(_access(aArgs[ct], 0) < 0 ) {
			WriteToConsoleA("\nERROR_FILE_NOT_FOUND: \""); WriteToConsoleA(aArgs[ct]); WriteToConsoleA("\"\n");
			ExitProcess(0x2); } } // ERROR_FILE_NOT_FOUND
// Check if a 3rd argument has been passed to backup the original exe
	if(aArgs[3]) {
		CopyFileA(aArgs[2], aArgs[3], 0);
		aArgs[2] = aArgs[3]; }
// read 1st 22 bytes from ico file: https://en.wikipedia.org/wiki/ICO_(file_format)
	void*	hdIco= CreateFileA(aArgs[1], 0x80000000, 0, NULL, 3, 0, NULL);
	void*	lpIco= malloc(22);
	int64_t adrIco = (int64_t)lpIco;
	unsigned long*	dwBytesRead = 0;
	ReadFile(hdIco, lpIco, 22, dwBytesRead, NULL);

// printf("\n");
// printf("wReserved:   %x\n",	*(int16_t*)(adrIco));
// printf("wType:       %x\n",	*(int16_t*)(adrIco + 2));
// printf("wNbImg:      %x\n",	*(int16_t*)(adrIco + 4));
// printf("bWidth:      %x\n",	* (int8_t*)(adrIco + 6));
// printf("bHeight:     %x\n",	* (int8_t*)(adrIco + 7));
// printf("bNbCol:      %x\n",	* (int8_t*)(adrIco + 8));
// printf("bReserved:   %x\n",	* (int8_t*)(adrIco + 9));
// printf("wColPlanes:  %x\n",	*(int16_t*)(adrIco + 10));
// printf("wBitPerPx:   %x\n",	*(int16_t*)(adrIco + 12));
// printf("dwImgBytSiz: %x\n",	*(int32_t*)(adrIco + 14));
// printf("dwImgOffset: %x\n",	*(int32_t*)(adrIco + 18));

// Check if 1st 2 WORDs of .ico file are valid
	if (*(int16_t*)adrIco != 0 || *(int16_t*)(adrIco + 2) != 1 ){
		WriteToConsoleA("\nERROR_BAD_ARGUMENTS: \""); WriteToConsoleA(aArgs[1]); WriteToConsoleA("\" is not a valid .ico file\n");
		CloseHandle(hdIco);
		ExitProcess(0xA0); } // ERROR_BAD_ARGUMENTS
// Get .ico 1st image + info
	*(int16_t*)(adrIco + 4) = 1;																					// ICONDIR->wNbImg set to 1, we only care about the 1st icon
	int32_t dwImgBytSiz = *(int32_t*)(adrIco + 14);																	// ICONDIRENTRY[1]->dwImgBytSiz
	int32_t dwImgOffset = *(int32_t*)(adrIco + 18);																	// ICONDIRENTRY[1]->dwImgOffset
	*(int16_t*)(adrIco + 18) = 1;																					// replace ICONDIRENTRY[1]->dwImgOffset with GRPICONDIRENTRY[1]->wOrdinalName = 1
	void*	lpbRes = malloc(dwImgBytSiz);
	SetFilePointer(hdIco, dwImgOffset, NULL, 0);
	ReadFile(hdIco, lpbRes, dwImgBytSiz, dwBytesRead, NULL);
	CloseHandle(hdIco);
// Update .exe ressource
	void* hdUpdRes = BeginUpdateResourceA(aArgs[2], 0);
	UpdateResourceA(hdUpdRes, MAKEINTRESOURCEA(3), MAKEINTRESOURCEA(1), 0, lpbRes, dwImgBytSiz);
	UpdateResourceA(hdUpdRes, MAKEINTRESOURCEA(14), MAKEINTRESOURCEA(1), 0, lpIco, 20);
	EndUpdateResourceA(hdUpdRes, 0);
// Done
	ExitProcess(0);
}

// -------------------- Get arguments from command line A -------------------- function.. just a personal preference for char* instead of the wchar_t*/LPWSTR type provided by "CommandLineToArgvW()"
// Notes: this function does not work with \" inside double quotes
void GetCommandLineArgvA(char* pCommandLine, char** aArgs) {
	while (*pCommandLine) {
		while (*pCommandLine && *pCommandLine == ' ') pCommandLine++;												// Trim white-spaces before the argument
		char cEnd = ' ';																							// end of argument is defined as white-space..
		if (*pCommandLine == '\"') { pCommandLine++; cEnd = '\"'; }													// ..or as a double quote if argument is between double quotes
		*aArgs = pCommandLine;																						// Save argument pointer
		while (*pCommandLine && *pCommandLine != cEnd) pCommandLine++;
		if (*pCommandLine) *pCommandLine = 0;																		// Set NULL separator between arguments
		pCommandLine++;
		aArgs++; }
	*aArgs = 0;
}

// -------------------- "Write to Console A" -------------------- function to save >20KB compared to printf and <stdio.h>
void WriteToConsoleA(char* lpMsg) {
	WriteConsoleA(hdConsoleOut, lpMsg, strlen(lpMsg), NULL, NULL);
}