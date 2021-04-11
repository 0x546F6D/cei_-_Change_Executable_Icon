// # cei - Change Executable Icon

// Change Executable Icon for command line:
//   - Based on https://github.com/fatrolls/IconChanger
//   - Only the 1st image of the .ico file is added to the .exe
//   - Tested on Windows 10 Pro, build 19041.685, locale en-US

// Compiled with MSYS2/MinGW-w64:
//	$ gcc -o cei cei.c -s -O3 -Wl,--gc-sections -fno-asynchronous-unwind-tables -nostartfiles --entry=cei

// Usage:
//	> cei Path\to\ICO Path\to\EXE [Path\to\newEXEwithICO]
//   - If [Path\to\newEXEwithICO] is ommitted, cei.exe edits the original .exe located at Path\to\EXE
//   - If [Path\to\newEXEwithICO] is passed, cei.exe makes a copy of the original .exe to Path\to\newEXEwithICO, and edits this copy

// Notes:
//   - !! Some executables wont work anymore after changing their icon.. !!
//   - ..So make a backup beforehand and/or pass [Path\to\newEXEwithICO] as a 3rd argument

#include <windows.h>
#include <stdint.h>
// #include <stdio.h>

// ----------------------- Project Functions Prototype ------------------------ //
void CommandLineToArgvA(char* cmdLine_cp, char** args_cpa);						// Get arguments from command line.. just a personal preference for char* instead of the wchar_t*/LPWSTR type provided by "CommandLineToArgvW()"
void WriteToConsoleA(char* cpMsg);												// "Write to Console A" function to save >20KB compared to printf and <stdio.h>

// --------------------------- Functions Prototype ---------------------------- //
int access(const char* path, int mode);											// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/access-waccess?view=msvc-160
// void* __stdcall GetStdHandle(int nStdHandle);							// https://docs.microsoft.com/en-us/windows/console/getstdhandle
// void* GetCommandLineA();														// https://docs.microsoft.com/en-us/windows/win32/api/processenv/nf-processenv-getcommandlinea
// unsigned long strlen(const char *str);										// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/strlen-wcslen-mbslen-mbslen-l-mbstrlen-mbstrlen-l?view=msvc-160
// int __stdcall WriteConsoleA(void* hConsoleOutput, const char* lpBuffer,int nNumberOfCharsToWrite, unsigned long* lpNumberOfCharsWritten,void* lpReserved);  // https://docs.microsoft.com/en-us/windows/console/writeconsole
// int CopyFileA(const char* lpExistingFileName, const char* lpNewFileName, int bFailIfExists);  // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-copyfilea
// void* malloc(size_t size);													// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/malloc?view=msvc-160
// void* CreateFileA(const char* lpFileName, unsigned long dwDesiredAccess, unsigned long dwShareMode, void* lpSecurityAttributes, unsigned long dwCreationDisposition, unsigned long dwFlagsAndAttributes, void* hTemplateFile);  // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
// int ReadFile(void* hFile, void* lpBuffer, unsigned long nNumberOfBytesToRead, unsigned long* lpNumberOfBytesRead, void* lpOverlapped);  // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile
// unsigned long SetFilePointer(void* hFile, long lDistanceToMove, long* lpDistanceToMoveHigh, unsigned long dwMoveMethod);  // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-setfilepointer
// int CloseHandle(void* hObject);												// https://docs.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
// void* BeginUpdateResourceA(const char* pFileName, int bDeleteExistingResources);  // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-beginupdateresourcea
// int UpdateResourceA(void* hUpdate, const char* lpType, const char* lpName,short wLanguage, void* lpData, int cb);  // https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-updateresourcea
// int EndUpdateResourceA(void* hUpdate, int fDiscard);							// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-endupdateresourcea
// void ExitProcess(unsigned int uExitCode);									// https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-exitprocess

