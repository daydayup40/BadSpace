# Ubuntu 上配置Postfix邮件服务器

## 1. 组件安装
```
sudo apt-get install postfix
```
_type of mail : Internet Site_  
_mail name : user-PC_

```
su
apt-get install dovecot-common
apt-get install dovecot-imapd dovecot-pop3d
apt-get install bsd-mailx
```

## 2. 基本设置
编辑 /etc/postfix/main.cf 文件，做如下更改：  
### (1). 为支持 TLS 安全连接，确保证书可用（通常默认安装已生成相应文件）
smtpd_tls_cert_file=/etc/ssl/certs/ssl-cert-snakeoil.pem  
smtpd_tls_key_file=/etc/ssl/private/ssl-cert-snakeoil.key  
smtpd_use_tls=yes  

使用安全连接可保证通过客户端发送邮件时不被截获和窃取。

### (2). 保证邮件服务器的域名存在于下述列表中
mydestination = $myhostname, user-PC, localhost.localdomain, , localhost   
这样收件人为该域名的邮件才会被服务器留存而不是转给其他 MTA。

### (3). 侦听所有网口
inet_interfaces = all  

### (4). 使用 Maildir 格式存放数据
home_mailbox = Maildir/  

### (5). 最后
```
sudo service postfix reload
sudo service postfix restart
```

## 3. 验证 Postfix 和添加账户
### (1). 测试一下
执行
```
telnet localhost smtp
```
后会出现
```
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
220 user-PC.localdomain ESMTP Postfix (Ubuntu)
```
然后依次键入：(注意quit前面的'.'一定是单独一行输入后换行,另外，下面user位置需要填入一个确实存在的用户名)
```
mail from: root@localhost  
rcpt to: user@localhost
data  
Subject: Hello  
Hi, how are you?  
Regards,  
Admin.  
.  
quit  
```
如果执行正常，   
_/home/user_   
下会出现Maildir目录，且   
_/home/user/Maildir/new/_   
目录下会找到刚才发送的那封邮件

## 4. 验证POP3与IMAP协议
打开
```
/usr/bin/thunderbird
```
账户设置:   
_名字：user_   
_邮件地址：user@localhost_ ----> (前面是linux用户名)   
_密码：XXX_ ----> (这是linux用户密码)   

选择POP3或IMAP

# 未完......
