#include "ga_source_list.h"
#include <stdlib.h>

struct GaSourceList* ga_new_source_list(const char** paths, size_t size) {
	struct GaSourceList* source_list = malloc(sizeof(struct GaSourceList));
	if (source_list == NULL) return NULL;

	source_list->list = malloc(sizeof(struct GaSource*) * size);
	if (source_list->list == NULL) {
		free(source_list);
		return NULL;
	}

	for (size_t i = 0; i < size; i++) {
		source_list->list[i] = ga_new_source(paths[i]);
		if (source_list->list[i] == NULL) {
			for (size_t j = 0; j < i; j++) {
				ga_free_source(source_list->list[j]);
			}
			free(source_list->list);
			free(source_list);
			return NULL;
		}
	}

	source_list->size = size;
	return source_list;

}

void ga_free_source_list(struct GaSourceList* source_list) {
	if (source_list == NULL || source_list->list == NULL) return;

	for (size_t i = 0; i < source_list->size; i++) {
		ga_free_source(source_list->list[i]);
	}

	free(source_list->list);
	free(source_list);
}
