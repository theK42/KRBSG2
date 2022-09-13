#pragma once

#include "SpriteRenderer.h"
#include <map>

namespace KEngineCore {
    class DataTree;
}

namespace KEngineOpenGL {
    class ShaderFactory;
    class TextureFactory;
}

class SpriteFactory
{
public:
    SpriteFactory();
    ~SpriteFactory();

    void Init(KEngineOpenGL::ShaderFactory* shaderFactory, KEngineOpenGL::TextureFactory* textureFactory, KEngineCore::DataTree * dataRoot);
    void Deinit();

    const KEngineOpenGL::Sprite* GetSprite(const KEngineCore::StringHash& spriteId);

private:
    std::map<KEngineCore::StringHash, KEngineOpenGL::Sprite> mSpriteCache;

    bool  mInitialized{ false };

    KEngineOpenGL::ShaderFactory* mShaderFactory;
    KEngineOpenGL::TextureFactory* mTextureFactory;

    KEngineCore::DataTree* mSpriteData;
};