// ------------------------------ Windows Stuffs ------------------------------ //
// #define MAKEINTRESOURCEA(r)	((unsigned long)(unsigned short) r)				// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-makeintresourcea
// #define MAKELANGID(p,s) ((((unsigned long)(s)) << 10) | (unsigned long)(p))	// https://docs.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-makelangid

// ----------------------------- Global Variables ----------------------------- //
void* __stdcall consOut_vp;

// --------------------------- entry point function --------------------------- //
void cei() {
	consOut_vp = GetStdHandle(-11);
// Get arguments from command line
	const int nbArgs_i = 3;														// number of expected arguments
	char*	args_cpa[nbArgs_i+1];												// +1: 1st "argument" is this program path
	char*	cmdLine_cp = GetCommandLineA();
	CommandLineToArgvA(cmdLine_cp, args_cpa);									// Get arguments from command line
// Check that enough arguments were passed
	if(!args_cpa[2]) {
		WriteToConsoleA("\nERROR_BAD_ARGUMENTS: Arguments missing\n");
		WriteToConsoleA("\nUsage: > cei Path\\to\\ICO Path\\to\\EXE [Path\\to\\newEXEwithICO]\n");
		ExitProcess(0xA0); }													// 0xA0: ERROR_BAD_ARGUMENTS	
// Check if 1st and 2nd arguments are a path to a file that exists
	for (int ct=1; ct<=2; ct++) {
		if(access(args_cpa[ct], 0) < 0 ) {
			WriteToConsoleA("\nERROR_FILE_NOT_FOUND: \""); WriteToConsoleA(args_cpa[ct]); WriteToConsoleA("\"\n");
		ExitProcess(0x2); } }													// 0x2: ERROR_FILE_NOT_FOUND
// Check if a 3rd argument has been passed to backup the original exe
	if(args_cpa[3]) {
		CopyFileA(args_cpa[2], args_cpa[3], 0);
		args_cpa[2] = args_cpa[3]; }
// read 1st 22 bytes from ico file: https://en.wikipedia.org/wiki/ICO_(file_format)
	void* icoFile_vp = CreateFileA(args_cpa[1], 0x80000000, 0, NULL, 3, 0, NULL);
	void* icoInfo_vp = malloc(22);
	long long icoInfoAdr_ll = (long long)icoInfo_vp;
	unsigned long* procByte_ulp = 0;
	ReadFile(icoFile_vp, icoInfo_vp, 22, procByte_ulp, NULL);
// printf("\n");
// printf("reserved_s:	%x\n",	*(short*)(icoInfoAdr_ll));
// printf("type_s:      %x\n",	*(short*)(icoInfoAdr_ll + 2));
// printf("nbImg_s:     %x\n",	*(short*)(icoInfoAdr_ll + 4));
// printf("width_c:		%x\n",	* (char*)(icoInfoAdr_ll + 6));
// printf("height_c:	%x\n",	* (char*)(icoInfoAdr_ll + 7));
// printf("nbCol_c:		%x\n",	* (char*)(icoInfoAdr_ll + 8));
// printf("reserved_c:	%x\n",	* (char*)(icoInfoAdr_ll + 9));
// printf("colPlanes_s:	%x\n",	*(short*)(icoInfoAdr_ll + 10));
// printf("bitPerPx_s:	%x\n",	*(short*)(icoInfoAdr_ll + 12));
// printf("imgSize_i:	%x\n",	*(int*)(icoInfoAdr_ll + 14));
// printf("imgOffset_i:	%x\n",	*(int*)(icoInfoAdr_ll + 18));

// Check if 1st 2 WORDs of .ico file are valid
	if (*(short*)icoInfoAdr_ll != 0 || *(short*)(icoInfoAdr_ll + 2) != 1 ){		// ICONDIR->Reserved = 0 && ICONDIR->Type = 1
		WriteToConsoleA("\nERROR_BAD_ARGUMENTS: \""); WriteToConsoleA(args_cpa[1]); WriteToConsoleA("\" is not a valid .ico file\n");
		CloseHandle(icoFile_vp);
		ExitProcess(0xA0); }													// 0xA0: ERROR_BAD_ARGUMENTS	
// Get .ico 1st image + info
	int	imgOffset_i = *(int*)(icoInfoAdr_ll +18);								// +18: ICONDIRENTRY[1]->imgOffset_i (1st image offset)
	int	imgSize_i = *(int*)(icoInfoAdr_ll +14);									// +14: ICONDIRENTRY[1]->imgSize_i (1st image size in byte)
	void* icoReadBuf_vp = malloc(imgSize_i);
	SetFilePointer(icoFile_vp, imgOffset_i, NULL, 0);
	ReadFile(icoFile_vp, icoReadBuf_vp, imgSize_i, procByte_ulp, NULL);
	CloseHandle(icoFile_vp);
	*(short*)(icoInfoAdr_ll +4) = 1;											// Set ICONDIR->NbImg = 1: only 1st image is used
	*(short*)(icoInfoAdr_ll +18) = 1;											// Replace ICONDIRENTRY[1]->imgOffset_i with GRPICONDIRENTRY[1]->OrdinalName = 1
// Update .exe ressource
	void* updateRes_vp = BeginUpdateResourceA(args_cpa[2], 0);
	UpdateResourceA(updateRes_vp, (char*)3, (char*)1, 0, icoReadBuf_vp, imgSize_i);	// (char*)3: MAKEINTRESOURCE(3)  = RT_ICON; 0: MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL)
	UpdateResourceA(updateRes_vp, (char*)14, (char*)1, 0, icoInfo_vp, 20);		// (char*)14: MAKEINTRESOURCE(14) = RT_GROUP_ICON
	EndUpdateResourceA(updateRes_vp, 0);
// Clean Up
	free(icoInfo_vp);
	free(icoReadBuf_vp);
// Done
	ExitProcess(0);
}

