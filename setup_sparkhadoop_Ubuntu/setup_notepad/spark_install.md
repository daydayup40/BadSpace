__在虚拟机环境中配置一个1+3的hadoop+spark+hbase的环境__

[A] 所有主机均需要执行   
[M] master主机需要进行的安装   
[S] slave主机需要进行的安装   

# 1. 安装一个模板机32bit ubuntu操作系统============================================
## 1.[M]
用户名设置为hd
主机名随意，后面再进行调整
所有口令建议统一，方便后面设置
ubuntu-14.04.1-desktop-amd64.iso

## 2.[M] 安装java环境
setupJDK.sh 和 jdk-8u77-linux-x64.tar.gz
```
sudo ./setupJDK.sh jdk1.8.0_77 jdk-8u77-linux-x64.tar.gz
```
注意，这里的命令设计到环境变量的关系，不要仅仅使用sudo进行安装，需要将环境切换到root用户下再执行该命令

## 3·[M] 安装ssh同时再卸载一下防火墙
```
sudo apt-get install ssh
sudo apt-get remove iptables
```

## 4.[M] 开放所有ssh的root登录权限
```
sudo gedit /etc/ssh/sshd_config
```

找到Authentication部分，将PermitRootLogin without-password进行注释，取而代之，添加这个
PermitRootLogin yes

然后重启ssh服务
```
sudo service ssh restart
```

## 5.[M] 重新设置root口令
```
sudo passwd
```

## 6.[M]
更新一下所有软件后重启机器（顺手可以打一个快照）

# 2. 在模板机上对hadoop进行安装和基本配置============================================
## 1.[M] 部署hadoop
将hadoop-2.7.1.tar.gz放置到用户 ~/ 目录下，然后解压，随后创建几个目录
```
tar zxvf hadoop-2.7.1.tar.gz
mkdir ./hadoop-2.7.1/tmp
mkdir ./hadoop-2.7.1/hdfs
mkdir ./hadoop-2.7.1/hdfs/datanode
mkdir ./hadoop-2.7.1/hdfs/namenode
```

## 2.[M] 修改环境变量,切换到root权限下
```
vim ~/.bashrc
```

```
//添加：
export HADOOP_HOME=/home/hd/hadoop-2.7.1
export HADOOP_COMMON_HOME=$HADOOP_HOME
export HADOOP_HDFS_HOME=$HADOOP_HOME
export HADOOP_MAPRED_HOME=$HADOOP_HOME
export HADOOP_YARN_HOME=$HADOOP_HOME
export HADOOP_CONF_DIR=$HADOOP_HOME/etc/hadoop
export PATH=$PATH:$HADOOP_HOME/bin:$HADOOP_HOME/sbin:$HADOOP_HOME/lib
export HADOOP_COMMON_LIB_NATIVE_DIR=$HADOOP_HOME/lib/native
export HADOOP_OPTS="-Djava.library.path=$HADOOP_HOME/lib"
export JAVA_LIBRARY_PATH=$HADOOP_HOME/lib/native
```
```
//修改执行
source ~/.bashrc
```
```
vim /etc/profile
```
```
//添加：
export HADOOP_HOME=/home/hd/hadoop-2.7.1
export HADOOP_COMMON_HOME=$HADOOP_HOME
export HADOOP_HDFS_HOME=$HADOOP_HOME
export HADOOP_MAPRED_HOME=$HADOOP_HOME
export HADOOP_YARN_HOME=$HADOOP_HOME
export HADOOP_CONF_DIR=$HADOOP_HOME/etc/hadoop
export PATH=$PATH:$HADOOP_HOME/bin:$HADOOP_HOME/sbin:$HADOOP_HOME/lib
export HADOOP_COMMON_LIB_NATIVE_DIR=$HADOOP_HOME/lib/native
export HADOOP_OPTS="-Djava.library.path=$HADOOP_HOME/lib"
export JAVA_LIBRARY_PATH=$HADOOP_HOME/lib/native
```
```
//修改执行
source /etc/profile
```

