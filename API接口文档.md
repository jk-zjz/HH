<div align='center' ><font size='60'>API接口文档</font></div>

## LOG日志接口

| LOG API                              | 说明           | 备注                                             | 有无重载 | 是否为虚   | 类是否为父类 | 父类                                   |
|--------------------------------------|--------------|------------------------------------------------|------|--------|--------|--------------------------------------|
| hh::Logger                           | 日志器类         |                                                |      |        | 否      | std::enable_shared_from_this<Logger> |
| hh::Logger::Log                      | 日志输出方法       | 输出                                             |      |        |
| hh::Logger::addAppender              | 日志输出添加方法     | 设置                                             |      |        |
| hh::logger::delectAppender           | 日志输出删除方法     | 设置                                             |      |        |
| hh::logger::clearAppender            | 日志输出清空方法     | 设置                                             |      |        |
| hh::logger::setLevel                 | 日志输出等级设置方法   | 设置                                             |      |        |
| hh::logger::setFileName              | 日志输出文件设置方法   | 设置                                             | 有    |        |
| hh::logger::getLevel                 | 日志输出等级获取方法   | 获取                                             |      |        |
| hh::logger::getName                  | 日志输出名称获取方法   | 获取                                             |      |        |
| hh::logger::get_Fromatter            | 日志输出格式获取方法   | 获取                                             |      |        |
| hh::LogAppender                      | 日志输出器类       |                                                |      |        | 是      |
| hh::LogAppender::log                 | 日志输出方法       | 输出                                             |      | 是      |
| hh::LogAppender::setFormatter        | 日志输出格式设置方法   | 设置                                             |
| hh::LogAppender::set_level           | 日志输出等级设置方法   | 设置                                             |
| hh::LogAppender::setFatherFormatter  | 设置是否为自定义格式   | 设置                                             |
| hh::LogAppender::setFatherFormatter  | 设置是否为自定义格式   | 设置                                             |
| hh::LogAppender::get_level           | 日志输出等级获取方法   | 获取                                             |
| hh::LogAppender::getFormatter        | 日志输出格式获取方法   | 获取                                             |
| hh::LogAppender::getFatherFormatter  | 日志输出格式获取方法   | 获取                                             |
| hh::LogFormatter                     | 日志输出格式类      |                                                |      |        | 否      |
| hh::LogFormatter::LogFormatter       | 通过格式字符串设置格式器 | 构造                                             |
| hh::LogFormatter::format             | 格式化日志输出方法    | 输出                                             |
| hh::LogFormatter::is_Error           | 判断是否为错误日志等级  | 判断                                             |
| hh::LogEventWrap                     | 日志事件管理类      |                                                |      |        | 否      |
| hh::LogEventWrap::LogEventWrap       | 日志事件管理类构造方法  | 构造                                             |      |        |        |
| hh::LogEventWrap::~LogEventWrap      | 日志析构输出方法     | 输出                                             |      |        |        |
| hh::LogEventWrap::setSS              | 设置日志输出流内容    | 设置                                             |      |        |        |
| hh::LogEventWrap::getSS              | 获取日志输出流内容    | 获取                                             |      |        |        |
| hh::LogEventWrap::getEvent           | 获取日志事件       | 获取                                             |      |        |        |
| hh::LogEvent                         | 日志事件类        |                                                |      |        | 否      |
| hh::LogEvent::setSS                  | 设置日志输出流内容    | 设置                                             |      |        |        |
| hh::LogEvent::format                 | 可变参数格式化方法内容  | 设置                                             | 有    |        |        |
| hh::LogEvent::setFormatter           | 设置日志格式器      | 设置                                             | 有    |        |        |
| hh::LogEvent::get_Formatter          | 获取日志格式器      | 获取                                             |      |        |        |
| hh::LogEvent::getFile                | 获取日志文件名      | 获取                                             |      |        |        |
| hh::LogEvent::Line                   | 获取日志行号       | 获取                                             |      |        |        |
| hh::LogEvent::getElapsed             | 获取日志时间       | 获取                                             |      |        |        |
| hh::LogEvent::getThreadId            | 获取日志线程号      | 获取                                             |      |        |        |
| hh::LogEvent::getFiberId             | 获取日志协程号      | 获取                                             |      |        |        |
| hh::LogEvent::getContent             | 获取日志内容       | 获取                                             |      |        |        |
| hh::LogEvent::getTime                | 获取日志时间       | 获取                                             |      |        |        |
| hh::LogEvent::getSS                  | 获取日志输出流内容    | 获取                                             |      |        |        |
| hh::LogEvent::getLevel               | 获取日志等级       | 获取                                             |      |        |        |
| hh::LogEvent::getLohger              | 获取日志器        | 获取                                             |      |        |        |
| hh::LogLevel                         | 日志等级类        |                                                |      |        |        |                                      |
| hh::LogLevel::ToString               | 日志等级转字符串     | 静态方法                                           |      |        |        |                                      |
| hh::LogLevel::FromString             | 字符串转日志等级     | 静态方法                                           |      |        |        |                                      |
| hh::FileLogAppender                  | 日志输出器类       |                                                |      |        |        | LogAppender                          |
| hh::FileLogAppender::FileLogAppender | 日志输出器类构造方法   | 构造                                             |      |        |        |                                      |
| hh::FileLogAppender::log             | 日志输出方法       | 输出                                             |      | 虚方法的实现 |        |                                      |
| hh::FileLogAppender::reopen          | 日志文件重开       | 设置                                             |      |        |        |                                      |
| hh::StdoutLogAppender                | 日志输出器类       |                                                |      |        |        | LogAppender                          |
| hh::StdoutLogAppender::log           | 日志输出方法       | 输出                                             |      | 虚方法的实现 |        |                                      |
| hh::LoggerManager                    | 日志管理器类       | 基本为静态类返回                                       |      |        |        |                                      |
| hh::LoggerManager::LoggerMgr         | 关键静态类        | typedef hh::Singleton<LoggerManager> LoggerMgr |      |        |        |                                      |
| hh::LoggerManager::getLogger         | 获取日志器        | 获取                                             |      |        |        |                                      |
| hh::LoggerManager::GetRoot           | 获取根日志器       | 获取                                             |      |        |        |                                      |
| hh::LoggerManager::getLoggers        | 获取日志器集合      | 获取                                             |      |        |        |                                      |
| hh::LoggerManager::setLogger         | 设置日志器        | 设置                                             |      |        |        |                                      |

