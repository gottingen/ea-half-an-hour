hala ea
==================

* 安装基础环境 详细参考 [EA 基础环境部署](https://github.com/gottingen/kumo-search/blob/master/docs/install.md)
* 使用 `ea` 命令行工具 `carbin` 创建项目
* 编译项目

在EA的基础环境中，运行
    
    ```bash
    mkdir halaea
    cd halaea
    carbin create --name halaea --benchmark --test --examples --requirements
    mkdir build
    cd build
    cmake ..
    make
    make package
    ```
恭喜你，你已经成功编译了一个EA项目。