## 3.[M] 配置hadoop文件
```
vim ~/hadoop-2.7.1/etc/hadoop/core-site.xml
```
```
------------------------------------------------------------------
<property>
	<name>fs.defaultFS</name>
	<value>hdfs://master:9000</value>
</property>
<property>
        <name>hadoop.tmp.dir</name>
	<value>file:/home/hd/hadoop-2.7.1/tmp</value>			<!--这里注意路径-->
	<description>Abase for other temporary directories.</description>
</property>
<property>
	<name>io.file.buffer.size</name>
	<value>131702</value>
</property>
<property>
	<name>hadoop.proxyuser.hd.hosts</name>				<!--这里注意proxyuser.XXX.hosts中间是用户名-->
	<value>*</value>
</property>
<property>
	<name>hadoop.proxyuser.hd.groups</name>				<!--这里注意proxyuser.XXX.hosts中间是用户名-->
	<value>*</value>
</property>
------------------------------------------------------------------
```
```
vim ~/hadoop-2.7.1/etc/hadoop/hdfs-site.xml
```
```
------------------------------------------------------------------
<property>
	<name>dfs.namenode.name.dir</name>
	<value>file:/home/hd/hadoop-2.7.1/hdfs/namenode</value>
</property>
<property>
	<name>dfs.datanode.data.dir</name>
	<value>file:/home/hd/hadoop-2.7.1/hdfs/datanode</value>
</property>
<property>
	<name>dfs.replication</name>
	<value>2</value>
</property>
------------------------------------------------------------------
```
```
cp ~/hadoop-2.7.1/etc/hadoop/mapred-site.xml.template ~/hadoop-2.7.1/etc/hadoop/mapred-site.xml
vim ~/hadoop-2.7.1/etc/hadoop/mapred-site.xml
```
```
------------------------------------------------------------------
<property>
	<name>mapreduce.framework.name</name>
	<value>yarn</value>
</property>
<property>
        <name>mapreduce.jobtracker.address</name>
        <value>master:9001</value>
</property>
<property>
	<name>mapreduce.jobhistory.address</name>
	<value>master:10020</value>
</property>
<property>
	<name>mapreduce.jobhistory.webapp.address</name>
	<value>master:19888</value>
</property>
<!--
这是个打开uber模式的位置，不填佳默认为false
<property>
	<name>mapreduce.job.ubertask.enable</name>
	<value>true</value>
</property>
-->
------------------------------------------------------------------
```
```
gedit ~/hadoop-2.7.1/etc/hadoop/yarn-site.xml
```
```
------------------------------------------------------------------
<property>
	<name>yarn.nodemanager.aux-services</name>
	<value>mapreduce_shuffle</value>
</property>
<property>
	<name>yarn.nodemanager.aux-services.mapreduce.shuffle.class</name>
	<value>org.apache.hadoop.mapred.ShuffleHandler</value>
</property>
<property>
	<name>yarn.resourcemanager.address</name>
	<value>master:8032</value>
</property>
<property>
	<name>yarn.resourcemanager.scheduler.address</name>
	<value>master:8030</value>
</property>
<property>
	<name>yarn.resourcemanager.resource-tracker.address</name>
	<value>master:8031</value>
</property>
<property>
	<name>yarn.resourcemanager.admin.address</name>
	<value>master:8033</value>
</property>
<property>
	<name>yarn.resourcemanager.webapp.address</name>
	<value>master:8088</value>
</property>
------------------------------------------------------------------
```

## 4.[M]配置namenode,修改env环境变量文件
hadoop-env.sh、mapred-env.sh、yarn-env.sh这几个文件中的JAVA_HOME改为/usr/jdk/java
如：export JAVA_HOME=/usr/jdk/java
```
vim ~/hadoop-2.7.1/etc/hadoop/hadoop-env.sh
vim ~/hadoop-2.7.1/etc/hadoop/mapred-env.sh
vim ~/hadoop-2.7.1/etc/hadoop/yarn-env.sh
```

## 5.[M] 修改slave配置文件
```
vim ~/hadoop-2.7.1/etc/hadoop/slaves
```
添加
```
slave1
slave2
slave3
```

## 6.[M]
模板系统安装完毕,可以打快照了

# 3. 复制slave节点============================================
## 1.[M]
关闭当前虚拟机，拷贝出3份作为slave主机，主机名字区分
注意：拷贝出来的虚拟机一定不要从原有master主机的快照启动，无论如何要自己启动

## 2.[S] 变更主机名
```
sudo gedit /etc/hostname
```
节点分别命名为
```
slave1
slave2
slave3
```

## 3.[A] 主机IP地址变更
变更详情同下已部分信息

## 4.[A] 主机变更hosts文件（/etc/hosts）
文件中添加如下信息
```
192.168.59.100	master
192.168.59.101	slave1
192.168.59.102	slave2
192.168.59.103	slave3
```

