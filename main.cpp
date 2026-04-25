#include <bits/stdc++.h>
using namespace std;

// Note: Per requirements, avoid STL containers except std::string.
// We only use basic arrays and minimal helpers; no <algorithm> usage.

struct User {
    string username;
    string password;
    string name;
    string mail;
    int privilege = 0;
    bool online = false;
};

struct UsersDB {
    User *data = nullptr;
    int sz = 0;
    int cap = 0;

    ~UsersDB() { delete [] data; }

    void clear() {
        delete [] data; data = nullptr; sz = 0; cap = 0;
    }

    void reserve(int ncap) {
        if (ncap <= cap) return;
        if (ncap < 4) ncap = 4;
        User *nd = new User[ncap];
        for (int i = 0; i < sz; ++i) nd[i] = data[i];
        delete [] data; data = nd; cap = ncap;
    }

    int findIndex(const string &u) const {
        for (int i = 0; i < sz; ++i) {
            const string &x = data[i].username;
            if (x.size() == u.size()) {
                bool eq = true;
                for (size_t k = 0; k < u.size(); ++k) if (x[k] != u[k]) { eq = false; break; }
                if (eq) return i;
            }
        }
        return -1;
    }

    User* find(const string &u) {
        int idx = findIndex(u);
        if (idx < 0) return nullptr;
        return &data[idx];
    }

    bool exists(const string &u) const { return findIndex(u) != -1; }

    bool add(const User &u) {
        if (exists(u.username)) return false;
        if (sz == cap) reserve(cap ? cap * 2 : 4);
        data[sz++] = u;
        return true;
    }
};

static UsersDB db;

static bool parse_kv(const vector<string> &tokens, char key, string &out) {
    for (size_t i = 1; i + 1 < tokens.size(); ++i) {
        const string &t = tokens[i];
        if (t.size() == 2 && t[0] == '-' && t[1] == key) {
            out = tokens[i+1];
            return true;
        }
    }
    return false;
}

static bool parse_kv_int(const vector<string> &tokens, char key, int &out) {
    string s; if (!parse_kv(tokens, key, s)) return false;
    // parse integer (non-negative expected)
    long long v = 0; bool neg = false; size_t i = 0;
    if (!s.empty() && s[0] == '-') { neg = true; i = 1; }
    for (; i < s.size(); ++i) {
        char c = s[i]; if (c < '0' || c > '9') { return false; }
        v = v * 10 + (c - '0');
        if (v > 1000000000LL) break;
    }
    if (neg) v = -v;
    out = (int)v; return true;
}

static void cmd_add_user(const vector<string> &toks) {
    string cur, u, p, n, m; int g = 0; bool hasg = parse_kv_int(toks, 'g', g);
    parse_kv(toks, 'c', cur);
    bool hasu = parse_kv(toks, 'u', u);
    bool hasp = parse_kv(toks, 'p', p);
    bool hasn = parse_kv(toks, 'n', n);
    bool hasm = parse_kv(toks, 'm', m);

    if (!hasu || !hasp || !hasn || !hasm) { cout << -1 << '\n'; return; }

    if (db.sz == 0) {
        // First user: ignore -c and -g, privilege = 10
        User nu; nu.username = u; nu.password = p; nu.name = n; nu.mail = m; nu.privilege = 10; nu.online = false;
        if (!db.add(nu)) { cout << -1 << '\n'; return; }
        cout << 0 << '\n';
        return;
    }

    // Need current user logged in and new user's privilege < cur's privilege
    if (cur.empty()) { cout << -1 << '\n'; return; }
    User *cu = db.find(cur);
    if (!cu || !cu->online) { cout << -1 << '\n'; return; }
    if (!hasg) { cout << -1 << '\n'; return; }
    if (g >= cu->privilege) { cout << -1 << '\n'; return; }
    if (db.exists(u)) { cout << -1 << '\n'; return; }
    User nu; nu.username = u; nu.password = p; nu.name = n; nu.mail = m; nu.privilege = g; nu.online = false;
    if (!db.add(nu)) { cout << -1 << '\n'; return; }
    cout << 0 << '\n';
}

