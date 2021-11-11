#include "TextureInterface.h"
#include <utility>

TextureInterface::TextureInterface(){
    create();
}

TextureInterface::~TextureInterface() {
    remove();
}

TextureInterface::TextureInterface(TextureInterface &&toMove) {
    remove();
    textureID = std::exchange(toMove.textureID, 0);
}

TextureInterface &TextureInterface::operator=(TextureInterface &&toMove) {
    remove();
    textureID = std::exchange(toMove.textureID, 0);
    return *this;
}

GLuint TextureInterface::getTextureID() {
    return textureID;
}

void TextureInterface::create() {
    glGenTextures(1, &textureID);
}

void TextureInterface::remove() {
    glDeleteTextures(1, &textureID);
    textureID = 0;
}