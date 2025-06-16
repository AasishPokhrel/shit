//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2018 Ingo Ruhnke <grumbel@gmail.com>
//                2023 Vankata453
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_SUPERTUX_SUPERTUX_GAME_OBJECT_MANAGER_HPP
#define HEADER_SUPERTUX_SUPERTUX_GAME_OBJECT_MANAGER_HPP

#include "squirrel/exposable_class.hpp"

#include <functional>
#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "supertux/game_object.hpp"
#include "supertux/game_object_change.hpp"
#include "util/uid_generator.hpp"

class DrawingContext;
class MovingObject;
class TileMap;

template<class T> class GameObjectRange;

/**
 * @scripting
 * @summary This class provides basic controlling functions for a sector. Applies for both worldmap and in-level sectors.
 * @instances For in-level sectors, an instance under ""sector.settings"" is available from scripts and the console.${SRG_NEWPARAGRAPH}
              For worldmap sectors, such instance is available under ""worldmap.settings"".
 */
class GameObjectManager : public ExposableClass
{
public:
  static bool s_draw_solids_only;

public:
  static void register_class(ssq::VM& vm);

private:
  struct NameResolveRequest
  {
    std::string name;
    std::function<void (UID)> callback;
  };

public:
  GameObjectManager(bool undo_tracking = false);
  virtual ~GameObjectManager() override;

  virtual std::string get_exposed_class_name() const override { return "GameObjectManager"; }

  /** Queue an object up to be added to the object list */
  GameObject& add_object(std::unique_ptr<GameObject> object);
  void clear_objects();

  template<typename T, typename... Args>
  T& add(Args&&... args)
  {
    auto obj = std::make_unique<T>(std::forward<Args>(args)...);
    obj->update_version();
    T& obj_ref = *obj;
    add_object(std::move(obj));
    return obj_ref;
  }

  void update(float dt_sec);
  void draw(DrawingContext& context);

  const std::vector<std::unique_ptr<GameObject> >& get_objects() const;

  /** Commit the queued up additions and deletions to the object list */
  void flush_game_objects();

  /**
   * @scripting
   * @description Sets the sector's ambient light to the specified color.
   * @param float $red
   * @param float $green
   * @param float $blue
   */
  void set_ambient_light(float red, float green, float blue);
  /**
   * @scripting
   * @description Fades to a specified ambient light color in ""fadetime"" seconds.
   * @param float $red
   * @param float $green
   * @param float $blue
   * @param float $fadetime
   */
  void fade_to_ambient_light(float red, float green, float blue, float fadetime);
  /**
   * @scripting
   * @description Returns the red channel of the ambient light color.
   */
  float get_ambient_red() const;
  /**
   * @scripting
   * @description Returns the green channel of the ambient light color.
   */
  float get_ambient_green() const;
  /**
   * @scripting
   * @description Returns the blue channel of the ambient light color.
   */
  float get_ambient_blue() const;

  /**
   * @scripting
   * @description Sets the sector's music.
   * @param string $music Full filename, relative to the "music" folder.
   */
  void set_music(const std::string& music);

  /**
   * @scripting
   * @description Adds a ""MovingObject"" to the manager.
                  Note: If adding objects to a worldmap sector, ""posX"" and ""posY"" have to be tile positions (sector position / 32).
   * @param string $class_name GameObject's class.
   * @param string $name Name of the created object.
   * @param float $pos_x X position inside the current sector.
   * @param float $pos_y Y position inside the current sector.
   * @param string $direction Direction.
   * @param string $data Additional data in S-Expression format (check object definitions in level files).
   */
  void add_object(const std::string& class_name, const std::string& name,
                  float pos_x, float pos_y, const std::string& direction,
                  const std::string& data);

  float get_width() const;
  float get_height() const;
  float get_editor_width() const;
  float get_editor_height() const;

  /** returns the width (in tiles) of a worldmap */
  float get_tiles_width() const;

  /** returns the height (in tiles) of a worldmap */
  float get_tiles_height() const;

  /** Hook that is called before an object is added to the vector */
  virtual bool before_object_add(GameObject& object) = 0;

  /** Hook that is called before an object is removed from the vector */
  virtual void before_object_remove(GameObject& object) = 0;

  template<class T>
  GameObjectRange<T> get_objects_by_type() const
  {
    return GameObjectRange<T>(*this);
  }

  const std::vector<GameObject*>&
  get_objects_by_type_index(std::type_index type_idx) const
  {
    auto it = m_objects_by_type_index.find(type_idx);
    if (it == m_objects_by_type_index.end()) {
      // use a dummy return value to avoid making this method non-const
      static std::vector<GameObject*> dummy;
      return dummy;
    } else {
      return it->second;
    }
  }

  template<class T>
  T& get_singleton_by_type() const
  {
    const auto& range = get_objects_by_type<T>();
    assert(range.begin() != range.end());
    assert(range.begin()->is_singleton());
    return *range.begin();
  }

  template<class T>
  T* get_object_by_uid(const UID& uid) const
  {
    auto it = m_objects_by_uid.find(uid);
    if (it == m_objects_by_uid.end())
    {
      // FIXME: Is this a good idea? Should gameobjects be made
      // accessible even when not fully inserted into the manager?
      for (auto&& itnew : m_gameobjects_new)
      {
        if (itnew->get_uid() == uid)
          return static_cast<T*>(itnew.get());
      }
      return nullptr;
    }
    else
    {
#ifdef NDEBUG
      return static_cast<T*>(it->second);
#else
      // Since uids should be unique, there should be no need to guess
      // the type, thus we assert() when the object type is not what
      // we expected.
      auto ptr = dynamic_cast<T*>(it->second);
      assert(ptr != nullptr);
      return ptr;
#endif
    }
  }

