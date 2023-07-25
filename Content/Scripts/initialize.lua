local factory = require("entity_factory")
local sela_map = require("sela_map")

local GraphicsSYSTEM = Engine:Get_GraphicsSYSTEM()
local WorldSYSTEM = Engine:Get_WorldSYSTEM()
GraphicsSYSTEM:Register_Texture_From_File("player_sprites" ,"Content/graphics/player_spritesheet.png")
local player1 = factory.Player.new(500,800)

local player2 = factory.Player.new(500,600)
player2.inputs = {
    [SCANCODE["LEFT_KEY"]] = "MOVE_LEFT" ,
    [SCANCODE["UP_KEY"]] = "MOVE_UP",
    [SCANCODE["DOWN_KEY"]] = "MOVE_DOWN",
    [SCANCODE["RIGHT_KEY"]] = "MOVE_RIGHT",
    [SCANCODE["RSHIFT_KEY"]] = "SPRINT"
}

WorldSYSTEM:Register_Tilemap(sela_map)