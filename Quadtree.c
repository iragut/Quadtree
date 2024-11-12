#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

// Crearea structurilor pentru arbore, coada si a structurilor ajutatoare 
typedef struct Pixeli{
    unsigned char Red;
    unsigned char Green;
    unsigned char Blue;
} Pixeli;

typedef struct Pozitia_Bloc{
    int start_x, start_y;
    int end_x, end_y;
    int size;
} Pozitia_Bloc;

typedef struct QuatricTree {
    unsigned char tip_node;
    struct Pixeli RGB;
    struct Pozitia_Bloc pozitia;
    struct QuatricTree *left_up, *right_up, *right_down, *left_down;
} QuatricTree;

typedef struct QueueNode{
	QuatricTree *node_tree;
	struct QueueNode *next;
}QueueNode;

typedef struct Queue{
	QueueNode *front;
	QueueNode *rear;
}Queue;

// Functie de creare unui nod din tree
QuatricTree* creare_leaf(){
    QuatricTree *node = (QuatricTree*)malloc(sizeof(QuatricTree));
    node->right_up = NULL;
    node->left_down = NULL;
    node->right_down = NULL;
    node->left_up = NULL;
    node->tip_node = 0;
    return node;
}

// Functie de creare unei cozi
Queue* createQueue(){
	Queue *coada = (Queue*)malloc(sizeof(Queue));
	coada->front = NULL;
	coada->rear = NULL;
	return coada;
}

// Functie de adaugare in coada
void enqueue(Queue *coada,  QuatricTree *node_tree){
	QueueNode *node = (QueueNode*)malloc(sizeof(QueueNode));
	node->node_tree = node_tree;
	node->next = NULL;

    if (coada->rear == NULL)
    {
        coada->front = node;
        coada->rear = node;
    }
    else
    {
        coada->rear->next = node;
        coada->rear = node;
    }
}

// Functie de scoatere din coada
QuatricTree* dequeue(Queue* coada){
	QueueNode *current = coada->front;
    if (current == NULL) {
        return NULL;
    }
    QuatricTree *node_tree = current->node_tree;
	coada->front = coada->front->next;
	if (coada->front == NULL){
		coada->rear = NULL;
	}
	free(current);
    return node_tree;
} 

// Functie de adaugarea in nod indicii de pozitie a submatricei reprezentate de nod
QuatricTree* add_in_leaf(QuatricTree *leaf, int size, int x, int y, int end_x, int end_y){
    leaf->pozitia.start_x = x;
    leaf->pozitia.start_y = y;
    leaf->pozitia.end_x = end_x;
    leaf->pozitia.end_y = end_y;
    leaf->pozitia.size = size;
    return leaf;
}

// Functie de adaugare in noduri valorilor rgb
QuatricTree* add_in_leaf_2(QuatricTree* leaf, int red, int green, int blue){
    leaf->tip_node = 1;
    leaf->RGB.Red = red;
    leaf->RGB.Green = green;
    leaf->RGB.Blue = blue;
    return leaf;
}

// Calcularea culorii medii din submatricea data folosint formulele date
unsigned long long find_the_mean(Pixeli **vector, int start_i, int start_j, 
    int width, int height, int size, unsigned long long *red, 
        unsigned long long *green, unsigned long long *blue){

    int i, j;
    unsigned long long mean = 0;

    for (i = start_i; i < height; i++)
    {
        for ( j = start_j; j < width; j++)
        {
            (*red) += vector[i][j].Red;
            (*green) += vector[i][j].Green;
            (*blue) += vector[i][j].Blue;
        }
    }
    (*red) = (*red) / (size * size);
    (*green) = (*green) / (size * size);
    (*blue) = (*blue) / (size * size);

    for (i = start_i; i < height; i++)
    {
        for ( j = start_j; j < width; j++)
        {
            mean += (((*red) - vector[i][j].Red) * ((*red) - vector[i][j].Red)) +
                (((*green) - vector[i][j].Green) * ((*green) - vector[i][j].Green)) +
                    (((*blue) - vector[i][j].Blue) * ((*blue) - vector[i][j].Blue));
        }
    }
    mean = mean / (3 * size * size);
    return mean;
}

