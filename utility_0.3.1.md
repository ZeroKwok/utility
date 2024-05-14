# utility

version: 0.3.1

- include
  - build.h.in      (构建信息)
  - resource.rc.in  (资源文件 for windows)
  - utility
    - config.h      (配置文件 for all projects)
    - utility.h     (all in one header)
    - error.h
    - platform.h    (平台相关)
    - cpu.h         (cpu 相关)
    - filepath.h    (文件路径相关, 仅扩展 std::filesystem::path)
    - filesystem.h  (文件系统相关 包含 filepath.h, 跨平台的文件处理)
    - string.h      (字符串相关, 格式化使用 fmtlib or std::format)
    - win
      - service.h
      - registry.h
      - dump.h
      - process.h   (扩展 boost::process)
    - console.h
    - math.h
    - unit.h
    - time.h
    - version.h
    - semaphore.h
    - scope.h
    - digest.h
    - assert.h
    - encryption.h
    - bytes.h
