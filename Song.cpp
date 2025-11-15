#include "Song.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iostream>

using namespace std;

// 在匿名命名空间中实现辅助函数
namespace {

string trim_copy(const string &s) {
    auto first = find_if_not(s.begin(), s.end(),
                             [](const unsigned char ch){ return isspace(ch); });
    auto last  = find_if_not(s.rbegin(), s.rend(),
                             [](const unsigned char ch){ return isspace(ch); }).base();
    if (first >= last) return {};
    return {first, last};
}

string to_lower_copy(const string &s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(),
              [](unsigned char c){ return tolower(c); });
    return result;
}

string join_tags(const vector<string> &tags) {
    if (tags.empty()) return "";
    
    ostringstream oss;
    for (size_t i = 0; i < tags.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << tags[i];
    }
    return oss.str();
}

} // 匿名命名空间结束

// 定义静态成员变量
int Song::next_id_ = 1;

// 实现构造函数
Song::Song(const string &title, const string &artist, 
           int duration_sec, int rating) {
    // 先清理输入数据
    string clean_title = trim_copy(title);
    string clean_artist = trim_copy(artist);
    
    // 验证数据
    if (clean_title.empty()) {
        cout << "[错误] 标题不能为空\n";
        return;
    }
    if (clean_artist.empty()) {
        cout << "[错误] 艺人不能为空\n";
        return;
    }
    if (duration_sec <= 0) {
        cout << "[错误] 时长必须为正整数（秒）\n";
        return;
    }
    if (rating < 1 || rating > 5) {
        cout << "[错误] 评分必须在 1...5 之间\n";
        return;
    }
    
    // 所有验证通过，设置成员变量
    id_ = next_id_++;
    title_ = clean_title;
    artist_ = clean_artist;
    duration_sec_ = duration_sec;
    rating_ = rating;
    valid_ = true;
}

// 实现 set_title 函数
bool Song::set_title(const string &t) {
    string clean_title = trim_copy(t);
    if (clean_title.empty()) {
        cout << "[提示] 标题不能为空，已忽略本次修改\n";
        return false;
    }
    title_ = clean_title;
    return true;
}

// 实现 set_artist 函数
bool Song::set_artist(const string &a) {
    string clean_artist = trim_copy(a);
    if (clean_artist.empty()) {
        cout << "[提示] 艺人不能为空，已忽略本次修改\n";
        return false;
    }
    artist_ = clean_artist;
    return true;
}

// 实现 set_duration 函数
bool Song::set_duration(int sec) {
    if (sec <= 0) {
        cout << "[提示] 时长需为正整数，已忽略本次修改\n";
        return false;
    }
    duration_sec_ = sec;
    return true;
}

// 实现 set_rating 函数
bool Song::set_rating(int r) {
    if (r < 1 || r > 5) {
        cout << "[提示] 评分需在 1..5，已忽略本次修改\n";
        return false;
    }
    rating_ = r;
    return true;
}

// 实现 add_tag 函数
bool Song::add_tag(const string &tag) {
    string clean_tag = trim_copy(tag);
    if (clean_tag.empty()) {
        cout << "[提示] 空标签已忽略\n";
        return false;
    }
    
    // 检查标签是否已存在（忽略大小写）
    string lower_tag = to_lower_copy(clean_tag);
    for (const auto &existing_tag : tags_) {
        if (to_lower_copy(existing_tag) == lower_tag) {
            cout << "[提示] 标签已存在（忽略大小写）\n";
            return false;
        }
    }
    
    tags_.push_back(clean_tag);
    return true;
}

// 实现 remove_tag 函数
bool Song::remove_tag(const string &tag) {
    string clean_tag = trim_copy(tag);
    if (clean_tag.empty()) {
        cout << "[提示] 空输入忽略。\n";
        return false;
    }
    
    string lower_tag = to_lower_copy(clean_tag);
    for (size_t i = 0; i < tags_.size(); ++i) {
        if (to_lower_copy(tags_[i]) == lower_tag) {
            tags_.erase(tags_.begin() + i);
            return true;
        }
    }
    
    cout << "[提示] 未找到该标签\n";
    return false;
}

// 实现 matches_keyword 函数
bool Song::matches_keyword(const string &kw) const {
    string clean_kw = trim_copy(kw);
    if (clean_kw.empty()) {
        return false;
    }
    
    string lower_kw = to_lower_copy(clean_kw);
    string lower_title = to_lower_copy(title_);
    string lower_artist = to_lower_copy(artist_);
    
    // 检查标题和艺人
    if (lower_title.find(lower_kw) != string::npos ||
        lower_artist.find(lower_kw) != string::npos) {
        return true;
    }
    
    // 检查标签
    for (const auto &tag : tags_) {
        if (to_lower_copy(tag).find(lower_kw) != string::npos) {
            return true;
        }
    }
    
    return false;
}

// 实现输出操作符 <<
ostream &operator<<(ostream &os, const Song &s) {
    os << "[#" << s.id_ << "] " << s.artist_ << " - " << s.title_ 
       << " (" << s.duration_sec_ << "s) " << string(s.rating_, '*');
    
    if (!s.tags_.empty()) {
        os << "  [tags: " << join_tags(s.tags_) << "]";
    }
    
    return os;
}

// 实现小于操作符 <（用于排序）
bool operator<(const Song &a, const Song &b) {
    // 1. 比较评分（降序）
    if (a.rating_ > b.rating_) return true;
    if (a.rating_ < b.rating_) return false;
    
    // 2. 评分相等，比较标题（升序）
    if (a.title_ < b.title_) return true;
    if (a.title_ > b.title_) return false;
    
    // 3. 标题相等，比较 ID（升序）
    return a.id_ < b.id_;
}