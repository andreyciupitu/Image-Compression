/* BMP_COMPRESS.H - structuri si functii utilizate pentru lucrul cu fisiere BMP */

#ifndef BMP_COMPRESS_H
#define BMP_COMPRESS_H
	
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <string.h>
	#include <math.h>
	
	typedef struct file_header
	{
		uint16_t signature;
		uint32_t file_size;
		uint32_t reserved;
		uint32_t offset;
	}__attribute__((packed)) file_header;

	typedef struct info_header
	{
		uint32_t size;
		uint32_t width;
		uint32_t height;	
		int16_t planes;
		int16_t bit_counts;
		uint32_t compression;
		uint32_t image_size;
		uint32_t x_pixels;
		uint32_t y_pixels;
		uint32_t colors_used;
		uint32_t colors_important;
	}__attribute__((packed)) info_header;

	typedef struct image
	{
		unsigned char blue;
		unsigned char green;
		unsigned char red;
		unsigned char reserved;
	}__attribute__((packed)) image;

	typedef struct tnode
	{
		image color;
		uint32_t area;
		struct tnode *topleft, *botleft, *botright, *topright;
	}__attribute__((packed)) tnode;

	typedef struct tree_vector
	{
		image color;
		uint32_t area;
		int32_t botleft, topleft;
		int32_t botright, topright;
	}__attribute__((packed)) tree_vector;

	void get_headers(file_header *file, info_header *info, FILE *f);
	void get_image(image *a[], uint32_t n, uint32_t m, FILE *f);
	void make_array(image ***a, uint32_t n, uint32_t m);
	void make_vector(tree_vector **v, uint32_t n);
	void write_headers(file_header *file, info_header *info, FILE *f);
	void write_array(image *a[], uint32_t, uint32_t m, FILE *f);
	void write_vector(tree_vector *v, uint32_t n, uint32_t color_count, FILE *f);
	void init_tree(tnode **root);
	tnode* make_tnode();
	tnode* array_to_tree(tnode *root, image *a[], uint32_t i, uint32_t j, uint32_t size, uint32_t *node_count);
	int same_color(image *a[], uint32_t n1, uint32_t m1, uint32_t n2, uint32_t m2);
	uint32_t tree_to_vector(tnode *root, tree_vector *v, uint32_t *n);
	void tree_to_array(tnode *root, image *a[], uint32_t n, uint32_t m);
	tnode* vector_to_tree(tnode *root, tree_vector *v, int32_t pos);
	tnode* rotate(tnode *root);
	tnode* find_common_parent(tnode *root, image color1, image color2);
	int contains_color(tnode *root, image color);
	tnode* destroy_tree(tnode *root);

#endif /* BMP_COMPRESS_H */