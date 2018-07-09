// 探索する。SATソルバは30秒で解が見つからなければ解なしとして扱う。
#include <bits/stdc++.h>

using namespace std;

vector<string> exec(string cmd) {
  system((cmd + string(" >/tmp/tmp.txt")).c_str());
  string line;
  vector<string> lines;
  std::ifstream ifs("/tmp/tmp.txt");
  while (getline(ifs, line)) {
    lines.push_back(line);
  }
  return lines;
}

int typeCode(string s) {
         if (s == "M") { return 0; } 
    else if (s == "F") { return 1; }
    else               { return 2; }
}

const int D = 4;
struct Group {
  int id;
  int n[D];
  int type;
  int rank;
};
vector<Group> g, groupById;
Group findGroupById(int id) {
  return groupById[id];
}

struct Room {
  int id;
  int rank;
  int cap;
};
vector<Room> r, roomById;
Room findRoomById(int id) {
  return roomById[id];
}

void input() {
  g.clear();
  r.clear();
  int G; cin >> G;
  for (int i = 0; i < G; i++) {
    Group group;
    group.id = i;
    cin >> group.n[0];
    cin >> group.n[1];
    cin >> group.n[2];
    cin >> group.n[3];
    string s; cin >> s;
    group.type = typeCode(s);
    cin >> group.rank;
    g.push_back(group);
  }

  int R; cin >> R;
  for (int i = 0; i < R; i++) {
    Room room;
    room.id = i;
    cin >> room.rank;
    cin >> room.cap;
    r.push_back(room);
  }

  // ランクが強い順にソート。ランクが等しい場合は id 順。
  sort(g.begin(), g.end(), [](const Group& a, const Group& b) -> bool {
    if (a.rank != b.rank) return a.rank > b.rank;
    return a.id < b.id;
  });
  groupById.resize(g.size());
  for (const Group group : g) {
    groupById[group.id] = group;
  }

  // ランクが強い順にソート。ランクが等しい場合は id 順。
  sort(r.begin(), r.end(), [](const Room& a, const Room& b) -> bool {
    if (a.rank != b.rank) return a.rank > b.rank;
    return a.id < b.id;
  });
  roomById.resize(r.size());
  for (const Room room : r) {
    roomById[room.id] = room;
  }
}


vector<vector<int>> getRoom2Groups(const vector<int> group2room) {
  vector<vector<int>> room2groups(r.size());
  for (int gid = 0; gid < group2room.size(); gid++) {
    const int rid = group2room[gid];
    if (rid == -1) continue;
    const Room room = findRoomById(rid);
    const Group group = findGroupById(gid);
    room2groups[room.id].push_back(group.id);
  }
  return room2groups;
}
vector<int> getGroop2room(const vector<vector<int>> room2groups) {
  vector<int> group2room(g.size());
  for (int rid = 0; rid < room2groups.size(); rid++) {
    for (int gid : room2groups[rid]) {
      group2room[gid] = rid;
    }
  }
  return group2room;
}


bool validate(const vector<vector<int>> room2groups) {
  bool ok = true;
  vector<int> appear(g.size());

  for (const Room room : r) {
    int q[D][3] = {};
    for (int i = 0; i < room2groups[ room.id ].size(); i++) {
      const Group group = findGroupById( room2groups[room.id][i] );
      for (int d = 0; d < D; d++) {
        q[d][group.type] += group.n[d];
      }
      appear[ group.id ]++;
    }
    for (int d = 0; d < D; d++) {
      for (int t = 0; t < 3; t++) {
        if (q[d][t] > room.cap) {
          cout << "violation!" << " room" << room.id << " overcapacity." << endl;
          ok = false;
        }
      }
      for (int t1 = 0; t1 < 3; t1++) {
        for (int t2 = t1 + 1; t2 < 3; t2++) {
          if (q[d][t1] > 0 && q[d][t2] > 0) {
            cout << "violation!" << " room" << room.id << " mixed." << endl;
            ok = false;
          }
        }
      }
    }
  }

  for (const Group group : g) {
    int i = group.id;
    if (appear[i] != 1) {
      cout << "violation!" << " group" << i << " assigned " << appear[i] << " times." << endl;
      ok = false;
    }
  }

  if (ok) {
    cout << "OK!" << endl;
  }
  return ok;
}