## config 配置API

| CONFIG API                        | 说明          | 备注 | 有无重载 | 是否为虚 | 类是否为父类 | 父类            |
|-----------------------------------|-------------|----|------|------|--------|---------------|
| hh::ConfigVarBase                 | 配置变量基类      |    |      |      |        |               |
| hh::ConfigVarBase::getName        | 获取配置变量名     | 获取 |      |      |        |               |
| hh::ConfigVarBase::getDescription | 获取配置变量描述    | 获取 |      |      |        |               |
| hh::ConfigVarBase::toString       | 配置变量转字符串    | 获取 |      | 是    |        |               |
| hh::ConfigVarBase::gettype        | 获取配置变量类型    | 获取 |      | 是    |        |               |
| hh::ConfigVarBase::fromString     | 字符串转配置变量    | 设置 |      | 是    |        |               |
| hh::ConfigVar<T>                  | 配置变量类       |    |      |      |        | ConfigVarBase |
| hh::ConfigVar::toString           | 配置变量转字符串    | 获取 |      |      |        |               |
| hh::ConfigVar::fromString         | 字符串转配置变量    | 设置 |      |      |        |               |
| hh::ConfigVar::getValue           | 获取配置变量值     | 获取 |      |      |        |               |
| hh::ConfigVar::setValue           | 设置配置变量值     | 设置 |      |      |        |               |
| hh::ConfigVar::gettype            | 获取配置变量类型    | 获取 |      |      |        |               |
| hh::ConfigVar::getOcb             | 获取配置变量回调函数  | 获取 |      |      |        |               |
| hh::ConfigVar::addOcb             | 添加配置变量回调函数  | 设置 |      |      |        |               |
| hh::ConfigVar::deleteOcb          | 删除配置变量回调函数  | 设置 |      |      |        |               |
| hh::ConfigVar::eraseOcb           | 删除配置变量回调函数  | 设置 |      |      |        |               |
| hh::Config                        | 配置文件类       |    |      |      |        |               |
| hh::Config::Lookup                | 查找配置变量      | 获取 | 有    |      |        |               |
| hh::Config::loadFromYaml          | 从YAML文件加载配置 | 设置 |      |      |        |               |
| hh::Config::LookipBase            | 查找配置变量基类    | 获取 |      |      |        |               |
| hh::Config::visit                 | 遍历配置变量      | 获取 |      |      |        |               |

