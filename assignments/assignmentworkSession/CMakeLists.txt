file(
 GLOB_RECURSE ASSIGNMENTworkSession_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE ASSIGNMENTworkSession_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)
#Copies this assignmentworkSession's asset folder to bin when it is built
add_custom_target(copyAssetsAworkSession ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${ASSIGNMENTworkSession_INC} DESTINATION include/assignmentworkSession)
add_executable(assignmentworkSession ${ASSIGNMENTworkSession_SRC} ${ASSIGNMENTworkSession_INC})
target_link_libraries(assignmentworkSession PUBLIC core IMGUI assimp)
target_include_directories(assignmentworkSession PUBLIC ${CORE_INC_DIR} ${stb_INCLUDE_DIR})

#Trigger asset copy when assignmentworkSession is built
add_dependencies(assignmentworkSession copyAssetsAworkSession)