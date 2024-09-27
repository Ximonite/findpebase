#include <Windows.h>
#include <ImageHlp.h>
#include <stdio.h>

// MiniCRT Functions
extern "C" int mini_printf(const char *format, ...);
extern "C" int mini_sprintf_s(char *str, size_t SizeInBytes, const char *format, ...);
extern "C" int mini_strcat_s(char *dest, rsize_t SizeInBytes, const char *source);

int main() {
	// Values
	char SystemPath[MAX_PATH];
	HANDLE SysFileHandle;
	HANDLE OutputTxtHandle;
	WIN32_FIND_DATAA FileData;
	PLOADED_IMAGE LoadedFile;
	DWORD BytesWritten;
	char ImageInfo[400];
	DWORD dwAddress = 0x10000000;

	// Set SystemPath
	GetSystemDirectoryA(SystemPath, MAX_PATH);
	mini_strcat_s(SystemPath, MAX_PATH, "\\*");
	// Create output.txt
	OutputTxtHandle = CreateFileA("output.txt", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
	
	// Find files
	SysFileHandle = FindFirstFileA(SystemPath, &FileData);
	// Exit if file cannot be found
	if(SysFileHandle == INVALID_HANDLE_VALUE){
		return -1;
	}
	do {
		// Find PE Base
		LoadedFile = ImageLoad(FileData.cFileName, NULL);
		if(LoadedFile){
			// Write data into ImageInfo
			mini_sprintf_s(ImageInfo, 256, "0x%08x | %x | %s\n", LoadedFile->FileHeader->OptionalHeader.ImageBase, LoadedFile->FileHeader->OptionalHeader.SizeOfImage, FileData.cFileName);
			// Display info in console
			mini_printf("%s %d\n", ImageInfo, GetLastError());
			// Write data from ImageInfo into output.txt
			WriteFile(OutputTxtHandle, ImageInfo, strlen(ImageInfo), &BytesWritten, NULL);
			// PE image needs to be unloaded to free up LoadedFile (also to free them from memory)
			ImageUnload(LoadedFile);
		}
		FindNextFileA(SysFileHandle, &FileData);
	}
	while(GetLastError() != ERROR_NO_MORE_FILES);
}