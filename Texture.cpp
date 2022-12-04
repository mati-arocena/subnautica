#include "Texture.h"

Texture::Texture(const char* texturePath, const std::string& name, bool alpha, bool minmap) 
    : name(name), path (texturePath), alpha(alpha), minmap(minmap)
{
    ID = NULL; // Initialization
    unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

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

Texture::Texture(int width, int height, int internalFormat, int format, const std::string& name, int attachment, const bool& clampBorder)
    : name(name), width(width), height(height), internalFormat(internalFormat), format(format)
{
    this->alpha = false;
    this->minmap = false;
    this->path.clear();
    this->nrChannels = 4;
    
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (clampBorder)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }

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

void Texture::resize(const glm::ivec2& size)
{
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}
