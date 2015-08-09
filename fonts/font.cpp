#include "fonts/font.h"
#include <qgl.h>


void TextureFont::Load() {
    id = 1;
    CreateUbuntuMono7x15(*this);
}

void TextureFont::LoadData(int _w, int _h, short *codedBytes, int l, int iw, int ih) {

    w = _w;
    h = _h;

    fcw = w / (double)iw;
    fch = h / (double)ih;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	// GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    unsigned char *bytes = new unsigned char[iw * ih * 4];

    int k = 0;
    for (int i = 0; i < l; ++i) {

        if (codedBytes[i] > 255)
            for (int j = 0; j < codedBytes[i] - 255; ++j) {
                bytes[k] = bytes[k + 1] = bytes[k + 2] = 255;
                bytes[k + 3] = 0;
                k += 4;
            }
        else {
            bytes[k] = bytes[k + 1] = bytes[k + 2] = 255;
            bytes[k + 3] = codedBytes[i];
            k += 4;
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iw, ih, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    delete[] bytes;
}

void TextureFont::DrawChar(unsigned char c, double i, double j) {
    double tx1 = (c - 32) * fcw;
    double tx2 = tx1 + fcw;

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2d(tx1, 0.0);
    glVertex2d(i, j);
    glTexCoord2d(tx1, fch);
    glVertex2d(i, j + h);
    glTexCoord2d(tx2, fch);
    glVertex2d(i + w, j + h);
    glTexCoord2d(tx2, 0.0);
    glVertex2d(i + w, j);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void TextureFont::PaintText(const std::string &t, double x, double y) {
    for (std::string::const_iterator c = t.begin(); c != t.end(); ++c) {
        DrawChar(*c, (int)x, (int)y);
        x += w;
    }
}
