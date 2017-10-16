#include "bmp_compress.h"

/* Functii de citire si scriere  pentru fisiere BMP */

void get_headers(file_header *file, info_header *info, FILE *f)
{
	fread(file, sizeof(file_header), 1, f);
	fread(info, sizeof(info_header), 1, f);
}

void make_array(image ***a, uint32_t n, uint32_t m)
{
	int i;
	*a = (image**)calloc(n, sizeof(image*));
	for (i = 0; i < n; i++)
		(*a)[i] = (image*)calloc(m, sizeof(image));
}

void make_vector(tree_vector **v, uint32_t n)
{
	*v = (tree_vector*)calloc(n, sizeof(tree_vector));
}

void get_image(image *a[], uint32_t n, uint32_t m, FILE *f)
{
	int i;
	for (i = n-1; i >= 0; i--)
		fread(a[i], sizeof(image), m, f);
}

void write_headers(file_header *file, info_header *info, FILE *f)
{
	fwrite(file, sizeof(file_header), 1, f);
	fwrite(info, sizeof(info_header), 1, f);
}

void write_vector(tree_vector *v, uint32_t n, uint32_t color_count, FILE *f)
{
	fwrite(&color_count, sizeof(uint32_t), 1, f);
	fwrite(&n, sizeof(uint32_t), 1, f);
	fwrite(v, sizeof(tree_vector), n, f);
}

void write_array(image *a[], uint32_t n, uint32_t m, FILE *f)
{
	int i;
	for (i = n-1; i >= 0; i--)
		fwrite(a[i], sizeof(image), m, f);
}