// Functie de aflarea inaltimei tree-ului
int height_tree(QuatricTree* node) {
    if (node != NULL) 
    {
        int left_up = height_tree(node->left_up);
        int right_up = height_tree(node->right_up);
        int right_down = height_tree(node->right_down);
        int left_down = height_tree(node->left_down);

        int maxim = left_up;
        if (right_up > maxim){
            maxim = right_up;
        }
        if (right_down > maxim){
            maxim = right_down;
        }
        if (left_down > maxim){
            maxim = left_down;
        }
        return maxim + 1;
    }
    return 0;
}

// Calculare nodurilor frunze, se cauta in tree toate nodurile de tip 1
int number_of_block(QuatricTree* node){
    if (node != NULL)
    {
        int numarul = 0;

        if (node->tip_node == 1){
            numarul++;
        }
        numarul += number_of_block(node->left_up);
        numarul += number_of_block(node->right_up);
        numarul += number_of_block(node->right_down);
        numarul += number_of_block(node->left_down);

        return numarul;
    }
    return 0;
}

// Functie de creare tree-ului,divizare imaginei si decomprimarea ei
void image_division(QuatricTree* node, Pixeli **vector, unsigned long long mean) {
    unsigned long long mean_aux = 0;
    unsigned long long red, green, blue;

    // Creare nodurilor si adaugarea in ele indicii de pozitie pentru 
    // fiecare submatrice pe care o reprezinta
    node->left_up = creare_leaf();
    node->left_up = add_in_leaf(node->left_up , node->pozitia.size / 2,
        node->pozitia.start_x, node->pozitia.start_y, (node->pozitia.start_x
             + node->pozitia.end_x) / 2, (node->pozitia.start_y + node->pozitia.end_y) / 2);

    node->right_up = creare_leaf();
    node->right_up = add_in_leaf(node->right_up, node->pozitia.size / 2,
        node->pozitia.start_x, (node->pozitia.start_y + node->pozitia.end_y) / 2,
            (node->pozitia.start_x + node->pozitia.end_x) / 2, node->pozitia.end_y);

    node->right_down = creare_leaf();
    node->right_down = add_in_leaf(node->right_down, node->pozitia.size / 2,
        (node->pozitia.start_x + node->pozitia.end_x) / 2, (node->pozitia.start_y +
            node->pozitia.end_y) / 2, node->pozitia.end_x, node->pozitia.end_y);

    node->left_down = creare_leaf();
    node->left_down = add_in_leaf(node->left_down, node->pozitia.size / 2,
        (node->pozitia.start_x + node->pozitia.end_x) / 2, node->pozitia.start_y,
            node->pozitia.end_x, (node->pozitia.start_y + node->pozitia.end_y) / 2);

    // Calculare valorilor medii a culorilor in submatrice
    // Daca este valoarea medie e mai mica decat gradul impus se scriu valorile medii rgb in node
    red = 0, green = 0, blue = 0;
    mean_aux = find_the_mean(vector, node->left_up->pozitia.start_x, 
        node->left_up->pozitia.start_y, node->left_up->pozitia.end_y, 
            node->left_up->pozitia.end_x, node->left_up->pozitia.size , &red, &green, &blue);
    if (mean_aux > mean)
    {   
        node->left_up->tip_node = 0;
        image_division(node->left_up, vector, mean);
    }
    else{
        node->left_up = add_in_leaf_2(node->left_up, red, green, blue);
    }

    red = 0, green = 0, blue = 0;
    mean_aux = find_the_mean(vector, node->right_up->pozitia.start_x, 
        node->right_up->pozitia.start_y, node->right_up->pozitia.end_y, 
            node->right_up->pozitia.end_x, node->right_up->pozitia.size, &red, &green, &blue);
    if (mean_aux > mean)
    {
        node->right_up->tip_node = 0;
        image_division(node->right_up, vector, mean);
    }
    else{
        node->right_up = add_in_leaf_2(node->right_up, red, green, blue);
    }

    red = 0, green = 0, blue = 0;
    mean_aux = find_the_mean(vector, node->right_down->pozitia.start_x, 
        node->right_down->pozitia.start_y, node->right_down->pozitia.end_y, 
            node->right_down->pozitia.end_x, node->right_down->pozitia.size, &red, &green, &blue);
    if (mean_aux > mean)
    {
        node->right_down->tip_node = 0;
        image_division(node->right_down, vector, mean);
    }
    else{
        node->right_down = add_in_leaf_2(node->right_down, red, green, blue);
    }
    
    red = 0, green = 0, blue = 0;
    mean_aux = find_the_mean(vector, node->left_down->pozitia.start_x, 
        node->left_down->pozitia.start_y, node->left_down->pozitia.end_y, 
            node->left_down->pozitia.end_x, node->left_down->pozitia.size, &red, &green, &blue);
    if (mean_aux > mean)
    {
        node->left_down->tip_node = 0;
        image_division(node->left_down, vector, mean);
    }
    else{
        node->left_down = add_in_leaf_2(node->left_down, red, green, blue);
    }
}

