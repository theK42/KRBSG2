--local renderer = require "renderer"
local playerships = require "playerships"
local enemyships = require "enemyships"
local timer = require "timer"

--local worldWidth, worldHeight = renderer.getDimensions();

local position = {x = 400, y = 300};
local playerShip = playerships.spawnPlayerShip(position);

position.y = 0;

while true do
	timer.waits(1);
	position.x = math.random(0, 800);
	local enemyShip = enemyships.spawnEnemyShip(position);
end