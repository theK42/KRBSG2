--local renderer = require "renderer"
local krbsg = require "krbsg"
local timer = require "timer"
local input = require "input"
local audio = require "audio"

--local worldWidth, worldHeight = renderer.getDimensions();

--audio.playMusic("gameMusic");

local position = {x = 400, y = 300};
local playerShip = krbsg.spawnPlayerShip("basic", position);

position.y = 0;

local exit = input.setOnButtonDown("pause", 
	function()
		krbsg.exit();
	end
);

while true do
	timer.waits(1);
	position.x = math.random(0, 800);
	local enemyShip = krbsg.spawnEnemyShip("basic", position);
end