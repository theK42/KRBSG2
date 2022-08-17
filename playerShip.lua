local input = require "input"
local playerShips = require "playerships"

local playerShip = playerShips.wrapPlayerShip(...);


move = input.setOnCombinedAxisTilt("primary", 
	function(tilt)
		playerShip:move(tilt);
	end
);


while true do coroutine.yield() end