void output(const vector<vector<int>> room2groups) {
  for (const Room room : r) {
    cout << "room" << room.id << "(capacity:" << room.cap << ",rank:" << room.rank << "):";
    for (int i = 0; i < room2groups[ room.id ].size(); i++) {
      const Group group = findGroupById(room2groups[ room.id ][i]);
      cout << group.id << "(rank:"<< group.rank << ")" << ",";
    }
    cout << endl;

    int q[D][3] = {};
    for (int i = 0; i < room2groups[ room.id ].size(); i++) {
      const Group group = findGroupById(room2groups[room.id][i]);
      for (int d = 0; d < D; d++) {
        q[d][group.type] += group.n[d];
      }
    }
    
    for (int d = 0; d < D; d++) {
      cout << "day" << d << ":";
      for (int t = 0; t < 3; t++) cout << q[d][t] << ",";
      cout << endl;
    }
  }
  return;
}



vector<vector<int>> call_sugar(const vector<int> group2room) {

  FILE *csp = fopen("room_assign.csp", "w");

  fprintf(csp, "; kita_room_assign\n");
  for (const Group group : g) {
    for (const Room room : r) {
      fprintf(csp, "(int x_%d_%d 0 1)\n", group.id, room.id);
    }
  }

  // 一部屋に割り当て
  for (const Group group : g) {
    fprintf(csp, "(eq 1 (+");
    for (const Room room : r) {
      fprintf(csp, " x_%d_%d", group.id, room.id);
    }
    fprintf(csp, ") )\n");
  }

  // 男女
  for (int i1 = 0; i1 < g.size(); i1++) {
    for (int i2 = i1 + 1; i2 < g.size(); i2++) {
      const Group g1 = g[i1];
      const Group g2 = g[i2];
      if (g1.type == 0 && g2.type == 0) continue;
      if (g1.type == 1 && g2.type == 1) continue;
      bool can_same_room = true;
      for (int d = 0; d < D; d++) {
        if (g1.n[d] > 0 && g2.n[d] > 0) 
          can_same_room = false;
      }
      if (!can_same_room) {
        for (const Room room : r) {
          fprintf(csp, "(or (eq 0 x_%d_%d) (eq 0 x_%d_%d))\n", g1.id, room.id, g2.id, room.id);
        }
      }
    }
  }

  // 人数
  for (const Room room : r) {
    for (int d = 0; d < D; d++) {
        fprintf(csp, "(weightedsum (");
        for (const Group group : g) {
          fprintf(csp, "(%d x_%d_%d) ", group.n[d], group.id, room.id);
        }
        fprintf(csp, ") le %d)\n", room.cap);
    }
  }

  // 既に決め打ちした部屋割り当てを追加
  for (int gid = 0; gid < group2room.size(); gid++) {
    const int rid = group2room[gid];
    if (rid == -1) continue;
    fprintf(csp, "(eq 1 x_%d_%d)\n", gid, rid);
  }

  fprintf(csp, "; END\n");
  fflush(csp);
  fclose(csp);

  vector<string> res = exec(string("timeout 30 /cygdrive/c/project/kita_room_assign/sugar-v2-3-3/bin/sugar -v room_assign.csp"));
  // if (res[0].find("s SATISFIABLE") == string::npos) {
    // cout << res[0] << endl;
    // return 0;
  // }

  // sugarの出力をパース
  vector<vector<int>> room2groups(r.size());

  for (int i = 1; i < res.size(); i++) {
    string a, b, c;
    stringstream ss(res[i]);
    ss >> a >> b >> c;
    if (c == "1") {
      replace(b.begin(), b.end(), '_', ' ');
      stringstream ss2(b);
      string dummy;
      int gi, rj;
      ss2 >> dummy >> gi >> rj;
      // cout << "group" << gi << "  ->  room" << rj << endl;
      room2groups[rj].push_back(gi);
    }
  }

  // 出力
  output(room2groups);

  // バリデーション
  bool validation_ok = validate(room2groups);

  if (validation_ok) 
    return room2groups;
  else
    return vector<vector<int>>();
}

vector<int> best_group2room;
deque<vector<int>> cache;

bool matches(const vector<int> group2room, const vector<int> known) {
  for (int i = 0; i < group2room.size(); i++) if (group2room[i] != -1) {
    if (known[i] != group2room[i])
      return false;
  }
  return true;
}