// Functie de adugare in matrice a valorilor rgb
Pixeli** construct_the_image(Pixeli **vector, int start_i, int start_j, int end_i, int end_j, 
    unsigned char red, unsigned char green, unsigned char blue){
    int i, j;
    for (i = start_i; i < end_i; i++)
    {
        for ( j = start_j; j < end_j; j++)
        {
            vector[i][j].Red = red;
            vector[i][j].Green = green;
            vector[i][j].Blue = blue;
        }
    }
    return vector;
}

// Punerea indexii de pozitie a unui arbore deja construit
void put_position(QuatricTree* node){
    if (node != NULL){
        if (node->left_up != NULL){
            node->left_up = add_in_leaf(node->left_up , node->pozitia.size / 2,
                node->pozitia.start_x, node->pozitia.start_y, (node->pozitia.start_x
                    + node->pozitia.end_x) / 2, (node->pozitia.start_y + node->pozitia.end_y) / 2);
        }
        if (node->right_up != NULL){
            node->right_up = add_in_leaf(node->right_up, node->pozitia.size / 2,
                node->pozitia.start_x, (node->pozitia.start_y + node->pozitia.end_y) / 2,
                    (node->pozitia.start_x + node->pozitia.end_x) / 2, node->pozitia.end_y);
        }
        if (node->right_down != NULL){
            node->right_down = add_in_leaf(node->right_down, node->pozitia.size / 2,
                (node->pozitia.start_x + node->pozitia.end_x) / 2, (node->pozitia.start_y +
                    node->pozitia.end_y) / 2, node->pozitia.end_x, node->pozitia.end_y);
        }
        if (node->left_down != NULL){
            node->left_down = add_in_leaf(node->left_down, node->pozitia.size / 2,
                (node->pozitia.start_x + node->pozitia.end_x) / 2, node->pozitia.start_y,
                    node->pozitia.end_x, (node->pozitia.start_y + node->pozitia.end_y) / 2);
        }

        put_position(node->left_up);
        put_position(node->right_up);
        put_position(node->right_down);
        put_position(node->left_down);
    }
}

