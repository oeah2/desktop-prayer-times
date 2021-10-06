//#ifndef FILE_H_INCLUDED
//#define FILE_H_INCLUDED
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

/** \brief Returns length of file with additional buffer
 *
 * \param file FILE* of which the lengh shall be determined
 * \return size_t file length, including terminating zero and additional buffer
 *
 */
size_t file_find_length(FILE* file);

/** \brief Reads all content of file into destination array
 *
 * \param file FILE*
 * \param max_len size_t max size to be read, including terminating 0
 * \param dest[max_len] char
 * \return bool true on success, false otherwise
 *
 */
bool file_read_all(FILE* file, size_t max_len, char dest[max_len]);

//#endif // FILE_H_INCLUDED
