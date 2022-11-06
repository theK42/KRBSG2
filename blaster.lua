local input = require "input"
local krbsg = require "krbsg"
local audio = require "audio"
local timer = require "timer"

local blaster = krbsg.wrapWeapon(...);

local firing = false;

local startFiring = input.setOnButtonDown("fire",
	function()
		firing = true;
	end
);

local stopFiring = input.setOnButtonUp("fire",
	function()
		firing = false;
	end
);

function fire()
	audio.playSound("pew");
	local offset = {x = 0, y = 0};
	local rotation = 0;
	blaster:createProjectile("BlasterBolt", blaster:getDamage(), offset, rotation);
end

while true do
	input.waitForButtonDown("fire");
	firing = true;
	while (firing) do
		fire();
		timer.waits(blaster:getCooldown());
	end
end