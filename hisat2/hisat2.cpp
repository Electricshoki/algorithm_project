/*
 * Author: 야기시타쇼키
 * Student ID: 2021111972
 * University: 동국대학교
 * File: [hisat2.c]
 * Description: hisat2 의 코드 구조를 그대로 재현해보는데 의의를 둠. BWT+FM-Index, Mismatch, 단일 인덱스를 사용하는 구조를 차용했음 
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <set>
using namespace std;

#define READ_LEN 25
#define MAX_MISMATCH 1

string load_file(const string& filename) {
    ifstream in(filename);
    string seq, line;
    while (getline(in, line)) seq += line;
    return seq + "$";
}

vector<int> build_suffix_array(const string& s) {
    int n = s.size();
    vector<int> sa(n);
    for (int i = 0; i < n; ++i) sa[i] = i;
    sort(sa.begin(), sa.end(), [&](int i, int j) {
        return s.substr(i) < s.substr(j);
        });
    return sa;
}

string build_bwt(const string& s, const vector<int>& sa) {
    string bwt;
    for (int i = 0; i < sa.size(); ++i) {
        int idx = sa[i];
        bwt += (idx == 0) ? '$' : s[idx - 1];
    }
    return bwt;
}

map<char, int> build_C(const string& bwt) {
    map<char, int> freq, C;
    for (size_t i = 0; i < bwt.size(); ++i) freq[bwt[i]]++;
    int total = 0;
    for (map<char, int>::iterator it = freq.begin(); it != freq.end(); ++it) {
        C[it->first] = total;
        total += it->second;
    }
    return C;
}

map<char, vector<int>> build_Occ(const string& bwt) {
    map<char, vector<int>> Occ;
    for (size_t i = 0; i < bwt.size(); ++i) Occ[bwt[i]] = vector<int>(bwt.size() + 1, 0);
    for (size_t i = 0; i < bwt.size(); ++i) {
        for (map<char, vector<int>>::iterator it = Occ.begin(); it != Occ.end(); ++it) {
            char ch = it->first;
            vector<int>& vec = it->second;
            vec[i + 1] = vec[i];
        }
        Occ[bwt[i]][i + 1]++;
    }
    return Occ;
}

void backward_search(const string& read, const string& bwt, const map<char, int>& C,
    const map<char, vector<int>>& Occ, const vector<int>& SA,
    int max_mismatch, set<int>& result, int idx, int l, int r, int mismatches) {
    if (mismatches > max_mismatch) return;
    if (idx < 0) {
        for (int i = l; i < r; ++i)
            result.insert(SA[i]);
        return;
    }
    char c = read[idx];
    for (map<char, int>::const_iterator it = C.begin(); it != C.end(); ++it) {
        char a = it->first;
        int l_new = C.at(a) + Occ.at(a)[l];
        int r_new = C.at(a) + Occ.at(a)[r];
        if (l_new < r_new) {
            int mismatch_inc = (a == c ? 0 : 1);
            backward_search(read, bwt, C, Occ, SA, max_mismatch, result, idx - 1, l_new, r_new, mismatches + mismatch_inc);
        }
    }
}

int main() {
    string ref = load_file("test_reference.txt");
    vector<int> sa = build_suffix_array(ref);
    string bwt = build_bwt(ref, sa);
    map<char, int> C = build_C(bwt);
    map<char, vector<int>> Occ = build_Occ(bwt);

    ifstream fr("reads.txt");
    ofstream fout("reconstructed.txt");
    string line;
    vector<char> reconstructed(ref.size() - 1, '?');

    while (getline(fr, line)) {
        if (line.empty()) continue;
        set<int> candidates;
        backward_search(line, bwt, C, Occ, sa, MAX_MISMATCH, candidates, line.size() - 1, 0, bwt.size(), 0);
        if (!candidates.empty()) {
            int pos = *candidates.begin();
            for (int i = 0; i < READ_LEN && pos + i < reconstructed.size(); ++i) {
                if (reconstructed[pos + i] == '?')
                    reconstructed[pos + i] = line[i];
            }
        }
    }
    for (size_t i = 0; i < reconstructed.size(); ++i) fout << reconstructed[i];
    fout << endl;
    fout.close();

    string my_genome = load_file("MY_genome.txt");

    int best_match = 0, best_offset = 0;
    for (int offset = 0; offset <= (int)(my_genome.size() - reconstructed.size()); ++offset) {
        int match = 0;
        for (size_t i = 0; i < reconstructed.size(); ++i) {
            if (reconstructed[i] == '?') continue;
            if (reconstructed[i] == my_genome[offset + i]) match++;
        }
        if (match > best_match) {
            best_match = match;
            best_offset = offset;
        }
    }

    int total = reconstructed.size();
    int missing = count(reconstructed.begin(), reconstructed.end(), '?');
    int matched = best_match;
    double full_accuracy = (double)matched / total * 100.0;
    double known_accuracy = (double)matched / (total - missing) * 100.0;

    cout << "총 길이: " << total << endl;
    cout << "복원된 문자 수: " << total - missing << endl;
    cout << "복원 실패 문자 수: " << missing << endl;
    cout << "전체 정확도 (?? 포함): " << full_accuracy << "%" << endl;
    cout << "복원된 영역 기준 정확도: " << known_accuracy << "%" << endl;
    return 0;
}
