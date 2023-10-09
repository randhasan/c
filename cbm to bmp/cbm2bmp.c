#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> //for free()
#include <string.h> //for strcmp()

#pragma pack(1) //makes sure theres no padding between elements
struct BMPHeader { //14 bytes
    uint8_t formatIdentifier[2]; //magic num
    uint32_t fileSize; //size of file in bytes
    uint16_t reservedValue1; //a 2 byte reserved value
    uint16_t reservedValue2; //another 2 byte reserved value
    uint32_t offset; //offset to the start of the pixel array
};

struct DIBHeader {
    uint32_t dibSize; //size of DIB header in bytes (40)
    uint32_t width; //width of img in pixels (num of pixels per row)
    uint32_t height; //height of img in pixels (num of rows)
    uint16_t numPlanes; //number of color planes (1)
    uint16_t bitsPerPixel; //number of bits per pixel (24)
    uint32_t compScheme; //compression scheme used
    uint32_t imgSize; //image size in bytes
    uint32_t horzRes; //horizontal resolution
    uint32_t vertRes; //vertical resolution
    uint32_t numColors; //number of colors in the palette
    uint32_t numImportantColors; //number of important colors
};

struct Pixel{ //each pixel of a 24 bit color is 3 8-bit ints representing blue, green, and red intensity on a scale of 0-255
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};

struct CBMHeader {
    uint16_t formatIdentifierCBM; //formate identifier (magic number)
    uint32_t widthCBM; //width of img in pixels
    uint32_t heightCBM; //height of the img in pixels
    uint32_t numColorsCBM; //num of colors in the palette
    uint32_t paletteOffsetCBM; //offset of the palette in the file
    uint32_t arrOffsetCBM; //offset of the img array in the file
};

struct Color { //represents each color in the palette (an array of colors)
    uint8_t redCBM;
    uint8_t greenCBM;
    uint8_t blueCBM;
};
#pragma pack(4) //forces alignment to be a multiple of 1B which will prob make memory access slower but the struct will be compact.  allows you to read a struct from a file in one go, rather than element-by-element

void displayCBMInfo(const char* cbmFileName)
{
    FILE *cbmFile = fopen(cbmFileName, "r"); //open file for reading

    if (!cbmFile) { //error msg if file DNE
        printf("Error: Failed to open the CBM file");
        return;
    }

    struct CBMHeader cbmHeader;
    fread(&cbmHeader, sizeof(struct CBMHeader), 1, cbmFile); //read CBM file header

    if (cbmHeader.formatIdentifierCBM != 0x7449) { //must have format 0x7449
        fclose(cbmFile);
        printf("The format is not supported.\n");
        return;
    }

    printf("=== CBM Header ===\n");
    printf("Magic: 0x%04X\n", cbmHeader.formatIdentifierCBM); //4 character uppercase hexadecimal value (for 7449)
    printf("Width: %u\n", cbmHeader.widthCBM);
    printf("Height: %u\n", cbmHeader.heightCBM);
    printf("Number of colors: %u\n", cbmHeader.numColorsCBM);
    printf("Color array offset: %u\n", cbmHeader.paletteOffsetCBM);
    printf("Image array offset: %u\n", cbmHeader.arrOffsetCBM);

    struct Color *palette = malloc(sizeof(struct Color) * cbmHeader.numColorsCBM); //allocate memory for the palette.  multiplies size of one strut Color element by the number of colors in the palette
    if (!palette) {
        printf("Error: Failed to allocate memory for palette");
        fclose(cbmFile);
        return;
    }

    fseek(cbmFile, cbmHeader.paletteOffsetCBM, SEEK_SET); //move pointer to palette
    fread(palette, sizeof(struct Color), cbmHeader.numColorsCBM, cbmFile);

    printf("=== Palette (R, G, B) ===\n");
    for (uint32_t i = 0; i < cbmHeader.numColorsCBM; i++) {
        printf("Color %u: (%u, %u, %u)\n", i, palette[i].redCBM, palette[i].greenCBM, palette[i].blueCBM);
    }

    free(palette); //deallocation of memory
    fclose(cbmFile); //always close files
}

