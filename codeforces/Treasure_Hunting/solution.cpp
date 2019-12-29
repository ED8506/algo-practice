#include <bits/stdc++.h>

using namespace std;

using Col = int;
using ColRange = pair<Col, Col>;
using OptColRange = optional<pair<Col, Col>>;
using Row = int;
using Steps = long long;

enum class LevelAction {
    COLLECT_LEFTY_EXIT_LEFTY,
    COLLECT_LEFTY_EXIT_RIGHTY,
    COLLECT_RIGHTY_EXIT_LEFTY,
    COLLECT_RIGHTY_EXIT_RIGHTY
};

struct Island final {
    int cols;
    set<Col> exit_cols;
    vector<OptColRange> treasure_cols_by_row;
    int treasures;
};

struct IslandReduced final {
    Steps baseline;
    int cols;
    Col start;
    set<Col> exit_cols;
    vector<ColRange> treasure_cols_by_row;
    int treasures;
};

Steps compute(const Island &isl) { return 0; }

Island read_input() {
    int n; // Rows
    int m; // Columns
    int k; // Treasures
    int q; // Safe columns
    cin >> n >> m >> k >> q;

    vector<set<Col>> treasure_column_sets_by_row(n);
    for (int i = 0; i != k; ++i) {
        int r1;
        int c1;
        cin >> r1 >> c1;
        treasure_column_sets_by_row[r1 - 1].insert(c1 - 1);
    }

    vector<OptColRange> treasure_cols_by_row(n);
    transform(treasure_column_sets_by_row.cbegin(),
              treasure_column_sets_by_row.cend(), treasure_cols_by_row.begin(),
              [](const set<Col> &cols_set) -> OptColRange {
                  if (cols_set.empty()) return nullopt;
                  return ColRange{*cols_set.cbegin(), *cols_set.crbegin()};
              });

    set<Col> exit_columns;
    for (int i = 0; i != q; ++i) {
        int c1;
        cin >> c1;
        exit_columns.insert(c1 - 1);
    }

    return {m, exit_columns, treasure_cols_by_row, k};
}

int main() { cout << compute(read_input()) << '\n'; }