## thread 线程池API

| THREAD API          | 说明      | 备注 | 有无重载 | 是否为虚 | 类是否为父类 | 父类 |
|---------------------|---------|----|------|------|--------|----|
| hh::Thread          | 线程类     |    |      |      |        |    |
| hh::Thread::Thread  | 线程类构造方法 | 构造 |      |      |        |    |
| hh::Thread::join    | 线程等待    | 设置 |      |      |        |    |
| hh::Thread::run     | 线程启动    | 设置 |      |      |        |    |
| hh::Thread::get_id  | 获取线程ID  | 获取 |      |      |        |    |
| hh::Thread::GetName | 获取线程名   | 获取 |      |      |        |    |
| hh::Thread::SetName | 设置线程名   | 设置 |      |      |        |    |
| hh::Thread::GetThis | 获取当前线程  | 获取 |      |      |        |    |
| hh::MUtex           | 互斥锁类    |    |      |      |        |    |
| hh::RWMutex         | 读写锁类    |    |      |      |        |    |
| hh::NullMutex       | 空互斥锁类   |    |      |      |        |    |
| hh::NullRWMutex     | 空读写锁类   |    |      |      |        |    |
| hh::CASlock         | CAS锁类   |    |      |      |        |    |
| hh::SpinLock        | 旋锁类     |    |      |      |        |    |

## fiber 协程API

| FIBER API                 | 说明          | 备注 | 有无重载 | 是否为虚 | 类是否为父类 | 父类 |
|---------------------------|-------------|----|------|------|--------|----|
| hh::Fiber                 | 协程类         |    |      |      |        |    |
| hh::Fiber::Fiber          | 协程类构造方法     | 构造 |      |      |        |    |
| hh::Fiber::reset          | 重置协程        | 设置 |      |      |        |    |
| hh::Fiber::swapIn         | 协程切换到当前协程   | 设置 |      |      |        |    |
| hh::Fiber::swapOut        | 当前协程切换到其他协程 | 设置 |      |      |        |    |
| hh::Fiber::getState       | 获取协程状态      | 获取 |      |      |        |
| hh::Fiber::getThis        | 获取当前协程      | 获取 |      |      |        |    |
| hh::Fiber::SetThis        | 设置当前协程      | 设置 |      |      |        |    |
| hh::Fiber::YieldToHold    | 协程切换到其他协程   | 设置 |      |      |        |    |
| hh::Fiber::YieldToReady   | 协程切换到其他协程   | 设置 |      |      |        |    |
| hh::Fiber::GetTotalFibers | 获取当前协程数量    | 获取 |      |      |        |    |
| hh::Fiber::Mainfunc       | 协程主函数       | 获取 |      |      |        |    |
| hh::Fiber::getFiber_id    | 获取协程ID      | 获取 |      |      |        |    |

## scheduler 协程调度器API

| SCHEDULER API                | 说明           | 备注 | 有无重载 | 是否为虚 | 类是否为父类 | 父类 |
|------------------------------|--------------|----|------|------|--------|----|
| hh::Scheduler                | 协程调度器类       |    |      |      | 是      |    |
| hh::Scheduler::Scheduler     | 协程调度器类构造方法   | 构造 |      |      |        |    |
| hh::Scheduler::start         | 协程调度器启动      | 设置 |      |      |        |    |
| hh::Scheduler::stop          | 协程调度器停止      | 设置 |      |      |        |    |
| hh::Scheduler::schedule      | 添加任务         | 设置 | 有    |      |        |    |
| hh::Scheduler::GetThis       | 获取当前调度器      | 获取 |      |      |        |    |
| hh::Scheduler::GetMainFiber  | 获取主协程        | 获取 |      |      |        |    |
| hh::Scheduler::getName       | 获取调度器名       | 获取 |      |      |        |    |
| hh::Scheduler::switchTo      | 切换协程         | 设置 |      |      |        |    |
| hh::Scheduler::dump          | 协程调度器状态      | 获取 |      |      |        |    |\
| hh::Scheduler::tickle        | 协程调度器调度      | 设置 |      |      |        |    |
| hh::Scheduler::stopping      | 协程调度器停止      | 设置 |      |      |        |    |
| hh::Scheduler::setThis       | 设置当前调度器      | 设置 |      |      |        |    |
| hh::Scheduler::run           | 协程调度器运行      | 设置 |      |      |        |    |
| hh::Scheduler::idle          | 协程调度器空闲      | 设置 |      |      |        |    |
| hh::Scheduler::hasIdleThread | 协程调度器是否有空闲线程 | 设置 |      |      |        |    |