// Functie de adugarea valorilor rgb in matricea originala ce reprezint o imagine
Pixeli** put_rgb_data_in_matrix(Pixeli **Imaginea, QuatricTree *node){
    if (node == NULL){
        return Imaginea;
    }
    if (node->left_up != NULL){
        Imaginea = construct_the_image(Imaginea, node->left_up->pozitia.start_x, 
            node->left_up->pozitia.start_y, node->left_up->pozitia.end_x, 
                node->left_up->pozitia.end_y, node->left_up->RGB.Red, 
                    node->left_up->RGB.Green, node->left_up->RGB.Blue);
    }
    if (node->right_up != NULL){
        Imaginea = construct_the_image(Imaginea, node->right_up->pozitia.start_x, 
            node->right_up->pozitia.start_y, node->right_up->pozitia.end_x, 
                node->right_up->pozitia.end_y, node->right_up->RGB.Red, 
                    node->right_up->RGB.Green, node->right_up->RGB.Blue);
    }
    if (node->right_down != NULL){
        Imaginea = construct_the_image(Imaginea, node->right_down->pozitia.start_x, 
            node->right_down->pozitia.start_y, node->right_down->pozitia.end_x, 
                node->right_down->pozitia.end_y, node->right_down->RGB.Red, 
                    node->right_down->RGB.Green, node->right_down->RGB.Blue); 
    }
    if (node->left_down != NULL){
        Imaginea = construct_the_image(Imaginea, node->left_down->pozitia.start_x, 
            node->left_down->pozitia.start_y, node->left_down->pozitia.end_x, 
                node->left_down->pozitia.end_y, node->left_down->RGB.Red, 
                    node->left_down->RGB.Green, node->left_down->RGB.Blue);
    }

    Imaginea = put_rgb_data_in_matrix(Imaginea, node->left_up);
    Imaginea = put_rgb_data_in_matrix(Imaginea, node->right_up);
    Imaginea = put_rgb_data_in_matrix(Imaginea, node->right_down);
    Imaginea = put_rgb_data_in_matrix(Imaginea, node->left_down); 

    return Imaginea;
}

// Cautarea celui mai mare bloc nedevizat in tree in nivelul dat
int find_biggest_block_in_level(QuatricTree* node, int level){
    if (node != NULL)
    {   
        int size;
        if (node->tip_node == 1){
            return node->pozitia.size;
        }
        if (level > 1) 
        {
            size = find_biggest_block_in_level(node->left_up, level - 1);
            if (size != 0){
                return size;
            }
            size = find_biggest_block_in_level(node->right_up, level - 1);
            if (size != 0){
                return size;
            }
            size = find_biggest_block_in_level(node->right_down, level - 1);
            if (size != 0){
                return size;
            }
            size = find_biggest_block_in_level(node->left_down, level - 1);
            if (size != 0){
                return size;
            }
        }
    }
    return 0;
}

// Functie auxiliara pentru a gasi cel mai mare bloc nedevizat
int find_biggest_block(QuatricTree *node) {
    int i, size;
    int h = height_tree(node);

    for (i = 0; i <= h; i++)
    {
        size = find_biggest_block_in_level(node, i);
        if (size != 0){
            return size;
        }
    }
    return 0;
}

// Parcurgerea arborului pe nivel si scrierea datelor intr-un fiser binar
void write_in_binary(QuatricTree* node, int level, FILE *fisier) {
    if (node != NULL)
    {
        if (level == 0) {
            if (node->tip_node == 0)
            {
                fwrite(&node->tip_node, sizeof(unsigned char), 1, fisier);
            }
            else
            {
                fwrite(&node->tip_node, sizeof(unsigned char), 1, fisier);
                fwrite(&node->RGB.Red, sizeof(unsigned char), 1, fisier);
                fwrite(&node->RGB.Green, sizeof(unsigned char), 1, fisier);
                fwrite(&node->RGB.Blue, sizeof(unsigned char), 1, fisier);
            }
            return;
        }
        write_in_binary(node->left_up, level - 1, fisier);
        write_in_binary(node->right_up, level - 1, fisier);
        write_in_binary(node->right_down, level - 1, fisier);
        write_in_binary(node->left_down, level - 1, fisier);
    }
}

