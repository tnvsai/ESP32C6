idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "."
)

# Allow user to select demo via: idf.py build -DDEMO_NAME=tasks_demo
if(DEFINED DEMO_NAME)
    set(DEMO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/demos/${DEMO_NAME}.c")
    if(EXISTS ${DEMO_PATH})
        message(STATUS "✅ Building demo: ${DEMO_NAME}")
        target_sources(${COMPONENT_LIB} PRIVATE ${DEMO_PATH})
    else()
        message(FATAL_ERROR "❌ Demo '${DEMO_NAME}' not found in ${CMAKE_CURRENT_SOURCE_DIR}/demos/")
    endif()
else()
    message(WARNING "⚠️ No DEMO_NAME specified, defaulting to 'factory_demo'")
    target_sources(${COMPONENT_LIB} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/demos/factory_demo.c)
endif()
