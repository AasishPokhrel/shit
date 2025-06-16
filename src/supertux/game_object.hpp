//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_GAME_OBJECT_HPP
#define HEADER_SUPERTUX_SUPERTUX_GAME_OBJECT_HPP

#include "squirrel/exposable_class.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <optional>
#include <typeindex>

#include "editor/object_settings.hpp"
#include "supertux/game_object_component.hpp"
#include "util/fade_helper.hpp"
#include "util/gettext.hpp"
#include "util/uid.hpp"

class DrawingContext;
class GameObjectComponent;
class GameObjectManager;
class ObjectRemoveListener;
class ReaderMapping;
class Writer;

namespace ssq {
class VM;
} // namespace ssq

struct GameObjectType
{
  const std::string id;
  const std::string name;
};
typedef std::vector<GameObjectType> GameObjectTypes;

/**
  A helper structure to list all the type_indexes of the classes in the
  type hierarchy of a given class. This makes it easier to register e.g.
  a MrIceblock in lists for MrIceBlock, WalkingBadguy, Badguy, Portable,
  MovingSprite, MovingObject, and GameObject.
*/
struct GameObjectClasses
{
  std::vector<std::type_index> types;

  GameObjectClasses& add(const std::type_info& info)
  {
    types.emplace_back(info);
    return *this;
  }
};

/**
   This class is responsible for:
    * Updating and drawing the object. This should happen in the update() and
      draw() functions. Both are called once per frame.
    * Providing a safe way to remove the object by calling the remove_me
      functions.
 */
/**
 * @scripting
 * @summary Base class for all the things that make up Levels' Sectors.${SRG_NEWPARAGRAPH}

            Each sector of a level holds a list of active ""GameObject""s, while the
            game is played.${SRG_NEWPARAGRAPH}
*/
class GameObject : public ExposableClass
{
  friend class GameObjectManager;

public:
  static void register_class(ssq::VM& vm);

public:
  GameObject(const std::string& name = "");
  GameObject(const ReaderMapping& reader);
  virtual ~GameObject() override;

  /** Called after all objects have been added to the Sector and the
      Sector is fully constructed. If objects refer to other objects
      by name, those connection can be resolved here. */
  virtual void finish_construction() {}

  inline UID get_uid() const { return m_uid; }

  /** This function is called once per frame and allows the object to
      update it's state. The dt_sec is the time that has passed since
      the last frame in seconds and should be the base for all timed
      calculations (don't use SDL_GetTicks directly as this will fail
      in pause mode). This function is not called in the Editor. */
  virtual void update(float dt_sec) = 0;

  /** The GameObject should draw itself onto the provided
      DrawingContext if this function is called. */
  virtual void draw(DrawingContext& context) = 0;

  /** This function saves the object. Editor will use that. */
  virtual void save(Writer& writer);
  std::string save();
  virtual std::string get_class_name() const { return "game-object"; }
  virtual std::string get_exposed_class_name() const override { return "GameObject"; }
  /**
   * @scripting
   * @description Returns the display name of the object, translated to the user's locale.
   */
  virtual std::string get_display_name() const { return _("Unknown object"); }
  /** List notable classes in inheritance hierarchy of class. This makes it possible
      to efficiently look up all objects deriving from a particular intermediate class */
  virtual GameObjectClasses get_class_types() const;

  /** Version checking/updating, patch information */
  virtual std::vector<std::string> get_patches() const;
  virtual void update_version();
  /**
   * @scripting
   * @description Returns the current version of the object.
   */
  inline int get_version() const { return m_version; }
  /**
   * @scripting
   * @description Returns the latest version of the object.
   */
  int get_latest_version() const;
  /**
   * @scripting
   * @description Checks whether the object's current version is equal to its latest one.
   */
  bool is_up_to_date() const;

  /** If true only a single object of this type is allowed in a
      given GameObjectManager */
  virtual bool is_singleton() const { return false; }

  /** Does this object have variable size
      (secret area trigger, wind, etc.) */
  virtual bool has_variable_size() const { return false; }

  /** Indicates if the object will be saved. If false, the object will
      be skipped on saving and can't be cloned in the editor. */
  virtual bool is_saveable() const { return true; }

  /** Indicates if the object's state should be tracked.
      If false, load_state() and save_state() calls would not do anything. */
  virtual bool track_state() const { return true; }

  /** Indicates if the object should be added at the beginning of the object list. */
  virtual bool has_object_manager_priority() const { return false; }

  /** Returns the amount of coins that this object is worth.
      This is considered when calculating all coins in a level. */
  virtual int get_coins_worth() const { return 0; }

