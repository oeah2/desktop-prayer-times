#include "file.h"
#include <assert.h>

size_t file_find_length(FILE* file)
{
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

bool file_read_all(FILE* file, size_t max_len, char dest[max_len])
{
    bool ret = false;
    if(file) {
        size_t file_length = file_find_length(file);
        if(file_length > max_len)
            return ret;

        size_t read_bytes = fread(dest, sizeof(char), file_length, file);
        assert(max_len > read_bytes);
        dest[read_bytes] = '\0';
        if(read_bytes < (file_length - 1) && !feof(file)) {
            printf("Error while reading config file. Here is the content: \n");
            puts(dest);
            return false;    // depends on library implementation(?)
        }
        ret = true;
    }
    return ret;
}
