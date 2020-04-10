#include <bits/stdc++.h>

using namespace std;

template <typename T> ostream &operator<<(ostream &os, const vector<T> &xs) {
    os << '[';

    for (auto i = xs.cbegin(); i != xs.cend(); ++i) {
        if (i != xs.cbegin()) os << ' ';
        os << *i;
    }

    os << ']';
    return os;
}

struct Tower final {
    int left;
    int right;
    int height;
    
    Tower(const vector<int> &src): left{src[0]}, right{src[1]}, height{src[2]} {}
};

map<int, vector<Tower>> gather_key_xs(const vector<vector<int>> &towers) {
    map<int, vector<Tower>> ans;
    
    for (const auto &t : towers) {
        const int left = t[0];
        const int right = t[1];
            
        ans.emplace(left, vector<Tower>{});
        ans.emplace(right, vector<Tower>{});
    }
    
    return ans;
}

struct TowersLess final {
    bool operator()(const Tower &lhs, const Tower &rhs) const {
        return lhs.height < rhs.height;
    }
};

void gather_tower_heaps_by_x(const vector<vector<int>> &towers, map<int, vector<Tower>> &key_xs) {
    for (const auto &tsrc : towers) {
        const Tower t(tsrc);
        
        for (auto it = key_xs.lower_bound(t.left); it != key_xs.end() && it->first <= t.right; ++it) {
            it->second.push_back(t);
            push_heap(it->second.begin(), it->second.end(), TowersLess{});
        }
    }
}

Tower pop(vector<Tower> &heap) {
    pop_heap(heap.begin(), heap.end(), TowersLess{});
    Tower ans = heap.back();
    heap.pop_back();
    return ans;
}

void supplement(vector<vector<int>> &skyline, const int x, const int h) {
    if (skyline.empty() && !h) return;
    if (skyline.size() && h == skyline.back()[1]) return;
    skyline.push_back({x, h});
}

struct Solution final {
    vector<vector<int>> getSkyline(const vector<vector<int>> &towers) const {
        auto tower_heaps_by_x = gather_key_xs(towers);
        gather_tower_heaps_by_x(towers, tower_heaps_by_x);
        
        vector<vector<int>> ans;
        
        for (auto [x, heap] : tower_heaps_by_x) {
            const Tower dom = pop(heap);            
            
            if (x != dom.left && x != dom.right) {
                supplement(ans, x, dom.height);
            } else if (x == dom.left) {
                optional<Tower> sub;
                
                while (heap.size()) {
                    sub = pop(heap);
                    
                    if (sub->left != x) break;
                    else sub = nullopt;
                }
                
                if (sub) supplement(ans, x, sub->height);    
                else supplement(ans, x, 0);
                
                supplement(ans, x, dom.height);
            } else {
                assert(x == dom.right);
                supplement(ans, x, dom.height);
                
                optional<Tower> sub;
                
                while (heap.size()) {
                    sub = pop(heap);
                    
                    if (sub->right != x) break;
                    else sub = nullopt;
                }
                
                if (sub) supplement(ans, x, sub->height);    
                else supplement(ans, x, 0);
            }
        }
        
        return ans;
    }
};

vector<vector<int>> pyramid(const int sz) {
    vector<vector<int>> ans;
    
    for (int i = 0; i != sz / 2; ++i) {
        ans.push_back({i, sz - i - 1, i + 1});
    }
    
    return ans;
}

struct TestCase final {
    const vector<vector<int>> input;
    const vector<vector<int>> expected_answer;
};

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    
    const vector<TestCase> testCases{
        {
            {{2, 9, 10}, {3, 7, 15}, {5, 12, 12}, {15, 20, 10}, {19, 24, 8}},
            {{2, 10}, {3, 15}, {7, 12}, {12, 0}, {15, 10}, {20, 8}, {24, 0}}
        },
        {
            {{1,2,1},{1,2,2},{1,2,3}},
            {{1,3},{2,0}}
        },
        {
            {{0,5,7},{5,10,7},{5,10,12},{10,15,7},{15,20,7},{15,20,12},{20,25,7}},
            {{0,7},{5,12},{10,7},{15,12},{20,7},{25,0}}
        },
        {
            {{2,7,1},{3,6,2},{4,5,3}},
            {{2,1},{3,2},{4,3},{5,2},{6,1},{7,0}}
        }
    };
    
    for (const auto [input, expected] : testCases) {
        const auto actual = Solution{}.getSkyline(input);
        cout << "ACT " << actual << "\nEXP " << expected << '\n' << endl;
        assert(actual == expected);
    }
    
    const auto t0 = chrono::steady_clock::now();
    Solution{}.getSkyline(pyramid(4000));    
    const auto t1 = chrono::steady_clock::now();
    
    const auto elapsed_ms = chrono::duration_cast<std::chrono::milliseconds> (t1 - t0).count();
    cout << elapsed_ms << " ms\n";
    
    return 0;
}
