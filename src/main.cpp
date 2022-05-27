#include <iostream>
#include "skiplist.h"
#define FILE_PATH "./store/dumpFile"

int main() {

    // 键值中的key用int型，如果用其他类型，需要自定义比较函数
    // 而且如果修改key的类型，同时需要修改skipList.load_file函数
    SkipList<int, std::string> skipList(6);
	skipList.insert_element(1, "从明天起，做一个幸福的人"); 
	skipList.insert_element(3, "喂马、劈柴，周游世界"); 
	skipList.insert_element(7, "我有一所房子"); 
	skipList.insert_element(8, "面朝大海"); 
	skipList.insert_element(9, "春暖花开"); 
	skipList.insert_element(19, "从明天起"); 
	skipList.insert_element(20, "和每一个亲人通信"); 
    skipList.insert_element(30, "告诉他们我的幸福"); 

    std::cout << "skipList size:" << skipList.size() << std::endl;

    skipList.dump_file();

    // skipList.load_file();

    skipList.search_element(9);
    skipList.search_element(18);


    skipList.display_list();

    skipList.delete_element(3);
    skipList.delete_element(7);

    std::cout << "skipList size:" << skipList.size() << std::endl;

    skipList.display_list();
}

