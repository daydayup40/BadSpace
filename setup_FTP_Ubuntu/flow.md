# ubuntu上配置FTP

## 1. 首先用命令检查是否安装了vsftpd

```
vsftpd -version
```
可以显示版本信息，如果没有显示，安装一下
```
sudo apt-get install vsftpd
```

## 2. 新建一个文件夹用于FTP的工作目录
```
su
mkdir /home/ftp
```

## 3. 新建FTP用户并设置密码以及工作目录
ftpname为你为该ftp创建的用户名
```
su
useradd -d /home/ftp -s /bin/bash ftpname
passwd ftpname
chown ftpname -c /home/ftp
```

## 4. 修改vsftpd配置文件
```
su
vi /etc/vsftpd.conf
```
设置属性值

anonymous_enable=NO

local_enable=YES

write_enable =YES

保存返回

## 5. 启动vsftpd服务
```
sudo service vsftpd start
```

## 6. 本地访问测试一下
