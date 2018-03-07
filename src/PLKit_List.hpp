#pragma once

template <typename T>
struct Node {
  typedef T type;
  Node(T v): next(NULL), value(v) {}
  Node *next;
  T value;
};

template <typename NodeT>
class List {
public:
  List(): mStart(NULL), mEnd(NULL) {}

  NodeT* insert(typename NodeT::type v) {
    NodeT *node = new NodeT(v);
    if (mStart == NULL) {
      mStart = node;
    } else {
      mEnd->next = node;
    }
    mEnd = node;
    return mEnd;
  }

  NodeT* getStart() const {
    return mStart;
  }

protected:
  NodeT *mStart, *mEnd;
};
