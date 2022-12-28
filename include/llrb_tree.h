#include <iterator>
#include <memory>
#include <queue>

template <typename KeyType>
struct RbTreeNode {
    using NodePtr = RbTreeNode *;
    using ConstNodePtr = const RbTreeNode *;

    enum RbTreeNodeColor { BLACK, RED };

   public:
    KeyType key;
    RbTreeNodeColor color;
    NodePtr parent, lch, rch;

   public:
    RbTreeNode() = default;
    constexpr RbTreeNode(const KeyType &_key, RbTreeNodeColor _color = RbTreeNodeColor::RED,
                         NodePtr _parent = nullptr, NodePtr _lch = nullptr, NodePtr _rch = nullptr)
        : key(_key), color(_color), parent(_parent), lch(_lch), rch(_rch) {}
    constexpr RbTreeNode(KeyType &&_key, RbTreeNodeColor _color = RbTreeNodeColor::RED,
                         NodePtr _parent = nullptr, NodePtr _lch = nullptr, NodePtr _rch = nullptr)
        : key(std::move(_key)), color(_color), parent(_parent), lch(_lch), rch(_rch) {}

    ~RbTreeNode() = default;
};

template <typename KeyType>
inline RbTreeNode<KeyType> *leftRotate(RbTreeNode<KeyType> *z) {
    using NodePtr = RbTreeNode<KeyType> *;

    if (z->rch == nullptr) {
        return z;
    }
    NodePtr y = z->rch;
    z->rch = y->lch;
    if (y->lch != nullptr) {
        y->lch->parent = z;
    }
    y->parent = z->parent;
    if (z->parent == nullptr) {
    } else if (z == z->parent->lch) {
        z->parent->lch = y;
    } else if (z == z->parent->rch) {
        z->parent->rch = y;
    }
    y->lch = z;
    z->parent = y;
    y->color = z->color;
    z->color = RbTreeNode<KeyType>::RbTreeNodeColor::RED;
    return y;
}

template <typename KeyType>
inline RbTreeNode<KeyType> *rightRotate(RbTreeNode<KeyType> *z) {
    using NodePtr = RbTreeNode<KeyType> *;

    if (z->lch == nullptr) {
        return z;
    }
    NodePtr y = z->lch;
    z->lch = y->rch;
    if (y->rch != nullptr) {
        y->rch->parent = z;
    }
    y->parent = z->parent;
    if (z->parent == nullptr) {
    } else if (z == z->parent->lch) {
        z->parent->lch = y;
    } else if (z == z->parent->rch) {
        z->parent->rch = y;
    }
    y->rch = z;
    z->parent = y;
    y->color = z->color;
    z->color = RbTreeNode<KeyType>::RbTreeNodeColor::RED;
    return y;
}

template <typename KeyType>
inline bool isRed(RbTreeNode<KeyType> *node) {
    if (node == nullptr) {
        return false;
    }
    return node->color == RbTreeNode<KeyType>::RbTreeNodeColor::RED;
}

template <typename KeyType>
inline RbTreeNode<KeyType> *colorFlip(RbTreeNode<KeyType> *z) {
    using NodePtr = RbTreeNode<KeyType> *;

    auto negColor = [](NodePtr node) -> RbTreeNode<KeyType>::RbTreeNodeColor {
        return node->color == RbTreeNode<KeyType>::RbTreeNodeColor::RED
                   ? RbTreeNode<KeyType>::RbTreeNodeColor::BLACK
                   : RbTreeNode<KeyType>::RbTreeNodeColor::RED;
    };
    z->color = negColor(z);
    if (z->lch) {
        z->lch->color = negColor(z->lch);
    }
    if (z->rch) {
        z->rch->color = negColor(z->rch);
    }
    return z;
}

template <typename KeyType>
inline RbTreeNode<KeyType> *insertFixup(RbTreeNode<KeyType> *z) {
    if (z == nullptr) {
        return z;
    }
    if (isRed(z->rch) && !isRed(z->lch)) {
        z = leftRotate(z);
    }
    if (isRed(z->lch) && isRed(z->lch->lch)) {
        z = rightRotate(z);
    }
    if (isRed(z->lch) && isRed(z->rch)) {
        z = colorFlip(z);
    }
    return z;
}

template <typename KeyType, typename Less>
RbTreeNode<KeyType> *nodeInsert(RbTreeNode<KeyType> *root, RbTreeNode<KeyType> *newNode,
                                Less less) {
    if (root == nullptr) {
        return newNode;
    }
    if (root->key == newNode->key) {
    } else if (less(newNode->key, root->key)) {
        root->lch = nodeInsert(root->lch, newNode, less);
        if (root->lch) {
            root->lch->parent = root;
        }
    } else {
        root->rch = nodeInsert(root->rch, newNode, less);
        if (root->rch) {
            root->rch->parent = root;
        }
    }
    root = insertFixup(root);
    return root;
}

