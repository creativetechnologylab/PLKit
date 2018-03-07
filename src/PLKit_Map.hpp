#pragma once

#include "PLKit_List.hpp"

#define NUM_BINS 20

template <typename T>
struct MapNode {
    typedef T type;
    MapNode(T v): next(NULL), value(v) {}
    MapNode *next;
    T value;
    char key[20];
};

template <typename ValueT>
class StringMap {
public:
  typedef MapNode<ValueT> NodeType;
  typedef List<NodeType> ListType;

public:
  StringMap() {}

  void insert(const char *key, ValueT value) {
    ListType &list = getList(key);
    MapNode<ValueT> *slot = list.insert(value);
    strcpy(slot->key, key);
  }

  bool safeGet(const char *key, ValueT *value) {
    ListType &list = getList(key);
    NodeType *node = list.getStart();
    while (node != NULL) {
      if (strcmp(key, node->key) == 0) {
        *value = node->value;
        return true;
      }
      node = node->next;
    }
    return false;
  }

  ValueT get(const char *key) {
    ListType &list = getList(key);
    NodeType *node = list.getStart();
    while (node != NULL) {
      if (strcmp(key, node->key) == 0) {
        return node->value;
      }
      node = node->next;
    }
    return ValueT();
  }

protected:
  ListType& getList(const char *key) {
    return mLists[key[0] % NUM_BINS];
  }

protected:
  List<MapNode<ValueT>> mLists[NUM_BINS];
};

typedef StringMap<int> StringIntMap;
typedef StringMap<char> StringCharMap;
typedef StringMap<float> StringFloatMap;