// Functie de eliberarea memorie folosite pentru tree
void free_tree(QuatricTree *node){
    if (node != NULL)
    {
        free_tree(node->left_up);
        free_tree(node->right_up);
        free_tree(node->right_down);
        free_tree(node->left_down);
        free(node);
    }
}

// Functie de creare tree-ului dintr-un fisier comprimat 
QuatricTree* decompresing_the_image(FILE *fisier, QuatricTree *Arbore){
    unsigned char tip;
    Queue *Coada = createQueue();
    QuatricTree *node ;
    // Se citeste primul element daca radacina e nod frunza se introduc datele rgb
    // Daca e tip node ,se creaza cele patru noduri si sunt trimise in coada
    fread(&tip, sizeof(unsigned char), 1, fisier);
    if (tip == 0)
    {
        Arbore = creare_leaf();
        Arbore->left_up = creare_leaf();
        Arbore->right_up = creare_leaf();
        Arbore->right_down = creare_leaf();
        Arbore->left_down= creare_leaf();
        enqueue(Coada, Arbore->left_up);
        enqueue(Coada, Arbore->right_up);
        enqueue(Coada, Arbore->right_down);
        enqueue(Coada, Arbore->left_down);
    } else {
        Arbore = creare_leaf();
        Arbore->tip_node = 1;
        fread(&Arbore->RGB.Red, sizeof(unsigned char), 1, fisier);
        fread(&Arbore->RGB.Green, sizeof(unsigned char), 1, fisier);
        fread(&Arbore->RGB.Blue, sizeof(unsigned char), 1, fisier);
        free(Coada);
        return Arbore;
    }
    node = dequeue(Coada);
    // Pana numai ramane nimic in coada se citeste fiecare nod si datele respective pentru acel node
    while (node != NULL)
    {
        fread(&tip, sizeof(unsigned char), 1, fisier);
        // Daca e de tip 0 se mai adauga in coada 4 noduri de tip tree
        if (tip == 0)
        {
            node->RGB.Red = 0;
            node->RGB.Green = 0;
            node->RGB.Blue = 0;
            node->left_up = creare_leaf();
            node->right_up = creare_leaf();
            node->right_down = creare_leaf();
            node->left_down = creare_leaf();
            enqueue(Coada, node->left_up);
            enqueue(Coada, node->right_up);
            enqueue(Coada, node->right_down);
            enqueue(Coada, node->left_down);
        }
        else
        {
            node->tip_node = 1;
            fread(&node->RGB.Red, sizeof(unsigned char), 1, fisier);
            fread(&node->RGB.Green, sizeof(unsigned char), 1, fisier);
            fread(&node->RGB.Blue, sizeof(unsigned char), 1, fisier);
        }
        node = dequeue(Coada);
    }
    free(Coada);
    return Arbore;
}

// Scrierea in fisier a matricei si a datelor pentru creare imaginei de tip ppm
void write_in_file(int size, Pixeli **Imaginea, FILE *fisier){
    int i, j;

    fprintf(fisier, "P6\n");
    fprintf(fisier, "%d ", size);
    fprintf(fisier, "%d\n", size);
    fprintf(fisier, "255\n");
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            fwrite(&Imaginea[i][j].Red, sizeof(unsigned char), 1, fisier);
            fwrite(&Imaginea[i][j].Green, sizeof(unsigned char), 1, fisier);
            fwrite(&Imaginea[i][j].Blue, sizeof(unsigned char), 1, fisier);
        }
    }
}