## IOManager IO协程调度器API

| IOManager API                         | 说明           | 备注 | 有无重载 | 是否为虚 | 类是否为父类 | 父类                     |
|---------------------------------------|--------------|----|------|------|--------|------------------------|
| hh::IOManager                         | IO协程调度器类     |    |      |      |        | Scheduler,TimerManager |
| hh::IOManager::IOManager              | IO协程调度器类构造方法 | 构造 |      |      |        |                        |
| hh::IOManager::addEvent               | 添加事件         | 设置 |      |      |        |                        |
| hh::IOManager::cancelEvent            | 取消事件         | 设置 |      |      |        |                        |
| hh::IOManager::delEvent               | 删除事件         | 设置 |      |      |        |                        |
| hh::IOManager::onTimerInsertedAtFront | 添加事件回调       | 设置 |      | 是    |        |                        |
| hh::IOManager::stopping               | 停止调度器        | 设置 | 是    | 是    |        |                        |
| hh::IOManager::tickle                 | 唤醒调度器        | 设置 |      | 是    |        |                        |
| hh::IOManager::idle                   | 空闲调度器        | 设置 |      | 是    |        |                        |
| hh::IOManager::cancelAll              | 取消所有事件       | 设置 |      | 是    |        |                        |

## TimerManager 定时器调度器API

| TimerManager API                         | 说明          | 备注 | 有无重载 | 是否为虚 | 类是否为父类 | 父类                                   |
|------------------------------------------|-------------|----|------|------|--------|--------------------------------------|
| hh::TimerManager                         | 定时器调度器类     |    |      |      | 是      |                                      |
| hh::TimerManager::TimerManager           | 定时器调度器类构造方法 | 构造 |      |      |        |                                      |
| hh::TimerManager::addTimer               | 添加定时器       | 设置 |      |      |        |                                      |
| hh::TimerManager::addConditionTimer      | 添加条件定时器     | 设置 |      |      |        |                                      |
| hh::TimerManager::getNextTimer           | 获取下一个定时器    | 获取 |      |      |        |                                      |
| hh::TimerManager::listExpiredCb          | 获取过期定时器回调   | 获取 |      |      |        |                                      |
| hh::TimerManager::hasTimer               | 判断是否有定时器    | 获取 |      |      |        |                                      |
| hh::TimerManager::onTimerInsertedAtFront | 添加定时器回调     | 设置 |      | 是    |        |                                      |
| hh::TimerManager::addTimer               | 添加定时器       | 设置 |      | 是    |        |                                      |
| hh::TimerManager::detectClockRollover    | 检测时钟回滚      | 设置 |      | 是    |        |                                      |
| hh::TimerManager::cancel                 | 取消定时器       | 设置 |      | 是    |        |                                      |
| hh::TimerManager::reset                  | 重置定时器       | 设置 |      | 是    |        |                                      |
| hh::TimerManager::refresh                | 刷新定时器       | 设置 |      | 是    |        |                                      |
| hh::Timer                                | 定时器类        |    |      |      |        | TimerManager，enable_shared_from_this |
| hh::Timer::Timer                         | 定时器类构造方法    | 构造 | 是    |      |        |                                      |
| hh::Timer::cancel                        | 取消定时器       | 设置 |      | 是    |        |                                      |
| hh::Timer::reset                         | 重置定时器       | 设置 |      | 是    |        |                                      |
| hh::Timer::refresh                       | 刷新定时器       | 设置 |      | 是    |        |                                      |

## timer定时器API

