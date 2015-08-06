#ifndef ORDEREDMAP_H
#define ORDEREDMAP_H

#include <QHash>
#include <QList>

template <class Key, class T>
class OrderedMap
{
public:
    OrderedMap();
    ~OrderedMap();

    int append(const Key& key, const T& value);
    inline OrderedMap<Key, T>& operator<<(const T& t) { append(t); return *this; }

    inline const T& at(int i) const {return plist[i];};
    inline int indexOf(const Key& key) const {return phash->value(key, -1);};

    const T value(const Key& key) const;
    inline const T operator[](const Key& key) const {return value(key);};

private:
    QHash<Key, int> *phash;
    QList<T> *plist;
};

template <class Key, class T>
OrderedMap<Key, T>::OrderedMap()
{
    phash = new QHash<Key, int>();
    plist = new QList<T>();
}

template <class Key, class T>
OrderedMap<Key, T>::~OrderedMap()
{
    delete phash;
    phash = NULL;
    delete plist;
    plist = NULL;
}

template <class Key, class T>
const T OrderedMap<Key, T>::value(const Key& key) const
{
    int i = indexOf(key);
    return (i == -1) ? T() : plist->value(i);
}

template <class Key, class T>
int OrderedMap<Key, T>::append(const Key& key, const T& value)
{
    int i = plist->size();
    plist->append(value);
    Q_ASSERT(plist->at(i) == value);
    (*phash)[key] = i;
    return i;
}

#endif // ORDEREDMAP_H
