function(install_pdb target_name)

    #set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
    #set_target_properties(${PROJECT_NAME} PROPERTIES 
    #PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    #COMPILE_PDB_NAME_DEBUG  "${PROJECT_NAME}d"
    #COMPILE_PDB_NAME_RELEASE ${PROJECT_NAME}
    #COMPILE_PDB_NAME_MINSIZEREL ${PROJECT_NAME}
    #COMPILE_PDB_NAME_RELWITHDEBINFO ${PROJECT_NAME})

    get_target_property(PROJECT_PDB_NAME_DEBUG    ${target_name} COMPILE_PDB_NAME_DEBUG)
    get_target_property(PROJECT_PDB_NAME_RELEASE  ${target_name} COMPILE_PDB_NAME_RELEASE)
    get_target_property(PROJECT_PDB_DIRECTORY     ${target_name} PDB_OUTPUT_DIRECTORY)
    set(PROJECT_PDB_NAME "$<$<CONFIG:Debug>:${PROJECT_PDB_NAME_DEBUG}>$<$<NOT:$<CONFIG:Debug>>:${PROJECT_PDB_NAME_RELEASE}>.pdb")
    
    # 在cmd中pdb输出目录为 ${PROJECT_PDB_DIRECTORY}/${CMAKE_INSTALL_CONFIG_NAME}
    install(FILES "${PROJECT_PDB_DIRECTORY}/\${CMAKE_INSTALL_CONFIG_NAME}/${PROJECT_PDB_NAME}" DESTINATION lib OPTIONAL)

    # 在vs2019中pdb输出目录为 ${PROJECT_PDB_DIRECTORY}/${CMAKE_INSTALL_CONFIG_NAME}
    install(FILES "${PROJECT_PDB_DIRECTORY}/${PROJECT_PDB_NAME}" DESTINATION lib OPTIONAL)
endfunction()