| timer 定时器API                             | 说明          | 备注 | 有无重载 | 是否为虚 | 类是否为父类 | 父类                                  |
|------------------------------------------|-------------|----|------|------|--------|-------------------------------------|
| hh::Timer                                | 定时器类        |    |      |      | 是      | std::enable_shared_from_this<Timer> |
| hh::Timer::Timer                         | 定时器类构造方法    | 构造 |      |      |        |                                     |
| hh::Timer::cancel                        | 取消定时器       | 设置 |      | 是    |        |                                     |
| hh::Timer::reset                         | 重置定时器       | 设置 |      | 是    |        |                                     |
| hh::Timer::refresh                       | 刷新定时器       | 设置 |      | 是    |        |                                     |
| hh::TimerManager                         | 定时器调度器类     |    |      |      |        |                                     |
| hh::TimerManager::TimerManager           | 定时器调度器类构造方法 | 构造 |      |      |        |                                     |
| hh::TimerManager::addTimer               | 添加定时器       | 设置 |      |      |        |                                     |
| hh::TimerManager::addConditionTimer      | 添加条件定时器     | 设置 |      |      |        |                                     |
| hh::TimerManager::getNextTimer           | 获取下一个定时器    | 获取 |      |      |        |                                     |
| hh::TimerManager::listExpiredCb          | 获取过期定时器回调   | 获取 |      |      |        |                                     |
| hh::TimerManager::hasTimer               | 判断是否有定时器    | 获取 |      |      |        |                                     |
| hh::TimerManager::onTimerInsertedAtFront | 添加定时器回调     | 设置 |      | 是    |        |                                     |
| hh::TimerManager::detectClockRollover    | 检测时钟回滚      | 设置 |      | 是    |        |                                     |

## hook钩子API

| hook 钩子API | 说明                 | 备注 | 有无重载 | 是否为虚 | 类是否为父类 | 父类           |
|------------|--------------------|----|------|------|--------|--------------|
| sleep      | 定时器                | 设置 | 是    |      |        | sleep_f      |
| usleep     | 定时器                | 设置 | 是    |      |        | usleep_f     |
| nanosleep  | 定时器                | 设置 | 是    |      |        | nanosleep_f  |
| socket     | 创建socket           | 设置 | 是    |      |        | socket_f     |
| accept     | 提取socket           | 设置 | 是    |      |        | accept_f     |
| connect    | 连接服务器              | 设置 | 是    |      |        | connect_f    |
| read       | 读取                 | 设置 | 是    |      |        | read_f       |
| readv      | 分缓冲区读取             | 设置 | 是    |      |        | readv_f      |
| recv       | 可以使用宏来读取           | 设置 | 是    |      |        | recv_f       |
| recvfrom   | 可以存储发送方的地址src_addr | 设置 | 是    |      |        | recvfrom_f   |
| recvmsg    | 描述了消息的缓冲区和辅助数据     | 设置 | 是    |      |        | recvmsg_f    |
| write      | 写                  | 设置 | 是    |      |        | write_f      |
| writev     | 分缓冲区写              | 设置 | 是    |      |        | writev_f     |
| send       | 可以使用宏来写            | 设置 | 是    |      |        | send_f       |
| sendto     | 可以存储发送方的地址src_addr | 设置 | 是    |      |        | sendto_f     |
| sendmsg    | 描述了发送的的缓冲区和辅助数据    | 设置 | 是    |      |        | sendmsg_f    |
| close      | 关闭                 | 设置 | 是    |      |        | close_       
| fcntl      | 设置文件描述符            | 设置 | 是    |      |        | fcntl_f      |
| ioctl      | 设备输入输出操作的系统调用      | 设置 | 是    |      |        | ioctl        
| getsockopt | 获取句柄信息             | 获取 | 是    |      |        | getsockopt_f |
| setsockopt | 设置句柄信息             | 设置 | 是    |      |        | setsockopt_f |

## Address 地址API