## 5.[M]
完成之后可以ping一下各个节点试试
```
ping slaveX
```
# 4. 配置ssh免密码登录，注意：本部分所有操作在root权限下进行============================================
## 1.[M] 住节点配置免登录
```
//生成公私钥，一路点回车

ssh-keygen -t rsa -P ""


//把公钥加到用于认证的公钥文件中，这里的authorized_keys是用于认证的公钥文件
cat ~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys

//登录自己，在本机自行测试是否成功
ssh localhost

//退出
exit
```

## 2.[S] 子节点密钥生成
```
//生成公私钥，一路点回车
ssh-keygen -t rsa -P ""

//各个子节点将自己的公钥拷贝到master节点上，分别执行一下命令
scp ~/.ssh/id_rsa.pub master:/root/.ssh/id_rsa.pub.s1
scp ~/.ssh/id_rsa.pub master:/root/.ssh/id_rsa.pub.s2
scp ~/.ssh/id_rsa.pub master:/root/.ssh/id_rsa.pub.s3
```

## 3.[M]
```
//主节点中追加公钥
cat ~/.ssh/id_rsa.pub.s1 >> ~/.ssh/authorized_keys
cat ~/.ssh/id_rsa.pub.s2 >> ~/.ssh/authorized_keys
cat ~/.ssh/id_rsa.pub.s3 >> ~/.ssh/authorized_keys

//下发authorized_keys到各个子节点
scp ~/.ssh/authorized_keys slave1:/root/.ssh/
scp ~/.ssh/authorized_keys slave2:/root/.ssh/
scp ~/.ssh/authorized_keys slave3:/root/.ssh/
```
相互ssh一下试试

# 5. 启动执行============================================
## 1.[M] 先格式化namenode
```
sudo /home/hd/hadoop-2.7.1/bin/hadoop namenode -format
```
## 2.[M] 启动
```
sudo /home/hd/hadoop-2.7.1/sbin/start-dfs.sh
sudo /home/hd/hadoop-2.7.1/sbin/start-yarn.sh
（
关闭
sudo /home/hd/hadoop-2.7.1/sbin/stop-dfs.sh
sudo /home/hd/hadoop-2.7.1/sbin/stop-yarn.sh
）
```

## 3.[M] 进入web管理界面
http://192.168.59.100:8088

## 4.[A]
建议设置所有主机可以正常上网，并将自动更新时间功能打开，时间不一致不能够运行

## 5.
注意，如果反复执行先格式化，可能会导致，需要使ID号一致，然后重启dfs
master
/home/hd/hadoop-2.7.1/hdfs/datanode/current/VERSION

slaveX
/home/hd/hadoop-2.7.1/hdfs/datanode/current/VERSION

保证里面的clusterID字段一致

# 6. 执行一个任务测试一下============================================
```
//创建一个目录
hdfs dfs -mkdir /tmp

//拷贝本地一个文件值hadoop端
hdfs dfs -copyFromLocal /home/hd/hadoop-2.7.1/LICENSE.txt /tmp

//执行命令
hadoop jar /home/hd/hadoop-2.7.1/share/hadoop/mapreduce/hadoop-mapreduce-examples-2.7.1.jar wordcount /tmp/LICENSE.txt /tmp-output

```
得到结果包含如下信息
```
File Input Format Counters
		Bytes Read=15429
File Output Format Counters
		Bytes Written=8006
```

# 7. 配置普通用户的hadoop============================================
## 1.[A] 所有ssh相关文件向用户目录拷贝
```
sudo -f /root/.ssh/* /home/hd/.ssh/
sudo chown -c hd -R /home/hd/.ssh/
```

## 2.[A] 将hadoop目录整体降权
```
sudo chown -c hd -R /home/hd/hadoop-2.7.1
```
以后就可以在hd用户下正常使用hadoop了

# 8. master主机上配置eclipse============================================
## 1.首先安装好eclipse

## 2.将hadoop-eclipse-plugin-2.7.1.jar拷贝到
/usr/lib/eclipse/dropins/sdk/plugins目录下

## 3.配置
重启eclipse,打开Window->Show View->Other 选择Map/Reduce Tools
单击new hadoop location按钮在新的界面中进行配置:
### a.Location Name:
这个可以随意配置
### b.Map/Reduce Master:
这个位置依据mapred-site.xml文件的mapreduce.jobtracker.address项目内容
### c.DFS Master:
这个位置依据core-site.xml里fs.defaultFS项目内容
### d.username需要依据当前hadoop执行的用户填写，例如root