  /** Indicates if get_settings() is implemented. If true the editor
      will display Tip and ObjectMenu. */
  virtual bool has_settings() const { return is_saveable(); }
  virtual ObjectSettings get_settings();

  /** Get all types of the object, if available. **/
  virtual GameObjectTypes get_types() const;
  /**
   * @scripting
   * @description Returns the type index of the object.
   */
  inline int get_type() const { return m_type; }

  virtual void after_editor_set();

  /** When level is flipped vertically */
  virtual void on_flip(float height) {}

  /** schedules this object to be removed at the end of the frame */
  virtual void remove_me() { m_scheduled_for_removal = true; }

  /** returns true if the object is not scheduled to be removed yet */
  inline bool is_valid() const { return !m_scheduled_for_removal; }

  /** registers a remove listener which will be called if the object
      gets removed/destroyed */
  void add_remove_listener(ObjectRemoveListener* listener);

  /** unregisters a remove listener, so it will no longer be called if
      the object gets removed/destroyed */
  void del_remove_listener(ObjectRemoveListener* listener);

  inline void set_name(const std::string& name) { m_name = name; }
  /**
   * @scripting
   * @description Returns the name of the object.
   */
  inline const std::string& get_name() const { return m_name; }

  /** stops all looping sounds */
  virtual void stop_looping_sounds() {}

  /** continues all looping sounds */
  virtual void play_looping_sounds() {}

  template<typename T>
  T* get_component() {
    for(auto& component : m_components) {
      if (T* result = dynamic_cast<T*>(component.get())) {
        return result;
      }
    }
    return nullptr;
  }

  void add_component(std::unique_ptr<GameObjectComponent> component) {
    m_components.emplace_back(std::move(component));
  }

  void remove_component(GameObjectComponent* component) {
    auto it = std::find_if(m_components.begin(), m_components.end(),
                           [component](const std::unique_ptr<GameObjectComponent>& lhs){
                             return lhs.get() == component;
                           });
    if (it != m_components.end()) {
      m_components.erase(it);
    }
  }

  /** Save/check the current state of the object. */
  virtual void save_state();
  virtual void check_state();

  /** The editor requested the deletion of the object */
  virtual void editor_delete() { remove_me(); }

  /** The user clicked on the object in the editor and selected it*/
  virtual void editor_select() {}

  /** The object got deselected */
  virtual void editor_deselect() {}

  /** Called each frame in the editor, used to keep linked objects
      together (e.g. platform on a path) */
  virtual void editor_update() {}

  inline GameObjectManager* get_parent() const { return m_parent; }

protected:
  /** Parse object type. **/
  void parse_type(const ReaderMapping& reader);

  /** When the type has been changed from the editor. **/
  enum TypeChange { INITIAL = -1 }; // "old_type < 0" indicates initial call
  virtual void on_type_change(int old_type) {}

  /** Conversion between type ID and value. **/
  int type_id_to_value(const std::string& id) const;
  std::string type_value_to_id(int value) const;

private:
  inline void set_uid(const UID& uid) { m_uid = uid; }

private:
  /** The parent GameObjectManager. Set by the manager itself. */
  GameObjectManager* m_parent;

protected:
  /** a name for the gameobject, this is mostly a hint for scripts and
      for debugging, don't rely on names being set or being unique */
  std::string m_name;

  /** Type of the GameObject. Used to provide special functionality,
      based on the child object. */
  int m_type;

  /** Fade Helpers are for easing/fading script functions */
  std::vector<std::unique_ptr<FadeHelper>> m_fade_helpers;

  /** Track the following creation/deletion of this object for undo.
      If track_state() returns false, this object would not be tracked,
      regardless of the value of this variable. */
  bool m_track_undo;

private:
  /** The object's type at the time of the last get_settings() call.
      Used to check if the type has changed. **/
  int m_previous_type;

  /** Indicates the object's version. By default, this is equal to 1.
      Useful for retaining retro-compatibility for objects, whilst allowing for
      updated behaviour in newer levels.
      The version of an object can be updated from the editor. */
  int m_version;

  /** A unique id for the object to safely refer to it. This will be
      set by the GameObjectManager. */
  UID m_uid;

  /** this flag indicates if the object should be removed at the end of the frame */
  bool m_scheduled_for_removal;

  /** The object's settings at the time of the last state save.
      Used to check for changes that may have occured. */
  std::optional<ObjectSettings> m_last_state;

  std::vector<std::unique_ptr<GameObjectComponent> > m_components;

  std::vector<ObjectRemoveListener*> m_remove_listeners;

private:
  GameObject(const GameObject&) = delete;
  GameObject& operator=(const GameObject&) = delete;
};

#endif

/* EOF */
