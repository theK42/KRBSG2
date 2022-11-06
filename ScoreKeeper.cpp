#include "ScoreKeeper.h"
#include "Transform2D.h"

ScoreKeeper::ScoreKeeper()
{
}

ScoreKeeper::~ScoreKeeper()
{
	Deinit();
}

void ScoreKeeper::Init(int score)
{
	mScore = score;
}

void ScoreKeeper::Deinit()
{
	mScore = 0;
	mChangeHandler = nullptr;
}

int ScoreKeeper::GetScore() const
{
	return mScore;
}

void ScoreKeeper::AddScore(int score, KEngine2D::Point location)
{
	mScore += score;
	if (mChangeHandler)
	{
		mChangeHandler(score, location);
	}
}

void ScoreKeeper::SetScoreChangeHandler(std::function<void(int, KEngine2D::Point)> handler)
{
	mChangeHandler = handler;
}