// -------------------- Get arguments from command line A --------------------- //
// Notes:
//	- Personal preference for char* instead of the wchar_t* provided by "CommandLineToArgvW()"
//	- Probably works with double quoted arguments containing escaped quotes.. in most cases:
//		- "Such as this \"Double Quoted\" Argument with \"Escaped Quotes\" and \\\"Escaped BackSlash\"\\"
void CommandLineToArgvA(char* cmdLine_cp, char** args_cpa) {
	char endChar_c;
	while (*cmdLine_cp) {
		while (*cmdLine_cp && *cmdLine_cp == ' ') cmdLine_cp++;					// Trim white-spaces before the argument
		endChar_c = ' ';														// end of argument is defined as white-space..
		if (*cmdLine_cp == '\"') { endChar_c = '\"'; cmdLine_cp++; }			// ..or as a double quote if argument is between double quotes
		*args_cpa = cmdLine_cp;													// Save argument pointer
// Find end of argument ' ' or '\"', while skipping '\\\"' if endChar_c = '\"'
		int prevBackSlash_b = 0;
		while (*cmdLine_cp && (*cmdLine_cp != endChar_c || (endChar_c == '\"' && prevBackSlash_b))) {
			prevBackSlash_b = 0;
			int checkBackSlash_i = 0;
			while(*(cmdLine_cp-checkBackSlash_i) == '\\') {
				checkBackSlash_i++;
				prevBackSlash_b = !prevBackSlash_b; }
			cmdLine_cp++; }
		if(*cmdLine_cp) {
			*cmdLine_cp = 0; cmdLine_cp++; }
		args_cpa++; }
}

// --------------------------- "Write to Console A" --------------------------- //
// Note: Saves >20KB compared to printf and <stdio.h>
void WriteToConsoleA(char* cpMsg) {
	WriteConsoleA(consOut_vp, cpMsg, strlen(cpMsg), NULL, NULL);
}