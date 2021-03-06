#include <mpi.h>

#include "utils.h"
#include "io.h"

void read_block(MPI_Comm cart_comm, char *filename, char *old_image, Block *myblock, int global_cols, unsigned int color_bytes) {
    const int local_cols = myblock->col_end - myblock->col_start;
    const int local_rows = myblock->row_end - myblock->row_start;
    
    MPI_File img_file;
    MPI_CHECK(MPI_File_open(cart_comm, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &img_file));

    for (int i = 0; i < local_rows; ++i) {
        MPI_File_seek(img_file, color_bytes * GLOBAL_OFFSET(myblock->row_start + i, myblock->col_start, global_cols), MPI_SEEK_SET);
        MPI_File_read(img_file, &old_image[color_bytes * LOCAL_OFFSET(i, 0, local_cols)], local_cols * color_bytes, MPI_UNSIGNED_CHAR, MPI_STATUS_IGNORE);
    }

    MPI_File_close(&img_file);
}

void write_block(MPI_Comm cart_comm, char *filename, char *new_image, Block *myblock, int global_cols, unsigned int color_bytes) {
    const int local_cols = myblock->col_end - myblock->col_start;
    const int local_rows = myblock->row_end - myblock->row_start;

    MPI_File out_file;

    MPI_File_open(cart_comm, filename, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &out_file);

    for (int i = 0; i < local_rows; ++i) {
        MPI_File_seek(out_file, color_bytes * GLOBAL_OFFSET(myblock->row_start + i, myblock->col_start, global_cols), MPI_SEEK_SET);
        MPI_File_write(out_file, &new_image[color_bytes * LOCAL_OFFSET(i, 0, local_cols)], local_cols * color_bytes, MPI_UNSIGNED_CHAR, MPI_STATUS_IGNORE);
    }

    MPI_File_close(&out_file);
}