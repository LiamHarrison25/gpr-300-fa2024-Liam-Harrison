file(
 GLOB_RECURSE ASSIGNMENTworkSession2_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE ASSIGNMENTworkSession2_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)
#Copies this assignmentworkSession2's asset folder to bin when it is built
add_custom_target(copyAssetsAworkSession2 ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${ASSIGNMENTworkSession2_INC} DESTINATION include/assignmentworkSession2)
add_executable(assignmentworkSession2 ${ASSIGNMENTworkSession2_SRC} ${ASSIGNMENTworkSession2_INC})
target_link_libraries(assignmentworkSession2 PUBLIC core IMGUI assimp)
target_include_directories(assignmentworkSession2 PUBLIC ${CORE_INC_DIR} ${stb_INCLUDE_DIR})

#Trigger asset copy when assignmentworkSession2 is built
add_dependencies(assignmentworkSession2 copyAssetsAworkSession2)