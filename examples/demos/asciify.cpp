
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;

// -----------------------------------------------------------------------------

void asciify_buffer(unsigned char* bufin, unsigned char* bufout, long sizein) {
  unsigned char* pi = bufin;
  unsigned char* bufin_end = bufin + sizein;
  unsigned char* po = bufout;
  
  for ( ; pi < bufin_end; pi++, po += 2) {
    *po = ((*pi & 0xF0) >> 4) + '#';  // '#' to avoid the " char
    *(po+1) = (*pi & 0xF) + '#';
  }
}

void unasciify_buffer(unsigned char* bufin, unsigned char* bufout, long sizein) {
  unsigned char* pi = bufin;
  unsigned char* bufin_end = bufin + sizein;
  unsigned char* po = bufout;
  
  for ( ; pi < bufin_end; pi +=2, po++) {
    *po = (*pi - '#') << 4 | (*(pi+1) - '#');
  }
}

// -----------------------------------------------------------------------------

int asciify(const char* name_in) 
{
  if (!name_in || !*name_in) return -1;

  int ll = strlen(name_in);
  bool to_ascii = true;
  if (ll > 2 && name_in[ll-2]=='.' && name_in[ll-1]=='A') to_ascii = false;

  char* name_out = new char[ll+5];
  if (to_ascii) 
    ::sprintf(name_out, "%s.A", name_in);  // add a ending ".A"
  else {
    ::strcpy(name_out, name_in);
    name_out[ll-2] = 0;  // remove the ending ".A"
    name_out[ll-1] = 0;
  }
  
  int fin = -1, fout = -1;
  struct stat fin_stat;
  unsigned char *bufin = 0, *bufout = 0;
  int sizein = 0, sizeout = 0;
  int res = 0;
  
  fin = ::open(name_in, O_RDONLY, 0);
  if (fin == -1) { 
    cerr << "cannot open input file " << name_in <<endl;
    res = -1;
    goto END;
  }

  if (::fstat(fin, &fin_stat) == -1 || (fin_stat.st_mode & S_IFMT) != S_IFREG) {
    cerr << "Wrong type for input file " << name_in << endl;
    res = -1;
    goto END;
  }

  sizein = fin_stat.st_size;
  sizeout = to_ascii ? sizein*2 : sizein/2;
  
  if (sizein <= 0) {
    cerr << "Empty input file " << name_in << endl;
    res = -1;
    goto END;
  }
  
  fout = ::open(name_out, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
  if (fout == -1) { 
    cerr << "cannot create output file " << name_out<< endl;
    res = -2;
    goto END;
  }
  
  if (! (bufin = new unsigned char[sizein])
      || ! (bufout = new unsigned char[sizeout])) {
    cerr << "cannot alloc memory " << endl;
    res = -3;
    goto END;
  }
  
  if (::read(fin, bufin, sizein) <= 0) {
    cerr << "cannot read file " << endl;
    res = -4;
    goto END; 
  }
  
  if (to_ascii) asciify_buffer(bufin, bufout, sizein);
  else unasciify_buffer(bufin, bufout, sizein);
  
  if (::write(fout, bufout, sizeout) <= 0) {
    cerr << "cannot write file " << name_out <<endl;
    res = -5;
    goto END;
  }
  
END:
  if (fin != -1) ::close(fin);
  if (fout != -1) ::close(fout);
  delete[] name_out;
  delete[] bufin;
  delete[] bufout;
  return (res < 0 ? res : sizein);
}

// -----------------------------------------------------------------------------

int main(int argc, char ** argv) {
  if (argc != 2) {
    cout << "Syntax: " << argv[0] << " file-name"<< endl;
    return 1;
  }
  
  int len = asciify(argv[1]);
  
  cout << "File length: " << len << endl;
  return 0;
}