## 4.全部修改完毕之后，重启eclipse

## 5.这个时候如果配置正确，左侧的树状图里应该已经可以查看hadoop的目录了

## 6.对刚才已经配置的配置标签单击右键并选择编辑，进入刚才的配置界面后，选择Advanced Parameters标签继续进行配置
dfs.replication:    
	这个这里默认是3，但是要根据你的Datanode的个数来定，如果你仅有2个datanode，那么就写2，如果只有一个，就写1，大于或者等于3个，写3   
hadoop.tmp.dir:   
	这个要跟上节建立的Hadoop集群配置中core-site.xml 中的hadoop.tmp.dir设置一样，上节填写的是/tmp/hadoop-root，这里也如是填写   
	这里填写/home/hd/hadoop-2.7.1/tmp   

## 7.设置根目录
eclipse -> window -> preferences -> Map/Reduce  选择hadoop根目录
/home/hd/hadoop-2.7.1
重启eclipse


__*********************************从下面开始为spark1.5.0的安装部署过程***********************************__
参考文献:http://blog.csdn.net/yeruby/article/details/49805121

# 1. 安装Scala[M]============================================
建议使用2.10的版本，2.11貌似容易出现其他问题
## 1.将压缩包解压至/home/hd/ 目录
```
tar zxvf /home/hd/scala-2.10.4.tgz -C /home/hd/
```

## 2.创建一个链接
```
ln -s /home/hd/scala-2.10.4 /home/hd/scala
```

## 3.设置环境变量(by root)
```
vim /etc/profile
```
```
#追加如下内容
export SCALA_HOME=/usr/scala/scala
export PATH=$PATH:$SCALA_HOME/bin
```
## 4.使之生效：
```
source /etc/profile
```

## 5.测试Scala是否安装配置成功（不用root权限）：
```
scala -version
```

# 2. 安装Spark============================================
官网下载spark1.5.0：spark-1.5.0-bin-hadoop2.6.tgz
## 1.将压缩包解压至/home/hd/ 目录
```
tar zxvf /home/hd/spark-1.5.0-bin-hadoop2.6.tgz -C /home/hd/
```

## 2.创建一个链接
```
ln -s /home/hd/spark-1.5.0-bin-hadoop2.6 /home/hd/spark
```

## 3.设置环境变量(by root)
```
vim /etc/profile
```
```
#追加如下内容
export SPARK_HOME=/home/hd/spark
export PATH=$PATH:$SPARK_HOME/bin:$SPARK_HOME/sbin
```

## 4.使之生效：
```
source /etc/profile
```

## 5.测试spark是否安装配置成功：
```
spark-shell --version
//再计算一个PI值实验下一运行情况
run-example org.apache.spark.examples.SparkPi 10
```

## 6.修改spark-env.sh
```
cd /home/hd/spark/conf/
cp spark-env.sh.template spark-env.sh
gedit spark-env.sh
```
```
#添加内容如下
export SCALA_HOME=/home/hd/scala
export JAVA_HOME=/usr/jdk/java
export SPARK_MASTER_IP=192.168.59.100
export SPARK_WORKER_MEMORY=1024m
```

## 7.配置slaves
```
cd /home/hd/spark/conf/
cp slaves.template slaves
gedit slaves
```
```
#添加如下内容
slave1
slave2
slave3
```

## 8.在所有worker上安装并配置Spark
既然master上的这个文件件已经配置好了，把它拷贝到所有的worker。注意，三台机器spark所在目录必须一致，因为master会登陆到worker上执行命令，master认为worker的spark路径与自己一样。   
另外记得添加对应的link
```
scp -r spark-1.5.0-bin-hadoop2.6 slaveX:/home/hd/
ssh slaveX
ln -s /home/hd/spark-1.5.0-bin-hadoop2.6 /home/hd/spark
```

## 9.启动
启动主节点（Master）: start-master.sh
启动从节点（Slaves）: start-slaves.sh
直接完全启动:
spark/sbin/start-all.sh
web管理页面查看
http://192.168.59.100:8080/

10执行一个测试
启动scala命令：
spark/bin/spark-shell
http://192.168.59.100:4040/

