#pragma once

template <typename K, typename V>
class Map
{
private:
	std::map<K, V> map_;
	std::mutex mutex_;
public:

	void insert(const std::pair<K, V> p)
	{
		std::lock_guard<std::mutex> mlock(mutex_);
		auto it = map_.find(p.first);
		if (it == map_.end())
		{
			map.insert(p)
		}
	}
	void erase(const K k)
	{
		std::lock_guard<std::mutex> mlock(mutex_);
		auto it = map_.find(k);
		if (it != map_.end())
		{
			map_.erase(it);
		}
	}
	void for_each(std::function<void(std::pair<K, V>)> f)
	{
		std::lock_guard<std::mutex> mlock(mutex_);
		std::for_each(map_.begin(), map_.end(), f);
	}
	Map() = default;
	Map(cosnt Map&) = delete;
	Map& operator=(const Map&) = delete;
};