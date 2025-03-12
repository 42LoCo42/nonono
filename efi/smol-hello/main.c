#include <efi/efi.h>
#include <efi/efilib.h>

#define CONCAT(x, y) x##y
#define LSTR(s) CONCAT(L, s)

#define abort(msg)                                                             \
	{                                                                          \
		Print(L"%s:%d: error: %s\n", LSTR(__FILE__), __LINE__, LSTR(msg));     \
		return -1;                                                             \
	}

#define CInput(msg, buf, siz)                                                  \
	{                                                                          \
		ST->ConOut->EnableCursor(ST->ConOut, TRUE);                            \
		Input(msg, buf, siz);                                                  \
		ST->ConOut->EnableCursor(ST->ConOut, FALSE);                           \
	}

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
	InitializeLib(ImageHandle, SystemTable);
	Print(L"Hello, world!\n");

	// get the currently loaded image
	EFI_LOADED_IMAGE* img = NULL;
	BS->HandleProtocol(ImageHandle, &LoadedImageProtocol, (void**) &img);
	if (img == NULL)
		abort("could not get LOADED_IMAGE");

	// get the current volume
	EFI_FILE_HANDLE vol = LibOpenRoot(img->DeviceHandle);
	if (vol == NULL)
		abort("could not get volume FILE_HANDLE");

	// open the data file
	EFI_FILE_HANDLE file = NULL;
	vol->Open(vol, &file, L"data", EFI_FILE_MODE_READ, 0);
	if (file == NULL)
		abort("could not open file");

	EFI_FILE_INFO* info = LibFileInfo(file);
	if (info == NULL)
		abort("could not get file info");

	UINT64 size = info->FileSize;
	Print(L"size: %d bytes\n", size);

	UINT8* buf = AllocatePool(size + 2);
	if (buf == NULL)
		abort("could not allocate buffer");

	if (file->Read(file, &size, buf) != EFI_SUCCESS)
		abort("could not read file");
	buf[size + 0] = 0;
	buf[size + 1] = 0;

	Print(L"%s", buf);

	FreePool(buf);
	file->Close(file);

	{
		CHAR16 buf[128] = {0};
		CInput(L"> ", buf, 128);
		Print(L"\n< %s\n", buf);
	}

	return EFI_SUCCESS;
}