```scala
sc.
textFile("file:////home/hd/hadoop-2.7.1/README.txt").
flatMap(_.split("\\s")).
map(_.toUpperCase).
map((_, 1)).
filter(_._1.forall((ch)=>ch>'A'&&ch<'Z')).
reduceByKey(_+_).
sortByKey().
foreach(println)
```

# 3. 利用intellij 编译scala程序，并部署在spark上运行============================================
以求Pi程序为例
## 1.建立scala程序
代码如下：
```scala
package com.hh
import scala.math.random
import org.apache.spark._
object Pi {
  def main(args: Array[String]) {
    val conf = new SparkConf().setAppName("MySparkPi")
  .setMaster("spark://192.168.59.100:7077")//.setJars(List("/home/hd/IdeaProjects/Test/out/artifacts/Test_jar/Test.jar"))
    val spark = new SparkContext(conf)
    val slices = if (args.length > 0) args(0).toInt else 2
    val n = math.min(100000L * slices, Int.MaxValue).toInt // avoid overflow
    val count = spark.parallelize(1 until n, slices).map { i =>
        val x = random * 2 - 1
        val y = random * 2 - 1
        if (x*x + y*y < 1) 1 else 0
      }.reduce(_ + _)
    println("============================ Pi is roughly " + 4.0 * count / n)
    spark.stop()
  }
}
```

需要注意：
### a.“package com.hh”需要记住，如果打包的时候没有将这个路径添全，那么在部署的时候--class参数需要指明这个路径
### b.在修改spark原有代码下val conf = new SparkConf().setAppName("XXXX")后需要添加.setMaster("spark://192.168.59.100:7077")来指定以下master,如果不添加，需要在提交任务时进行指明

## 2.编译器需要添加一个jar库，路径为spark安装目录下的/lib/spark-assembly-XXX-hadoopXXX.jar

## 3.打包时指定主类object路径：com.hh.Pi，如果不指定，启动时需要添加--class参数进行路径说明

## 4.执行:
```
spark-submit --class com.hh.Pi --master spark://192.168.59.100:7077 /home/hd/IdeaProjects/Test/out/artifacts/Test_jar/Test.jar

//如果是在打包时执行了包全路径，且制定了master地址，那么可以直接提交
spark-submit /home/hd/IdeaProjects/Test/out/artifacts/Test_jar/Test.jar
```
这个是wordcount的例子
```scala
object WordCount {
  def main(args: Array[String]) {
    if (args.length < 1) {
      System.err.println("Usage: <file>")
      System.exit(1)
      }
//    val filepath = "hdfs://192.168.59.100:9000/test2/in/test2"
    val conf = new SparkConf()
    val sc = new SparkContext(conf)
    val line = sc.textFile(args(0))
//    val line = sc.textFile(filepath)

    line.flatMap(_.split(" ")).map((_, 1)).reduceByKey(_+_).collect().foreach(println)

    sc.stop()
    }
}
```

# 4. 利用intellij 编译scala程序，在本地进行调试============================================
## 1.
去除.setMaster("spark://192.168.59.100:7077")的路径指定
## 2.
在运行配置中VM Options 设置为-Dspark.master=local

# 5. 安装部署zookeeper============================================
## 1.解压zookeeper-3.4.8.tar.gz

## 2.建立链接
ln -s /home/hd/zookeeper-3.4.8 /home/hd/zookeeper

## 3.修改zookeeper配置文件zoo.cfg
进入到conf目录，将zoo_sample.cfg拷贝一份命名为zoo.cfg（Zookeeper 在启动时会找这个文件作为默认配置文件），打开该文件进行修改为以下格式（注意权限问题，如果最后配置有问题请检查过程中权限是否正确）。
```
#修改
dataDir=/home/hd/zkdata/data
server.0=192.168.59.100:2888:3888
server.1=192.168.59.101:2888:3888
server.2=192.168.59.102:2888:3888
server.3=192.168.59.103:2888:3888
```
(备注：其中，2888端口号是zookeeper服务之间通信的端口，而3888是zookeeper与其他应用程序通信的端口。而zookeeper是在hosts中已映射了本机的ip。
initLimit：这个配置项是用来配置 Zookeeper 接受客户端（这里所说的客户端不是用户连接 Zookeeper服务器的客户端，而是 Zookeeper 服务器集群中连接到 Leader 的 Follower 服务器）初始化连接时最长能忍受多少个心跳时间间隔数。当已经超过 10 个心跳的时间（也就是 tickTime）长度后 Zookeeper 服务器还没有收到客户端的返回信息，那么表明这个客户端连接失败。总的时间长度就是 5*2000=10 秒。
syncLimit：这个配置项标识 Leader 与 Follower 之间发送消息，请求和应答时间长度，最长不能超过多少个 tickTime 的时间长度，总的时间长度就是 2*2000=4 秒。
server.A=B：C：D：其中 A 是一个数字，表示这个是第几号服务器；B 是这个服务器的 ip 地址；C 表示的是这个服务器与集群中的 Leader 服务器交换信息的端口；D 表示的是万一集群中的 Leader 服务器挂了，需要一个端口来重新进行选举，选出一个新的 Leader，而这个端口就是用来执行选举时服务器相互通信的端口。如果是伪集群的配置方式，由于 B 都是一样，所以不同的 Zookeeper 实例通信端口号不能一样，所以要给它们分配不同的端口号。)

