#include "file.h"
#include <assert.h>

#define FILE_ADDITIONAL_BUFFER 10

/** \brief Returns length of file
 *
 * \param file FILE* of which the lengh shall be determined
 * \return size_t file length, including terminating zero
 *
 */
static size_t file_find_actual_length(FILE* file) {
    size_t ret = 0;
    if(file) {
        rewind(file);
        int fseek_result = fseek(file, 0, SEEK_END);
        assert(!fseek_result);
        ret = ftell(file);
        rewind(file);
    }
    return ret;
}

size_t file_find_length(FILE* file)
{
    return file_find_actual_length(file) + FILE_ADDITIONAL_BUFFER;
}

bool file_read_all(FILE* file, size_t max_len, char dest[max_len])
{
    bool ret = false;
    if(file) {
        size_t file_length = file_find_actual_length(file);
        if(file_length > max_len)
            return ret;

        size_t read_bytes = fread(dest, sizeof(char), file_length, file);
        assert(max_len > read_bytes);
        dest[read_bytes] = '\0';
        if(read_bytes < (file_length - 1) && !feof(file)) {
            myperror("file_read_all: Error while reading config file.");
            return false;    // depends on library implementation(?)
        }
        ret = true;
    }
    return ret;
}
