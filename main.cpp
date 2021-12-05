#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>

using namespace std;

/*-----------------------------------------------------------------------------------*/

template<class T>
class NodeT{
    public:
        T data;
        NodeT * left, * right, * parent;
        bool isBlack;
        NodeT(){
            data = NULL;
            isBlack = false;
            left = right = parent = nullptr;
        }
        NodeT(const T input_data){
            data = input_data;
            isBlack = false;
            left = nullptr;
            right = nullptr;
            parent = nullptr;
        }
        NodeT(const NodeT<T>* nodeptr, NodeT<T>* const parentptr){
            data = nodeptr->data;
            isBlack = nodeptr->isBlack;
            parent = parentptr;
            if(nodeptr->left != nullptr){
                left = new NodeT<T>(nodeptr->left, this);
            } else {
                left = nullptr;
            }
            if(nodeptr->right != nullptr){
                right = new NodeT<T>(nodeptr->right, this);
            } else {
                right = nullptr;
            }
        }
        ~NodeT(){
            if(left){
                left->~NodeT();
            }
            if(right){
                right->~NodeT();
            }
        }
};

/*-----------------------------------------------------------------------------------*/

template<class T>
class RedBlackTree{
    public:
        NodeT<T>* root;
        int cur_size;
        vector<T> value_arr;

        RedBlackTree();
        ~RedBlackTree();
        RedBlackTree(const RedBlackTree<T> &);

        RedBlackTree<T> & operator=(const RedBlackTree<T> &);

        void bstInsert(NodeT<T>*);
        NodeT<T>* bstRemove(T x) const;
        NodeT<T>* GetSuccessorOf(NodeT<T>*) const;
        void left_rotate(NodeT<T>*);
        void right_rotate(NodeT<T>*);
        void rbDeleteFixUp(NodeT<T>*);

        bool insert(const T);
        bool remove(const T);

        bool search(const T);
        vector<T> search(const T, const T);

        T closestLess(const T);
        T closestGreater(const T);

        vector<T> values();

        int const size();

        template <class Tjwme>
        friend NodeT<Tjwme>* JWMEgetRoot(const RedBlackTree<Tjwme> & rbt);
};

/*-----------------------------------------------------------------------------------*/

template <class T>
RedBlackTree<T>::RedBlackTree()
{
    root = nullptr;
    cur_size = 0;
}

template <class T>
RedBlackTree<T>::RedBlackTree(const RedBlackTree<T> &tree) {
    cur_size = tree.cur_size;
    value_arr = tree.value_arr;
    NodeT<T>* tmp = tree.root;
    root = new NodeT<T>(tmp, nullptr);
    root->isBlack = true;
}

template <class T>
RedBlackTree<T> & RedBlackTree<T>::operator=(const RedBlackTree<T> &t) {
    if (t != this) {
        ~RedBlackTree();
        this = new RedBlackTree<T>(t);
    }
    return *this;
}

template <class T>
RedBlackTree<T>::~RedBlackTree(){
    root->~NodeT<T>();
}

template <class T>
void RedBlackTree<T>::bstInsert(NodeT<T>* x){
    NodeT<T>* temp = root;
    NodeT<T>* temp2 = root;
    if(root == NULL)
        root = x;
    else {
        while(temp != NULL) {
            temp2 = temp;
            if(x->data < temp->data)
                temp = temp->left;
            else
                temp = temp->right;
        }
        x->parent = temp2;
        if(x->data < temp2->data)
            temp2->left = x;
        else
            temp2->right = x;
    }
    cur_size++;
    value_arr.push_back(x->data);
}

template <class T>
NodeT<T>* RedBlackTree<T>::bstRemove(T x) const {
    NodeT<T>* temp = root;
    if(root->data == x)
        return temp;
    else {
        while(temp != NULL) {
            if(x < temp->data)
                temp = temp->left;
            else if(x > temp->data)
                temp = temp->right;
            else
                return temp;
        }
        return temp;
    }
}

template <class T>
NodeT<T>* RedBlackTree<T>::GetSuccessorOf(NodeT<T>* x) const {
    NodeT<T>* y;
    if((y = x->right) != NULL) {
        while(y->left != NULL) {
            y=y->left;
        }
        return y;
    } else {
        y = x->parent;
        while(x == y->right && y != NULL) {
            x = y;
            y = y->parent;
        }
        if(y == root)
            return NULL;
        return y;
    }
}