void displayBMPInfo(const char* cbmFileName)
{
    //.bmp file has the same file name as the corresponding .cbm file just a different extension
    char bmpFileName[256];  //create enough space for potential bmp file name
    int i;
    for (i = 0; i < strlen(cbmFileName); i++) { //process to remove .cbm extension and replace with .bmp
        if (cbmFileName[i] == '.' && strncmp(&cbmFileName[i], ".cbm", 4) == 0) {
            break; //found ".cbm" so exit the loop
        }
        bmpFileName[i] = cbmFileName[i]; //just store the file name (no extension)
    }
    strcpy(&bmpFileName[i], ".bmp"); //add bmp extension

    FILE *bmpFile = fopen(bmpFileName, "r"); //try opening the .bmp file which SHOULD have been created from the convert function
    if (!bmpFile) {
        printf("Failed to open the BMP file");
        return;
    }

    struct BMPHeader bmpHeader; //read the BMP file header
    fread(&bmpHeader, sizeof(struct BMPHeader), 1, bmpFile);

    printf("=== BMP Header ===\n");
    printf("Type: %c%c\n", bmpHeader.formatIdentifier[0], bmpHeader.formatIdentifier[1]);
    printf("Size: %u\n", bmpHeader.fileSize);
    printf("Reserved 1: %u\n", bmpHeader.reservedValue1);
    printf("Reserved 2: %u\n", bmpHeader.reservedValue2);
    printf("Image offset: %u\n", bmpHeader.offset);

    struct DIBHeader dibHeader; //read the DIB header
    fread(&dibHeader, sizeof(struct DIBHeader), 1, bmpFile);

    printf("\n=== DIB Header ===\n");
    printf("Size: %u\n", dibHeader.dibSize);
    printf("Width: %u\n", dibHeader.width);
    printf("Height: %u\n", dibHeader.height);
    printf("# color planes: %u\n", dibHeader.numPlanes);
    printf("# bits per pixel: %u\n", dibHeader.bitsPerPixel);
    printf("Compression scheme: %u\n", dibHeader.compScheme);
    printf("Image size: %u\n", dibHeader.imgSize);
    printf("Horizontal resolution: %u\n", dibHeader.horzRes);
    printf("Vertical resolution: %u\n", dibHeader.vertRes);
    printf("# colors in palette: %u\n", dibHeader.numColors);
    printf("# important colors: %u\n", dibHeader.numImportantColors);

    fclose(bmpFile); //always close file
}

