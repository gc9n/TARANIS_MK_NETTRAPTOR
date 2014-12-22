--do the local variables here ie 
local besttime = 0
local bat = 0
local amps = 0
local time = 0
local sats = 0
local speed = 0
local mkerror
local timer
local text = "Mikrokopter"
local temp = 0
local errorcode
local headtmp
local timz
local lat = {0,0,0,0,0,0,0,0,0,0}
local lon = {0,0,0,0,0,0,0,0,0,0}
local heading = {0,0,0,0,0,0,0,0,0,0}
local NCFlags = 0
local NCFlagsArray = {0,0,0,0,0,0,0,0}
local FCFlags = 0
local FCFlagsArray = {0,0,0,0,0,0,0,0}
local FCFlags2 = 0
local FCFlags2Array = {0,0,0,0,0,0,0,0}


function tobits(num,quien)
 	local retString=""
	local i = 8
	local tab ={0,0,0,0,0,0,0,0}
	while i >= 0 do
		if math.floor(num /(2^i)) >= 1 then
			retString = retString .."1"
			tab[i]=1
			num = num-math.pow(2,i)
		else 
			retString = retString .. "0"
			tab[i]=0
		end
		i = i-1
	end 
	if quien == "NCFlags" then 
		NCFlagsArray = tab
	end 
	
	if quien == "FCFlags" then 
		FCFlagsArray = tab
	end 
	
	if quien == "FCFlags2" then 
		FCFlags2Array = tab
	end 
	
end

local function init()
--set up initial stuff here not really always needed. this will be run once at model load.
	headtmp=0
	headtoh=0
	headfromh=0
	z1 = 0
	z2 = 0
	pilotlat = getValue("pilot-latitude")
	pilotlon = getValue("pilot-longitude")
end
local function background()
--tim1 = model.getTimer(1)
--stuff here is done all time 
	timer = model.getTimer(0)
end
--	num = math.floor(getValue(220)*100)


local function run(event)
	background()

	print(event)  
	--lines
	lcd.drawLine(0,31,211,31, SOLID, 0)	
	lcd.drawLine(0,53,211,53, SOLID, 0)	
	lcd.drawLine(106,0,106,53, SOLID, 0)

	-- bat
	lcd.drawText(15, 3, getValue(216) .. " V", MIDSIZE)   -- Voltage
	
	
