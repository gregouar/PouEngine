#include "PouEngine/tools/TexturesPacker.h"

namespace pou
{

TexturesPacker::TexturesPacker(glm::vec2 texturesSize) :
    m_texturesSize(texturesSize)
{
    //ctor
}

TexturesPacker::~TexturesPacker()
{
    //dtor
}

std::pair<TextureAsset*, MathTools::Box> TexturesPacker::packTexture(size_t width, size_t height, const std::vector<uint8_t> &buffer)
{
    assert(width <= m_texturesSize.x && height <= m_texturesSize.y);

    MathTools::Box box;

    std::shared_ptr<TexturesPacker_Atlas> atlas;
    for(auto atlasIt : m_atlases)
    {
         box = this->findAndSplitEmptySpace(atlasIt, width, height);
         ///box = this->findAndSplitEmptySpace(&atlasIt->rootNode, width, height);
         if(box.size == glm::vec2(width, height))
         {
            atlas = atlasIt;
            break;
         }
    }

    if(!atlas)
    {
        atlas = this->createAtlas();
        ///box = this->findAndSplitEmptySpace(&atlas->rootNode, width, height);
        box = this->findAndSplitEmptySpace(atlas, width, height);
    }

    this->writeTexture(atlas, box, buffer);

    return {atlas->texture, box};
}

///
///Protected
///

std::shared_ptr<TexturesPacker_Atlas> TexturesPacker::createAtlas()
{
    auto atlas = std::make_shared<TexturesPacker_Atlas>();

    auto bufferSize = m_texturesSize.x * m_texturesSize.y * 4;
    atlas->textureBuffer = std::vector<uint8_t>(bufferSize, 128);

    for(int y = 0 ; y < m_texturesSize.y ; y++)
    for(int x = 0 ; x < m_texturesSize.x ; x++)
    {
         atlas->textureBuffer[(y*m_texturesSize.x + x)*4 + 0] = 255;//x % 256;
         atlas->textureBuffer[(y*m_texturesSize.x + x)*4 + 1] = 0;//y % 256;
         atlas->textureBuffer[(y*m_texturesSize.x + x)*4 + 2] = 0;
         atlas->textureBuffer[(y*m_texturesSize.x + x)*4 + 3] = 255;
    }

    atlas->texture = TextureAsset::generateTexture(m_texturesSize.x, m_texturesSize.y, atlas->textureBuffer);

    atlas->freeSpaces.push_back({m_texturesSize, glm::vec2(0)});
    /**atlas->rootNode.isEmpty = true;
    atlas->rootNode.box.position = glm::vec2(0);
    atlas->rootNode.box.position = m_texturesSize;**/

    m_atlases.push_back(atlas);
    return atlas;
}

/**MathTools::Box TexturesPacker::findAndSplitEmptySpace(TexturePacker_TreeNode *treeNode, size_t width, size_t height)
{
    MathTools::Box resultBox;
    resultBox.size = glm::vec2(0);

    return resultBox;
}**/


MathTools::Box TexturesPacker::findAndSplitEmptySpace(std::shared_ptr<TexturesPacker_Atlas> atlas, size_t width, size_t height)
{
    MathTools::Box resultBox;
    resultBox.size = glm::vec2(0);

    auto freeSpaceIt = atlas->freeSpaces.rbegin();
    for(;freeSpaceIt != atlas->freeSpaces.rend() ; ++freeSpaceIt)
        if(freeSpaceIt->size.x >= width && freeSpaceIt->size.y >= height)
            break;

    if(freeSpaceIt == atlas->freeSpaces.rend())
        return resultBox;

    resultBox.center = freeSpaceIt->center;

    MathTools::Box smallSplit, bigSplit;

    glm::vec2 freeSpace = freeSpaceIt->size - glm::vec2(width, height);

    if(freeSpace.y > freeSpace.x)
    {
        smallSplit.center = resultBox.center + glm::vec2(width,0);
        smallSplit.size = glm::vec2(freeSpace.x, height);
        bigSplit.center = resultBox.center + glm::vec2(0,height);
        bigSplit.size = glm::vec2(freeSpaceIt->size.x, freeSpace.y);
    } else {
        smallSplit.center = resultBox.center + glm::vec2(0,height);
        smallSplit.size = glm::vec2(width, freeSpace.y);
        bigSplit.center = resultBox.center + glm::vec2(width,0);
        bigSplit.size = glm::vec2(freeSpace.x, freeSpaceIt->size.y);
    }

    //atlas->freeSpaces.erase(freeSpaceIt.base() - 1);
    (*freeSpaceIt) = atlas->freeSpaces.back();
    atlas->freeSpaces.pop_back();

    if(bigSplit.size.x != 0 && bigSplit.size.y != 0)
        atlas->freeSpaces.push_back(bigSplit);
    if(smallSplit.size.x != 0 && smallSplit.size.y != 0)
        atlas->freeSpaces.push_back(smallSplit);

    resultBox.size = glm::vec2(width, height);
    return resultBox;
}


void TexturesPacker::writeTexture(std::shared_ptr<TexturesPacker_Atlas> atlas, MathTools::Box box, const std::vector<uint8_t> &buffer)
{
    for(int y = 0 ; y < box.size.y ; ++y)
    for(int x = 0 ; x < box.size.x ; ++x)
    {
        auto relY =  box.center.y + y;
        auto relX =  box.center.x + x;

        for(auto i = 0 ; i < 4 ; ++i)
            atlas->textureBuffer[(relY*m_texturesSize.x + relX)*4 + i] = buffer[(y*box.size.x + x)*4 + i];
    }


    atlas->texture->writeTexture(atlas->textureBuffer);
}



}
