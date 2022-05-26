#include <iostream> 
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>

#define STORE_FILE "store/dumpFile"

std::mutex mtx;     // 临界区的互斥锁
std::string delimiter = ":"; //分隔符

//用类模板实现要存储的节点数据
template<class K, class V>
class Node {
public:
    Node() {}
    Node(K k, V v, int);
    ~Node() {};
    K get_key() const;
    V get_value() const;
    void set_value(V);
    //指针数组，每个指针代表一层，指向下一个节点
    //层数是从1开始的，第0层其实是next指针，所以level层总共有level + 1个指针
    Node<K, V>** forward;
    int node_level;
private:
    K key;
    V value;
};

template<class K, class V> 
Node<K, V>::Node(const K k, const V v, int level) {
    this->key = k;
    this->value = v;
    this->node_level = level;

    //level + 1,因为数组索引是从0 - level
    this->forward = new Node<K, V>*[level + 1];
    //填充forward数组为0
    memset(this->forward, 0, sizeof(Node(K, V)*)*(level + 1));
};

template<class K, class V> 
Node<K, V>::~Node() {
    delete []forward;
};

template<class K, class V>
K Node<K, V>::get_key() const {
    return key;
};

template<typename K, typename V> 
V Node<K, V>::get_value() const {
    return value;
};
template<typename K, typename V> 
void Node<K, V>::set_value(V value) {
    this->value = value;
};

//跳表的类模板
template<class K, class V>
class SkipList {
public:
    SkipList(int); //构造函数，初始化最大层数
    ~SkipList();
    int get_random_level(); //随机获取一个节点的层数，有讲究
    Node<K, V>* create_node(K, V, int); //新建键值对
    int insert_element(K, V);   //插入键值对
    void display_list();        //输出链表
    bool search_element(K);   
    void delete_element(K);
    void dump_file();
    void load_file();
    int size();              //返回键值对的个数
private:
    void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
    bool is_valid_string(const std::string& str);
private:
    //跳表的最大层数
    int _max_level;
    //跳表当前的最大层数
    int _skip_list_level;
    //头结点指针;
    Node<K, V>* _header;
    //文件操作
    std::ofstream _file_writer;
    std::ifstream _file_reader;
    //跳表的当前元素个数
    int _element_count;
};


//构造函数
template<class K, class V> 
SkipList<K, V>::SkipList(int max_level) {
    this->_max_level = max_level;
    this->_skip_list_level = 0;
    this->_element_count = 0;
    //创建虚拟节点
    K k;
    V v;
    this->_header = new Node<K, V>(k, v, _max_level);
};
//析构函数
template<class K, class V> 
SkipList<K, V>::~SkipList() {

    if (_file_writer.is_open()) {
        _file_writer.close();
    }
    if (_file_reader.is_open()) {
        _file_reader.close();
    }
    delete _header;
}



//建立一个跳表节点
template<class K, class V> 
Node<K, V>* SkipList<K, V>::create_node(const K k, const V v, int level) {
    Node<K, V>* n = new Node<K, V>(k, v, level);
    return n;
}



//跳表元素查找
/*
从当前最大的层数开始找，如果要查找的键比cur的下一个节点的键值大，cur就往后移动
找到大于等于key的第一个节点，如果那个节点等于key，就说明找到了，否则没有该key
*/

// Search for element in skip list 
/*
                           +------------+
                           |  select 60 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |
level 3         1+-------->10+------------------>50+           70       100
                                                   |
                                                   |
level 2         1          10         30         50|           70       100
                                                   |
                                                   |
level 1         1    4     10         30         50|           70       100
                                                   |
                                                   |
level 0         1    4   9 10         30   40    50+-->60      70       100
*/
template<typename K, typename V> 
bool SkipList<K, V>::search_element(K key) {

    std::cout << "search_element-----------------" << std::endl;
    Node<K, V> *current = _header;

    // 从最高层开始查找
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
    }
    //到达第0层，将第零层的右边节点赋值给当前节点
    //reached level 0 and advance pointer to right node, which we search
    current = current->forward[0];

    // if current node have key equal to searched key, we get it
    if (current and current->get_key() == key) {
        std::cout << "Found key: " << key << ", value: " << current->get_value() << std::endl;
        return true;
    }

    std::cout << "Not Found Key:" << key << std::endl;
    return false;
}

//插入给定的键值对到跳表中
//返回值为1代表插入元素已经存在
//返回值为0代表插入元素成功
/*
                           +------------+
                           |  insert 50 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |                      insert +----+
level 3         1+-------->10+---------------> | 50 |          70       100
                                               |    |
                                               |    |
level 2         1          10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 1         1    4     10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 0         1    4   9 10         30   40  | 50 |  60      70       100
                                               +----+
*/

template<class K, class V>
int SkipList<K, V>::insert_element(const K key, const V value) {
    
    mtx.lock();
    Node<K, V> *current = this->_header;

    // 创建一个更新数组并且初始化
    // update是一个指针数组，该数组放置了节点，节点->forward[i]应该在以后操作
    Node<K, V> *update[_max_level+1];
    memset(update, 0, sizeof(Node<K, V>*)*(_max_level+1));  

    // 从跳表的当前最高层开始
    for(int i = _skip_list_level; i >= 0; i--) {
        while(current->forward[i] != NULL && current->forward[i]->get_key() < key) {
            current = current->forward[i]; 
        }
        update[i] = current;
    }

    // 已经到达级别0，并将指针向前移动到右侧节点，该节点需要插入。
    current = current->forward[0];

    //如果当前节点对于我们查找的值有相等的，就可以获得它，直接返回已经存在
    if (current != NULL && current->get_key() == key) {
        std::cout << "key: " << key << ", exists" << std::endl;
        mtx.unlock();
        return 1;
    }

    //如果当前指针为空就代表着我们已经到达这层的终点 
    //如果当前的键值不等于我们插入的键值就代表这我们需要在update[0]和current之间插入该节点 
    if (current == NULL || current->get_key() != key ) {
        
        // 给节点创建一个随机层
        int random_level = get_random_level();

        //如果随机的层数大于跳表的列表的当前的层数，则使用指向标头的指针初始化更新值。
        //
        if (random_level > _skip_list_level) {
            for (int i = _skip_list_level+1; i < random_level+1; i++) {
                update[i] = _header;
            }
            //将当前跳表层数更新为最大的随机层数。
            _skip_list_level = random_level;
        }

        //使用生成的随机级别创建新节点
        Node<K, V>* inserted_node = create_node(key, value, random_level);
        
        // 插入新的节点
        for (int i = 0; i <= random_level; i++) {
            inserted_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_node;
        }
        std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;
        //元素个数加1
        _element_count ++;
    }
    mtx.unlock();
    return 0;
}
template<typename K, typename V>
int SkipList<K, V>::get_random_level(){

    int k = 1;
    while (rand() % 2) {
        k++;
    }
    k = (k < _max_level) ? k : _max_level;
    return k;
};