static void cmd_login(const vector<string> &toks) {
    string u, p;
    if (!parse_kv(toks, 'u', u) || !parse_kv(toks, 'p', p)) { cout << -1 << '\n'; return; }
    User *usr = db.find(u);
    if (!usr) { cout << -1 << '\n'; return; }
    if (usr->password != p) { cout << -1 << '\n'; return; }
    if (usr->online) { cout << -1 << '\n'; return; }
    usr->online = true;
    cout << 0 << '\n';
}

static void cmd_logout(const vector<string> &toks) {
    string u;
    if (!parse_kv(toks, 'u', u)) { cout << -1 << '\n'; return; }
    User *usr = db.find(u);
    if (!usr || !usr->online) { cout << -1 << '\n'; return; }
    usr->online = false;
    cout << 0 << '\n';
}

static void cmd_query_profile(const vector<string> &toks) {
    string cur, u;
    if (!parse_kv(toks, 'c', cur) || !parse_kv(toks, 'u', u)) { cout << -1 << '\n'; return; }
    User *cu = db.find(cur);
    User *tu = db.find(u);
    if (!cu || !cu->online || !tu) { cout << -1 << '\n'; return; }
    if (!(cu->privilege > tu->privilege || cu->username == tu->username)) { cout << -1 << '\n'; return; }
    cout << tu->username << ' ' << tu->name << ' ' << tu->mail << ' ' << tu->privilege << '\n';
}

static void cmd_modify_profile(const vector<string> &toks) {
    string cur, u, p, n, m; int g = 0; bool hasg = parse_kv_int(toks, 'g', g);
    if (!parse_kv(toks, 'c', cur) || !parse_kv(toks, 'u', u)) { cout << -1 << '\n'; return; }
    User *cu = db.find(cur);
    User *tu = db.find(u);
    if (!cu || !cu->online || !tu) { cout << -1 << '\n'; return; }
    if (!(cu->privilege > tu->privilege || cu->username == tu->username)) { cout << -1 << '\n'; return; }
    if (hasg && !(g < cu->privilege)) { cout << -1 << '\n'; return; }
    string tp, tn, tm; int tg = tu->privilege;
    if (parse_kv(toks, 'p', tp)) tu->password = tp;
    if (parse_kv(toks, 'n', tn)) tu->name = tn;
    if (parse_kv(toks, 'm', tm)) tu->mail = tm;
    if (hasg) tu->privilege = g;
    cout << tu->username << ' ' << tu->name << ' ' << tu->mail << ' ' << tu->privilege << '\n';
}

static void cmd_clean() {
    db.clear();
    cout << 0 << '\n';
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string line;
    while (true) {
        if (!std::getline(cin, line)) break;
        if (line.empty()) continue;
        // tokenize by space
        vector<string> toks;
        toks.reserve(16);
        string cur;
        for (size_t i = 0; i < line.size(); ) {
            // skip spaces
            while (i < line.size() && line[i] == ' ') ++i;
            if (i >= line.size()) break;
            size_t j = i;
            while (j < line.size() && line[j] != ' ') ++j;
            toks.push_back(line.substr(i, j - i));
            i = j;
        }
        if (toks.empty()) continue;

        const string &cmd = toks[0];
        if (cmd == add_user) {
            cmd_add_user(toks);
        } else if (cmd == login) {
            cmd_login(toks);
        } else if (cmd == logout) {
            cmd_logout(toks);
        } else if (cmd == query_profile) {
            cmd_query_profile(toks);
        } else if (cmd == modify_profile) {
            cmd_modify_profile(toks);
        } else if (cmd == clean) {
            cmd_clean();
        } else if (cmd == exit) {
            cout << byen;
            break;
        } else {
            // Unsupported commands: follow spec to print failure value when applicable.
            // For unknown commands, output -1 to be safe.
            cout << -1 << '\n';
        }
    }
    return 0;
}
