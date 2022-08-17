--local renderer = require "renderer"
local playerShip = require "playerships"
local timer = require "timer"
--[[local input = require "input"]]--

--local worldWidth, worldHeight = renderer.getDimensions();

local position = {x = 400, y = 300};
local playerShip = playerShip.spawnPlayerShip(position);


--[[
while doBalls do 
	randomBox();
	timer.waits(1);
	randomBall();
	timer.waits(1);
end
]]--


--[[
while true do
	input.waitForInput();
	local radius = math.random(.5, 4.5);
	local mass = math.random(1, 2) * math.pi * radius ^ 2;
	local position = {x = math.random(radius, worldWidth - radius), y = math.random(radius, worldHeight - radius)};
	local velocity = {vx = math.random(0, 20), vy = math.random(0, 20)};
	
	randomBalls[#randomBalls + 1] = ball.new(position, velocity, radius, mass);
end]]--

while true do coroutine.yield() end