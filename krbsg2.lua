local scheduler = require "scheduler"
local time = require "time"
local input = require "input"
local krbsg = require "krbsg"

print("welcome to krbsg2");

local context = ...;
local width = context.width;
local height = context.height;

while (true) do
	--attract mode kinda works but isn't really ready for prime time.
	--[[
	local attractor = scheduler.create(function(...)
		while (true) do
			time.wait(5, ...);
			print('attract wait done')
			krbsg.createGameSession("game.lua", width, height, ...);
			time.wait(30, ...);
			krbsg.destroyGameSession(...);
		end
	end, ...);	

			
	input.waitForButtonDown("any", ...);

	scheduler.kill(attractor); -- or should it be attractor:kill()?
	
	krbsg.destroyGameSession(...); -- if exists

	]]--

	krbsg.createGameSession("game.lua", width, height, context);
		
	local paused = false;
	local pause = input.setOnButtonDown("pause", 
		function()
			if not paused then
				krbsg.pause(context);
				krbsg.createPauseMenu(context);
				paused = true;
			else
				krbsg.resume(context);
				krbsg.destroyPauseMenu(context);
				paused = false;
			end
		end,
	context);

	krbsg.waitForGameOver(context);

	krbsg.destroyGameplaySession(context);

end
