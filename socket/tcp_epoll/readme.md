C-S模式的tcp通信

重点:

    - tcp
    - 服务器用epoll管理所有fd
    - 客户端用select就行
---

服务器

    - 可以重用socket
    - 可以处理新的tcp连接
    - 可以最后一个连接的客户端沟通
    - 可以接收所有客户端的数据
    - 可以处理客户端的异常退出


---

客户端

    - 可以正常与服务器建立连接
    - 可以给服务器发送请求
    - 可以接收服务器返回的数据
    - 可以处理服务器的异常退出