--------- NCFlags -------------------------------------
--  36 #define NC_FLAG_FREE                            0x01
-- 37 #define NC_FLAG_PH                              0x02
--  38 #define NC_FLAG_CH                              0x04
--  39 #define NC_FLAG_RANGE_LIMIT                     0x08
--  40 #define NC_FLAG_NOSERIALLINK                    0x10
--  41 #define NC_FLAG_TARGET_REACHED                  0x20
--  42 #define NC_FLAG_MANUAL                          0x40
--  43 #define NC_FLAG_GPS_OK                          0x80
--  44 
--  45 // ------- FCStatusFlags -------------------------------
--  46 #define FC_STATUS_MOTOR_RUN                     0x01
--  47 #define FC_STATUS_FLY                           0x02
--  48 #define FC_STATUS_CALIBRATE                     0x04
--  49 #define FC_STATUS_START                         0x08
--  50 #define FC_STATUS_EMERGENCY_LANDING             0x10
--  51 #define FC_STATUS_LOWBAT                        0x20
--  52 #define FC_STATUS_VARIO_TRIM_UP                 0x40
--  53 #define FC_STATUS_VARIO_TRIM_DOWN               0x80
--  54 
--  55 // ------- FCStatusFlags2 ------------------------------
--  56 #define FC_STATUS2_CAREFREE_ACTIVE              0x01
--  57 #define FC_STATUS2_ALTITUDE_CONTROL_ACTIVE      0x02
--  58 #define FC_STATUS2_FAILSAFE_ACTIVE              0x04
--  59 #define FC_STATUS2_OUT1                         0x08
--  60 #define FC_STATUS2_OUT2                         0x10
--  61 #define FC_STATUS2_RES1                         0x20
--  62 #define FC_STATUS2_RES2                         0x40
--  63 #define FC_STATUS2_RES3                         0x80
	
	--lcd.drawText(60,3, tobits(getValue(222)),0) --NCflags
	
	NCFlags = getValue(220) * 100
	tobits(NCFlags,"NCFlags")
	--FCFlags = getValue(221) * 100
	FCFlags = 3
	tobits(FCFlags,"FCFlags")
	FCFlags2 = getValue(222) * 100
	tobits(FCFlags2,"FCFlags2")
	
	--lcd.drawText(60,3, NCFlags ,0) --NCflags
	lcd.drawText(75,3, FCFlagsArray[1] ,0) --NCflags
	if FCFlagsArray[1] == 1 then 
		model.setGlobalVariable(1, 0, 100)
	end
	--lcd.drawText(90,3, FCFlags2 ,0) --NCflags
	
	lcd.drawTimer(65, 20, timer.value, 0)            --timer
	lcd.drawText(15,20, getValue(208) .. " mAh",0)   --current mAh

	lcd.drawText(15,33, getValue(207) .. " sats", 0)   --sats in use
	lcd.drawText(15,41, getValue(211) .. " km/h", 0)  --gps speed

	lcd.drawText( 108,3, "Alt:" .. math.floor(getValue("altitude")*10) .. " m gAlt:" .. getValue(213) .. "m",0)  --altitude and gps altitude
	lcd.drawText( 108,12, "Dir: " .. getValue(223),0)  --heading
	lcd.drawText( 108,20, "I: " .. getValue(217),0) --current

	lcd.drawText( 108,33, "Home: " .. getValue(212) .. " m" ,0) --gps distance
	
	--Then we calculate the heading from the pilot position to the current position.
	pilotlat = getValue("pilot-latitude")
	pilotlon = getValue("pilot-longitude")
	lat = getValue("latitude")
	lon = getValue("longitude")
	
	if pilotlat~=0 and lat~=0 and pilotlon~=0 and lon~=0 then 

		z1 = math.sin(math.rad(lon) - math.rad(pilotlon)) * math.cos(math.rad(lat))
		z2 = math.cos(math.rad(pilotlat)) * math.sin(math.rad(lat)) - math.sin(math.rad(pilotlat)) * math.cos(math.rad(lat)) * math.cos(math.rad(lon) - math.rad(pilotlon))
		headfromh = math.deg(math.atan2(z1, z2))
		if headfromh < 0 then
			headfromh=headfromh+360
		end
		--Subtract 180 and get the heading to get back home.
		headtoh = headfromh-180
		if headtoh < 0 then
			headtoh = headtoh+360
		end
	else
		headfromh = 0
		headtoh = 0
	end
	lcd.drawText( 108,41, "DirToHome: " .. math.floor(headtoh),0)
	
	--all stuff written here is only done when screen is on and seemed to be the best place for writing on screen i

	--1 "FC not compatible "
	--2 "MK3Mag not compatible "
	--3 "no FC communication "
	--4 "no MK3Mag communication "
	--5 "no GPS communication "
	--6 "bad compass value"
	--7 "RC Signal lost "
	--8 "FC spi rx error "
	--9: "ERR: no NC communication"
	--10 "ERR: FC Nick Gyro"
	--11 "ERR: FC Roll Gyro"
	--12 "ERR: FC Yaw Gyro"
	--13 "ERR: FC Nick ACC"
	--14 "ERR: FC Roll ACC"
	--15 "ERR: FC Z-ACC"
	--16 "ERR: Pressure sensor"
	--17 "ERR: FC I2C"
	--18 "ERR: Bl Missing"
	--19 "Mixer Error"
	--20 "FC: Carefree Error"
	--21 "ERR: GPS lost"
	--22 "ERR: Magnet Error"
	--23 "Motor restart"
	--24 "BL Limitation"
	--25 "Waypoint range"
	--26 "ERR:No SD-Card"
	--27 "ERR:SD Logging aborted"
	--28 "ERR:Flying range!"
	--29 "ERR:Max Altitude"
	--30 "No GPS Fix"
	--31 "compass not calibrated"
	--32 "BL Selftest error"
	--33 "no ext. compass"
	--34 "compass sensor"
	--0 "No Error"
	
	if getValue(210) == 1 then
		text = "FC not compatible"
	end
	
	if getValue(210) == 2 then
		text = "MK3Mag not compatible "
	end
	
	if getValue(210) == 3 then
		text = "no FC communication  "
	end
	
	if getValue(210) == 4 then
		text = "no MK3Mag communication  "
	end
	
	if getValue(210) == 5 then
		text = "no GPS communication "
	end
	
	if getValue(210) == 6 then
		text = "bad compass value "
	end
	
	if getValue(210) == 7 then
		text = "RC Signal lost "
	end
	
	if getValue(210) == 8 then
		text = "FC spi rx error "
	end
	
	if getValue(210) == 9 then
		text = "ERR: no NC communication "
	end
	
	if getValue(210) == 10 then
		text = "ERR: FC Nick Gyro "
	end 
	
	if getValue(210) == 11 then
		text = "ERR: FC Roll Gyro "
	end 
	
	if getValue(210) == 12 then
		text = "ERR: FC Yaw Gyro "
	end 
	
	if getValue(210) == 13 then
		text = "ERR: FC Nick ACC "
	end 
	
	if getValue(210) == 14 then
		text = "ERR: FC Roll ACC "
	end 
	if getValue(210) == 15 then
		text = "ERR: FC Z-ACC "
	end 
	if getValue(210) == 16 then
		text = "ERR: Pressure sensor "
	end 
	if getValue(210) == 17 then
		text = "ERR: FC I2C "
	end 
	if getValue(210) == 18 then
		text = "ERR: Bl Missing "
	end 
	if getValue(210) == 19 then
		text = "Mixer Error "
	end 
	if getValue(210) == 20 then
		text = "FC: Carefree Error "
	end 
	if getValue(210) == 21 then
		text = "ERR: GPS lost "
	end 
	if getValue(210) == 22 then
		text = "ERR: Magnet Error "
	end 
	if getValue(210) == 23 then
		text = "Motor restart "
	end 
	if getValue(210) == 24 then
		text = "BL Limitation "
	end 
	if getValue(210) == 25 then
		text = "Waypoint range "
	end 
	if getValue(210) == 26 then
		text = "ERR:No SD-Card "
	end 
	if getValue(210) == 27 then
		text = "ERR:SD Logging aborted "
	end 

	if getValue(210) == 28 then
		text = "ERR:Flying range! "
	end 
	if getValue(210) == 29 then
		text = "ERR:Max Altitude "
	end 
	if getValue(210) == 30 then
		text = "No GPS Fix "
	end 
	if getValue(210) == 31 then
		text = "compass not calibrated "
	end 
	if getValue(210) == 32 then
		text = "BL Selftest error "
	end 
	if getValue(210) == 33 then
		text = "no ext. compass"
	end 
	if getValue(210) == 34 then
		text = "compass sensor"
	end 

	if getValue(210) == 0 then 
		lcd.drawText (105 -  50 / 2, 56, "Mikrokopter", 0)
	else 
		lcd.drawText (20, 56, text, 0)
		playFile("/SOUNDS/en/system/timerlt3.wav")
	end
end

return { init=init, background=background, run=run }