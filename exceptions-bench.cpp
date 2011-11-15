#include <iostream>
#include <cmath>
#include <exception>

using namespace std;

extern "C"
{
  #include <time.h>
}


class Ex: public exception
{
private:
  const string w;
public:
  Ex(const string& what):w(what){}
  virtual ~Ex() throw() {}

  virtual const char* what() const throw()
  {
    return w.c_str();
  }
};

class Timer
{
private:
  const clockid_t clkid;
  struct timespec ts, now;

  static double ts_to_double(const struct timespec &ts)
  {
    return ts.tv_sec*1000000 + ts.tv_nsec/(double)1000;
  }
public:
  Timer(const clockid_t clockid):clkid(clockid)
  {
    ts.tv_sec  = 0;
    ts.tv_nsec = 0;
  }

  void start()
  {
    if (clock_gettime(clkid, &ts)) throw Ex("clock_gettime() failed");
  }

  double stop()  //return time in us
  {
    if (clock_gettime(clkid, &now)) throw Ex("clock_gettime() failed");
    return ts_to_double(now) - ts_to_double(ts);
  }
};

//template<class T> class List
class List
{
public:
  typedef unsigned int T;
private:
  struct node
  {
    T value;
    node *next;
    node(const T& value, node *next = NULL):value(value), next(next) {}
  };

  node *root, *last;
public:
  List():root(NULL), last(NULL) {}
 ~List()
  {
    node* b;
    while(root)
    {
      b = root->next;
      delete root;
      root = b;
    }
  }

  List& operator<< (const T& x)
  {
    node *n = new node(x);
    if (root)
    {
      last->next = n;
      last = n;
    } else
    {
      root = last = n;
    }
    return *this;
  }

  const T& operator[] (const size_t pos) const throw(Ex)
  {
    node *n = root;
    for (size_t i = 0; n; i++)
    {
      if (i == pos) return n->value;
      n = n->next;
    }
    throw Ex("List::operator[]: invalid index");
  }

  bool at(const size_t pos, T& ret) const throw()
  {
    node *n = root;
    for (size_t i = 0; n; i++)
    {
      if (i == pos)
      {
        ret = n->value;
        return true;
      }
      n = n->next;
    }
    return false;
  }

};

//template<class T> class BenchData
class BenchData
{
public:
  typedef double T;
private:
  T sum, sumsq;
  unsigned int n;
public:
  BenchData():sum(0), sumsq(0), n(0) {}

  BenchData& operator<<(const T& sample)
  {
    sum += sample;
    sumsq += sample*sample;
    n++;
    return *this;
  }

  T mean() const
  {
    return sum / (T)n;
  }

  T stdDeviation() const
  {
    T m = mean();
    return sqrt(sumsq/(T)n - m*m);
  }
};


int main()
{
  Timer timer(CLOCK_PROCESS_CPUTIME_ID);                  //or CLOCK_REALTIME for sys too
  unsigned int i;

  //init list
  List list;
  const unsigned int itms = 30000;
  for (i = 0; i < itms; i++)
    list << i;

  BenchData data_ex, data_n, data_nnh, data_exi, data_ni;

  const unsigned int rounds = 100000;
  unsigned int errors = 0;
  unsigned long tsum = 0;

  cout << "\n\nItems:\t" << itms << endl;
  cout << "Rounds:\t" << rounds << endl;

  for (i = 0; i < rounds; i++)
  {
    unsigned int x = 0;

    timer.start();
    try { tsum += list[itms-1]; }
    catch(Ex& e) { errors++; }
    data_ex << timer.stop();

    timer.start();
    list.at(itms-1, x);
    tsum += x;
    data_nnh << timer.stop();

    timer.start();
    if (!list.at(itms-1, x)) { errors++; }
    else tsum += x;
    data_n << timer.stop();

    timer.start();
    try { tsum += list[itms+1]; }
    catch(Ex& e) { errors++; }
    data_exi << timer.stop();

    timer.start();
    if (!list.at(itms+1, x)) { errors++; }
    else tsum += x;
    data_ni << timer.stop();
  }

  cout.precision(5);

  cout << "\nError not raised:" << endl;
  cout << "\n  With exceptions:" << endl;
  cout << "\tmean:\t" << data_ex.mean() << " us" << endl;
  cout << "\tsdev:\t" << data_ex.stdDeviation() << " us" << endl;

  cout << "\n  Without exceptions, no error handling:" << endl;
  cout << "\tmean:\t" << data_nnh.mean() << " us" << endl;
  cout << "\tsdev:\t" << data_nnh.stdDeviation() << " us" << endl;

  cout << "\n  Without exceptions, ret val testing:" << endl;
  cout << "\tmean:\t" << data_n.mean() << " us" << endl;
  cout << "\tsdev:\t" << data_n.stdDeviation() << " us" << endl;

  cout << "\nError raised:" << endl;
  cout << "\n  With exceptions:" << endl;
  cout << "\tmean:\t" << data_exi.mean() << " us" << endl;
  cout << "\tsdev:\t" << data_exi.stdDeviation() << " us" << endl;

  cout << "\n  Without exceptions:" << endl;
  cout << "\tmean:\t" << data_ni.mean() << " us" << endl;
  cout << "\tsdev:\t" << data_ni.stdDeviation() << " us" << endl;

  cout << "\n\n(Errors: " << errors << ", tsum: " << tsum << ")\n" << endl;

  return 0;
}
