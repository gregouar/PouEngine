#ifndef TEXTURESPACKER_H
#define TEXTURESPACKER_H

#include "PouEngine/assets/TextureAsset.h"
#include "PouEngine/tools/MathTools.h"

namespace pou
{

///Need to add delay for uploading buffer to GPU

struct TexturesPacker_Atlas
{
    TextureAsset *texture;
    std::vector<uint8_t> textureBuffer;
    std::vector<MathTools::Box> freeSpaces;
};

class TexturesPacker
{
    public:
        TexturesPacker(glm::vec2 texturesSize);
        virtual ~TexturesPacker();

        std::pair<TextureAsset*, MathTools::Box> packTexture(size_t width, size_t height, const std::vector<uint8_t> &buffer);

    protected:
        std::shared_ptr<TexturesPacker_Atlas> createAtlas();

        MathTools::Box findAndSplitEmptySpace(std::shared_ptr<TexturesPacker_Atlas> atlas, size_t width, size_t height);
        void writeTexture(std::shared_ptr<TexturesPacker_Atlas> atlas, MathTools::Box box, const std::vector<uint8_t> &buffer);

        void updateMaxAvailableSize(std::shared_ptr<TexturesPacker_Atlas> atlas);

    private:
        glm::vec2 m_texturesSize;

        std::vector< std::shared_ptr<TexturesPacker_Atlas> > m_atlases;
};

}

#endif // TEXTURESPACKER_H
