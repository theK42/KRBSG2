local input = require "input"
local krbsg = require "krbsg"

local playerShip = krbsg.wrapPlayerShip(...);

move = input.setOnCombinedAxisTilt("primary", 
	function(tilt)
		tilt.x = tilt.x * 2;
		tilt.y = tilt.y * 2;
		playerShip:move(tilt);
	end
);

playerShip:addWeapon("Blaster");

while true do coroutine.yield() end