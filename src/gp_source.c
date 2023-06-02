#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "gp_source.h"

const wchar_t* gp_utf_8_to_utf_16(const char* utf8) {
	const int utf8_length = (int)strlen(utf8);
	const int wstr_length = MultiByteToWideChar(CP_UTF8, 0, utf8, utf8_length, NULL, 0);

	wchar_t* wstr = (wchar_t*)malloc(sizeof(wchar_t) * (wstr_length + 1));

	if (wstr == NULL) return NULL;

	MultiByteToWideChar(CP_UTF8, 0, utf8, utf8_length, wstr, wstr_length);
	wstr[wstr_length] = L'\0';

	return wstr;
}

struct GpSource* gp_new_source(const char* path) {
	struct GpSource* source = malloc(sizeof(struct GpSource));
	if (source == NULL) return NULL;

	source->size = strlen(path);
	source->path = malloc(source->size + 1);

	if (source->path == NULL) {
		free(source);
		return NULL;
	}

	memcpy((void*)source->path, path, source->size + 1);

	source->wpath = gp_utf_8_to_utf_16(source->path);

	if (source->wpath == NULL) {
		free((void*)source->path);
		free(source);
		return NULL;
	}

	return source;
}

void gp_free_source(struct GpSource* source) {
	if (source == NULL) return;

	free((void*)source->path);
	free((void*)source->wpath);
	free(source);
}