  /** Move an object to another GameObjectManager.
      NOTE: The UID of the object will be re-generated. */
  void move_object(const UID& uid, GameObjectManager& other);

  /** Register a callback to be called once the given name can be
      resolved to a UID. Note that this function is only valid in the
      construction phase, not during draw() or update() calls, use
      get_object_by_uid() instead. */
  void request_name_resolve(const std::string& name, std::function<void (UID)> callback);

  template<class T>
  T* get_object_by_name(const std::string& name) const
  {
    auto it = m_objects_by_name.find(name);
    if (it == m_objects_by_name.end())
    {
      return nullptr;
    }
    else
    {
      return dynamic_cast<T*>(it->second);
    }
  }

  /** Get total number of GameObjects of given type */
  template<class T>
  int get_object_count(std::function<bool(const T&)> predicate = nullptr) const
  {
    int total = 0;
    for (auto& obj : get_objects_by_type<T>())
    {
      if (predicate == nullptr || predicate(obj))
        total += 1;
    }
    return total;
  }

  inline const std::vector<TileMap*>& get_solid_tilemaps() const { return m_solid_tilemaps; }
  inline const std::vector<TileMap*>& get_all_tilemaps() const { return m_all_tilemaps; }
  
  void update_solid(TileMap* solid);

  /** Toggle object change tracking for undo/redo. */
  void toggle_undo_tracking(bool enabled);
  inline bool undo_tracking_enabled() const { return m_undo_tracking; }

  /** Set undo stack size. */
  void set_undo_stack_size(int size);

  /** Remove old object changes that exceed the undo stack size limit. */
  void undo_stack_cleanup();

  /** Undo/redo changes to GameObjects in the manager.
      Utilized by the Editor. */
  void undo();
  void redo();

  /** Apply saved object changes. */
  void apply_object_change(const GameObjectChange& change, bool track_undo);
  void apply_object_changes(const GameObjectChangeSet& changes, bool track_undo);

  /** Save object settings changes in the undo stack.
      Used to save an object's previous state before a change had occurred. */
  void save_object_change(const GameObject& object, const ObjectSettings& settings);

  /** Clear undo/redo stacks. */
  void clear_undo_stack();

  /** Indicate if there are any unsaved object changes in the undo stack.
      @see m_last_saved_change */
  bool has_object_changes() const;

  /** Called on editor level save. */
  void on_editor_save();

protected:
  /** Add a MovingObject from scripting. */
  virtual MovingObject& add_object_scripting(const std::string& class_name, const std::string& name,
                                             const Vector& pos, const std::string& direction,
                                             const std::string& data);

  void update_tilemaps();

  void process_resolve_requests();

  /** Same as process_resolve_requests(), but those it can't find will be kept in the buffer */
  void try_process_resolve_requests();

  template<class T>
  T* get_object_by_type() const
  {
    const auto& range = get_objects_by_type<T>();
    if (range.begin() == range.end()) {
      return nullptr;
    } else {
      return &*range.begin();
    }
  }

private:
  /** Create object from object change. */
  void create_object_from_change(const GameObjectChange& change, bool track_undo);

  /** Parse object settings ("supertux-game-object") from a string. */
  static void parse_object_settings(ObjectSettings& settings, const std::string& data);

  /** Save old or new state of object settings. */
  static std::string save_object_settings_state(const ObjectSettings& settings, bool new_state);

  /** Undo/redo object change. */
  void process_object_change(GameObjectChange& change);

  /** Save object state in the undo stack. */
  void save_object_state(GameObject& object, GameObjectChange::Action action);

  void this_before_object_add(GameObject& object);
  void this_before_object_remove(GameObject& object);

protected:
  /** An initial flush_game_objects() call has been initiated. */
  bool m_initialized;

private:
  UIDGenerator m_uid_generator;

  /** Undo/redo variables */
  UIDGenerator m_change_uid_generator;
  bool m_undo_tracking;
  int m_undo_stack_size;
  std::vector<GameObjectChangeSet> m_undo_stack;
  std::vector<GameObjectChangeSet> m_redo_stack;
  std::vector<GameObjectChange> m_pending_change_stack; // Before a flush, any changes go here
  UID m_last_saved_change;

  std::vector<std::unique_ptr<GameObject>> m_gameobjects;

  /** container for newly created objects, they'll be added in flush_game_objects() */
  std::vector<std::unique_ptr<GameObject>> m_gameobjects_new;

  /** Former UIDs of all objects moved to another GameObjectManager.
      Will be assigned back to any of the objects if they are moved back here. */
  std::unordered_map<GameObject*, UID> m_moved_object_uids;

  /** Fast access to solid tilemaps */
  std::vector<TileMap*> m_solid_tilemaps;

  /** Fast access to all tilemaps */
  std::vector<TileMap*> m_all_tilemaps;

  std::unordered_map<std::string, GameObject*> m_objects_by_name;
  std::unordered_map<UID, GameObject*> m_objects_by_uid;
  std::unordered_map<std::type_index, std::vector<GameObject*> > m_objects_by_type_index;

  std::vector<NameResolveRequest> m_name_resolve_requests;

private:
  GameObjectManager(const GameObjectManager&) = delete;
  GameObjectManager& operator=(const GameObjectManager&) = delete;
};

#include "supertux/game_object_iterator.hpp"

#endif

/* EOF */
