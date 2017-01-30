#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>

using namespace std;

// A maximal perfect substring is a perfect substring
// that is not bracketed by a complementary pair of bases,
// that is., it does not match B P B'.
// A maximal perfect range (MPR) consists of the indices
// of a maximal perfect substring.
struct mpr {
   size_t begin;
   size_t end;  // exclusive
   // The stem length of a MPR is the number of matching bases
   // at the beginning and end of the MPS.  In a picture of the
   // secondary structure, the MPS may branch, and the stem is
   // the part below the first branching point.
   size_t stem;
};

bool
operator<(mpr const & r, mpr const & s)
{
   auto tr = make_tuple(r.begin, r.begin + r.stem, r.end);
   auto ts = make_tuple(s.begin, s.begin + s.stem, s.end);
   return tr < ts;
}

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

   // MPRs indexed by the beginning indices of their stems and crowns
   // (The crown is the substring that is not part of the stem.)
   map<size_t, set<mpr> > maxPerfByBegin;
   // MPRs indexed by their ending indices of their stems and crowns
   map<size_t, set<mpr> > maxPerfByEnd;

   fifo_set<mpr> queue;

   // Seed with simple MPRs (no branching, just B+ (B+)').
   for (size_t i = 0; i <= s.size() - 2; ++i) {
      if (s[i] == complement[s[i + 1]]) {
         size_t j = i, k = i + 1;
         while (0 < j && k < s.size() - 1) {
            if (s[j - 1] == complement[s[k + 1]]) {
               --j;
               ++k;
            } else {
               break;
            }
         }
         size_t n = k + 1 - j;
         mpr x{.begin = j, .end = k + 1, .stem = n / 2};
         maxPerfByBegin[x.begin].insert(x);
         maxPerfByEnd[x.end].insert(x);
         if (x.stem > 0) {
            maxPerfByBegin[x.begin + x.stem].insert(x);
            maxPerfByEnd[x.end - x.stem].insert(x);
         }
         queue.insert(x);
      }
   }

   while (!queue.empty()) {
      mpr x = queue.pop_front();

      // The MPR represents multiple perfect substrings.
      // Each one corresponds to stripping off a different length
      // of the stem.

      // For one of the perfect substrings from x to be immediately followed
      // by another perfect substring from another MPR y,
      // there must be some lengths m and n,
      // with 0 <= m <= x.stem and 0 <= n <= y.stem,
      // such that x.end - m == y.begin + n.

      // y.begin == x.end - m - n
      // min: y.begin >= x.end - x.stem - y.stem
      //      y.begin + y.stem >= x.end - x.stem
      // max: y.begin <= x.end

      for (auto it = maxPerfByBegin.lower_bound(x.end - x.stem);
           it != maxPerfByBegin.end();
           ++it) {
         for (auto jt = it->second.begin();
              jt != it->second.end();
              ++jt) {
            mpr y = *jt;
            if (y.begin > x.end) {
               break;
            }

            //    ] x.end - x.stem
            //          ] x.end
            // CCCSSSSSS
            //      i
            //      [ m ]
            //   [n ]
            //   SSSSSCCCCC...
            //   [ y.begin
            //        [ y.begin + y.stem
            for (size_t i = max(x.end - x.stem, y.begin);
                 i < min(x.end, y.begin + y.stem);
                 ++i) {
               size_t m = x.end - i;
               size_t n = i - y.begin;

               // Expand stem.
               size_t u = x.begin + m;
               size_t v = y.end - n - 1;
               size_t k = 0;
               while (0 < u && v < s.size() - 1) {
                  if (s[u - 1] == complement[s[v + 1]]) {
                     --u;
                     ++v;
                     ++k;
                  } else {
                     break;
                  }
               }
               mpr z{.begin = u, .end = v + 1, .stem = k};
               maxPerfByBegin[z.begin].insert(z);
               maxPerfByEnd[z.end].insert(z);
               if (z.stem > 0) {
                  maxPerfByBegin[z.begin + z.stem].insert(z);
                  maxPerfByEnd[z.end - z.stem].insert(z);
               }
               queue.insert(z);
            }
         }
      }

      for (auto it = make_reverse_iterator(
               maxPerfByEnd.upper_bound(x.begin + x.stem));
           it != maxPerfByEnd.rend();
           ++it) {
         for (auto jt = it->second.begin();
              jt != it->second.end();
              ++jt) {
            mpr w = *jt;
            if (w.end < x.begin) {
               break;
            }

            //    ] w.end - w.stem
            //          ] w.end
            // CCCSSSSSS
            //      i
            //      [ m ]
            //   [n ]
            //   SSSSSCCCCC...
            //   [ x.begin
            //        [ x.begin + x.stem
            for (size_t i = max(w.end - w.stem, x.begin);
                 i < min(w.end, x.begin + x.stem);
                 ++i) {
               size_t m = w.end - i;
               size_t n = i - x.begin;

               // Expand stem.
               size_t u = w.begin + m;
               size_t v = x.end - n - 1;
               size_t k = 0;
               while (0 < u && v < s.size() - 1) {
                  if (s[u - 1] == complement[s[v + 1]]) {
                     --u;
                     ++v;
                     ++k;
                  } else {
                     break;
                  }
               }
               mpr z{.begin = u, .end = v + 1, .stem = k};
               maxPerfByBegin[z.begin].insert(z);
               maxPerfByEnd[z.end].insert(z);
               if (z.stem > 0) {
                  maxPerfByBegin[z.begin + z.stem].insert(z);
                  maxPerfByEnd[z.end - z.stem].insert(z);
               }
               queue.insert(z);
            }
         }
      }
   }

   if (s.size() % 2 == 0) {
      for (auto it = maxPerfByBegin[0].begin();
           it != maxPerfByBegin[0].end();
           ++it) {
         mpr x = *it;
         if (x.end == s.size()) {
            cout << "perfect" << endl;
            return 0;
         }
      }

      cout << "imperfect" << endl;
      return 0;
   }

   /*
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

   for (size_t i = 0; i < s.size(); ++i) {
      queue.insert(substring(i, i + 1));
   }

   while (!queue.empty()) {
      substring ij = queue.pop_front();
      size_t i = ij.first;
      size_t j = ij.second;
      //cout << "pop " << i << ":" << j << endl;

      if (i == 0 && j == s.size()) {
         cout << "almost perfect" << endl;
         return 0;
      }

      // Try expanding with B A B'.
      if (0 < i && j < s.size() &&
            s[i - 1] == complement[s[j]]) {
         queue.insert(substring(i - 1, j + 1));
      }

      // Try expanding with A P.
      for (auto it = perfectEnds[j].begin();
           it != perfectEnds[j].end(); ++it) {
         size_t k = *it;
         queue.insert(substring(i, k));
      }

      // Try expanding with P A.
      for (auto it = perfectStarts[i].begin();
           it != perfectStarts[i].end(); ++it) {
         size_t h = *it;
         queue.insert(substring(h, j));
      }
   }

   cout << "imperfect" << endl;
   */
   return 0;
}
