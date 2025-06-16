function initialize()
{
  Tux.deactivate();
  Yeti.set_action("stand-left");
  Effect.sixteen_to_nine(0);
  Effect.fade_in(1.5);
  Tux.walk(225);
  wait(3);
  Tux.walk(150);
  wait(0.3);
  Tux.walk(75);
  wait(0.1);
  Tux.walk(0)
  wait(0.5);
  Text.set_text(_("Exhausted from his long walk, Tux decided to take a little break."));
  Text.fade_in(1);
  wait(4);
  Text.fade_out(1);
  wait(1.5);
  play_sound("sounds/yeti_roar.wav");
  wait(0.2);
  Text.set_text(_("But then, all of a sudden he heard a loud roar from a distance."));
  Text.fade_in(1);
  wait(4);
  Text.fade_out(1);
  wait(1);
  Camera.scroll_to(1350, 300, 5);
  wait(5.5);
  Yeti.set_action("stomp-left");
  play_sound("sounds/yeti_gna.wav");
  Yeti.set_velocity(0, -300);
 wait(0.5);
  Yeti.set_action("stand-left");
  wait(0.5);
  Yeti.set_action("walking-right");
  play_sound("sounds/yeti_roar.wav");
  Yeti.set_velocity(250, 0);
  wait(1);
  play_sound("sounds/yeti_roar.wav");
  wait(1);
  Camera.scroll_to(825, 480, 6);
  wait(3);
  Tux.walk(200);
  wait(2);
  Tux.walk(150);
  wait(0.3);
  Tux.walk(75);
  wait(0.1);
  Tux.walk(0);
  wait(1);
  Text.set_text(_("A yeti seems to have observed him from afar."));
  Text.fade_in(1);
  wait(4);
  Text.fade_out(1);
  wait(2);
  Text.set_text(_("Concerned by the yeti's sudden retreat, Tux decided to continue his journey, ..."));
  Text.fade_in(1);
  wait(4);
  Text.fade_out(1);
  wait(0.5);
  Tux.walk(225);
  wait(1);
  Text.set_text(_("...while secretly hoping not to run into the yeti again, by any means."));
  Text.fade_in(1);
  wait(4);
  Text.fade_out(1);
  wait(2);
  Effect.fade_out(2);
  wait(3.5);
  end_cutscene();
  Level.finish(true);
}

state_idx <- 0;
states <- { init=0, start=1};
function trigger_state(state) {
  local idx = states[state];
  if(!idx || idx <= state_idx)
    return;
  state_idx = idx;
  switch(state) {
    case "start":
      initialize();
      break;
  }
}
