#ifndef ERA_ARRAY_SET_H_
#define ERA_ARRAY_SET_H_

#include <vector>
#include <stdint.h>

namespace era
{

struct Id
{
    Id() {}
    Id(uint64_t idx_, uint64_t rev_) : idx(idx_), rev(rev_) {}
    uint64_t idx;
    uint64_t rev;
};

template<typename T>
class ArraySet
{

public:

    struct Node
    {
        Node();
        Node(const T& v) : value(v), revision(1), key_rev(0), deleted(false) {}

        T value;
        uint64_t revision;
        uint64_t key_rev;
        bool deleted;
    };

    ArraySet() {}

    ~ArraySet() {}

    Id add(const T& v)
    {
        if (deleted_.empty())
        {
            uint64_t idx = v_.size();
            v_.push_back(Node(v));
            return Id(idx, 0);
        }
        else
        {
            uint64_t idx = deleted_.back();
            Node& n = v_[idx];
            n.value = v;
            n.revision = 0;
            n.deleted = false;
            deleted_.pop_back();
            return Id(idx, n.key_rev);
        }
    }

    T& operator[](const Id& key)
    {
        Node& n = v_[key.idx];
        if (n.key_rev != key.rev)
            throw;

        return n.value;
    }

    const T& operator[](const Id& key) const
    {
        const Node& n = v_[key.idx];
        if (n.key_rev != key.rev)
            throw;

        return n.value;
    }

    T* get(const Id& key)
    {
        Node& n = v_[key.idx];
        if (n.key_rev != key.rev)
            return nullptr;

        return &n.value;
    }

    const T* get(const Id& key) const
    {
        Node& n = v_[key.idx];
        if (n.key_rev != key.rev)
            return nullptr;

        return &n.value;
    }

    void remove(const Id& key)
    {
        Node& n = v_[key.idx];
        if (n.key_rev != key.rev)
            return;

        n.value = T();
        n.deleted = true;

        deleted_.push_back(key.idx);
        ++n.key_rev;
    }

    class NodeIterator : public std::iterator<std::forward_iterator_tag, T>
    {

    public:

        NodeIterator(const std::vector<Node>& v) : it_(v.begin()), it_end_(v.end())
        {
            // Skip possible zero-entities (deleted entities) at the beginning
            while(it_ != it_end_ && it_->deleted)
                ++it_;
        }

        NodeIterator(const NodeIterator& it) : it_(it.it_), it_end_(it.it_end_) {}

        NodeIterator(const typename std::vector<Node>::const_iterator& it) : it_(it) {}

        NodeIterator& operator++()
        {
            // Increase iterator and skip possible zero-entities (deleted entities)
            do { ++it_; if (it_ == it_end_) break; } while (it_->deleted);
            return *this;
        }

        NodeIterator operator++(int) { NodeIterator tmp(*this); operator++(); return tmp; }

        bool operator==(const NodeIterator& rhs) { return it_ == rhs.it_; }

        bool operator!=(const NodeIterator& rhs) { return it_ != rhs.it_; }

        const T& operator*() { return it_->value; }

    private:

        typename std::vector<Node>::const_iterator it_;
        typename std::vector<Node>::const_iterator it_end_;

    };

    typedef NodeIterator const_iterator;

    inline const_iterator begin() const { return const_iterator(v_); }

    inline const_iterator end() const { return const_iterator(v_.end()); }

private:

    std::vector<Node> v_;

    std::vector<uint64_t> deleted_;


};

} // end namespace era

#endif
