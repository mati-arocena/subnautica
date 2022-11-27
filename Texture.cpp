#include "Texture.h"

Texture::Texture(const char* texturePath, std::string name, bool alpha, bool minmap)
{
    ID = NULL; // Initialization
    unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

    this->alpha = alpha;
    this->minmap = minmap;
    this->name = name;
    this->path = texturePath;

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

Texture::Texture(int width, int height, int internalFormat, int format, std::string name, int attachment)
{
    this->name = name;
    this->alpha = false;
    this->minmap = false;
    this->path = "";
	this->format = format;
	this->internalFormat = internalFormat;
    
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, ID, 0);
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

void Texture::resize(glm::ivec2 size)
{
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}
