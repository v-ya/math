# math
* math script interpreter.
* See test.math for detailed usage.
* function usage see "Func-used.txt"
---
### 预计更新
+ 修复 Bmp Wav Avi 对象释放时产生的内存泄漏
+ 完善分项 Bpm
- 椭圆渲染函数
* 完善分项 Wav
* 完成 main.c main() "-var"
* 实现脚本传参
* 新加分项 Mat
* ...
---
### v1.0.9 2017.06.29
* add .load() .unload()
* 完善导入库基本函数接口调用
---
### v1.0.8 2017.06.27
* add .wav.clean() .wav.Samp .wav.mSamp .wav.mvol
---
### v1.0.7 2017.06.17
* add .void() .var()
* 增加命令行脚本传参 only string
---
### v1.0.6 2017.06.05
* fix object.set()
* add .wav.(padd pget getmax getcos getsin addcos addsin)
* add 运算符 >=, <=, !=, ?, $
* add .func(), 可自定义函数
* 修复若干错误
---
### v1.0.5 2017.05.29
* add .bmp.rgba 系列函数, .bmp.setxyt 函数
* fix .bmp.fset(), .bmp.fadd()
* add .avi 分项
* add .srand() .rand()
* add .bmp.load() .bmp.clean() .bmp.ffill()
* add .bmp.cset() .bmp.cadd() .bmp.fcset() .bmp.fcadd()
* add .bmp.arc_pra() .bmp.line() .bmp.arc() .bmp.next()
* rename .bmp.blockset() -> .bmp.putset(), .bmp.blockadd() -> .bmp.putadd()
* add .bmp.getset() .bmp.getadd()
* add .bmp.rota() .bmp.getzoom() .bmp.revw() .bmp.revh()
* 部分修正
---
### v1.0.4 2017.05.25
* 修复部分浮点数("float":double)传参错误
* Fix some floating point("float":double) passer errors
* 新增分项 Wav
* Add item Wav
---
### v1.0.3 2017.05.24
* 修订系统函数报错处理
* Revised system function error processing
* 增加非正常结束时脚本的行号显示
* Increase the line number of the script at the end of the nonstandard
---
### v1.0.2 2017.05.23
* 新增分项 Bmp
* Add item Bmp
* 新加 .bmp.{new write setcolor pset padd setalpha fset fadd fill blockset blockadd}()
* New .bmp.{new write setcolor pset padd setalpha fset fadd fill blockset blockadd}()
---
### v1.0.1 2017.05.21
* 修复 object.set() 权限问题
* Fix the object.set() permission issue
* 修复 .clear() 功能
* Fix the .clear() function
* 新加 .test()
* New .test()
* 提高部分函数效率
* Improve part of the function efficiency
* 以及其他细微改动
* As well as other minor changes
* 重写 .echo()
* Rewrite .echo()
* 新加 .sprintf() .strcmp()
* New .sprintf() .strcmp()
* 新加 .ldw_test() .lup_test()
* New .ldw_test() .lup_test()
---
### v1.0.0 2017.05.19


