## If xgettext is available, generate messages.pot for sources
option(GENERATE_MESSAGESPOT "Generate messages.pot files" OFF)

if(GENERATE_MESSAGESPOT)
  find_program(XGETTEXT_EXECUTABLE xgettext)

  if(NOT XGETTEXT_EXECUTABLE)
    message(STATUS "Warning: xgettext not found - will not update messages.pot")
  else()
    message(STATUS "Found xgettext: ${XGETTEXT_EXECUTABLE}")

    set(MESSAGES_POT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/data/locale/messages.pot)
    add_custom_command(
      OUTPUT ${MESSAGES_POT_FILE}
      COMMAND ${XGETTEXT_EXECUTABLE}
      ARGS --keyword=_ --language=C++ --output=${MESSAGES_POT_FILE} ${SUPERTUX_SOURCES_CXX}
      DEPENDS ${SUPERTUX_SOURCES_CXX}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMENT "Generating POT file ${MESSAGES_POT_FILE}"
      )
    list(APPEND MESSAGES_POT_FILES ${MESSAGES_POT_FILE})

    file(GLOB SUPERTUX_LEVEL_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} data/levels/*)
    foreach(SUPERTUX_LEVEL_DIR ${SUPERTUX_LEVEL_DIRS})
      file(GLOB SUPERTUX_LEVELS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${SUPERTUX_LEVEL_DIR}/*.stl ${SUPERTUX_LEVEL_DIR}/*.stwm ${SUPERTUX_LEVEL_DIR}/*.txt)

      ## Do not add target if SUPERTUX_LEVELS is empty. This is needed for cmake <=2.4
      if(SUPERTUX_LEVELS)
        set(MESSAGES_POT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${SUPERTUX_LEVEL_DIR}/messages.pot)
        add_custom_command(
          OUTPUT ${MESSAGES_POT_FILE}
          COMMAND ${XGETTEXT_EXECUTABLE}
          ARGS --keyword="_:1" --language=Lisp --force-po --sort-by-file "--output=${MESSAGES_POT_FILE}" ${SUPERTUX_LEVELS}
          DEPENDS ${SUPERTUX_LEVELS}
          WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
          COMMENT "Generating POT file ${MESSAGES_POT_FILE}"
          )
        list(APPEND MESSAGES_POT_FILES ${MESSAGES_POT_FILE})
      endif()

    endforeach(SUPERTUX_LEVEL_DIR)

    add_custom_target(
      supertux2-messages ALL
      DEPENDS ${MESSAGES_POT_FILES}
      )

  endif()

  mark_as_advanced(
    XGETTEXT_EXECUTABLE
    )
endif()

# EOF #
