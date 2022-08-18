--local renderer = require "renderer"
local playerShip = require "playerships"
local timer = require "timer"

--local worldWidth, worldHeight = renderer.getDimensions();

local position = {x = 400, y = 300};
local playerShip = playerShip.spawnPlayerShip(position);

while true do coroutine.yield() end