# git pull提交异常



> fatal: unable to access 'https://github.com/erfenjiao/Learngit/': gnutls_handshake() failed: The TLS connection was non-properly terminated.

解决：

https://www.cnblogs.com/ybixian/p/12331879.html

git config --global -l
uesr.email=630166475@qq.com
user.name=erfenjiao
user.email=630166475@qq.com
root@erfenjiao-PC:/home/erfenjiao/Learngit/Learn# env|grep -i proxy


git token:
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
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