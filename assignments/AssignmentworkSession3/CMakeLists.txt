file(
 GLOB_RECURSE ASSIGNMENTworkSession3_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE ASSIGNMENTworkSession3_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)
#Copies this assignmentworkSession3's asset folder to bin when it is built
add_custom_target(copyAssetsAworkSession3 ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${ASSIGNMENTworkSession3_INC} DESTINATION include/assignmentworkSession3)
add_executable(assignmentworkSession3 ${ASSIGNMENTworkSession3_SRC} ${ASSIGNMENTworkSession3_INC})
target_link_libraries(assignmentworkSession3 PUBLIC core IMGUI assimp)
target_include_directories(assignmentworkSession3 PUBLIC ${CORE_INC_DIR} ${stb_INCLUDE_DIR})

#Trigger asset copy when assignmentworkSession3 is built
add_dependencies(assignmentworkSession3 copyAssetsAworkSession3)