void convertCBMToBMP(const char* cbmFilename, const char* bmpFilename)
{
    FILE *cbmFile = fopen(cbmFilename, "r"); //try opening the CBM file
    if (!cbmFile) {
        printf("Error: Failed to open the CBM file");
        return;
    }

    struct CBMHeader cbmHeader;
    fread(&cbmHeader, sizeof(struct CBMHeader), 1, cbmFile); //read the CBM file header

    if (cbmHeader.formatIdentifierCBM != 0x7449) { //check the CBM format identifier- format must be 0x7449
        fclose(cbmFile);
        printf("Error: Invalid CBM file format\n");
        return;
    }

    struct Color *palette = (struct Color *)malloc(sizeof(struct Color) * cbmHeader.numColorsCBM); //try allocating memory for the color palette using malloc
    if (!palette) {
        printf("Error: Failed to allocate memory for palette");
        fclose(cbmFile);
        return;
    }

    fseek(cbmFile, cbmHeader.paletteOffsetCBM, SEEK_SET); //move file pointer to the beginning of the color palette
    fread(palette, sizeof(struct Color), cbmHeader.numColorsCBM, cbmFile); //read color palette from file

    FILE* bmpFile = fopen(bmpFilename, "w"); //open BMP file for writing
    if (!bmpFile) {
        printf("Error: Failed to create the BMP file");
        free(palette); //free allocated memory before returning... good to do in the event that even if the bmp file isn't made or written, that memory for the color palette will still be freed (deallocated) for other use.
        fclose(cbmFile);
        return;
    }

    struct BMPHeader fileHeader; //initialize values as told
    fileHeader.formatIdentifier[0] = 'B';
    fileHeader.formatIdentifier[1] = 'M'; //must be uppercase also note that no NUL terminator bc it's not a proper string
    fileHeader.reservedValue1 = 0;
    fileHeader.reservedValue2 = 0; //neither of these fields are needed
    fileHeader.offset = sizeof(struct BMPHeader) + sizeof(struct DIBHeader);

    struct DIBHeader dibHeader; //initialize values as told
    dibHeader.dibSize = 40;
    dibHeader.width = cbmHeader.widthCBM;
    dibHeader.height = cbmHeader.heightCBM;
    dibHeader.numPlanes = 1;
    dibHeader.bitsPerPixel = 24;
    dibHeader.compScheme = 0;
    dibHeader.imgSize = 0;
    dibHeader.horzRes = 0;
    dibHeader.vertRes = 0;
    dibHeader.numColors = 0;
    dibHeader.numImportantColors = 0;

    //calculate row padding
    int bytesPerPixel = dibHeader.bitsPerPixel / 8; // BMP uses 3 bytes per pixel (24 bit color)... 1 byte = 8 bits
    int rowSize = cbmHeader.widthCBM * bytesPerPixel; //row size in bytes = width of img * bytes per pixel
    int padding = (4 - (rowSize % 4)) % 4; //ensures each row's size is a multiple of 4 bytes
    uint8_t paddingBytes[4] = {0}; //an array of 4 bytes initialized w zeros to represent padding bytes

    fileHeader.fileSize = sizeof(struct BMPHeader) + sizeof(struct DIBHeader) + (dibHeader.width * dibHeader.height * sizeof(struct Pixel)) + (dibHeader.height * padding); //total size of bmp file in bytes (includes size of headers, DIB header, and pixel data + padding to align rows)

    //write BMP and DIB headers
    fwrite(&fileHeader, sizeof(struct BMPHeader), 1, bmpFile); //write BMP file header
    fwrite(&dibHeader, sizeof(struct DIBHeader), 1, bmpFile);  //write DIB header


  
    struct Pixel pixel;
    //iterate through CBM rows from bottom to top
    for (int i = cbmHeader.heightCBM - 1; i >= 0; i--) {
        fseek(cbmFile, cbmHeader.arrOffsetCBM + i * cbmHeader.widthCBM, SEEK_SET); //sets file pointer to offset. first pixel + (curr row * width of img in pixels.) multiplying i by width gives offset within the img dats where curr row starts 

        for (int j = 0; j < cbmHeader.widthCBM; j++) {
            uint8_t cbmIndex; //iterate thru CBM palette colors
            fread(&cbmIndex, 1, 1, cbmFile); //read palette color from CBM file

            //convert CBM index to BMP color using the palette
            if (cbmIndex >= cbmHeader.numColorsCBM) { //handle invalid CBM index... out of bounds
                pixel.blue = 0;
                pixel.green = 0;
                pixel.red = 0;
            } else { //convert CBM palette color to BMP pixel
                pixel.blue = palette[cbmIndex].blueCBM;
                pixel.green = palette[cbmIndex].greenCBM;
                pixel.red = palette[cbmIndex].redCBM;
            }
            fwrite(&pixel, sizeof(struct Pixel), 1, bmpFile); //write pixel to BMP file
        }
        fwrite(paddingBytes, 1, padding, bmpFile); //write padding bytes to ensure each row is a multiple of 4 bytes
    }
    fclose(bmpFile); //always close the BMP file
}

//argc is the argument count: tells you how many arguments your program received.  should be >= 1
//argv[] is the argument vector: it is an array of strings, each being a different argument
int main(int argc, char *argv[]) { 
    if (argc < 2) { 
        printf("Error: Incorrect # of arguments.");
        return 1;
    }

    const char *command = argv[1]; //--
    const char *cbmFile = NULL;
    const char *bmpFile = NULL;

    if (argc >= 3) { //--info or --bmp-info
        cbmFile = argv[2];
    }

    if (argc == 4) { //--convert
        bmpFile = argv[3];
    }

    if (strcmp(command, "--info") == 0) { //compare what user writes in terminal with these if-statements.  strcmp == 0 if they match
        displayCBMInfo(cbmFile);
    } else if (strcmp(command, "--bmp-info") == 0) {
        displayBMPInfo(cbmFile);
    } else if (strcmp(command, "--convert") == 0){
        convertCBMToBMP(cbmFile, bmpFile);
    } else {
        printf("Error: Invalid command.\n");
        return 1;
    }
    return 0;
}