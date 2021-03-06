/****************************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

#include "things_string.h"

#include <string.h>
#include "things_malloc.h"
#include "things_util.h"

#define TAG "[things_string]"

#ifdef ENABLE_THINGS_MALLOC
char *things_strdup(const char *str)
{
	if (str == NULL) {
		return NULL;
	}

	/* Allocate memory for original string length and 1 extra byte for '\0' */
	size_t length = strlen(str);
	char *dup = (char *)things_malloc(length + 1);
	if (dup != NULL) {
		memcpy(dup, str, length + 1);
	}

	return dup;
}
#endif

char *things_clone_string(const char *str)
{
	RET_NULL_IF_PARAM_IS_NULL(TAG, str);

	int len = strlen(str);
	char *dest_str = (char *)things_malloc(sizeof(char) *len + 1);
	if (NULL != dest_str) {
		strncpy(dest_str, str, len + 1);
	} else {
		THINGS_LOG(THINGS_ERROR, TAG, "Memory allocation failed.");
	}

	return dest_str;
}

void things_free_str_array(char **ptr, size_t size)
{
	RET_IF_PARAM_IS_NULL(TAG, ptr);
	RET_IF_PARAM_EXPR_IS_TRUE(TAG, size < 1);

	for (size_t index = 0; index < size; index++) {
		things_free(ptr[index]);
	}
	things_free(ptr);
}

int things_string_compare(const char *src, const char *dst)
{
	if ((NULL == src) || (NULL == dst)) {
		return -1;
	}

	int srcLen = strlen(src);
	int dstLen = strlen(dst);

	if (srcLen != dstLen) {
		return -1;
	}

	return strncmp(src, dst, dstLen);
}

int things_string_duplicate(const char *src, char **dst)
{
	if (NULL == src) {
		return 0;
	} else {
		int len = strlen(src);
		*dst = (char *)things_malloc(len + 1);
		if (NULL == *dst) {
			THINGS_LOG(THINGS_ERROR, TAG, "memory allocation failed.");
			return 0;
		}

		memset(*dst, 0, len + 1);
		things_strncpy(*dst, src, len);
	}
	return 1;
}

// Memory Leakage Should be check
void things_string_concat(char **target, char *attach)
{
	char buf[MAX_BUF_LEN] = { 0 };

	if (attach == NULL) {
		return;
	}

	if (*target != NULL) {
		things_strncpy(buf, *target, strlen(*target) + 1);
		things_free(*target);
		*target = NULL;
	}

	if (MAX_BUF_LEN >= (strlen(buf) + strlen(attach) + 1)) {
		strncat(buf, attach, MAX_BUF_LEN);
	} else {
		THINGS_LOG_ERROR(THINGS_ERROR, TAG, "Something went wrong");
		return;
	}

	*target = (char *)things_malloc(strlen(buf) + 1);
	if (NULL == *target) {
		THINGS_LOG_ERROR(THINGS_ERROR, TAG, THINGS_MEMORY_ERROR);
		return;
	}
	things_strncpy(*target, buf, strlen(buf) + 1);
}

int things_string_hex_to_int(const char *hex, int *num)
{
	return sscanf(hex, "%x", num);
}

int things_get_id_value_from_query(char idvalue[], char *inputQuery, int size)
{
	THINGS_LOG_D(THINGS_DEBUG, TAG, "Input query => %s", inputQuery);

	char seperators[] = "?;#&";

	if (NULL == inputQuery) {
		THINGS_LOG_D(THINGS_DEBUG, TAG, "Input query is NULL");
		return 0;
	}

	if (size > MAX_INPUT_QUERY_LEN) {
		THINGS_LOG_D(THINGS_DEBUG, TAG, "Input size bigger than maximum query size(%d)", MAX_INPUT_QUERY_LEN);
		return 0;
	}

	// Remove empty space
	char queries[MAX_INPUT_QUERY_LEN + 1] = { 0, };
	int index = 0;
	for (int i = 0; i < strlen(inputQuery); i++) {
		if (*(inputQuery + i) == ' ') {
			continue;
		}
		queries[index] = *(inputQuery + i);
		index++;
	}

	// Get Query start with "id="
	char *idQuery = strstr(queries, "id=");
	if (NULL == idQuery) {
		THINGS_LOG_D(THINGS_DEBUG, TAG, "There is no \"id=\" in input query");
		return 0;
	}
	// Seperate idQuery with seperators [?, ;, #]
	for (int i = 0; i < strlen(idQuery); i++) {
		if (*(idQuery + i) == '?' || *(idQuery + i) == ';' || *(idQuery + i) == '#' || *(idQuery + i) == '&') {
			strtok(idQuery, seperators);
			break;
		}
	}

	// Get id data after "id=" and validation check.
	for (int i = 3, k = 0; i < strlen(idQuery); i++) {
		if (idQuery[i] >= '0' && idQuery[i] <= '9') {
			idvalue[k++] = idQuery[i];
		} else {
			idvalue[0] = '\0';
			return 0;
		}
	}

	return 1;
}

char *things_strcat(char *dest, size_t destSize, const char *src)
{
	if (dest == NULL || src == NULL || destSize == 0) {
		THINGS_LOG_V_ERROR(THINGS_ERROR, TAG, "dest(0x%X) or src(0x%X) or dest_size=%d is NULL.", dest, src, destSize);
		return NULL;
	}

	if (strlen(src) >= (destSize - strlen(dest))) {
		THINGS_LOG_V_ERROR(THINGS_ERROR, TAG, "Source Size(%d) is over than Dest-FreeSize(%d).", strlen(src), destSize - strlen(dest) - 1);
		return NULL;
	}

	return strncat(dest, src, strlen(src));
}

char *things_strncpy(char *destination, const char *source, size_t num)
{
	if (NULL == source) {
		THINGS_LOG_V_ERROR(THINGS_ERROR, TAG, "Input String is NULL");
		return NULL;
	}

	return strncpy(destination, source, num);
}