template <typename KeyType>
inline RbTreeNode<KeyType> *treeMin(RbTreeNode<KeyType> *z) {
    if (z == nullptr) {
        return nullptr;
    }
    while (z->lch != nullptr) {
        z = z->lch;
    }
    return z;
}

template <typename KeyType>
inline RbTreeNode<KeyType> *treeMax(RbTreeNode<KeyType> *z) {
    if (z == nullptr) {
        return nullptr;
    }
    while (z->rch != nullptr) {
        z = z->rch;
    }
    return z;
}

template <typename KeyType>
inline bool treeIsLeftChild(RbTreeNode<KeyType> *z) {
    if (z == nullptr) {
        return false;
    }
    if (z->parent == nullptr) {
        return false;
    }
    return z == z->parent->lch;
}

template <typename KeyType>
inline RbTreeNode<KeyType> *moveRedLeft(RbTreeNode<KeyType> *z) {
    colorFlip(z);
    if (isRed(z->rch->lch)) {
        z->rch = rightRotate(z->rch);
        z = leftRotate(z);
        colorFlip(z);
    }
    return z;
}

template <typename KeyType>
inline RbTreeNode<KeyType> *moveRedRight(RbTreeNode<KeyType> *z) {
    colorFlip(z);
    if (isRed(z->lch->lch)) {
        z = rightRotate(z);
        colorFlip(z);
    }
    return z;
}

template <typename KeyType>
inline RbTreeNode<KeyType> *deleteMinNode(RbTreeNode<KeyType> *z) {
    if (z->lch == nullptr) {
        delete z;
        return nullptr;
    }
    if (!isRed(z->lch) && !isRed(z->lch->lch)) {
        z = moveRedLeft(z);
    }
    z->lch = deleteMinNode(z->lch);
    return insertFixup(z);
}

template <typename KeyType, typename Less>
inline RbTreeNode<KeyType> *deleteAnyNode(RbTreeNode<KeyType> *root, const KeyType &key,
                                          Less less) {
    if (less(key, root->key)) {
        if (!root->lch) {
            return nullptr;
        }
        if (!isRed(root->lch) && !isRed(root->lch->lch)) {
            root = moveRedLeft(root);
        }
        root->lch = deleteAnyNode(root->lch, key, less);
    } else {
        if (isRed(root->lch)) {
            root = rightRotate(root);
        }
        if (key == root->key && root->rch == nullptr) {
            delete root;
            return nullptr;
        }
        if (!isRed(root->rch) && !isRed(root->rch->lch)) {
            root = moveRedRight(root);
        }
        if (key == root->key) {
            root->key = treeMin(root->rch)->key;
            root->rch = deleteMinNode(root->rch);
        } else {
            root->rch = deleteAnyNode(root->rch, key, less);
        }
    }
    return insertFixup(root);
}

template <typename KeyType>
RbTreeNode<KeyType> *prevNode(RbTreeNode<KeyType> *z) {
    if (z == nullptr) {
        return nullptr;
    }
    if (z->lch != nullptr) {
        return treeMax(z->lch);
    }
    while (treeIsLeftChild(z)) {
        z = z->parent;
    }
    return z->parent;
}

template <typename KeyType>
RbTreeNode<KeyType> *nextNode(RbTreeNode<KeyType> *z) {
    if (z == nullptr) {
        return nullptr;
    }
    if (z->rch != nullptr) {
        return treeMin(z->rch);
    }
    while (!treeIsLeftChild(z)) {
        if (z->parent == nullptr) {
            return nullptr;
        }
        z = z->parent;
    }
    return z->parent;
}

template <typename KeyType, typename Less = std::less<KeyType>>
struct RbTree {
    using NodeType = RbTreeNode<KeyType>;
    using NodePtr = typename NodeType::NodePtr;
    using ConstNodePtr = typename NodeType::ConstNodePtr;
    using RbTreeNodeColor = typename NodeType::RbTreeNodeColor;

   public:
    Less less;

   public:
    NodePtr root;
    NodePtr leftmost;
    size_t nodeCnt;

   public:
    RbTree() : root(nullptr), leftmost(nullptr), nodeCnt(0) {}
    RbTree(const RbTree &other) { this->copyFrom(other); }
    RbTree(RbTree &&other)
        : root(std::move(other.root)),
          leftmost(std::move(other.leftmost)),
          nodeCnt(other.nodeCnt),
          less(other.less) {
        this->reset();
    }
    ~RbTree() { this->clear(); }

