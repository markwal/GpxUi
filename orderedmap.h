#ifndef ORDEREDMAP_H
#define ORDEREDMAP_H

#include <QHash>
#include <QLinkedList>

template <class Key, class T>
class OrderedMap
{
    typedef struct {
        Key key;
        T value;
    } Node;
    typedef typename QLinkedList<Node>::iterator NodeIterator;
    typedef typename QLinkedList<Node>::const_iterator ConstNodeIterator;
    typedef typename QHash<Key, NodeIterator>::iterator HashIterator;
    typedef typename QHash<Key, NodeIterator>::const_iterator ConstHashIterator;

public:
    OrderedMap() {}
    ~OrderedMap() {}

    class const_iterator;
    class iterator
    {
        friend class const_iterator;
        NodeIterator i;

    public:
        inline iterator() {};
        inline iterator(NodeIterator o) : i(o) { }
        inline const Key &key() const { return i->key; }
        inline T &value() const { return i->value; }
        inline T &operator*() const { return i->value; }
        inline T *operator->() const { return &i->value; }
        inline bool operator==(const iterator &other) const { return i == other.i; }
        inline bool operator!=(const iterator &other) const { return i != other.i; }
        inline iterator &operator++() { i++; return *this; }
        inline iterator operator++(int) { iterator r = *this; i++; return r; }
        inline iterator &operator--() { i--; return *this; }
        inline iterator operator--(int) { iterator r = *this; i--; return r; }
    };

    class const_iterator
    {
        friend class iterator;
        ConstNodeIterator i;

    public:
        inline const_iterator() {};
        inline const_iterator(ConstNodeIterator o) : i(o) { }
        inline const Key &key() const { return i->key; }
        inline const T &value() const { return i->value; }
        inline const T &operator*() const { return i->value; }
        inline const T *operator->() const { return &i->value; }
        inline bool operator==(const const_iterator &other) const { return i == other.i; }
        inline bool operator!=(const const_iterator &other) const { return i != other.i; }
        inline const_iterator &operator++() { i++; return *this; }
        inline const_iterator operator++(int) { const_iterator r = *this; i++; return r; }
        inline const_iterator &operator--() { i--; return *this; }
        inline const_iterator operator--(int) { const_iterator r = *this; i--; return r; }
    };

    iterator begin() { return iterator(list.begin()); }
    iterator end() { return iterator(list.end()); }

    const_iterator constBegin() { return const_iterator(list.constBegin()); }
    const_iterator constEnd() { return const_iterator(list.constEnd()); }

    iterator find(const Key &key);
    const_iterator constFind(const Key &key) const;

    void append(const Key &key, const T &value);
    int remove(const Key &key);

    const T &value(const Key &key, const T &valueDefault) const;
    const T &value(const Key &key) const;
    T &operator[](const Key &key);
    inline const T operator[](const Key &key) const {return value(key);}

    void clear();

private:
    QHash<Key, NodeIterator> hash;
    QLinkedList<Node> list;
};

template <class Key, class T>
typename OrderedMap<Key, T>::iterator OrderedMap<Key, T>::find(const Key &key)
{
    HashIterator ii = hash.find(key);
    return iterator(ii == hash.end() ? list.end() : ii.value());
}

template <class Key, class T>
typename OrderedMap<Key, T>::const_iterator OrderedMap<Key, T>::constFind(const Key &key) const
{
    ConstHashIterator ii = hash.constFind(key);
    return const_iterator(ii == hash.constEnd() ? list.constEnd() : ii.value());
}

template <class Key, class T>
T &OrderedMap<Key, T>::operator[](const Key &key)
{
    ConstHashIterator ii = hash.find(key);
    if (ii == hash.end()) {
        append(key, T());
        --ii;
    }
    return ii.value()->value;
}

template <class Key, class T>
const T &OrderedMap<Key, T>::value(const Key &key, const T &valueDefault) const
{
    ConstHashIterator ii = hash.find(key);
    return (ii == hash.end()) ? valueDefault : ii.value()->value;
}

template <class Key, class T>
const T &OrderedMap<Key, T>::value(const Key &key) const
{
    ConstHashIterator ii = hash.find(key);
    return (ii == hash.end()) ? T() : ii.value()->value;
}

template <class Key, class T>
void OrderedMap<Key, T>::append(const Key &key, const T &value)
{
    Node node = {key, value};
    hash[key] = list.insert(list.end(), node);
}

template <class Key, class T>
int OrderedMap<Key, T>::remove(const Key &key)
{
    ConstHashIterator ii = hash.find(key);
    if (ii == hash.end())
        return 0;

    NodeIterator i = ii.value();
    hash.remove(key);
    list.erase(i);
    return 1;
}

template <class Key, class T>
void OrderedMap<Key, T>::clear()
{
    hash.clear();
    list.clear();
}
#endif // ORDEREDMAP_H
