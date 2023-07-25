
EntitySYSTEM = Engine:Get_EntitySYSTEM()
ComponentSYSTEM = Engine:Get_ComponentSYSTEM()
GraphicsSYSTEM = Engine:Get_GraphicsSYSTEM()
EventSYSTEM = Engine:Get_EventSYSTEM()
TimeSYSTEM = Engine:Get_TimeSYSTEM()

function Rect(x,y,w,h)
    local rect = Rectangle.new() rect.x = x rect.y = y rect.w = w rect.h = h return rect
end
SCANCODE = {A_KEY = 4, W_KEY = 26, S_KEY = 22, D_KEY = 7, SPACE_KEY=44}

EVENTTYPES = {KEYDOWN = 768, KEYUP = 769, KEYSTATE = EventSYSTEM:Get_Event_Type("KEYSTATE")}

Entity_Types = {
    player = {
        Transform = Transform.new(50,50, 0),
        Velocity = Velocity.new(0,0,0),
        RenderInfo = RenderInfo.new(1.0, true, Rect(0,0,64,64)),
        Texture = Texture.new(),
        EventListener = EventListener.new(),
        TimeTracker = TimeTracker.new()
    },
    text = {
        Transform = Transform.new()
    }
}


function Create_Entity_Of_Type(type)
    local components = Entity_Types[type]
    local entityID = EntitySYSTEM:Create_Entity()
    print("creating entity type:", type)
    for component_name, component in pairs(components) do
        ComponentSYSTEM:Add_Component(entityID, component)
    end
    return entityID
end

local playerID = Create_Entity_Of_Type("player")
local sprite_sheet = {
    DOWN = {Rect(0,0,64,64),Rect(64,0,64,64),Rect(128,0,64,64),Rect(196,0,64,64)},
    LEFT = {Rect(0,64,64,64),Rect(64,64,64,64),Rect(128,64,64,64),Rect(196,64,64,64)},
    RIGHT = {Rect(0,128,64,64),Rect(64,128,64,64),Rect(128,128,64,64),Rect(196,128,64,64)},
    UP = {Rect(0,196,64,64),Rect(64,196,64,64),Rect(128,196,64,64),Rect(196,196,64,64)},
}

local time_tracker = TimeTracker.new(4, 200)
local SPEED = 2
function PlayerEventHandler(eventObj)
    TimeSYSTEM:Update_Time_Tracker(time_tracker)
    local sprite_state = time_tracker.current_state + 1
    local velocityComp = ComponentSYSTEM:Get_Component_Velocity(playerID)
    local renderComp = ComponentSYSTEM:Get_Component_RenderInfo(playerID)
    local MOVE_LEFT = SCANCODE["A_KEY"]
    local MOVE_UP = SCANCODE["W_KEY"]
    local MOVE_DOWN = SCANCODE["S_KEY"]
    local MOVE_RIGHT = SCANCODE["D_KEY"]
    if(eventObj.type == EVENTTYPES["KEYSTATE"]) then
        local KEY_PRESS = eventObj.data.scancode
        local PLAYER_ACTION_TABLE = {
            [MOVE_LEFT] = function () velocityComp.x = -SPEED; renderComp.clip = sprite_sheet["LEFT"][sprite_state] end,
            [MOVE_RIGHT] = function () velocityComp.x = SPEED renderComp.clip = sprite_sheet["RIGHT"][sprite_state] end,
            [MOVE_UP] = function () velocityComp.y = -SPEED renderComp.clip = sprite_sheet["UP"][sprite_state] end,
            [MOVE_DOWN] = function () velocityComp.y = SPEED renderComp.clip = sprite_sheet["DOWN"][sprite_state] end,
            [SCANCODE["SPACE_KEY"]] = function () SPEED = 4 time_tracker.state_duration = 100 end
        }
        if PLAYER_ACTION_TABLE[KEY_PRESS] then
            PLAYER_ACTION_TABLE[KEY_PRESS]()
        end
    elseif(eventObj.type == EVENTTYPES.KEYUP and eventObj.data.repeats) then
        local KEY_PRESS = eventObj.data.keysym.scancode
        local PLAYER_ACTION_TABLE = {
            [MOVE_LEFT] = function () velocityComp.x = 0 renderComp.clip = sprite_sheet["LEFT"][1] end,
            [MOVE_RIGHT] = function () velocityComp.x = 0 renderComp.clip = sprite_sheet["RIGHT"][1] end,
            [MOVE_UP] = function () velocityComp.y = 0 renderComp.clip = sprite_sheet["UP"][1] end,
            [MOVE_DOWN] = function () velocityComp.y = 0 renderComp.clip = sprite_sheet["DOWN"][1] end,
            [SCANCODE["SPACE_KEY"]] = function () SPEED = 2 time_tracker.state_duration = 200 end
        }
        if PLAYER_ACTION_TABLE[KEY_PRESS] then
            PLAYER_ACTION_TABLE[KEY_PRESS]()
        end
    end
end

local textureComp = ComponentSYSTEM:Get_Component_Texture(playerID)
textureComp.path = "Content/Graphics/player_spritesheet.png"
GraphicsSYSTEM:Load_Texture_From_File(textureComp)
local renderComp = ComponentSYSTEM:Get_Component_RenderInfo(playerID)
renderComp.ID = playerID
local eventListenerComp = ComponentSYSTEM:Get_Component_EventListener(playerID)
eventListenerComp.execute = PlayerEventHandler
local spriteSheetComp = ComponentSYSTEM:Get_Component_SpriteSheet(playerID)