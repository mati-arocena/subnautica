#include "Texture.h"

Texture::Texture(const char* texturePath, bool alpha, bool minmap)
{
    ID = NULL; // Initialization
    unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

    this->alpha = alpha;
    this->minmap = minmap;


    if (data)
    {
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        int color_type = alpha ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, color_type, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        if (minmap)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
}

void Texture::use(int pos) const
{
    pos += GL_TEXTURE0;
    glActiveTexture(pos);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::use() const
{
    Texture::use(0);
}