   public:
    RbTree &operator=(const RbTree &rhs) {
        if (this == &rhs) {
            return *this;
        }
        this->clear();
        this->copyFrom(rhs);
        return *this;
    }
    RbTree &operator=(RbTree &&rhs) {
        if (this == &rhs) {
            return *this;
        }
        this->clear();
        this->root = std::move(rhs.root);
        this->leftmost = std::move(rhs.leftmost);
        this->nodeCnt = rhs.nodeCnt;
        this->less = rhs.less;
        rhs.reset();
        return *this;
    }

   public:
    void insert(const KeyType &key) {
        NodePtr newNode = new NodeType(key, RbTreeNode<KeyType>::RbTreeNodeColor::RED);
        this->root = nodeInsert(this->root, newNode, less);
        this->root->color = RbTreeNode<KeyType>::RbTreeNodeColor::BLACK;
        if (!this->leftmost || less(newNode->key, this->leftmost->key)) {
            this->leftmost = newNode;
        }
        ++this->nodeCnt;
    }
    void insert(KeyType &&key) {
        NodePtr newNode = new NodeType(std::move(key), RbTreeNode<KeyType>::RbTreeNodeColor::RED);
        this->root = nodeInsert(this->root, newNode, less);
        this->root->color = RbTreeNode<KeyType>::RbTreeNodeColor::BLACK;
        if (!this->leftmost || less(newNode->key, this->leftmost->key)) {
            this->leftmost = newNode;
        }
        ++this->nodeCnt;
    }

   public:
    NodePtr find(const KeyType &_key) const {
        NodePtr ptr = this->root;
        while (ptr != nullptr) {
            if (this->less(_key, ptr->key)) {
                ptr = ptr->lch;
            } else if (_key == ptr->key) {
                break;
            } else {
                ptr = ptr->rch;
            }
        }
        return ptr;
    }

    void reset() {
        this->root = nullptr;
        this->leftmost = nullptr;
        this->nodeCnt = 0;
    }

    void clear() {
        if (root == nullptr) {
            return;
        }
        std::queue<NodePtr> q{};
        q.push(root);
        while (!q.empty()) {
            NodePtr _curr = q.front();
            q.pop();
            if (_curr == nullptr) {
                break;
            }
            NodePtr _lch = _curr->lch, _rch = _curr->rch;
            q.push(_lch);
            q.push(_rch);
            delete _curr;
        }
        root = nullptr;
        leftmost = nullptr;
        return;
    }

    void copyFrom(const RbTree &other) {
        this->nodeCnt = other.nodeCnt;
        this->less = other.less;

        if (other.nodeCnt == 0) {
            root = nullptr;
            leftmost = nullptr;
            return;
        }

        this->root = new NodeType(other.root->key);
        std::queue<NodePtr> q{}, qOther{};
        qOther.push(other.root);
        q.push(this->root);
        while (!q.empty()) {
            NodePtr _curr = q.front();
            q.pop();
            NodePtr _currOther = qOther.front();
            qOther.pop();
            if (_currOther->lch != nullptr) {
                _curr->lch = new NodeType(_currOther->lch->key);
                q.push(_curr->lch);
                qOther.push(_currOther->lch);
            }
            if (_currOther->rch != nullptr) {
                _curr->rch = new NodeType(_currOther->rch->key);
                q.push(_curr->rch);
                qOther.push(_currOther->rch);
            }
        }

        this->leftmost = treeMin(this->root);
    }

    void erase(const KeyType &key) {
        if (this->root != nullptr) {
            this->root = deleteAnyNode(this->root, key, less);
            this->leftmost = treeMin(this->root);
            --this->nodeCnt;
        }
    }
};

template <typename KeyType>
class ConstRbTreeIter;

template <typename KeyType>
class RbTreeIter;

template <typename KeyType>
class RbTreeIter {
   public:
    using NodePtr = typename RbTreeNode<KeyType>::NodePtr;
    using Iter = RbTreeIter<KeyType>;
    using ConstIter = ConstRbTreeIter<KeyType>;
    using Reference = KeyType &;
    using Pointer = KeyType *;

   public:
    NodePtr node;

   public:
    RbTreeIter() {}
    RbTreeIter(NodePtr _node) { this->node = _node; }
    RbTreeIter(const Iter &rhs) { this->node = rhs.node; }
    RbTreeIter(const ConstIter &rhs) { this->node = rhs.node; }

    Reference operator*() const { return this->node->key; }
    Pointer operator->() const { return &(this->operator*()); }

    Iter &operator++() {
        this->node = nextNode(this->node);
        return *this;
    }
    Iter operator++(int) {
        Iter tmp(*this);
        this->node = nextNode(this->node);
        return tmp;
    }
    Iter &operator--() {
        this->node = prevNode(this->node);
        return *this;
    }
    Iter operator--(int) {
        Iter tmp(*this);
        this->node = prevNode(this->node);
        return tmp;
    }

