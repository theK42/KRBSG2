local input = require "input"
local weapons = require "weapons"

local blaster = weapons.wrapWeapon(...);

fire = input.setOnButtonHold("fire", 1.0 / blaster:getCooldown(),
	function()
		blaster:fire();
	end
);

while true do coroutine.yield() end