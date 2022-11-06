#pragma once
#include <functional>

namespace KEngine2D
{
	struct Point;
}

class ScoreKeeper
{
public:
	ScoreKeeper();
	~ScoreKeeper();

	void Init(int score);
	void Deinit();

	int GetScore() const;
	void AddScore(int score, KEngine2D::Point location);

	void SetScoreChangeHandler(std::function<void(int, KEngine2D::Point)> handler);

private:
	int	mScore;

	std::function<void(int, KEngine2D::Point)>	mChangeHandler;
};

