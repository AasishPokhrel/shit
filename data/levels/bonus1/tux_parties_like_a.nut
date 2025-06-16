sector.keep_looping <- true

function init() {
	ss1.set_enabled(false)
	ss2.set_enabled(false)
	ss3.set_enabled(false)
	ss4.set_enabled(false)
	ss5.set_enabled(false)
	ss6.set_enabled(false)
}

function dance_floor() {
	while(true) {
		df6.tint_fade(0, 1, 0, 0.14, 1)
		df5.tint_fade(0, 0.86, 0, 1, 1)
		df4.tint_fade(0, 1, 0.35, 0, 1)
		df3.tint_fade(0, 0, 1, 0.87, 1)
		df2.tint_fade(0, 0, 0.6, 1, 1)
		df1.tint_fade(0, 0.8, 1, 0, 1)
		wait(1, true)
		if(!sector.keep_looping) return
		df6.tint_fade(0, 0.86, 0, 1, 1)
		df5.tint_fade(0, 1, 0.35, 0, 1)
		df4.tint_fade(0, 0, 1, 0.87, 1)
		df3.tint_fade(0, 0, 0.6, 1, 1)
		df2.tint_fade(0, 0.8, 1, 0, 1)
		df1.tint_fade(0, 1, 0, 0.14, 1)
		wait(1, true)
		if(!sector.keep_looping) return
		df6.tint_fade(0, 1, 0.35, 0, 1)
		df5.tint_fade(0, 0, 1, 0.87, 1)
		df4.tint_fade(0, 0, 0.6, 1, 1)
		df3.tint_fade(0, 0.8, 1, 0, 1)
		df2.tint_fade(0, 1, 0, 0.14, 1)
		df1.tint_fade(0, 0.86, 0, 1, 1)
		wait(1, true)
		if(!sector.keep_looping) return
		df6.tint_fade(0, 0, 1, 0.87, 1)
		df5.tint_fade(0, 0, 0.6, 1, 1)
		df4.tint_fade(0, 0.8, 1, 0, 1)
		df3.tint_fade(0, 1, 0, 0.14, 1)
		df2.tint_fade(0, 0.86, 0, 1, 1)
		df1.tint_fade(0, 1, 0.35, 0, 1)
		wait(1, true)
		if(!sector.keep_looping) return
		df6.tint_fade(0, 0, 0.6, 1, 1)
		df5.tint_fade(0, 0.8, 1, 0, 1)
		df4.tint_fade(0, 1, 0, 0.14, 1)
		df3.tint_fade(0, 0.86, 0, 1, 1)
		df2.tint_fade(0, 1, 0.35, 0, 1)
		df1.tint_fade(0, 0, 1, 0.87, 1)
		wait(1, true)
		if(!sector.keep_looping) return
		df6.tint_fade(0, 0.8, 1, 0, 1)
		df5.tint_fade(0, 1, 0, 0.14, 1)
		df4.tint_fade(0, 0.86, 0, 1, 1)
		df3.tint_fade(0, 1, 0.35, 0, 1)
		df2.tint_fade(0, 0, 1, 0.87, 1)
		df1.tint_fade(0, 0, 0.6, 1, 1)
		wait(1, true)
		if(!sector.keep_looping) return
	}
}

/*function lights() {
	while(sector.keep_looping) {
		partylights.tint_fade(0, 134, 217, 0, 0.2) //green
		wait(1, true)
		partylights.tint_fade(0, 205, 0, 96, 0.2) //purple
		wait(1, true)
		partylights.tint_fade(0, 232, 214, 0, 0.2) //yellow
		wait(1, true)
		partylights.tint_fade(0, 0, 205, 253, 0.2) //lightblue
		wait(1, true)
		partylights.tint_fade(0, 255, 113, 45, 0.2) //orange
		wait(1, true)
		partylights.tint_fade(0, 171, 52, 255, 0.2) //magenta
		wait(1, true)
		partylights.tint_fade(0, 248, 0, 58, 0.2) //red
		wait(1, true)
		partylights.tint_fade(0, 0, 106, 236, 0.2) //darkblue second cycle
		wait(1, true)
		partylights.tint_fade(0, 232, 214, 0, 0.2) //yellow
		wait(1, true)
		partylights.tint_fade(0, 171, 52, 255, 0.2) //magenta
		wait(1, true)
		partylights.tint_fade(0, 134, 217, 0, 0.2) //green
		wait(1, true)
		partylights.tint_fade(0, 205, 0, 96, 0.2) //purple
		wait(1, true)
		partylights.tint_fade(0, 248, 0, 58, 0.2) //red
		wait(1, true)
		partylights.tint_fade(0, 0, 205, 253, 0.2) //lightblue
		wait(1, true)
		partylights.tint_fade(0, 255, 113, 45, 0.2) //orange
		wait(1, true)
		partylights.tint_fade(0, 0, 106, 236, 0.2) //darkblue
		wait(1, true)
	}
}*/

