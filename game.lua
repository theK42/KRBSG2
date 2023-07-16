local krbsg = require "krbsg"
local time = require "time"
local input = require "input"
local audio = require "audio"

local context = ...

local width = context.width;
local height = context.height;

audio.playMusic("gameMusic");

local starfield = krbsg.spawnStarfield(width, height, ...);

local position = {x = width/2, y = height/2};
local playerShip = krbsg.spawnPlayerShip("basic", position, ...);

position.y = 0;

while true do
	time.wait(1, ...);
	position.x = math.random(0, width);
	local enemyShip = krbsg.spawnEnemyShip("basic", position, ...);
end
