// Original author: Alexander Ponomarev
// Original file: lrucache.hpp

#ifndef _LRUCACHE_HPP_INCLUDED_
#define _LRUCACHE_HPP_INCLUDED_

template <typename KeyType, typename ValueType>
class LRUCache
{
public:
  typedef typename std::pair<KeyType, ValueType> KeyValuePairType;
  typedef typename std::list<KeyValuePairType>::iterator ListIteratorType;

  // maybe we need constructor with callback lambda?
  LRUCache(size_t max_size)
    : max_size(max_size)
  {
  }

  void Put(const KeyType& key, const ValueType& value)
  {
    auto it = items_map.find(key);
    items_list.push_front(KeyValuePairType(key, value));
    if (it != items_map.end()) {
      items_list.erase(it->second);
      items_map.erase(it);
    }
    items_map[key] = items_list.begin();

    if (items_map.size() > max_size) {
      auto last = items_list.end();
      last--;
      if (erase_callback) {
        this->erase_callback(last->first, last->second);
      }
      items_map.erase(last->first);
      items_list.pop_back();
    }
  }

  // TODO: rework to optional
  ValueType* Get(const KeyType& key)
  {
    auto it = items_map.find(key);
    if (it == items_map.end()) {
      return nullptr;
    } else {
      items_list.splice(items_list.begin(), items_list, it->second);
      return &(it->second->second);
    }
  }

  void SetEraseCallback(std::function<void(const KeyType& key, const ValueType& value)> callback)
  {
    this->erase_callback = callback;
  }

  bool Exists(const KeyType& key) const { return items_map.find(key) != items_map.end(); }

  size_t Size() const { return items_map.size(); }

private:
  std::list<KeyValuePairType> items_list;
  std::unordered_map<KeyType, ListIteratorType> items_map;
  size_t max_size;
  std::function<void(const KeyType& key, const ValueType& value)> erase_callback;
};

#endif /* _LRUCACHE_HPP_INCLUDED_ */