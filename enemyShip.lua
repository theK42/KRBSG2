local enemyships = require "enemyships"

local enemyShip = enemyships.wrapEnemyShip(...);

local move = {x = 0, y = 4};
local counter = 0;
while counter < 100 do
	enemyShip:move(move);
	coroutine.yield();
end

enemyShip:despawn();