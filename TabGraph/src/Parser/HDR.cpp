/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 19:37:50
*/

#include "Parser/HDR.hpp"
#include "Texture/Texture2D.hpp" // for Texture2D
#include "Texture/TextureParser.hpp" // for TextureParser
#include <GL/glew.h> // for GLubyte, GL_FLOAT, GL_R11F_G11F_B10F
#include <glm/glm.hpp> // for s_vec2, glm::vec2
#include <iostream> // for operator<<, flush, basic_ostream, cout
#include <math.h> // for pow
#include <stdexcept> // for runtime_error
#include <stdio.h> // for getc, fclose, fread, feof, fseek, FILE
#include <string.h> // for memcmp, memcpy

typedef unsigned char RGBE[4];
#define R 0
#define G 1
#define B 2
#define E 3

#define MINELEN 8 // minimum scanline length for encoding
#define MAXELEN 0x7fff // maximum scanline length for encoding

static void workOnRGBE(RGBE* scan, int len, float* cols);
static bool decrunch(RGBE* scanline, int len, FILE* file);
static bool oldDecrunch(RGBE* scanline, int len, FILE* file);

//Add this parser to TextureParser !
auto __hdrParser = TextureParser::Add(".hdr", HDR::parse);

std::shared_ptr<Texture2D> HDR::parse(const std::string& texture_name, const std::string& path)
{
    std::cout << "Parsing " << texture_name;
    int i;
    char str[200];
    FILE* file;
    glm::ivec2 size;

    file = fopen(path.c_str(), "rb");
    if (!file)
        throw std::runtime_error("Invalid File");
    fread(str, 10, 1, file);
    if (memcmp(str, "#?RADIANCE", 10)) {
        fclose(file);
        throw std::runtime_error("Invalid Header");
    }
    fseek(file, 1, SEEK_CUR);
    i = 0;
    char c = 0, oldc;
    while (true) {
        oldc = c;
        c = getc(file);
        if (c == 0xa && oldc == 0xa)
            break;
    }
    std::cout << "." << std::flush;
    char reso[200] { 0 };
    i = 0;
    while (true) {
        c = getc(file);
        reso[i++] = c;
        if (c == 0xa)
            break;
    }
    std::cout << "." << std::flush;
    long w, h;
    if (!sscanf(reso, "-Y %ld +X %ld", &h, &w)) {
        fclose(file);
        throw std::runtime_error("Invalid Resolution");
    }
    size.x = w;
    size.y = h;
    auto cols = new float[w * h * 3];
    void* data = static_cast<void*>(cols);
    RGBE* scanline = new RGBE[w];
    if (!scanline) {
        fclose(file);
        throw std::runtime_error("Invalid Scanline");
    }
    std::cout << "." << std::flush;
    // convert image
    for (int y = 0; y < h; y++) {
        if (decrunch(scanline, w, file) == false)
            break;
        workOnRGBE(scanline, w, cols);
        cols += w * 3;
    }
    std::cout << "." << std::flush;

    delete[] scanline;
    fclose(file);
    auto t = Texture2D::Create(texture_name, size, GL_TEXTURE_2D, GL_RGB, GL_R11F_G11F_B10F, GL_FLOAT, data);
    std::cout << " Done." << std::endl;
    return (t);
}

float convertComponent(int expo, int val)
{
    float v = val / 256.0f;
    float d = (float)pow(2, expo);
    return v * d;
}

void workOnRGBE(RGBE* scan, int len, float* cols)
{
    while (len-- > 0) {
        int expo = scan[0][E] - 128;
        cols[0] = convertComponent(expo, scan[0][R]);
        cols[1] = convertComponent(expo, scan[0][G]);
        cols[2] = convertComponent(expo, scan[0][B]);
        cols += 3;
        scan++;
    }
}

bool decrunch(RGBE* scanline, int len, FILE* file)
{
    int i, j;

    if (len < MINELEN || len > MAXELEN)
        return oldDecrunch(scanline, len, file);
    i = getc(file);
    if (i != 2) {
        fseek(file, -1, SEEK_CUR);
        return oldDecrunch(scanline, len, file);
    }
    fread(&scanline[0][G], sizeof(GLubyte), 2, file);
    i = getc(file);
    if (scanline[0][G] != 2 || scanline[0][B] & 128) {
        scanline[0][R] = 2;
        scanline[0][E] = i;
        return oldDecrunch(scanline + 1, len - 1, file);
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < len;) {
            GLubyte code = getc(file);
            if (code > 128) {
                code &= 127;
                GLubyte val = getc(file);
                while (code--)
                    scanline[j++][i] = val;
            }
            else {
                std::vector<GLubyte> vals(code);
                fread(vals.data(), sizeof(GLubyte), code, file);
                auto k = 0;
                while (code--) {
                    scanline[j++][i] = vals.at(k++);
                }
            }
        }
    }

    return feof(file) ? false : true;
}

bool oldDecrunch(RGBE* scanline, int len, FILE* file)
{
    int i;
    int rshift = 0;

    while (len > 0) {
        fread(&scanline[0][R], sizeof(unsigned char), 4, file);
        if (feof(file))
            return false;
        if (scanline[0][R] == 1 && scanline[0][G] == 1 && scanline[0][B] == 1) {
            for (i = scanline[0][E] << rshift; i > 0; i--) {
                memcpy(&scanline[0][0], &scanline[-1][0], 4);
                scanline++;
                len--;
            }
            rshift += 8;
        } else {
            scanline++;
            len--;
            rshift = 0;
        }
    }
    return true;
}