## 4.建立目录
```
mkdir /home/hd/zkdata
mkdir /home/hd/zkdata/data
```

## 5.编辑myid文件
在/home/hd/zkdata/data目录下建立myid文件
注意：编辑“myid”文件，并在对应的IP的机器上输入对应的编号。
如在192.168.59.100上，“myid”文件内容就是0，
如在192.168.59.101上，内容就是1

## 6.在所有机器上进行部署，注意拷贝过去之后需要变更myid里的内容
scp -r zookeeper-3.4.8 slaveX:/home/hd/
scp -r zkdata slaveX:/home/hd/
ssh slaveX
ln -s /home/hd/zookeeper-3.4.8 /home/hd/zookeeper
//变更里面的id号
vi /home/hd/zkdata/data/myid

## 7.
### 启动zookeeper
Zookeeper的启动与hadoop不一样，需要每个节点都执行，分别进入每个节点的zookeeper目录，启动zookeeper：
```
/home/hd/zookeeper/bin/zkServer.sh start
```

### 关闭脚本
```
/home/hd/zookeeper/bin/zkServer.sh stop
```

### 查看状态
```
/home/hd/zookeeper/bin/zkServer.sh status
```
ps:这是启动客户端的脚本
```
/home/hd/zookeeper/bin/zkCli.sh -server zookeeper:2181
```

# 5. 安装部署hbase============================================
## 1.解压后设置链接
```
ln -s /home/hd/hbase-1.2.1/ /home/hd/hbase
```

另外修改/etc/profile文本
添加
```
export HBASE_HOME=/home/hd/hbase
export PATH=$PATH:$HBASE_HOME/bin
```
```
\\并执行
source /etc/profile
```

## 2.修改hbase-env.sh文件：
```
#添加如下
export JAVA_HOME=/usr/jdk/java/
export HBASE_CLASSPATH=${HADOOP_HOME}/conf
export HBASE_MANAGES_ZK=false
export HBASE_PID_DIR=/home/hd/
```
注意：需要注意的地方是 ZooKeeper的配置。这与 hbase-env.sh 文件相关，文件中 HBASE_MANAGES_ZK 环境变量用来设置是使用hbase默认自带的 Zookeeper还是使用独立的ZooKeeper。HBASE_MANAGES_ZK=false 时使用独立的，为true时使用默认自带的。

## 3.修改hbase-site.xml文件
```
#插入如下：
        <property>
           <name>hbase.rootdir</name>
           <value>hdfs://master:9000/hbase</value>
        </property>
        <property>
            <name>hbase.cluster.distributed</name>
            <value>true</value>
        </property>
        <property>
             <name>hbase.zookeeper.quorum</name>
             <value>master,slave1,slave2,slave3</value>
        </property>
        <property>
             <name>zookeeper.session.timeout</name>
             <value>60000</value>
        </property>
       <property>
          <name>hbase.zookeeper.property.clientPort</name>
          <value>2181</value>
       </property>
```

## 4.修改regionservers文件
```
#修改
master
slave1
slave2
slave3
```

## 5.在所有机器上部署
```
scp -r hbase-1.2.1 slaveX:/home/hd/
ssh slaveX
ln -s /home/hd/hbase-1.2.1 /home/hd/hbase
```

## 6.启动一下看看
/home/hd/hbase/bin/start-hbase.sh
通过http://192.168.59.100:16010/查看一下结果

## 7.关闭
/home/hd/hbase/bin/stop-hbase.sh

## 8.交互式管理
通过启动hbase shell命令
