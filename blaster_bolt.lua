local timer = require "timer"
local krbsg = require "krbsg"

local bolt = krbsg.wrapProjectile(...);


bolt:setForwardVelocity(500);

--timer.waits(5); --lifetime

--bolt:despawn();