| Address 地址API                      | 说明                     | 备注 | 有无重载 | 是否为虚 | 类是否为父类 | 父类        |
|------------------------------------|------------------------|----|------|------|--------|-----------|   
| hh::Address                        | 地址类                    |    |      |      | 是      |           |
| hh::Address::lookup                | 通用查询满版本(可以查询ip中的地址)    | 构造 |      |      |        |           |
| hh::Address::lookupAny             | 查询地址，不管是什么地址 ip或者为unix | 获取 |      |      |        |           |
| hh::Address::lookupAnyIPAddress    | 查询IP地址                 | 获取 |      |      |        |           |
| hh::Address::GetInterfaceAddresses | 获取网卡地址                 | 获取 | 是    |      |        |           |
| hh::Address::create                | 创建地址                   | 构造 | 是    |      |        |           |
| hh::Address::getFamily             | 获取地址类型                 | 获取 |      |      |        |           |
| hh::Address::getAddr               | 获取地址                   | 获取 |      | 是    |        |           |
| hh::Address::getAddrLen            | 获取地址长度                 | 获取 |      | 是    |        |           |
| hh::Address::insert                | 为流缓冲区插入地址              | 设置 |      | 是    |        |           |
| hh::Address:ToString               | 地址转换为字符串               | 获取 |      | 是    |        |
| hh::IPAddress                      | IP地址类                  |    |      |      | 是      | Address   |
| hh::IPAddress::Create              | 创建IP地址                 | 构造 |      |      |        |           |
| hh::IPAddress::broadcastAddress    | 获取广播地址                 | 获取 |      | 是    |        |           |
| hh::IPAddress::getPort             | 获取端口                   | 获取 |      | 是    |        |           |
| hh::IPAddress::setPort             | 设置端口                   | 设置 |      | 是    |        |           |
| hh::IPAddress::subnetMask          | 获取子网掩码                 | 获取 |      | 是    |        |           |
| hh::IPAddress::networkAddress      | 获取网络地址                 | 获取 |      | 是    |
| hh::IPv4Address                    | IPv4地址类                |    |      |      |        | IPAddress |
| hh::IPv4Address::Create            | 创建IPv4地址               | 构造 |      |      |        |           |
| hh::IPv4Address::getAddr           | 获取地址                   | 获取 |      |      |        |           |
| hh::IPv4Address::getAddrLen        | 获取地址长度                 | 获取 |      |      |        |           |
| hh::IPv4Address::getPort           | 获取端口                   | 获取 |      |      |        |           |
| hh::IPv4Address::setPort           | 设置端口                   | 设置 |      |      |        |           |
| hh::IPv4Address::subnetMask        | 获取子网掩码                 | 获取 |      |      |        |           |
| hh::IPv4Address::networkAddress    | 获取网络地址                 | 获取 |      |      |        |           |
| hh::IPv4Address::insert            | 为流缓冲区插入地址              | 设置 |      |      |        |           |
| hh::IPv6Address                    | IPv6地址类                |    |      |      |        | IPAddress |
| hh::IPv6Address::Create            | 创建IPv6地址               | 构造 |      |      |        |           |
| hh::IPv6Address::getAddr           | 获取地址                   | 获取 |      |      |        |           |
| hh::IPv6Address::getAddrLen        | 获取地址长度                 | 获取 |      |      |        |           |
| hh::IPv6Address::getPort           | 获取端口                   | 获取 |      |      |        |           |
| hh::IPv6Address::setPort           | 设置端口                   | 设置 |      |      |        |           |
| hh::IPv6Address::subnetMask        | 获取子网掩码                 | 获取 |      |      |        |           |
| hh::IPv6Address::networkAddress    | 获取网络地址                 | 获取 |      |      |        |           |
| hh::IPv6Address::insert            | 为流缓冲区插入地址              | 设置 |      |      |
| hh::IPv6Address::broadcastAddress  | 获取广播地址                 | 获取 |      |      |        |           |
| hh::UnixAddress                    | Unix地址类                |    |      |      |        | Address   |
| hh::UnixAddress::getAddr           | 获取地址                   | 获取 |      |      |        |           |
| hh::UnixAddress::getAddrLen        | 获取地址长度                 |
| hh::UnixAddress::insert            | 为流缓冲区插入地址              | 设置 |      |      |        |           |
| hh::UnixAddress::setAddrLen        | 设置地址长度                 | 设置 |      |      |        |           |
| hh::UnknownAddress                 | 未知地址类                  |    |      |      |        | Address   |
| hh::UnknownAddress::getAddr        | 获取地址                   | 获取 |      |      |        |           |
| hh::UnknownAddress::getAddrLen     | 获取地址长度                 |
| hh::UnknownAddress::insert         | 为流缓冲区插入地址              | 设置 |      |      |        |           |

## socket API