function tuxrap() {
	start_cutscene()
	Effect.sixteen_to_nine(1)
	Tux.deactivate()
	Tux.walk(160)
	wait(1)
	stop_music(1)
	wait(0.5)
	Tux.walk(0)
	wait(1)
	play_sound("speech/tux_rap.ogg")

	//RADIO.set_action("loud")
	dancefloor.set_pos(Tux.get_x(), Tux.get_y())
	ss1.set_enabled(true)
	ss2.set_enabled(true)
	ss3.set_enabled(true)
	ss4.set_enabled(true)
	ss5.set_enabled(true)
	ss6.set_enabled(true)

	// taken from the intro cutscene
	wait(6)
	Tux.do_duck()
	Tux.set_dir(false)
	wait(0.40625) // all times rounded to nearest 1/64 of a second
	Tux.do_standup()
	Tux.kick() // 0.3 seconds
	wait(0.40625)
	Tux.set_dir(true)
	wait(0.203125)
	Tux.set_dir(false)
	wait(0.203125)
	Tux.set_dir(true)
	wait(0.09375)
	Tux.do_duck() // t=7.3
	wait(0.5)
	Tux.set_dir(false)
	wait(0.203125)
	Tux.set_dir(true)
	wait(0.203125)
	Tux.set_dir(false)
	wait(0.203125)
	Tux.set_dir(true)
	wait(0.203125)
	Tux.set_dir(false)
	wait(0.203125)
	Tux.set_dir(true)
	Tux.do_standup() // t=8.8
	wait(0.203125)
	Tux.set_dir(false)
	wait(0.203125)
	Tux.kick()
	wait(0.40625)
	Tux.set_dir(true)
	Tux.do_jump(-300)
	wait(0.703125)
	Tux.do_duck() // t=10.7
	wait(0.296875)
	Tux.set_dir(false)
	wait(0.59375)
	Tux.set_dir(true)
	wait(0.703125)
	Tux.do_standup() // t=12.3
	Tux.kick()
	wait(0.5)
	Tux.do_duck()
	wait(0.5)
	Tux.do_backflip() // t=13.3
	wait(79.0/64)
	Tux.walk(200)
	wait(38.0/64)
	Tux.walk(0)
	wait(184.0/64)
	Tux.set_dir(false) //t=18.0
	wait(0.3)
	Tux.kick()
	wait(0.40625)
	Tux.do_duck()
	Tux.set_dir(true)
	wait(0.296875)
	Tux.set_dir(false) // t=19.0
	wait(0.5)
	Tux.do_standup()
	Tux.do_jump(-350)
	wait(1.5)
	Tux.kick() // t=21
	wait(0.5)
	Tux.do_duck()
	wait(0.5)
	Tux.do_jump(-400)
	wait(1.0)
	Tux.do_standup()
	wait(0.796875)
	Tux.set_dir(true) //t=23.8 - Tux sees Nolok!
	Tux.do_jump(-520)
	wait(1.5)
	sector.keep_looping = false

	df6.tint_fade(0, 0, 0, 0, 1)
	df5.tint_fade(0, 0, 0, 0, 1)
	df4.tint_fade(0, 0, 0, 0, 1)
	df3.tint_fade(0, 0, 0, 0, 1)
	df2.tint_fade(0, 0, 0, 0, 1)
	df1.tint_fade(0, 0, 0, 0, 1)

	ss1.set_enabled(false)
	ss2.set_enabled(false)
	ss3.set_enabled(false)
	ss4.set_enabled(false)
	ss5.set_enabled(false)
	ss6.set_enabled(false)

	end_cutscene()
	fade_in_music("castle/fortress.music", 1)
	Effect.four_to_three(1)
	wait(0.5)
	Tux.activate()
}