    bool operator==(const Iter &rhs) { return this->node == rhs.node; }
    bool operator!=(const Iter &rhs) { return this->node != rhs.node; }
};

template <typename KeyType>
class ConstRbTreeIter {
   public:
    using NodePtr = typename RbTreeNode<KeyType>::NodePtr;
    using Iter = RbTreeIter<KeyType>;
    using ConstIter = ConstRbTreeIter<KeyType>;
    using Reference = const KeyType &;
    using Pointer = const KeyType *;

   public:
    NodePtr node;

   public:
    ConstRbTreeIter() {}
    ConstRbTreeIter(NodePtr _node) { this->node = _node; }
    ConstRbTreeIter(const Iter &rhs) { this->node = rhs.node; }
    ConstRbTreeIter(const ConstIter &rhs) { this->node = rhs.node; }

    Reference operator*() const { return this->node->key; }
    Pointer operator->() const { return &(this->operator*()); }

    Iter &operator++() {
        this->node = nextNode(this->node);
        return *this;
    }
    Iter operator++(int) {
        Iter tmp(*this);
        this->node = nextNode(this->node);
        return tmp;
    }
    Iter &operator--() {
        this->node = prevNode(this->node);
        return *this;
    }
    Iter operator--(int) {
        Iter tmp(*this);
        this->node = prevNode(this->node);
        return tmp;
    }

    bool operator==(const Iter &rhs) { return this->node == rhs.node; }
    bool operator!=(const Iter &rhs) { return this->node != rhs.node; }
};

template <typename KeyType, typename Less = std::less<KeyType>>
struct set {
   public:
    using Iter = RbTreeIter<KeyType>;
    using ConstIter = RbTreeIter<KeyType>;

   public:
    RbTree<KeyType, Less> tree;

   public:
    set() = default;
    set(const set &other) : tree(other.tree) {}
    set(set &&other) : tree(std::move(other.tree)) {}
    set(std::initializer_list<KeyType> init) {
        for (auto &i : init) {
            this->insert(i);
        }
    }
    ~set() = default;

   public:
    set<KeyType> &operator=(const set<KeyType> &rhs) {
        if (this == &rhs) {
            return *this;
        }
        this->clear();
        this->tree = rhs.tree;
        return *this;
    }
    set<KeyType> &operator=(set<KeyType> &&rhs) {
        if (this == &rhs) {
            return *this;
        }
        this->clear();
        this->tree = std::move(rhs.tree);
        return *this;
    }

   public:
    friend bool operator==(const set &lhs, const set &rhs) {
        if (lhs.size() != rhs.size()) {
            return false;
        }
        auto iter1 = lhs.cbegin();
        auto iter2 = rhs.cbegin();
        while (iter1 != lhs.cend() && iter2 != rhs.cend()) {
            if (*iter1++ != *iter2++) {
                return false;
            }
        }
        return true;
    }
    friend bool operator!=(const set &lhs, const set &rhs) { return !(lhs == rhs); }
    friend bool operator<(const set &lhs, const set &rhs) {
        auto iter1 = lhs.begin(), iter2 = rhs.begin();
        if (iter1 == lhs.end() && iter2 != rhs.end()) {
            return true;
        }
        if (iter1 == lhs.end() && iter2 == rhs.end()) {
            return false;
        }
        if (iter1 != lhs.end() && iter2 == rhs.end()) {
            return false;
        }
        while (iter1 != lhs.end() && iter2 != rhs.end()) {
            if (Less{}(*iter1++, *iter2++)) {
                return true;
            }
        }
        if (lhs.size() < rhs.size()) {
            return true;
        }
        return false;
    }
    friend bool operator>(const set &lhs, const set &rhs) { return !(lhs == rhs) && !(lhs < rhs); }

   public:
    Iter begin() { return Iter(tree.leftmost); }
    Iter end() { return Iter(nullptr); }
    ConstIter begin() const { return Iter(tree.leftmost); }
    ConstIter end() const { return Iter(nullptr); }
    ConstIter cbegin() const { return ConstIter(treeMin(tree.root)); }
    ConstIter cend() const { return ConstIter(nullptr); }

   public:
    bool empty() { return tree.nodeCnt == 0; }
    size_t size() const { return tree.nodeCnt; }

   public:
    void insert(KeyType &&key) {
        if (!tree.find(key)) tree.insert(std::move(key));
    }
    void insert(const KeyType &key) {
        if (!tree.find(key)) tree.insert(key);
    }
    Iter find(const KeyType &key) { return Iter(tree.find(key)); }
    ConstIter find(const KeyType &key) const { return ConstIter(tree.find(key)); }
    void clear() {
        tree.clear();
        return;
    }
    void reset() {
        tree.reset();
        return;
    }
    void erase(const KeyType &key) {
        if (tree.find(key)) {
            tree.erase(key);
        }
        return;
    }
};