#ifndef __RANK_CACHE_H_
#define __RANK_CACHE_H_
#include <map>
#include "udt.h"
#include "macros.h"

NMS_BEGIN(kcommon)

template<typename Id> struct LessRankCacheKey {
	Id _id;
	int64 _data[3];

	typedef Id IdType;
	typedef std::less<LessRankCacheKey<Id> > CompareType;

	LessRankCacheKey() { _data[0] = _data[1] = _data[2] = 0ull; }
	LessRankCacheKey(Id id) : _id(id) { _data[0] = _data[1] = _data[2] = 0ull; }
	LessRankCacheKey(Id id, int64 d1) : _id(id) { _data[0] = d1; _data[1] = _data[2] = 0ull; }
	LessRankCacheKey(Id id, int64 d1, int64 d2) : _id(id) { _data[0] = d1; _data[1] = d2; _data[2] = 0ull; }
	LessRankCacheKey(Id id, int64 d1, int64 d2, int64 d3) : _id(id) { _data[0] = d1; _data[1] = d2; _data[2] = d3; }

	bool operator < (const LessRankCacheKey<Id> &other) const {
		if (_data[0] != other._data[0]) return _data[0] < other._data[0];
		if (_data[1] != other._data[1]) return _data[1] < other._data[1];
		if (_data[2] != other._data[2]) return _data[2] < other._data[2];
		return _id < other._id;
	}
};

template<typename Id> struct GreaterRankCacheKey {
	Id _id;
	int64 _data[3];

	typedef Id IdType;
	typedef std::greater<GreaterRankCacheKey<Id> > CompareType;

	GreaterRankCacheKey() { _data[0] = _data[1] = _data[2] = 0ull; }
	GreaterRankCacheKey(Id id) : _id(id) { _data[0] = _data[1] = _data[2] = 0ull; }
	GreaterRankCacheKey(Id id, int64 d1) : _id(id) { _data[0] = d1; _data[1] = _data[2] = 0ull; }
	GreaterRankCacheKey(Id id, int64 d1, int64 d2) : _id(id) { _data[0] = d1; _data[1] = d2; _data[2] = 0ull; }
	GreaterRankCacheKey(Id id, int64 d1, int64 d2, int64 d3) : _id(id) { _data[0] = d1; _data[1] = d2; _data[2] = d3; }

	bool operator > (const GreaterRankCacheKey<Id> &other) const {
		if (_data[0] != other._data[0]) return _data[0] > other._data[0];
		if (_data[1] != other._data[1]) return _data[1] > other._data[1];
		if (_data[2] != other._data[2]) return _data[2] > other._data[2];
		return _id > other._id;
	}
};

template<typename V, typename K> struct RankCacheTraits {
	typedef typename K::IdType IdType;
	typedef std::map<K, V, typename K::CompareType> DataMapType;
	typedef typename DataMapType::iterator DataMapIteratorType;
	typedef std::map<IdType, DataMapIteratorType> IdxMapType;
	typedef typename IdxMapType::iterator IdxMapIteratorType;
};

template<typename V, typename K> struct RankCacheTraits<V *, K> {
	typedef typename K::IdType IdType;
	typedef std::map<K, V *, typename K::CompareType> DataMapType;
	typedef typename DataMapType::iterator DataMapIteratorType;
	typedef std::map<IdType, DataMapIteratorType> IdxMapType;
	typedef typename IdxMapType::iterator IdxMapIteratorType;
};

template<typename K, typename V, typename TR = RankCacheTraits<V, K>, uint16 N = 100>
class RankCache {
public:
	typedef typename RankCacheTraits<V, K>::DataMapIteratorType iterator;

public:
	void update(const K &k, const V &v) {
		typename TR::IdxMapIteratorType iti = _idx.find(k._id);
		if (iti != _idx.end()) {
			_data.erase(iti->second);
			_idx.erase(iti);
		}

		std::pair<typename TR::DataMapIteratorType, bool> result = _data.insert(std::pair<K, V>(k, v));
		if (!result.second) return ;

		_idx[k._id] = result.first;

		if (_data.size() > N) {
			typename TR::DataMapIteratorType it = --_data.end();
			_idx.erase(it->first._id);
			_data.erase(it);
		}
	}

	void remove(const typename K::IdType &id) {
		typename TR::IdxMapIteratorType iti = _idx.find(id);
		if (iti == _idx.end()) return ;
		_data.erase(iti->second);
		_idx.erase(iti);
	}

	void clear() {
		_idx.clear();
		_data.clear();
	}

	iterator begin() { return _data.begin(); }
	iterator end() { return _data.end(); }

private:
	typename TR::IdxMapType _idx;
	typename TR::DataMapType _data;
};

NMS_END // end namespace kcommon

#endif // __RANK_CACHE_H_

