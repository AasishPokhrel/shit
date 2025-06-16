
import("/levels/preload_worldselect.nut");

state.world_select["/levels/world2/worldmap.stwm"].unlocked <- true;

if("ambient_r" in state && "ambient_g" in state && "ambient_b" in state){
  worldmap.settings.fade_to_ambient_light(state.ambient_r,
                                          state.ambient_g,
                                          state.ambient_b,
                                          0);
}

if(! ("ghostforest" in state)){
	state.ghostforest <- false;
  print("[DEBUG] Ghost Forest state initialized\n");
}

function go_ghostforest(under){
  fade_time <- 0.35;
  ghost_darkness.fade(under ? 1 : 0, fade_time);
  ghost_land.fade(under ? 1 : 0, fade_time);
  ghost_convex.fade(under ? 1 : 0, fade_time);
  ghost_edges.fade(under ? 1 : 0, fade_time);
  ghost_back.fade(under ? 1 : 0, fade_time);
  ghost_front.fade(under ? 1 : 0, fade_time);
  ghost_trees0.fade(under ? 1 : 0, fade_time);
  ghost_trees1.fade(under ? 1 : 0, fade_time);
  ghost_trees2.fade(under ? 1 : 0, fade_time);
  ghost_trees3.fade(under ? 1 : 0, fade_time);
  ghost_trees4.fade(under ? 1 : 0, fade_time);
  land_trees1.fade(under ? 0 : 1, fade_time);
  land_trees2.fade(under ? 0 : 1, fade_time);
  land_trees3.fade(under ? 0 : 1, fade_time);
  land_trees4.fade(under ? 0 : 1, fade_time);
  back_trees.fade(under ? 0 : 1, fade_time);
  state.ghostforest <- under;
}

go_ghostforest(state.ghostforest);
