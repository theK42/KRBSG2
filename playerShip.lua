local input = require "input"
local playerShips = require "playerships"

local playerShip = playerShips.wrapPlayerShip(...);


move = input.setOnCombinedAxisTilt("primary", 
	function(tilt)
		tilt.x = tilt.x * 2;
		tilt.y = tilt.y * 2;
		playerShip:move(tilt);
	end
);


while true do coroutine.yield() end