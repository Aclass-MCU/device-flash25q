cmake_minimum_required(VERSION 3.23)

# ── SFUD 库源文件 ────────────────────────────────────
set(SFUD_SOURCES
    sfud/src/sfud.c
    sfud/src/sfud_sfdp.c
    sfud/port/sfud_port.c
)

# ── aDev Flash 封装 ───────────────────────────────────
set(FLASH25Q_DEV_SOURCES
    aDev_flash25q.c
)

# ── 公共头文件 ────────────────────────────────────────
set(FLASH25Q_PUBLIC_HEADERS
    aDev_flash25q.h
)

set(SFUD_PUBLIC_HEADERS
    sfud/inc/sfud.h
    sfud/inc/sfud_cfg.h
    sfud/inc/sfud_def.h
    sfud/inc/sfud_flash_def.h
)

add_library(aFlash25q STATIC ${SFUD_SOURCES} ${FLASH25Q_DEV_SOURCES})

# aDev Flash 头文件
target_sources(aFlash25q PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES     ${FLASH25Q_PUBLIC_HEADERS}
)

# SFUD 头文件（独立 FILE_SET 名称，避免 BASE_DIRS 冲突）
target_sources(aFlash25q PUBLIC
    FILE_SET sfud_inc
    TYPE HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/sfud/inc
    FILES     ${SFUD_PUBLIC_HEADERS}
)

target_link_libraries(aFlash25q PUBLIC aDrv)

target_include_directories(aFlash25q PRIVATE ${PROJECT_SOURCE_DIR}/func/aShell)

install(TARGETS aFlash25q
    ARCHIVE   DESTINATION lib
    FILE_SET HEADERS  DESTINATION include/aFlash25q
    FILE_SET sfud_inc DESTINATION include/aFlash25q
)
