#include "SpriteFactory.h"
#include "OpenGLUtils.h"
#include "ShaderFactory.h"
#include "TextureFactory.h"
#include "StringHash.h"
#include "DataTree.h"
#include <string>
#include <cmath>
#include <assert.h>

SpriteFactory::SpriteFactory()
{
}

SpriteFactory::~SpriteFactory()
{
	Deinit();
}

void SpriteFactory::Init(KEngineOpenGL::ShaderFactory* shaderFactory, KEngineOpenGL::TextureFactory* textureFactory, KEngineCore::DataTree* dataRoot)
{
	mShaderFactory = shaderFactory;
	mTextureFactory = textureFactory;
	KEngineCore::DataTree* textureData = dataRoot->GetBranch(HASH("sheetName", 0x7E99E530), HASH("Textures", 0x5CF1DB40));
	for (auto textureDescriptor : textureData->GetBranches())
	{
		textureFactory->CreateTexture(textureDescriptor->GetHash(HASH("name", 0x5E237E06)), textureDescriptor->GetString(HASH("filename", 0x3C0BE965)));
	}

	KEngineCore::DataTree* shaderData = dataRoot->GetBranch(HASH("sheetName", 0x7E99E530), HASH("Shaders", 0xE1F9ADF3));
	for (auto shaderDescriptor : shaderData->GetBranches())
	{
		shaderFactory->CreateShaderProgram(shaderDescriptor->GetHash(HASH("name", 0x5E237E06)), shaderDescriptor->GetString(HASH("vertFilename", 0x6E8D4B27)), shaderDescriptor->GetString(HASH("fragFilename", 0xBC8C33CC)));
	}

	mSpriteData = dataRoot->GetBranch(HASH("sheetName", 0x7E99E530), HASH("Sprites", 0xC331555B));

}

void SpriteFactory::Deinit()
{
	for (auto spritePair : mSpriteCache)
	{
		KEngineOpenGL::DeleteVAO(spritePair.second.vertexArrayObject);
	}
	mSpriteCache.clear();
}

const KEngineOpenGL::Sprite* SpriteFactory::GetSprite(const KEngineCore::StringHash& spriteId)
{
	assert(mShaderFactory != nullptr);
	if (mSpriteCache.contains(spriteId))
	{
		return &mSpriteCache[spriteId];
	}

	auto spriteDescriptor = mSpriteData->GetBranch(HASH("name", 0x5E237E06), spriteId);

	float width = spriteDescriptor->GetInt(HASH("width", 0x8C1A452F));
	float height = spriteDescriptor->GetInt(HASH("height", 0xF54DE50F));
	const KEngineOpenGL::Vertex Vertices[] = {
		{ { width, 0.f, 0.f },{ 1.f, 1.f, 1.f, 1.f },{ 1.f, 0.f } },
		{ { width, height, 0.f },{ 1.f, 1.f, 1.f, 1.f },{ 1.f,1.f } },
		{ { 0.f, height, 0.f },{ 1.f, 1.f, 1.f, 1.f },{ 0.f,1.f } },
		{ { 0.f, 0.f, 0.f },{ 1.f, 1.f, 1.f, 1.f },{ 0.f,0.f } }
	};

	const GLushort Indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	KEngineOpenGL::Sprite& sprite = mSpriteCache[spriteId];
	sprite.width = width;
	sprite.height = height;

	sprite.vertexArrayObject = KEngineOpenGL::UploadModel(Vertices, 4, Indices, 6);
	sprite.shaderProgram = mShaderFactory->GetShaderProgram(spriteDescriptor->GetHash(HASH("shader", 0xFFF39E28)));
	sprite.indexCount = 6;
	sprite.texture = mTextureFactory->GetTexture(spriteDescriptor->GetHash(HASH("texture", 0x82660D72)));

	return &sprite;
}
