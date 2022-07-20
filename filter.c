#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Hargun Bedi
   400185463
   bedih
*/

typedef struct {
    unsigned char r, g, b;
} RGB;

typedef struct {
    int col, rows, max_value;
    RGB *image;
} PPM;

PPM *readPPM(char *filename) {

    FILE *fp; 
    if ((fp = fopen(filename, "r")) == NULL) {
        printf("Can't open the file");
        exit(EXIT_FAILURE);

    }
    char buff[16];

            if (!fgets(buff, sizeof(buff), fp)) {
              exit(1);
         }
    if (buff[0] != 'P' || buff[1] != '3') {
         fprintf(stderr, "Invalid image format (must be 'P6')\n");
         exit(1);
    }
    int rows, col, max ;
    fscanf(fp, "%d %d %d", &col, &rows, &max);
    int r, g, b;
    RGB *image = malloc(rows*col*sizeof(RGB));
    int j;

    for (j = 0 ; j < rows * col ; j++) {
        fscanf(fp, "%d %d %d ", &r, &g, &b);
        image[j].r = (unsigned char )r;
        image[j].g = (unsigned char )g;
        image[j].b = (unsigned char ) b;
    }
    PPM *ppm = malloc(sizeof(PPM));
    ppm->rows = rows;
    ppm->col = col;
    ppm->max_value = max;
    ppm->image = image;
    fclose(fp);
    return ppm;

}

double* flip(double* kernel, int size){
    double* new_kernel = malloc(size * size * sizeof(double));
    int i;
    for(i = 0; i < size*size; i ++){
        new_kernel[i] = kernel[(size*size)-1-i];
    }
    return new_kernel;
}

RGB* convolution(PPM* input_file, double* kernel, int size, int factor){
    double* flipped_kernel = flip(kernel, size);
    int zeros = (size)/2;
    int top_bottom_zeros = 2*zeros;
    int rows = input_file->rows;
    int col = input_file->col;
    int a2 = rows*col + (2*((zeros)*(rows + top_bottom_zeros))+(col*top_bottom_zeros)); 
    int new_col = col+top_bottom_zeros;
    RGB* newarray = malloc(a2*sizeof(RGB));
    int element = 0, element_real = 0;
    int n, m, l, k, g, h;
    for(n = 0; n < zeros*new_col; n ++) {
        newarray[n].r = (unsigned char ) 0;
        newarray[n].g = (unsigned char ) 0;
        newarray[n].b = (unsigned char ) 0;
        element++;
    }
    for (m = 0; m < rows; m++ ){ 
        for(g = 0; g < zeros; g ++){
            newarray[element].r = (unsigned char ) 0;
            newarray[element].g = (unsigned char ) 0;
            newarray[element].b = (unsigned char ) 0;
            element++;
        }
        for (l = 0; l < col; l++ ){ 
            newarray[element].r = input_file->image[element_real].r;
            newarray[element].g = input_file->image[element_real].g;
            newarray[element].b = input_file->image[element_real].b;
            element++;
            element_real++;
        }
        for(k = 0; k < zeros; k ++){
            newarray[element].r = (unsigned char ) 0;
            newarray[element].g = (unsigned char ) 0;
            newarray[element].b = (unsigned char ) 0;
            element++;
        }
    }
    for(h = element; h < zeros*new_col; h ++) {
        newarray[h].r = (unsigned char ) 0;
        newarray[h].g = (unsigned char ) 0;
        newarray[h].b = (unsigned char ) 0;
    }
    element += new_col;
    RGB* conv_array = malloc(rows*col*sizeof(RGB));
    int t;
    for(t = 0; t<rows*col; t ++) {
        conv_array[t].r = (unsigned char) 0;
        conv_array[t].g = (unsigned char) 0;
        conv_array[t].b = (unsigned char) 0;
    }

    int element_zero_padded = new_col*zeros;
    int i, j, c, d;
    for(i = 0; i < rows; i ++) {
        element_zero_padded += zeros; 
        for(j = 0; j<col; j++) {
            int element_new = i*col + j;
            int countc = 0;
            double accumlator_r = 0, accumlator_g = 0, accumlator_b = 0;
            for(c = -1*zeros; c <= zeros; c ++ ){
                int center =  element_zero_padded + new_col*c;
                int countd = 0;
                for(d = -1*zeros; d <= zeros; d++ ){
                    center += d;

                    accumlator_r += (newarray[center].r * flipped_kernel[countc*size + countd]);
                    accumlator_g += (newarray[center].g * flipped_kernel[countc*size + countd]);
                    accumlator_b +=  (newarray[center].b * flipped_kernel[countc*size + countd]);
                    center -= d;
                    countd++;
                }
                countc++;
                center -= new_col*c;
            }

            if (accumlator_r <= 0){
                accumlator_r = 0;
            } else if(accumlator_r >= 255) {
                accumlator_r = 255;
            }

            if (accumlator_g <= 0){
                accumlator_g = 0;
            } else if(accumlator_g >= 255) {
                accumlator_g = 255;
            }

            if (accumlator_b <= 0){
                accumlator_b = 0;
            } else if(accumlator_b >= 255) {
                accumlator_b = 255;
            }
            conv_array[element_new].r = (unsigned char) accumlator_r;
            conv_array[element_new].g = (unsigned char) accumlator_g;
            conv_array[element_new].b = (unsigned char) accumlator_b;
            element_zero_padded += 1; 
        }
        element_zero_padded += zeros;
    }

    free(flipped_kernel);
    free(newarray);
return conv_array;

}

