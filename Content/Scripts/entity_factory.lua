local factory = {}  -- Define the factory module

-- System retrieval
local EntitySYSTEM = Engine:Get_EntitySYSTEM()
local ComponentSYSTEM = Engine:Get_ComponentSYSTEM()
local GraphicsSYSTEM = Engine:Get_GraphicsSYSTEM()
local EventSYSTEM = Engine:Get_EventSYSTEM()
local TimeSYSTEM = Engine:Get_TimeSYSTEM()

local function Rect(x,y,w,h)
    local rect = Rectangle.new() rect.x = x rect.y = y rect.w = w rect.h = h return rect
end

-- Function to create entity from components
local function Create_Entity_From_Components(components)
    local entityID = EntitySYSTEM:Create_Entity()
    for component_name, component in pairs(components) do
        components[component_name] = ComponentSYSTEM:Add_Component(entityID, component)
    end
    return entityID
end

SCANCODE = {A_KEY = 4, W_KEY = 26, S_KEY = 22, D_KEY = 7, SPACE_KEY=44, UP_KEY=82, DOWN_KEY=81, LEFT_KEY=80, RIGHT_KEY=79, RSHIFT_KEY=229}
EVENTTYPES = {KEYDOWN = 768, KEYUP = 769, KEYSTATE = EventSYSTEM:Get_Event_Type("KEYSTATE")}


factory.Player = {}  -- Define the Player table in the factory
factory.Player.__index = factory.Player  -- Use itself for the metatable index

function factory.Player:Set_Velocity(vel_x, vel_y, sprite)
    self.components.Velocity.x = vel_x
    self.components.Velocity.y = vel_y
    self.components.RenderInfo.clip = sprite
end

function factory.Player.new(pos_x, pos_y)
    local self = setmetatable({}, factory.Player)  -- Set the metatable for the new object to Player
    self.components = {
        Transform = Transform.new(pos_x,pos_y, 0),
        Velocity = Velocity.new(0,0,0),
        RenderInfo = RenderInfo.new(1.0, true, Rect(0,0,64,64)),
        EventListener = EventListener.new(),
        RigidBody = RigidBody.new(Rect(pos_x, pos_y, 64, 64), true, false)
    }

    self.ID = Create_Entity_From_Components(self.components)
    GraphicsSYSTEM:Set_RenderInfo_Texture("player_sprites", self.components.RenderInfo)
    self.components["EventListener"].execute = function(eventObj) self:event_handler(eventObj) end

    self.sprites = {
        DOWN = {Rect(0,0,64,64),Rect(64,0,64,64),Rect(128,0,64,64),Rect(196,0,64,64)},
        LEFT = {Rect(0,64,64,64),Rect(64,64,64,64),Rect(128,64,64,64),Rect(196,64,64,64)},
        RIGHT = {Rect(0,128,64,64),Rect(64,128,64,64),Rect(128,128,64,64),Rect(196,128,64,64)},
        UP = {Rect(0,196,64,64),Rect(64,196,64,64),Rect(128,196,64,64),Rect(196,196,64,64)}
    }
    self.base_speed = 2
    self.speed = self.base_speed
    self.time_tracker = TimedStateMachine.new(4, 100)
    self.inputs = {
        [SCANCODE["A_KEY"]] = "MOVE_LEFT" ,
        [SCANCODE["W_KEY"]] = "MOVE_UP",
        [SCANCODE["S_KEY"]] = "MOVE_DOWN",
        [SCANCODE["D_KEY"]] = "MOVE_RIGHT",
        [SCANCODE["SPACE_KEY"]] = "SPRINT"
    }

    self.actions ={
        MOVE_LEFT = {
            KEY_DOWN = function (sprite, speed) self.components.Velocity.x = -speed self.components.RenderInfo.clip = self.sprites["LEFT"][sprite] end,
            KEY_UP = function (sprite) self.components.Velocity.x = 0 self.components.RenderInfo.clip = self.sprites["LEFT"][sprite] end
        },
        MOVE_RIGHT = {
            KEY_DOWN = function (sprite, speed) self.components.Velocity.x = speed self.components.RenderInfo.clip = self.sprites["RIGHT"][sprite] end,
            KEY_UP = function (sprite) self.components.Velocity.x = 0 self.components.RenderInfo.clip = self.sprites["RIGHT"][sprite] end
        },
        MOVE_UP = {
            KEY_DOWN = function (sprite, speed) self.components.Velocity.y = -speed self.components.RenderInfo.clip = self.sprites["UP"][sprite] end,
            KEY_UP = function (sprite) self.components.Velocity.y = 0 self.components.RenderInfo.clip = self.sprites["UP"][sprite] end
        },
        MOVE_DOWN = {
            KEY_DOWN = function (sprite, speed) self.components.Velocity.y = speed self.components.RenderInfo.clip = self.sprites["DOWN"][sprite] end,
            KEY_UP = function (sprite) self.components.Velocity.y = 0 self.components.RenderInfo.clip = self.sprites["DOWN"][sprite] end
        },
        SPRINT = {
            KEY_DOWN = function () self.speed = 2*self.base_speed self.time_tracker.state_duration = 100 end,
            KEY_UP = function () self.speed = self.base_speed self.time_tracker.state_duration = 200 end
        }
    }
    return self
end

function factory.Player:event_handler(eventObj)
    if(eventObj.type == EVENTTYPES["KEYSTATE"]) then
        local KEY_PRESS = eventObj.data.scancode
        if(self.inputs[KEY_PRESS]) then
            local state_type = self.inputs[KEY_PRESS]
            TimeSYSTEM:Update_Time_Tracker(self.time_tracker)
            local sprite_state = self.time_tracker.current_state + 1
            if(self.components.Velocity.x ~= 0 and self.components.Velocity.y ~= 0) then
                self.actions[state_type]["KEY_DOWN"](sprite_state, self.speed/math.sqrt(2))
            else
                self.actions[state_type]["KEY_DOWN"](sprite_state, self.speed)
            end
        end
    elseif(eventObj.type == EVENTTYPES.KEYUP and eventObj.data.repeats) then
        local KEY_PRESS = eventObj.data.keysym.scancode
        if(self.inputs[KEY_PRESS]) then
            local state_type = self.inputs[KEY_PRESS]
            self.actions[state_type]["KEY_UP"](1)
            self.time_tracker.elapsed_time = 0
        end
    end
end

return factory  -- Return the factory module