| socket API                      | 说明            | 备注 | 有无重载 | 是否为虚 | 类是否为父类 | 父类                                                |
|---------------------------------|---------------|----|------|------|--------|---------------------------------------------------|   
| hh::Socket                      | 套接字类          |    |      |      |        | Noncopyable, std::enable_shared_from_this<Socket> |
| hh::Socket::CreateTCP           | 创建TCP套接字      | 构造 |      |      |        |                                                   |
| hh::Socket::CreateUDP           | 创建UDP套接字      | 构造 |      |      |        |                                                   |
| hh::Socket::CreateTCPSocket     | 创建TCP套接字      | 构造 |      |      |        |                                                   |
| hh::Socket::CreateUDPSocket     | 创建UDP套接字      | 构造 |      |      |        |                                                   |
| hh::Socket::CreateTCPSocket6    | 创建TCP套接字      | 构造 |      |      |        |                                                   |
| hh::Socket::CreateUDPSocket6    | 创建UDP套接字      | 构造 |      |      |        |                                                   |
| hh::Socket::CreateUnixTCPSocket | 创建Unix域TCP套接字 | 构造 |      |      |        |                                                   |
| hh::Socket::CreateUnixUDPSocket | 创建Unix域UDP套接字 | 构造 |      |      |        |                                                   |
| hh::Socket::getSendTimeout      | 获取发送超时时间      | 获取 |      |      |        |                                                   |
| hh::Socket::getRecvTimeout      | 获取接收超时时间      | 获取 |      |      |        |
| hh::Socket::setSendTimeout      | 设置发送超时时间      | 获取 |      |      |        |                                                   |
| hh::Socket:setRecvTimeout       | 设置接收超时时间      | 获取 |      |      |        |                                                   |
| hh::Socket::getOption           | 获取套接字选项       | 获取 |      |      |        |                                                   |
| hh::Socket::setOption           | 设置套接字选项       | 设置 |      |      |        |                                                   |
| hh::Socket::accept              | 接收连接          | 获取 |      |      |        |                                                   |
| hh::Socket::connect             | 连接            | 设置 |      |      |        |                                                   |
| hh::Socket::bind                | 绑定            | 设置 |      |      |        |                                                   |
| hh::Socket::listen              | 监听            | 设置 |      |      |        |                                                   |
| hh::Socket::close               | 关闭            | 设置 |      |      |        |                                                   |
| hh::Socket::init                | 初始化           | 设置 |      |      |        |                                                   |
| hh::Socket::send                | 发送数据          | 设置 | 是    |      |        |                                                   |
| hh::Socket::sendTo              | 发送数据          | 设置 | 是    |      |        |                                                   |
| hh::Socket::recv                | 接收数据          | 设置 | 是    |      |        |                                                   |
| hh::Socket::recvFrom            | 接收数据          | 设置 | 是    |      |        |                                                   |
| hh::Socket::getLocalAddress     | 获取本地地址        | 获取 |      |      |        |                                                   |
| hh::Socket::getPeerAddress      | 获取对端地址        | 获取 |      |      |        |
| hh::Socket::isConnected         | 是否连接          | 获取 |      |      |        |                                                   |
| hh::Socket::getFamily           | 获取地址族         | 获取 |      |      |        |                                                   |
| hh::Socket::getType             | 获取套接字类型       | 获取 |      |      |        |                                                   |
| hh::Socket::getProtocol         | 获取协议          | 获取 |      |      |        |                                                   |
| hh::Socket::getSocket           | 获取套接字         | 获取 |      |      |        |                                                   |
| hh::Socket::getError            | 获取套接字错误       | 获取 |      |      |        |                                                   |
| hh::Socket::isValid             | 是否有效          | 获取 |      |      |        |                                                   |
| hh::Socket::dump                | 输出套接字信息       | 获取 |      |      |        |                                                   |
| hh::cancelRead                  | 取消读事件         | 设置 |      |      |        |                                                   |
| hh::cancelWrite                 | 取消写事件         | 设置 |      |      |        |                                                   |
| hh::Socket::cancelAccept        | 取消接收连接事件      | 设置 |      |      |        |                                                   |
| hh::Socket::cancelAll           | 取消所有事件        | 设置 |      |      |

## ByteArray API