int main(int argc, char const *argv[]){
    FILE *fisier_in, *fisier_out;
    Pixeli **Imaginea;
    QuatricTree *Arbore;

    int i, j;
    int width, height, size, max;
    unsigned long long mean ,prag;
    unsigned long long red = 0, green = 0, blue = 0;

    char format[20];
    // Se verifica cerinta si se citeste din fiserul dat 
    if (strcmp(argv[1], "-c1") == 0 || strcmp(argv[1], "-c2") == 0)
    {
        prag = atoi(argv[2]);
        fisier_in = fopen(argv[3], "rb");
        fisier_out = fopen(argv[4], "wb");
        
        fscanf(fisier_in, "%s\n", format);
        fscanf(fisier_in, "%d", &width);
        fscanf(fisier_in, "%d\n", &height);
        fscanf(fisier_in, "%d", &max);
        fseek(fisier_in, 1, SEEK_CUR);

        Arbore = creare_leaf();
        Arbore = add_in_leaf(Arbore, height, 0 ,0 , width, height);

        // Se construieste matricea cu datele rgb din fisier
        Imaginea = (Pixeli**)malloc(width * sizeof(Pixeli*));
        for ( i = 0; i < height; i++)
        {
            Imaginea[i] = (Pixeli*)malloc(height * sizeof(Pixeli));
        }
        for (i = 0; i < height; i++)
        {
            for ( j = 0; j < width; j++)
            {
                fread(&Imaginea[i][j], sizeof(Pixeli), 1, fisier_in);
            }
        }
        // Se calculeaza media si se construieste arborele
        size = width;
        mean = find_the_mean(Imaginea, 0, 0, width, height, size, &red, &green, &blue);
        
        if (mean > prag){
            image_division(Arbore, Imaginea, prag);
        } else {
            Arbore = add_in_leaf_2(Arbore, red, green, blue);
        }
    }
    else
    {
        fisier_in = fopen(argv[2], "rb");
        fisier_out = fopen(argv[3], "wb");
    }

    // Se pun in fisier datele despre tree cerute la cerinta 1
    if (strcmp(argv[1], "-c1") == 0)
    {   
        fprintf(fisier_out, "%d\n", height_tree(Arbore));
        fprintf(fisier_out, "%d\n", number_of_block(Arbore));
        fprintf(fisier_out, "%d\n", find_biggest_block(Arbore));
    }
    else if (strcmp(argv[1], "-c2") == 0)
    {
        // Se scrie in fisierul binar datele din tree
        fwrite(&height, sizeof(unsigned int), 1, fisier_out);
        int inaltime = height_tree(Arbore);
        for ( i = 0; i <= inaltime; i++)
        {
            write_in_binary(Arbore, i, fisier_out);
        }
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
        // Crearea unei matrice de tip RGB goale
        unsigned int dimensiune;
        fread(&dimensiune, sizeof(unsigned int), 1, fisier_in);

        Imaginea = (Pixeli**)malloc(dimensiune * sizeof(Pixeli*));
        for ( i = 0; i < dimensiune; i++)
        {
            Imaginea[i] = (Pixeli*)malloc(dimensiune * sizeof(Pixeli));
        }
        // Crearea tree-ului si scrierea in matrice a datelor din tree
        Arbore = decompresing_the_image(fisier_in, Arbore);
        Arbore = add_in_leaf(Arbore, dimensiune, 0, 0, dimensiune, dimensiune);
        Imaginea = construct_the_image(Imaginea, Arbore->pozitia.start_x, 
            Arbore->pozitia.start_y, Arbore->pozitia.end_x, Arbore->pozitia.end_y, 
                Arbore->RGB.Red, Arbore->RGB.Green, Arbore->RGB.Blue); 
        put_position(Arbore);
        Imaginea = put_rgb_data_in_matrix(Imaginea, Arbore);

        write_in_file(dimensiune, Imaginea, fisier_out);
        height = dimensiune;
    }
    
    // Eliberarea memoriei folosite 
    for (i = 0; i < height; i++) 
    {
        free(Imaginea[i]);
    }

    free(Imaginea);
    free_tree(Arbore);
    fclose(fisier_in);
    fclose(fisier_out);
	return 0;
}