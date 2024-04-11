function(install_pdb target_name)
    get_target_property(target_pdb_name_debug       ${target_name} COMPILE_PDB_NAME_DEBUG)
    get_target_property(target_pdb_name_release     ${target_name} COMPILE_PDB_NAME_RELEASE)
    get_target_property(target_pdb_output_directory ${target_name} PDB_OUTPUT_DIRECTORY)

    # ��vs2019��, �ļ����Ŀ¼ΪCMAKE_ARCHIVE_OUTPUT_DIRECTORY,
    # ��cmd������CMAKE_ARCHIVE_OUTPUT_DIRECTORY/CMAKE_INSTALL_CONFIG_NAME
    install(FILES
        "${target_pdb_output_directory}/\${CMAKE_INSTALL_CONFIG_NAME}/$<$<CONFIG:Debug>:${target_pdb_name_debug}>$<$<NOT:$<CONFIG:Debug>>:${target_pdb_name_release}>.pdb"
        DESTINATION lib
        OPTIONAL)

    install(FILES
        "${target_pdb_output_directory}/$<$<CONFIG:Debug>:${target_pdb_name_debug}>$<$<NOT:$<CONFIG:Debug>>:${target_pdb_name_release}>.pdb"
        DESTINATION lib
        OPTIONAL)
endfunction()