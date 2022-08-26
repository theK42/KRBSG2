local enemyships = require "enemyships"
local scheduler = require "scheduler"

local enemyShip = enemyships.wrapEnemyShip(...);

local move = {x = 0, y = 4};
local counter = 0;
while counter < 200 do
	enemyShip:move(move);
	coroutine.yield();
	counter = counter + 1;
end

local function despawn()
	enemyShip:despawn();
end

local despawnThread = scheduler.create(despawn);
scheduler.resume(despawnThread);


while true do coroutine.yield() end
