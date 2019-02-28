#pragma once

#if 1
// Jp->En engine
#define CODEPAGE 932
#define PATH "Nova\\JaEn\\EngineDll_je.dll"
#else
// Ko->En engine
#define CODEPAGE 949
#define PATH "Nova\\KoEn\\keeglib.dll"
#endif

typedef int(__cdecl *eg_init_t)(const char *path);
typedef int(__cdecl *eg_init2_t)(const char *path, int);
typedef int(__cdecl *eg_end_t)();
typedef int(__cdecl *eg_translate_multi_t)(int, const char *in, size_t out_size, char *out);
//typedef int(__cdecl *eg_translate_one_t)(int, const char *in, const char *, size_t out_size, char *out, void*, void*);
//typedef int(__cdecl *eg_setcallback_t)(int(__cdecl *callback)());
HMODULE hLEC;
eg_end_t eg_end;
eg_translate_multi_t eg_translate_multi;
int lecState; // 0 - not initialized, 1 - ready, -1 - not available;


wchar_t* LECTranslateFull(wchar_t* src) {
	size_t src_size = wcslen(src) + 1, dst_size;
	// direct engine call doesn't handle those characters well
	// better handling would be manual sentence splitting with
	//  brackets content replacement with signle symbol which is
	//  replaced back after independent translation of content
	for (size_t i = 0; i < src_size; i++) {
		switch (src[i])
		{
			case L'『':// src[i] = L'{'; break;
			case L'｢':
			case L'「': src[i] = L'['; break;
			case L'』':// src[i] = L'}'; break;
			case L'｣':
			case L'」': src[i] = L']'; break;
			case L'≪':
			case L'（': src[i] = L'('; break;
			case L'≫':
			case L'）': src[i] = L')'; break;
			case L'…': src[i] = L' '; break;
			case L'：': src[i] = L'￤'; break;
		}
	}
	char *src_buf = (char*)malloc(src_size*2);
	WideCharToMultiByte(CODEPAGE, 0, src, src_size, src_buf, src_size*2, "_", NULL);

	// we have no idea how much buffer we actually need here
	// src_size*3 looks like a good guess, but let's play it a bit more safe
	char *dst_buf = NULL;
	for (size_t size = src_size*4 + 0x100;;)
	{
		char *d = (char*)realloc(dst_buf, size);
		if (!d)
			break;
		dst_buf = d;
		//window_->eg_translate_one(0, src_buf, NULL, size, dst_buf, NULL, NULL);
		eg_translate_multi(0, src_buf, size, dst_buf);
		dst_size = strlen(dst_buf) + 1;
		if (dst_size < size)
			break;
		size *= 2;
	}
	free(src_buf);

	wchar_t *dst = (wchar_t*)malloc(dst_size*sizeof(wchar_t));
	MultiByteToWideChar(CODEPAGE, 0, dst_buf, dst_size, dst, dst_size);
	free(dst_buf);

	return dst;

}

bool LoadLECFromPath(char *path)
{
	strcat(path, PATH);
	if (hLEC = LoadLibraryA(path))
	{
		eg_init_t eg_init;
		eg_init2_t eg_init2 = (eg_init2_t)GetProcAddress(hLEC, "eg_init2");
		if (!eg_init2)
			eg_init = (eg_init_t)GetProcAddress(hLEC, "eg_init");
		eg_end = (eg_end_t)GetProcAddress(hLEC, "eg_end");
		eg_translate_multi = (eg_translate_multi_t)GetProcAddress(hLEC, "eg_translate_multi");
		if ((eg_init2 || eg_init) && eg_end && eg_translate_multi)
		{
			path[strlen(path) - sizeof(PATH) + 10] = 0;
			if (eg_init2 ? !eg_init2(path, 0) : !eg_init(path))
			{
				lecState = 1;
				return true;
			}
		}
		FreeLibrary(hLEC);
		hLEC = NULL;
	}
	return false;
}

void SetUpLEC()
{
	char path[MAX_PATH + 7 + sizeof(PATH)];
	path[GetModuleFileNameA(NULL, path, MAX_PATH)] = 0;
	if (char *p = strrchr(path, '\\'))
		p[1] = 0;
	strcat(path, "Plugins\\");
	if (LoadLECFromPath(path))
		return;

	lecState = -1;

	HKEY key;
	if (!RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\LogoMedia\\LEC Power Translator 15\\Configuration", 0, KEY_QUERY_VALUE, &key))
	{
		DWORD size = MAX_PATH;
		if (!RegQueryValueExA(key, "ApplicationPath", NULL, NULL, (LPBYTE)path, &size))
		{
			if (size && !path[size - 1])
				size--;
			if (size && path[size - 1] == '\\')
				size--;
			for (;size && path[size - 1] != '\\'; size--);
			if (size)
			{
				path[size] = 0;
				LoadLECFromPath(path);
			}
		}
		RegCloseKey(key);
	}
}

