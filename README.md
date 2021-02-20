# utility 0.2.1

2020-12 By GuoJH

## 提供c++以及boost中经常用到但并未提供的功能, 或者依赖于具体平台的扩展:

### platform(提供依赖于平台的扩展功能)
- windows 小型转储文件, 崩溃事件处理;
- windows 注册表便捷api;
- windows 系统服务编辑api;
- windows 获取系统信息的部分便捷api;
- windows/unix-like 获取cpu信息的便捷api;

### process(提供依赖于平台的扩展功能)
- windows 通过UAC请求特权运行指定进程;
- windows 特权进程以普通权限运行指定进程;
- windows 通过指定show参数运行指定进程; 

### string(跨平台)
- windows/unix-like 常用字符编码转换便捷api(对于windows通过native api实现, unix-like通过iconv实现);
- windows/unix-like 部分常用字符串处理函数 (对标准的扩展, 优先考虑标准库);

### filesystem(跨平台及扩展功能)
- windows/unix-like 大文件支持, 超过4GB;
- windows/unix-like 文件, 文件夹, 路径处理的便捷api;
- windows 提供创建快捷方式, 读取文件版本, 通过shell打开, 目录授权等扩展功能;

### common 一些常用而杂乱的功能, 略~
