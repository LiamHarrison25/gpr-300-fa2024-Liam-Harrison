file(
 GLOB_RECURSE ASSIGNMENTworkSession4_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE ASSIGNMENTworkSession4_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)
#Copies this assignmentworkSession4's asset folder to bin when it is built
add_custom_target(copyAssetsAworkSession4 ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${ASSIGNMENTworkSession4_INC} DESTINATION include/assignmentworkSession4)
add_executable(assignmentworkSession4 ${ASSIGNMENTworkSession4_SRC} ${ASSIGNMENTworkSession4_INC})
target_link_libraries(assignmentworkSession4 PUBLIC core IMGUI assimp)
target_include_directories(assignmentworkSession4 PUBLIC ${CORE_INC_DIR} ${stb_INCLUDE_DIR})

#Trigger asset copy when assignmentworkSession4 is built
add_dependencies(assignmentworkSession4 copyAssetsAworkSession4)