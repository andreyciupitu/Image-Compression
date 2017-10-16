#include "bmp_compress.h"

int main(int argc, char *argv[])
{
	int i;
	FILE *f;
	file_header file;
	info_header info;
	image **pixels;
	tnode *root;
	tree_vector *v = NULL;
	uint32_t node_count = 0;
	uint32_t color_count;
	uint32_t rotations;
	uint32_t position = 0;
	image color1, color2;

	/* Decompresia */
	if (strcmp(argv[1], "-d") == 0)
	{
		/* Citeste vectorul de compresie */
		f = fopen(argv[2], "rb");
		get_headers(&file, &info, f);
		fread(&color_count, sizeof(uint32_t), 1, f);
		fread(&node_count, sizeof(uint32_t), 1, f);
		make_array(&pixels, info.height, info.width);
		make_vector(&v, node_count);
		fread(v, sizeof(tree_vector), node_count, f);
		fclose(f);

		/* Obtine arborele cuaternar din vector si apoi matricea */
		init_tree(&root);
		root = vector_to_tree(root, v, 0);
		tree_to_array(root, pixels, 0, 0);

		/* Scrie matricea in fisier */
		f = fopen(argv[3], "wb");
		write_headers(&file, &info, f);
		write_array(pixels, info.height, info.width, f);
		fclose(f);
	}

	/* Compresia */
	if (strcmp(argv[1], "-c") == 0)
	{
		/* Citeste matricea imaginii */
		f = fopen(argv[2], "rb");
		get_headers(&file, &info, f);
		make_array(&pixels, info.height, info.width);
		get_image(pixels, info.height, info.width, f);
		fclose(f);

		/* Creeaza arborele asociat matricii */
		init_tree(&root);
		root = array_to_tree(root, pixels, 0, 0, info.height, &node_count);

		/* Creeaza vectorul de compresie si il scrie in fisier */
		make_vector(&v, node_count);
		color_count = tree_to_vector(root, v, &position);
		f = fopen(argv[3], "wb");
		write_headers(&file, &info, f);
		write_vector(v, node_count, color_count, f);
		fclose(f);
	}

	/* Rotatie in sens trigonometric */
	if (strcmp(argv[1], "-r") == 0)
	{
		/* Citeste numarul de rotatii */
		sscanf(argv[2], "%d", &rotations);

		/* Citeste matricea imaginii */
		f = fopen(argv[3], "rb");
		get_headers(&file, &info, f);
		make_array(&pixels, info.height, info.width);
		get_image(pixels, info.height, info.width, f);
		fclose(f);

		/* Construieste arborele asociat imaginii */
		init_tree(&root);
		root = array_to_tree(root, pixels, 0, 0, info.height, &node_count);

		/* Folosind arborele, roteste imaginea la 90 de grade */
		for (i = 0; i < rotations; i++)
			root = rotate(root);

		/* Scrie matricea dupa rotatie in fisier */
		tree_to_array(root, pixels, 0, 0);
		f = fopen(argv[4], "wb");
		write_headers(&file, &info, f);
		write_array(pixels, info.height, info.width, f);
		fclose(f);
	}
	if (strcmp(argv[1], "-b") == 0)
	{
		/* Citeste culorile ce trebuie cautate, folosind
			o structura */
		sscanf(argv[2], "%hhu", &color1.red);
		sscanf(argv[3], "%hhu", &color1.green);
		sscanf(argv[4], "%hhu", &color1.blue);
		sscanf(argv[5], "%hhu", &color2.red);
		sscanf(argv[6], "%hhu", &color2.green);
		sscanf(argv[7], "%hhu", &color2.blue);
		color1.reserved = 0;
		color2.reserved = 0;

		/* Citeste matricea imaginii */
		f = fopen(argv[8], "rb");
		get_headers(&file, &info, f);
		make_array(&pixels, info.height, info.width);
		get_image(pixels, info.height, info.width, f);
		fclose(f);

		/* Obtine arborele asociat matricii */
		init_tree(&root);
		root = array_to_tree(root, pixels, 0, 0, info.height, &node_count);

		/* Cauta stramosul comun al celor 2 culori */
		root = find_common_parent(root, color1, color2);

		/* Elibereaza vechea matrice */
		for (i = 0; i < info.height; i++)
			free(pixels[i]);
		free(pixels);

		/* Modifica parametrii din header */
		info.height = sqrt((double)root->area);
		info.width = sqrt((double)root->area);

		/* Contruieste matricea asociata nodului gasit */
		make_array(&pixels, info.height, info.width);
		tree_to_array(root, pixels, 0, 0);

		/* Scrie in fisier noua matrice */
		f = fopen(argv[9], "wb");
		write_headers(&file, &info, f);
		write_array(pixels, info.height, info.width, f);
		fclose(f);
	}
	for (i = 0; i < info.height; i++)
		free(pixels[i]);
	free(pixels);
	if (v != NULL)
		free(v);
	root = destroy_tree(root);
	return 0;
}