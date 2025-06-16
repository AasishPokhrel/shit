
// Preload data necessary for the world select menu

if(!("world_select" in state)) {
  state.world_select <- {};
}

state.world_select.prefix <- "/levels/world";

for(local i = 1; i <= 4; i++) {
  if(!("/levels/world" + i + "/worldmap.stwm" in state.world_select)){
    print("Initing world " + i);
    state.world_select["/levels/world" + i + "/worldmap.stwm"] <- {};
    state.world_select["/levels/world" + i + "/worldmap.stwm"].unlocked <- (i == 1);
  }
}
