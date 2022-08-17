#pragma once

#include "SpriteRenderer.h"
#include <map>

namespace KEngineOpenGL {
    class ShaderFactory;
    class TextureFactory;
}

class SpriteFactory
{
public:
    void Init(KEngineOpenGL::ShaderFactory* shaderFactory, KEngineOpenGL::TextureFactory* textureFactory);
    const KEngineOpenGL::Sprite* PlayerShipSprite(const KEngineCore::StringHash & shipId);
    const KEngineOpenGL::Sprite* EnemyShipSprite(const KEngineCore::StringHash& shipId);// , Color color);
    const KEngineOpenGL::Sprite* StarSprite(const KEngineCore::StringHash & starId);
private:
    
    KEngineOpenGL::Sprite playerShipSprite;  // Lol.

    bool  mInitialized{ false };

    KEngineOpenGL::ShaderFactory* mShaderFactory;
    KEngineOpenGL::TextureFactory* mTextureFactory;
};