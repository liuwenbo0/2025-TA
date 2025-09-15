#define MAXLENG 100

#include <cstring>

typedef int ElementType;

/*
    用数组模拟的顺序栈类
    采用静态分配的方式实现
    请参考ppt 第二讲 栈&队列 中如下页面：
    5-11’ 15-18‘
*/
class MyStack {
    public:
        ElementType elem[MAXLENG]; // 模拟栈的静态数组
        int top; // 栈顶指针, 注意本实现的顶元素为elem[top]!

        // 初始化静态栈
        // 本函数不需修改
        void Initstack() {
            memset(elem, -1, sizeof elem);
            top = -1;
        }

        // 判断栈空, 若为空栈,则Empty()返回true；否则返回false
        bool Empty() {
            //请在这里完成你的代码
        }

        // 判断栈满，若栈满，则Full()为true；否则为false
        bool Full() {
            //请在这里完成你的代码
        }

        // 元素e进栈，若栈满，则无法成功插入，插入成功返回true，否则返回false
        // @param
        // e: 将要入栈的元素
        bool Push(ElementType e) {
            //请在这里完成你的代码
        }

        // 栈的顶元素拷贝到e，若栈为空，则无法拷贝，返回false，成功拷贝则返回true
        // @param
        // e: 指向存放栈顶元素地址的指针
        bool Gettop(ElementType &e) {
            //请在这里完成你的代码
        }

        // 删除栈s的顶元素，并将删除的元素赋给e带出，若栈空，则无法成功删除，删除成功返回true，否则返回false
        // @param
        // e: 指向存放出栈元素地址的指针
        bool Pop(ElementType &e) {
            //请在这里完成你的代码
        }
};