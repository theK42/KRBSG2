local input = require "input"
local krbsg = require "krbsg"

local playerShip = krbsg.wrapPlayerShip(...);

move = input.setOnCombinedAxisTilt("primary", 
	function(tilt)
		tilt.x = tilt.x * 4;
		if (tilt.y < 0) then
			tilt.y = tilt.y * 2;
		else
			tilt.y = tilt.y * 4;
		end
		playerShip:move(tilt);
	end,
...);

playerShip:addWeapon("Blaster", ...);

while true do coroutine.yield() end