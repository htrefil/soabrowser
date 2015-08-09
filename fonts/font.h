#ifndef font_h
#define font_h

#include <string>


class TextureFont {
public:
    int w, h;
    double fcw, fch;
    unsigned int id;

    void Load();
    void LoadData(int, int, short *, int, int, int);
    void DrawChar(unsigned char, double, double);
    void PaintText(const std::string &t, double x, double y);
};

void CreateUbuntuMono6x12(TextureFont &);
void CreateUbuntuMono7x15(TextureFont &);
void CreateUbuntuMono8x16(TextureFont &);

#endif