bool isPossible(const vector<int> group2room) {

  cout << count_if(group2room.begin(), group2room.end(), [](int x){ return x != -1; }) 
      << "/" << g.size() << endl;
  
  // まず明らかにダメかどうかをチェック
  auto room2groups = getRoom2Groups(group2room);

  int total_cap[D] = {}; // 日ごとの部屋の残り数
  int total_cap_type[D][3] = {}; // 男女を考慮した部屋ごとの残り数

  for (const Room room : r) {
    int q[D][3] = {};
    for (int i = 0; i < room2groups[room.id].size(); i++) {
      const Group group = findGroupById( room2groups[room.id][i] );
      for (int d = 0; d < D; d++) {
        q[d][group.type] += group.n[d];
      }
    }
    for (int d = 0; d < D; d++) {
      for (int t = 0; t < 3; t++) {
        if (q[d][t] > room.cap) {
          // cout << "violation!" << " room" << room.id << " overcapacity." << endl;
          // ok = false;
          return false;
        }
      }
      for (int t1 = 0; t1 < 3; t1++) {
        for (int t2 = t1 + 1; t2 < 3; t2++) {
          if (q[d][t1] > 0 && q[d][t2] > 0) {
            // cout << "violation!" << " room" << room.id << " mixed." << endl;
            // ok = false;
            return false;
          }
        }
      }

      int reserved = 0;
      for (int t = 0; t < 3; t++) {
        reserved += q[d][t];
      }
      total_cap[d] += room.cap - reserved;

      if (reserved == 0) {
        // まだ誰も入っていれば、全タイプが入れる
        for (int t = 0; t < 3; t++) {
          total_cap_type[d][t] += room.cap;
        }
      } else {
        // すでに誰かが入っていれば、以後そのタイプしか入れない。type==2 の場合は誰も入れない。
        for (int t = 0; t < 3; t++) {
          if (q[d][t] > 0 && t != 2) {
            total_cap_type[d][t] += room.cap - reserved;
          }
        }
      }
    }
  }

  // 枝刈り。
  for (int gid = 0; gid < group2room.size(); gid++) {
    if (group2room[gid] == -1) {
      const Group group = findGroupById(gid);
      for (int d = 0; d < D; d++) {
        total_cap[d] -= group.n[d];
        if (total_cap[d] < 0) return false;
      }
    }
  }

  // 他にも枝刈り。
  for (int gid = 0; gid < group2room.size(); gid++) {
    if (group2room[gid] == -1) {
      const Group group = findGroupById(gid);
      for (int d = 0; d < D; d++) {
        total_cap_type[d][group.type] -= group.n[d];
        if (total_cap_type[d][group.type] < 0) return false;
      }
    }
  }

  // 今調べようとしている列をprefixに持つ解が既に見つかっているなら真
  for (auto v : cache) {
    if (matches(group2room, v))
      return true;
  }

  // もっと枝刈り考えろ。


  // 自明な枝刈りをパスしたらSATで検証
  vector< vector<int> > res = call_sugar(group2room);
  if (res.size() == 0) return false;

  cache.push_front(getGroop2room(res));
  if (cache.size() > 150) cache.pop_back();

  return true;
}

bool dfs(int depth, vector<int>& group2room) {

  // 達成不可能なら戻る
  if (!isPossible(group2room)) {
    return false;
  }

  // 到達したら解を保存して終わり
  if (depth == g.size()) {
    best_group2room = group2room;
    return true;
  }
  
  // 先頭から決定していき達成可能かチェック、を繰り返す
  for (const Room room : r) {
    group2room[ g[depth].id ] = room.id;
    bool done = dfs(depth + 1, group2room);
    if (done) return true;
    group2room[ g[depth].id ] = -1;
  }
  return false;
}


void check(const vector<int> group2room) {
  auto g2r = getRoom2Groups(group2room);
  output(g2r);
  validate(g2r);
  for (const Group group : g) {
    const Room room = findRoomById( group2room[group.id] );
    cout << "group" << group.id << "(rank:"<< group.rank << ") = room" << room.id << "(rank:" << room.rank << ")" << endl;
  }
}

int main() {
  input();

  // vector<int> g2r({1,0,1,1,3,7,6,6,0,4,0,5,3,7,0,0,4,22,26,8,7,17,18,21,22,14,17,18,21,10,26,23,20,2,11,15,16,8,8,12,35,31,29,28,37,7,35,31,0,40,42,43,49,46,25,19,5,10,10,14,14,13,13,33,30,27,23,14,36,41,24,5,29,28,37,5,28,37,36,40,42,43,49,1,1,46,25,19,8,8,13,13,35,33,31,30,27,1,25,9,12,12,38,39,48,47,50,45,44,24,19,2,2,9,34,32});
  // check(g2r);
  // return 0;

  // call_sugar({});
  // return 0;

  best_group2room.clear();
  cache.clear();
  vector<int> group2room(g.size(), -1);
  dfs(0, group2room);

  if (best_group2room.size() == 0) {
    cout << "NO SOLUTION." << endl;
  } else {
    check(best_group2room);
  }

  return 0;
}