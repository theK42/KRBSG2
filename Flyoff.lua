local krbsg = require "krbsg"
local tweening = require "tweening"
local transforms = require "transforms"
local sprites = require "sprites"

local flyoff = krbsg.wrapFlyoff(...);

local movement = transforms.createMoveVertical(flyoff:getTransform(), -50);
local movementEase = tweening.createEaseOut(movement);
local fade = sprites.createFadeOut(flyoff:getGraphic());
local fadeEase = tweening.createEaseIn(fade);
local duration = tweening.createDuration(.5, movementEase, fadeEase);

tweening.runAndWait(duration);

flyoff:despawn();

--tweens are owned by Lua script