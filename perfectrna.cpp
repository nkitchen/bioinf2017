#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

using std::cin;
using std::cout;
using std::deque;
using std::endl;
using std::ostream;
using std::set;
using std::string;
using std::unordered_map;
using std::unordered_set;

typedef std::pair<size_t, size_t> substring;

template<class T>
class fifo_set {
  private:
    set<T> members;
    deque<T> order;

  public:
    void insert(T const & t) {
       auto p = members.insert(t);
       if (p.second) {
          order.push_back(t);
       }
       assert(members.size() == order.size());
    }

    T pop_front() {
       T t = order.front();
       order.pop_front();
       members.erase(t);
       assert(members.size() == order.size());
       return t;
    }

    bool empty() const {
       return members.empty();
    }

    typename deque<T>::iterator begin() {
       return order.begin();
    }

    typename deque<T>::iterator end() {
       return order.end();
    }

    typename deque<T>::const_iterator begin() const {
       return order.begin();
    }

    typename deque<T>::const_iterator end() const {
       return order.end();
    }
};

template<class T>
ostream &
operator<<(ostream & out,
           unordered_map<T, unordered_set<T> > const & s)
{
   out << "{" << endl;
   for (auto it = s.begin(); it != s.end(); ++it) {
      out << "  " << it->first << ":";
      for (auto jt = it->second.begin();
           jt != it->second.end();
           ++jt) {
         out << " " << *jt;
      }
      out << endl;
   }
   out << "}";

   return out;
}

int main() {
   char complement[256];
   std::fill(complement, complement + 256, '\0');
   complement['A'] = 'U';
   complement['U'] = 'A';
   complement['G'] = 'C';
   complement['C'] = 'G';

   string s;
   cin >> s;

   // Grammar for perfect strings:
   // P ::= B B'
   //     | B P B'
   //     | P P

   // perfectEnds[i] contains j if s[i:j] is perfect.
   // perfectStarts[j] contains i if s[i:j] is perfect.
   unordered_map<size_t, unordered_set<size_t> > perfectEnds;
   unordered_map<size_t, unordered_set<size_t> > perfectStarts;

   fifo_set<substring> queue;

   // Seed with B B'.
   for (size_t i = 0; i <= s.size() - 2; ++i) {
      if (s[i + 1] == complement[s[i]]) {
         perfectEnds[i].insert(i + 2);
         perfectStarts[i + 2].insert(i);
         queue.insert(substring(i, i + 2));
      }
   }

   while (!queue.empty()) {
      substring ij = queue.pop_front();
      size_t i = ij.first;
      size_t j = ij.second;

      // Try expanding with B P B'.
      if (0 < i && j < s.size() &&
            s[i - 1] == complement[s[j]]) {
         perfectEnds[i - 1].insert(j + 1);
         perfectStarts[j + 1].insert(i - 1);
         queue.insert(substring(i - 1, j + 1));
      }

      // Try expanding with P P, where this is the first P.
      for (auto it = perfectEnds[j].begin();
           it != perfectEnds[j].end(); ++it) {
         size_t k = *it;
         perfectEnds[i].insert(k);
         perfectStarts[k].insert(i);
         queue.insert(substring(i, k));
      }

      // Try expanding with P P, where this is the second P.
      for (auto it = perfectStarts[i].begin();
           it != perfectStarts[i].end(); ++it) {
         size_t h = *it;
         perfectStarts[j].insert(h);
         perfectEnds[h].insert(j);
         queue.insert(substring(h, j));
      }
   }

   if (s.size() % 2 == 0) {
      if (perfectEnds[0].find(s.size()) !=
          perfectEnds[0].end()) {
         cout << "perfect" << endl;
      } else {
         cout << "imperfect" << endl;
      }
      return 0;
   }

   // Grammar for almost perfect strings:
   // A ::= B C B'
   //     | B A B'
   //     | C P
   //     | P C
   //     | A P
   //     | P A
   //
   // The code is simpler if we treat single bases as almost perfect,
   // so that a lone C is an A.

   //cout << "perfectEnds " << perfectEnds << endl;
   //cout << "perfectStarts " << perfectStarts << endl;

   unordered_map<size_t, unordered_set<size_t> > almostPerfectEnds;
   unordered_map<size_t, unordered_set<size_t> > almostPerfectStarts;

   for (size_t i = 0; i < s.size(); ++i) {
      almostPerfectEnds[i].insert(i + 1);
      almostPerfectStarts[i + 1].insert(i);
      queue.insert(substring(i, i + 1));
   }

   while (!queue.empty()) {
      substring ij = queue.pop_front();
      size_t i = ij.first;
      size_t j = ij.second;
      //cout << "pop " << i << ":" << j << endl;

      // Try expanding with B A B'.
      if (0 < i && j < s.size() &&
            s[i - 1] == complement[s[j]]) {
         almostPerfectEnds[i - 1].insert(j + 1);
         almostPerfectStarts[j + 1].insert(i - 1);
         queue.insert(substring(i - 1, j + 1));
      }

      // Try expanding with A P.
      for (auto it = perfectEnds[j].begin();
           it != perfectEnds[j].end(); ++it) {
         size_t k = *it;
         almostPerfectEnds[i].insert(k);
         almostPerfectStarts[k].insert(i);
         queue.insert(substring(i, k));
      }

      // Try expanding with P A.
      for (auto it = perfectStarts[i].begin();
           it != perfectStarts[i].end(); ++it) {
         size_t h = *it;
         almostPerfectStarts[j].insert(h);
         almostPerfectEnds[h].insert(j);
         queue.insert(substring(h, j));
      }

      //cout << "ends " << almostPerfectEnds << endl;
      //cout << "starts " << almostPerfectStarts << endl;
   }

   if (almostPerfectEnds[0].find(s.size()) !=
       almostPerfectEnds[0].end()) {
      cout << "almost perfect" << endl;
   } else {
      cout << "imperfect" << endl;
   }
   return 0;
}
