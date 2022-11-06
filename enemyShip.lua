local scheduler = require "scheduler"
local krbsg = require "krbsg"

local enemyShip = krbsg.wrapEnemyShip(...);

local move = {x = 0, y = 4};
local counter = 0;
while counter < 200 do
	enemyShip:move(move);
	coroutine.yield();
	counter = counter + 1;
end

enemyShip:despawn();
