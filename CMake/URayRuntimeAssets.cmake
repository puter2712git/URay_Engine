add_custom_command(TARGET URay_Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${URAY_SOURCE_DIR}/Asset"
        "$<TARGET_FILE_DIR:URay_Engine>/Asset"
)

set_target_properties(URay_Engine PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:URay_Engine>"
)