| ByteArray API                       | 说明           | 备注     | 有无重载 | 是否为虚 | 类是否为父类 | 父类 |
|-------------------------------------|--------------|--------|------|------|--------|----| 
| ByteArray::ByteArray                | 构造函数         |        |      |      |        |    |
| ByteArray::~ByteArray               | 析构函数         |        |      |      |        |    |
| ByteArray::writeFuint8              | 写入8位有符号整数    |        |      |      |        |    |
| ByteArray::writeFuint16             | 写入16位无符号整数   |        |      |      |        |    |
| ByteArray::writeFuint32             | 写入32位无符号整数   |        |      |      |        |    |
| ByteArray::writeFuint64             | 写入64位无符号整数   |        |      |      |        |    |
| ByteArray::writeFint8               | 写入8位有符号整数    |        |      |      |        |    |
| ByteArray::writeFint16              | 写入16位有符号整数   |        |      |      |        |    |
| ByteArray::writeFint32              | 写入32位有符号整数   |        |      |      |        |    |
| ByteArray::writeFint64              | 写入64位有符号整数   |        |      |      |        |    |
| ByteArray::writeUint32              | 压缩写入32位无符号整数 |        |      |      |        |    |
| ByteArray::writeUint64              | 压缩写入64位无符号整数 |        |      |      |        |    |
| ByteArray::writeInt32               | 压缩写入32位有符号整数 |        |      |      |        |    |
| ByteArray::writeInt64               | 压缩写入64位有符号整数 |        |      |      |        |    |
| ByteArray::writeDouble              | 写入64位浮点数     |        |      |      |        |    |
| ByteArray::writeFloat               | 写入32位浮点数     |        |      |      |        |    |
| ByteArray::writeString16            | 写入16位字符串     |        |      |      |        |    |
| ByteArray::writeString32            | 写入32位字符串     |        |      |      |        |    |
| ByteArray::writeString64            | 写入64位字符串     |        |      |      |        |    |
| ByteArray::writeStringVint          | 写入字符串        | 明确长度   |      |      |        |    |
| ByteArray::writeStringWithoutLength | 写入字符串        | 不明确长度  |      |      |        |    |
| ByteArray::readFuint8               | 读取8位无符号整数    |        |      |      |        |    |
| ByteArray::readFint8                | 读取8位有符号整数    |        |      |      |        |    |
| ByteArray::readFuint16              | 读取16位无符号整数   |        |      |      |        |    |
| ByteArray::readFint16               | 读取16位有符号整数   |        |      |      |        |    |
| ByteArray::readFuint32              | 读取32位无符号整数   |        |      |      |        |    |
| ByteArray::readFint32               | 读取32位有符号整数   |        |      |      |        |    |
| ByteArray::readFuint64              | 读取64位无符号整数   |        |      |
| ByteArray::readFint64               | 读取64位有符号整数   |        |      |      |        |
| ByteArray::readUint32               | 读取压缩32位无符号整数 |        |      |      |        |    |
| ByteArray::readUint64               | 读取压缩64位无符号整数 |        |      |      |        |    |
| ByteArray::readInt32                | 读取压缩32位有符号整数 |        |      |      |        |    |
| ByteArray::readInt64                | 读取压缩64位有符号整数 |        |      |
| ByteArray::readDouble               | 读取64位浮点数     |        |      |      |        |    |
| ByteArray::readFloat                | 读取32位浮点数     |        |      |
| ByteArray::readString16             | 读取16位字符串     |        |      |      |        |    |
| ByteArray::readString32             | 读取32位字符串     |        |      |      |
| ByteArray::readString64             | 读取64位字符串     |        |      |      |        |    |
| ByteArray::readStringVint           | 读取字符串        | 读取64   |      |      |        |    |
| ByteArray::clear                    | 清空           |        |      |      |        |    |
| ByteArray::write                    | 写入           |        |      |      |        |    |
| ByteArray::read                     | 读取           |        |      |      |        |    |
| ByteArray::read                     | 读取           | 读取无副作用 |      |      |        |    |
| ByteArray::setPosition              | 设置当前处理位置     |        |      |      |        |    |
| ByteArray::getPosition              | 获取当前处理位置     |        |      |      |        |    |
| ByteArray::writeToFile              | 写入文件         |        |      |      |        |    |
| ByteArray::readFromFile             | 读取文件         |        |      |      |        |    |
| ByteArray::toString                 | 将数据转换为字符串    |        |      |      |        |    |
| ByteArray::ByteArray                | 构造函数         |        |      |      |        |    |
| ByteArray::getRemainSize            | 获取剩余大小       |        |      |      |        |    |
| ByteArray::getBaseSize              | 获取基础大小       |        |      |      |        |    |
| ByteArray::isLittleEndian           | 是否是小端        |        |      |      |        |    |
| ByteArray::setLittleEndian          | 设置是否是小端      |        |      |      |        |    |
| ByteArray::getReadBuffers           | 获取读取缓冲区      | 无副作用   | 是    |      |        |    |
| ByteArray::getWriteBuffers          | 获取写入缓冲区      |        |      |      |        |    |
| ByteArray::getSize                  | 获取大小         |        |      |      |        |    |















