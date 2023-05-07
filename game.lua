local krbsg = require "krbsg"
local time = require "time"
local input = require "input"
local audio = require "audio"


local width = 800; -- TODO read as parameter?  Change entirely?
local height = 600;

audio.playMusic("gameMusic");

local starfield = krbsg.spawnStarfield(width, height, ...);

local position = {x = 400, y = 300};
local playerShip = krbsg.spawnPlayerShip("basic", position, ...);

position.y = 0;

while true do
	time.wait(1, ...);
	position.x = math.random(0, 800);
	local enemyShip = krbsg.spawnEnemyShip("basic", position, ...);
end