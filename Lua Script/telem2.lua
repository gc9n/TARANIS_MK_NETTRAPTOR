--do the local variables here ie 
local besttime = 0
local bat = 0
local amps = 0
local time = 0
local sats = 0
local speed = 0
local mkerror
local timer
--etc

local function init()
--set up initial stuff here not really always needed. this will be run once at model load.
end
local function background()
--tim1 = model.getTimer(1)
--stuff here is done all time 
	timer = model.getTimer(0)
end

local function run(event)
background()

print(event)  
--lines
lcd.drawLine(0,31,211,31, SOLID, 0)	
lcd.drawLine(0,53,211,53, SOLID, 0)	
lcd.drawLine(106,0,106,53, SOLID, 0)

-- bat
lcd.drawText(15, 3, getValue(216) .. " V", MIDSIZE)   -- Voltage
lcd.drawTimer(65, 20, timer.value, 0)            --timer
lcd.drawText(15,20, getValue(208) .. " mAh",0)   --current mAh

lcd.drawText(15,33, getValue(207) .. " sats", 0)   --sats in use
lcd.drawText(15,41, getValue(211) .. " km/h", 0)  --gps speed

lcd.drawText( 108,3, "Alt:" .. getValue("altitude") .. " m gAlt:" .. getValue(213) .. "m",0)  --altitude and gps altitude
lcd.drawText( 108,12, "Dir: " .. getValue(223),0)  --heading
lcd.drawText( 108,20, "I: " .. getValue(217),0) --current

lcd.drawText( 108,33, "Home: " .. getValue(212) .. " m" ,0) --gps distance
lcd.drawText( 108,41, "Dir: home",0)
--all stuff written here is only done when screen is on and seemed to be the best place for writing on screen ie

lcd.drawText (60, 56, "Mikrokopter", 0)
end

return { init=init, background=background, run=run }