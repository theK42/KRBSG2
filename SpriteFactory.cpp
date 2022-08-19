#include "SpriteFactory.h"
#include "OpenGLUtils.h"
#include "ShaderFactory.h"
#include "TextureFactory.h"
#include "StringHash.h"
#include <string>
#include <cmath>
#include <assert.h>

void SpriteFactory::Init(KEngineOpenGL::ShaderFactory* shaderFactory, KEngineOpenGL::TextureFactory* textureFactory)
{
	mShaderFactory = shaderFactory;
	mTextureFactory = textureFactory;
	shaderFactory->CreateShaderProgram(HASH("Textured", 0xDF225E87), "basic.vert", "textured.frag");
	textureFactory->CreateTexture(HASH("Ship", 0x5A02441A), "ship.png");
}

const int TEMP_SHIP_DIM = 40;

const KEngineOpenGL::Sprite* SpriteFactory::PlayerShipSprite(const KEngineCore::StringHash& shipId)
{
	assert(mShaderFactory != nullptr);
	static bool once = false;
	if (!once) {
		once = true;
		int width = TEMP_SHIP_DIM;
		int height = TEMP_SHIP_DIM;
		const KEngineOpenGL::Vertex Vertices[] = {
			{ { width, 0, 0 },{ 1, 1, 1, 1 },{ 1,0 } },
			{ { width, height, 0 },{ 1, 1, 1, 1 },{ 1,1 } },
			{ { 0, height, 0 },{ 1, 1, 1, 1 },{ 0,1 } },
			{ { 0, 0, 0 },{ 1, 1, 1, 1 },{ 0,0 } }
		};

		const GLushort Indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		KEngineOpenGL::Sprite& sprite = playerShipSprite;
		sprite.width = width;
		sprite.height = height;

		sprite.vertexArrayObject = KEngineOpenGL::UploadModel(Vertices, 4, Indices, 6);
		sprite.shaderProgram = mShaderFactory->GetShaderProgram(HASH("Textured", 0xDF225E87));
		sprite.indexCount = 6;
		sprite.texture = mTextureFactory->GetTexture(HASH("Ship", 0x5A02441A));
	}
	return &playerShipSprite;
}

const KEngineOpenGL::Sprite* SpriteFactory::EnemyShipSprite(const KEngineCore::StringHash& shipId)
{
	return PlayerShipSprite(shipId);
}