int max(RGB* array, int rows, int col) {
    int location = 0;
    int max_val = 0;
    int c;
    for (c = 1; c < rows*col; c++){
        if (array[c].r > array[location].r)
            location = c;
        if(array[location].r == 255)
            break; 
    }
    max_val = array[location].r;
    if(max_val != 255) {
        int e;
        for (e = 1; e < rows*col; e++){
            if (array[e].g > array[location].g)
                location = e;
            if(array[location].g == 255)
                break;
            }
        max_val = array[location].g;
        if (max_val != 255) {
            int r;
            for (r = 1; r < rows*col; r++){
                if (array[r].b > array[location].b)
                    location = r;
                if(array[location].b == 255)
                    break;
                }
        max_val = array[location].b;
        }
    }
    return max_val;
}

void writePPM(char *filename, PPM *ppm, RGB* convoluted_array, int max_val) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL){
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "P3\n");
    fprintf(fp, "%d %d\n%d\n", ppm->col, ppm->rows, max_val);
    
    int i;
    for (i = 0 ; i < ppm->rows*ppm->col ; i++) {
        fprintf(fp, "%d %d %d ", convoluted_array[i].r, convoluted_array[i].g,convoluted_array[i].b);
    }
    fclose(fp);
    free(ppm->image);
    free(ppm);
    free(convoluted_array);
}

int main(int argc, char **argv){

    if (argc != 4) {
        printf("usage: inputfile.ppm kernel outpfile.ppm");
        return 1;
    }
    char *input = argv[1];
    char *kernel = argv[2];
    char *output = argv[3];

    FILE *file;

    int array_size = 0, factor = 0, count = 0;

    if ((file = fopen(kernel, "r")) == NULL) {
        printf("Can't open the file");
        exit(EXIT_FAILURE);
    }
    char s[1000];
    array_size = atoi(fgets(s, sizeof(s), file));
    factor = atoi(fgets(s, sizeof(s), file));
    double *kernel_array = malloc(array_size * array_size * sizeof(double));
    while(fgets(s, sizeof(s), file)){
        char *line = s;
        char *Token;
        while((Token = strtok(line, " "))) {
            line = NULL;
            int element = atoi(Token);
            double factor_mult;
            if(factor == 1){
                factor_mult = 1.0;
            } else {
                factor_mult = 1.0/factor;
            }
            kernel_array[count++] = (factor_mult)*element;   
        }
    }
    fclose(file);
    PPM *input_ppm = readPPM(input);
    RGB* convoluted = convolution(input_ppm, kernel_array, array_size, factor);
    int max_value = max(convoluted, input_ppm->rows, input_ppm->col);
    writePPM(output, input_ppm, convoluted, max_value);
    return 0;

}
