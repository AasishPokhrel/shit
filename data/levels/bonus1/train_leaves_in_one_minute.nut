class TrainManager {
	image = null
	text = null

	time = 0
	time_multiplier = 1

	random_thing = 0

	sound = "sounds/phone.wav"

	train = [
		sector.trainint
		sector.trainbg
		sector.trainbg2
	]

	constructor(_time = 60) {
		image = FloatingImage("images/objects/train/train.png")
		image.set_anchor_point(ANCHOR_TOP)
		image.set_pos(-22, 43)

		text = TextObject()
		text.set_anchor_point(ANCHOR_TOP)
		text.set_pos(34, 50)
		text.set_front_fill_color(0, 0, 0, 0)
		text.set_back_fill_color(0, 0, 0, 0)

		random_thing = rand()
		sector.settings.add_object("scriptedobject", "dummy_tux_" + random_thing, -64, -64, "auto",
		"(physic-enabled #f)(solid #f)(name \"dummy_tux_" + random_thing+ "\")(sprite \"/images/creatures/tux/tux.sprite\")")

		time = _time + 1
	}

	function start() {
		image.set_visible(true)
		text.set_visible(true)
		while(time > 0) {
			time--
			text.set_text(time.tostring())
			if(time == 5) play_sound(sound)
			wait(time_multiplier)
		}
		leave()
	}

	function leave() {
		text.set_text("Train is departing!")
		//text.set_pos(50, 50)
		local cur_speed = 0
		depart()
		while(true) {
			cur_speed += 0.02
			image.set_pos(image.get_pos_x() + cur_speed, image.get_pos_y())
			wait(0.01)
		}
	}

	function depart(_sound = true) {
		if(_sound) play_sound(sound)
		foreach(v in train) v.start_moving()
	}

	function losing_cutscene() {
		if(time > 0) return

		Tux.deactivate()
		Tux.activate()

		stop_music(5)
		Tux.use_scripting_controller(true)
		wait(2)
		Tux.do_scripting_controller("left", true)
		wait(0.1)
		Tux.do_scripting_controller("left", false)
		wait(0.9)
		Tux.do_scripting_controller("right", true)
		wait(0.1)
		Tux.do_scripting_controller("right", false)
		wait(2)

		if(sector.Tux.get_bonus() == "none")
			DummyTux.set_action("small-stand-right")
		if(sector.Tux.get_bonus() == "grow")
			DummyTux.set_action("big-stand-right")
		if(sector.Tux.get_bonus() == "fireflower")
			DummyTux.set_action("fire-stand-right")
		if(sector.Tux.get_bonus() == "iceflower")
			DummyTux.set_action("ice-stand-right")
		if(sector.Tux.get_bonus() == "airflower")
			DummyTux.set_action("air-stand-right")
		if(sector.Tux.get_bonus() == "earthflower")
			DummyTux.set_action("earth-stand-right")

		Tux.set_visible(false)
		DummyTux.set_pos(Tux.get_x(), Tux.get_y())
		Tux.kill(true)
		wait(0.01)
		stop_music(0)
	}

	function win() {
		Tux.trigger_sequence("endsequence")
		depart(false)
	}
}

sector.train_manager <- TrainManager()

/*
sector.time <- 60

Text.set_anchor_point(ANCHOR_TOP)
//Text.set_pos(22, 50)
Text.set_front_fill_color(0, 0, 0, 0)
Text.set_back_fill_color(0, 0, 0, 0)
Text.set_visible(true)

//Train <- FloatingImage("images/creatures/tux/small/stand-0.png");
Train <- FloatingImage("levels/bonusislands/Train.png");
Train.set_anchor_point(ANCHOR_TOP)
Train.set_pos(-22, 43)
Train.set_visible(true)

//Text.set_pos(Train.get_width(), 50)
Text.set_pos(34, 50)

for(local t = sector.time; t > -1; t -= 1) {
sector.set_time <- function(timm) t = timm
Text.set_text(t.tostring())
sector.time = t
if(t == 5) play_sound("sounds/phone.wav")
wait(1)
}
Text.set_text(""sounds/phone.wav"")

play_sound("sounds/phone.wav")
trainint.start_moving()
trainbg.start_moving()
trainbg2.start_moving()

local speed = 0.01
while(true) {
Train.set_pos(Train.get_pos_x() + speed, Train.get_pos_y())
speed += 0.01
wait(0.01)
}

if(sector.time < 1) {
Tux.deactivate()
Tux.activate()

stop_music(5)
Tux.use_scripting_controller(true)
wait(2)
Tux.do_scripting_controller("left", true)
wait(0.1)
Tux.do_scripting_controller("left", false)
wait(0.9)
Tux.do_scripting_controller("right", true)
wait(0.1)
Tux.do_scripting_controller("right", false)
wait(2)

if((sector.Tux.get_bonus() == ("small"))
DummyTux.set_action("small-stand-right")
if((sector.Tux.get_bonus() == ("big"))
DummyTux.set_action("big-stand-right")
if((sector.Tux.get_bonus() == ("fire"))
DummyTux.set_action("fire-stand-right")
if((sector.Tux.get_bonus() == ("ice"))
DummyTux.set_action("ice-stand-right")
if((sector.Tux.get_bonus() == ("air"))
DummyTux.set_action("air-stand-right")
if((sector.Tux.get_bonus() == ("earth"))
DummyTux.set_action("earth-stand-right")

Tux.set_visible(false)
DummyTux.set_pos(Tux.get_x(), Tux.get_y())
Tux.kill(true)
wait(0.01)
stop_music(0)
}

if(sector.time > 0) {
//level ends
Tux.trigger_sequence("endsequence")

// train moves
trainint.start_moving();
trainbg.start_moving();
trainbg2.start_moving();
//wait(1);
//Effect.fade_out(1);
}
*/