local time = require "time"
local krbsg = require "krbsg"

local bolt = krbsg.wrapProjectile(...);

bolt:setForwardVelocity(500);

--time.wait(5, ...); --lifetime

--bolt:despawn();