template <class T>
void RedBlackTree<T>::left_rotate(NodeT<T>* x) {
    NodeT<T>* y = NULL;
    y = x->right;
    x->right = y->left;
    if (y->left != NULL)
        y->left->parent = x;
    y->parent = x->parent;
    if(x->parent == NULL)
        root = y;
    else if( x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

template <class T>
void RedBlackTree<T>::right_rotate(NodeT<T>* y){
    NodeT<T>* x = NULL;
    x = y->left;
    y->left = x->right;
    if (x->right != NULL)
        x->right->parent = y;
    x->parent = y->parent;
    if(y->parent == NULL)
        root = x;
    else if(y == y->parent->left) {
        y->parent->left = x;
    }
    else {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
}

template <class T>
bool RedBlackTree<T>::insert(const T d) {
    bool result = (!search(d));
    if(result) {
        NodeT<T>* y;
        NodeT<T>* x = new NodeT<T>(d);
        bstInsert(x);
        while(x != root && x->parent->isBlack == false) {
            if(x->parent == x->parent->parent->left) {
                y = x->parent->parent->right;
                if(y != NULL && y->isBlack == false) {
                    x->parent->isBlack = true;
                    y->isBlack = true;
                    x->parent->parent->isBlack = false;
                    x = x->parent->parent;
                } else {
                    if(x == x->parent->right) {
                        x = x->parent;
                        left_rotate(x);
                    }
                    x->parent->isBlack = true;
                    x->parent->parent->isBlack = false;
                    if(x->parent->parent != NULL)
                        right_rotate(x->parent->parent);
                }
            } else {
                y = x->parent->parent->left;
                if(y != NULL && y->isBlack == false) {
                    x->parent->isBlack = true;
                    y->isBlack = true;
                    x->parent->parent->isBlack = false;
                    x = x->parent->parent;
                } else {
                    if(x == x->parent->left) {
                        x = x->parent;
                        right_rotate(x);
                    }
                    x->parent->isBlack = true;
                    x->parent->parent->isBlack = false;
                    if(x->parent->parent != NULL)
                        left_rotate(x->parent->parent);
                }
            }
        }
        root->isBlack = true;
    }
    return result;
}

template <class T>
void RedBlackTree<T>::rbDeleteFixUp(NodeT<T>* x) {
    NodeT<T>* w = NULL;
    while(x != root && x->isBlack == true) {
        if(x == x->parent->left) {
            w = x->parent->right;
            if(w != NULL && w->isBlack == false) {
                w->isBlack = true;
                w->parent->isBlack = false;
                left_rotate(x->parent);
                w = x->parent->right;
            }
            if((w->left != NULL && w->right != NULL) && (w->left->isBlack == true && w->right->isBlack == true)) {
                w->isBlack = false;
                x = x->parent;
            } else {
                if((w->right != NULL) && w->right->isBlack == true) {
                    if(w->left != NULL)
                        w->left->isBlack = true;
                    w->isBlack = false;
                    right_rotate(w);
                    w = x->parent->right;
                }
                w->isBlack = x->parent->isBlack;
                x->parent->isBlack = true;
                if(w->right != NULL)
                    w->right->isBlack = true;
                left_rotate(x->parent);
                x = root;
            }
        } else {
            w = x->parent->left;
            if(w != NULL && w->isBlack == false) {
                w->isBlack = true;
                w->parent->isBlack = false;
                right_rotate(x->parent);
                w = x->parent->left;
            }
            if(w != NULL && (w->left != NULL && w->right != NULL) && (w->right->isBlack == true && w->left->isBlack == true)) {
                w->isBlack = false;
                x = x->parent;
            } else {
                if(w != NULL && (w->left != NULL) && w->left->isBlack == true) {
                    if(w->right != NULL)
                        w->right->isBlack = true;
                    w->isBlack = false;
                    left_rotate(w);
                    w = x->parent->left;
                }
                if(w != NULL)
                    w->isBlack = x->parent->isBlack;
                x->parent->isBlack = true;
                if(w != NULL && w->left != NULL)
                    w->left->isBlack = true;
                right_rotate(x->parent);
                x = root;
            }
        }
    }
    x->isBlack = true;
}

template <class T>
bool RedBlackTree<T>::remove(const T d) {
    if(!search(d))
        return false;
    NodeT<T>* z = bstRemove(d);
    NodeT<T>* x = NULL;
    NodeT<T>* y = NULL;
    if(root == z && (root->left == NULL && root->right == NULL))
    {
        delete root;
        root = NULL;
        cur_size = 0;
        value_arr = vector<T>();
        return true;
    }
    cur_size--;
    for (auto i = value_arr.begin(); i != value_arr.end(); ++i) {
        if (*i == d) {
            value_arr.erase(i);
            break;
        }
    }
    if(z->left == NULL || z->right == NULL) //one or no children
        y = z;
    else
        y = GetSuccessorOf(z); //two children
    if((z->left == NULL && z->right != NULL) || (z->right == NULL && z->left != NULL)) {
        if(y->left != NULL) //identify if y's only child is right or left
            x = y->left;
        else
            x = y->right;
        x->parent = y->parent;
        if(y->parent == NULL)
            root = x;
        else
            if(y == y->parent->left)
                y->parent->left = x;
            else
                y->parent->right = x;
    }

    if(z->left == NULL && z->right == NULL) {
        if(y->parent == NULL) {
            root = NULL;
            x = NULL;
        }
        if(y == y->parent->left) {
            x = NULL;
            y->parent->left = NULL;
        } else {
            x = NULL;
            y->parent->right = NULL;
        }
    }
    if(y != z) {
        z->data = y->data;
        if(y->right = NULL) {
            if(y == y->parent->left)
                y->parent->left = NULL;
            else
                y->parent->right = NULL;
            x = NULL;
        } else {
            x = y->right;
            if(y == y->parent->left)
                y->parent->left = y->right;
            else
                y->parent->right = y->right;
        }
    }
    if(y->isBlack && x != NULL)
        rbDeleteFixUp(x); // fix the red black tree
    delete y;
    return true;
}

template <class T>
bool RedBlackTree<T>::search(const T key) {
    vector<T> v = values();
    if(find(v.begin(), v.end(), key) != v.end())
        return true;
    else
        return false;
}

template <class T>
vector<T> RedBlackTree<T>::search(const T key1, const T key2) {
    vector<T> v = values();
    vector<T> res;
    T min = (key1 <= key2) ? key1 : key2;
    T max = (key1 >= key2) ? key1 : key2;
    for(auto& it : v){
        if(it >= min && it <= max)
            res.push_back(it);
    }
    return res;
}

template <class T>
 T RedBlackTree<T>::closestLess(const T key) {
    vector<T> v = values();
    T res = NULL;
    for(auto& it : v){
        if(it < key)
            res = it;
        else if(it > key){
            break;
        }
    }
    if(res != NULL)
        return res;
    else
        return key;
}

template <class T>
T RedBlackTree<T>::closestGreater(const T key) {
    vector<T> v = values();
    T res = NULL;
    for(auto& it : v){
        if(it > key)
            return it;
    }
    return key;
}

template <class T>
vector<T> RedBlackTree<T>::values() {
    sort(value_arr.begin(), value_arr.end());
    return value_arr;
}

template <class T>
int const RedBlackTree<T>::size() {
    return cur_size;
}

/*-----------------------------------------------------------------------------------*/

void statistics(string filename){
    RedBlackTree<double> stat_tree;
    ifstream readfile;
    readfile.open(filename);
    string thisline;

    while(getline(readfile, thisline)){
        double tmp = stod(thisline);
        stat_tree.insert(tmp);
    }

    vector<double> vals = stat_tree.values();
    double sum = 0;
    int div = 0;
    for(auto& it : vals){
        sum += it;
        div++;
    }
    double avg = sum/div;
    double median;

    if(div % 2 == 0){
        div = div/2;
        sum = vals[div-1] + vals[div];
        median = sum/2;
    } else {
        div = (int)div / 2;
        median = vals[div];
    }

    cout
     << "# of values:  " << stat_tree.size() << endl
     << "average:      " << avg << endl
     << "median:       " << median << endl
     << "closest < 42: " << stat_tree.closestLess(42) << endl
     << "closest > 42: " << stat_tree.closestGreater(42) << endl;
}

/*-----------------------------------------------------------------------------------*/

void simpleTest(){
    RedBlackTree<int> rb1;
    if (rb1.insert(42)){
        cout << "inserted 42" << endl;
    }
    rb1.insert(71);
    rb1.insert(13);

    RedBlackTree<int> rb2(rb1);
    if (rb1.remove(13))
        cout << "removed 13" << endl;
    if (rb1.search(42))
        cout << "found 42" << endl;
    vector<int> v1 = rb1.search(50, 100); //should contain 71
    cout << "v1 contains : { ";
    for(auto& it : v1)
        cout << it << " ";
    cout << "}" << endl;
    vector<int> v2 = rb1.values(); //should contain {42,71}
    cout << "v2 contains : { ";
    for(auto& it : v2)
        cout << it << " ";
    cout << "}" << endl;

    cout << "closestLess(70) = " << rb1.closestLess(70) << endl;
    cout << "closestGreater(40) = " << rb1.closestGreater(40) << endl;
    cout << "size = " << rb1.size() << endl; //should be 2}
    statistics("fuckthis.txt");
}

/*-----------------------------------------------------------------------------------*/

int main() {
    cout << "Hello, World!" << endl;
    simpleTest();
    return 0;
}

/*-----------------------------------------------------------------------------------*/