#include "bmp_compress.h"

/* Functii pentru lucrul cu un arbore cuaternar */

void init_tree(tnode **root)
{
	*root = NULL;
}

/* Creeaza un arbore cuaternar pentru matricea imaginii BMP
	si returneaza radacina acestuia si numarul de noduri */
tnode* array_to_tree(tnode *root, image *a[], uint32_t n, uint32_t m, uint32_t size, uint32_t *node_count)
{
	root = make_tnode();
	root->area = size * size;
	(*node_count)++;
	if (same_color(a, n, m, n + size, m + size) == 1)
	{
		root->color = a[n][m];		
	}
	else
	{
		root->topleft = array_to_tree(root->topleft, a, n, m, size/2, node_count);
		root->topright = array_to_tree(root->topright, a, n, m + size/2, size/2, node_count);
		root->botright = array_to_tree(root->botright, a, n + size/2, m + size/2, size/2, node_count);
		root->botleft = array_to_tree(root->botleft, a, n + size/2, m, size/2, node_count);
	}
	return root;
}

/* Construieste matricea imaginii BMP din arborele
	cuaternar asociat acesteia */
void tree_to_array(tnode *root, image *a[], uint32_t n, uint32_t m)
{
	uint32_t i, j;
	uint32_t size = sqrt((double)root->area);
	if (root->topleft == NULL)
	{
		for (i = n; i < n + size; i++)
			for (j = m; j < m + size; j++)
				a[i][j] = root->color;
		return;
	}
	tree_to_array(root->topleft, a, n, m);
	tree_to_array(root->topright, a, n, m + size/2);
	tree_to_array(root->botright, a, n + size/2, m + size/2);
	tree_to_array(root->botleft, a, n + size/2, m);
}

/* Construieste arborele asociat imaginii BMP pornind de la
	vectorul de compresie */
tnode* vector_to_tree(tnode *root, tree_vector *v, int32_t pos)
{
	root = make_tnode();
	root->area = v[pos].area;
	if (v[pos].topleft == -1)
		root->color = v[pos].color;
	else
	{
		root->topleft = vector_to_tree(root->topleft, v, v[pos].topleft);
		root->topright = vector_to_tree(root->topright, v, v[pos].topright);
		root->botright = vector_to_tree(root->botright, v, v[pos].botright);
		root->botleft = vector_to_tree(root->botleft, v, v[pos].botleft);
	}
	return root;
}

/* Verifica daca o zona din matrice are o singura culoare*/
int same_color(image *a[], uint32_t n1, uint32_t m1, uint32_t n2, uint32_t m2)
{
	uint32_t i, j;
	for (i = n1; i < n2; i++)
		for (j = m1; j < m2; j++)
			if ((a[i][j].blue != a[n1][m1].blue) || (a[i][j].green != a[n1][m1].green) ||
				(a[i][j].red != a[n1][m1].red))
				return 0;
	return 1;
}

tnode* make_tnode()
{
	tnode *new = (tnode*)calloc(1, sizeof(tnode));
	new->area = 0;
	new->topleft = NULL;
	new->botleft = NULL;
	new->botright = NULL;
	new->topright = NULL;
	return new;
}

/* Construieste vectorul de compresie din arbore, returnand numarul
	de noduri frunza din arbore */
uint32_t tree_to_vector(tnode *root, tree_vector *v, uint32_t *pos)
{
	uint32_t tmp = *pos;
	uint32_t color_count = 0;
	v[tmp].color = root->color;
	v[tmp].area = root->area;
	if (root->topleft == NULL)
	{
		v[tmp].topleft = -1;
		v[tmp].topright = -1;
		v[tmp].botright = -1;
		v[tmp].botleft = -1;
		color_count++;
	}
	else
	{
		v[tmp].topleft = ++(*pos); 
		color_count += tree_to_vector(root->topleft, v, pos);
		v[tmp].topright = ++(*pos);
		color_count += tree_to_vector(root->topright, v, pos);
		v[tmp].botright = ++(*pos);
		color_count += tree_to_vector(root->botright, v, pos);
		v[tmp].botleft = ++(*pos);
		color_count += tree_to_vector(root->botleft, v, pos);
	}
	return color_count;
}

/* Roteste imaginea asociata arborelui la 90 de grade */
tnode* rotate(tnode *root)
{
	tnode *aux;
	if (root->topleft != NULL)
	{
		aux = root->botleft;
		root->botleft = root->topleft;
		root->topleft = root->topright;
		root->topright = root->botright;
		root->botright = aux;
		root->topleft = rotate(root->topleft);
		root->topright = rotate(root->topright);
		root->botright = rotate(root->botright);
		root->botleft = rotate(root->botleft);
	}
	return root;
}

/* Verifica daca o zona din imagine contine o anumita culoare */
int contains_color(tnode *root, image color)
{
	if (root->topleft == NULL)
	{	
		if ((root->color.blue == color.blue) && (root->color.green == color.green)
			&& (root->color.red == color.red))
			return 1;
		return 0;
	}
	if (contains_color(root->topleft, color) == 1)
		return 1;
	if (contains_color(root->topright, color) == 1)
		return 1;
	if (contains_color(root->botright, color) == 1)
		return 1;
	if (contains_color(root->botleft, color) == 1)
		return 1;
	return 0;
}

/* Returneaza stramosul comun al celor 2 culori */
tnode* find_common_parent(tnode *root, image color1, image color2)
{
	if ((contains_color(root, color1)) && (contains_color(root, color2)))
	{
		if ((contains_color(root->topleft, color1)) && (contains_color(root->topleft, color2)))
			return find_common_parent(root->topleft, color1, color2);
		if ((contains_color(root->topright, color1)) && (contains_color(root->topright, color2)))
			return find_common_parent(root->topright, color1, color2);
		if ((contains_color(root->botright, color1)) && (contains_color(root->botright, color2)))
			return find_common_parent(root->botright, color1, color2);
		if ((contains_color(root->botleft, color1)) && (contains_color(root->botleft, color2)))
			return find_common_parent(root->botleft, color1, color2);
		return root;
	}
	else
		return NULL;
}

tnode* destroy_tree(tnode *root)
{
	if (root->topleft == NULL)
	{
		free(root);
		return NULL;
	}
	root->topleft = destroy_tree(root->topleft);
	root->topright = destroy_tree(root->topright);
	root->botright = destroy_tree(root->botright);
	root->botleft = destroy_tree(root->botleft);
	free(root);
	return NULL;
}