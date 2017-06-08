#ext4_utils#

没有在 Windows 模式编译测试，只在 Linux 下编译测试通过

基于 Android 5.0 代码修改

编译步骤：

```shell
    mkdir workspace
    cd wrokspace
    git clone git@192.168.100.22:tenfar/ext4_utils.git ext4_utils
    cd ext4_utils
    make -B
```

然后生成的linux可执行文件就在 ext4_utils 文件夹下

此项目代码包含 SELinux 相关代码，支持 Sparse 格式文件系统

使用实例：

创建 make_ext4.bat

写入：

E:\git-base.192.168.100.22-new\tengfar_ext4_utils_on_server\make_ext4fs.exe -v -l 500M -a /system -S E:\pack_unpack\system.selinux.nosparse.nomount.readonly.img.selinux_context.txt -C E:\pack_unpack\system.selinux.nosparse.nomount.readonly.img.fs_config.txt E:\pack_unpack\my.system.cygwin.img E:\pack_unpack\system.selinux.nosparse.nomount.readonly.img.dir 2>E:\pack_unpack\system.cygwin.img.txt

-v 打印详细信息
-l 是打包大小
-a 是挂载点
-S 是selinux context文件，需要做去正则转义
-C 指定文件权限，属主属组，以及capibilities

E:\pack_unpack\my.system.cygwin.img 是输出文件
E:\pack_unpack\system.selinux.nosparse.nomount.readonly.img.dir 是待打包文件夹
2>E:\pack_unpack\system.cygwin.img.txt 是stderr重定向文件

以下两个文件已上传，内容格式可以作为参考：

system.selinux.nosparse.nomount.readonly.img.selinux_context.txt
system.selinux.nosparse.nomount.readonly.img.fs_config.txt




