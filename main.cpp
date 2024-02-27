#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class File {
public:
  bool isDir;
  int size; // size is init to -1 if it is a dir, is default in bytes?
  string name;
  File *parent;            // for now the root will point to NULL
  vector<File *> children; // should always be of size 0 when isDir is false
  int childrenPrinted;
  int x, y;
  int depth;
};

string getNthWord(
    int n,
    string s) { // if n is longer than the words in s, returns the last word
  istringstream sin(s);
  int i = 1;
  while (sin >> s) {
    if (i >= n)
      return s;
    i++;
  }
  return s;
}

int countChars(string s, char c) { // counts the amount of c in s
  int i;
  int ret = 0;
  for (i = 0; i < s.size(); i++) {
    if (s[i] == c)
      ret++;
  }
  return ret;
}

int main(int argc, char **argv) {
  string s;
  vector<string> v;
  vector<File *> files;
  vector<File *> parentTrace;
  File *currParent;
  File *f;
  File *senti = new File;
  senti->name = "///SENTI///";
  int i, j, pn, pos, rootDepth, currDepth, lastDepth, x, y;
  currDepth = 0;
  /* these params effect the sizes and styling of the graph */
  int linesPerPage = 30;
  int linePadding = 8;
  double fontSize = 1;
  int offset;
  pn = 0;
  int yMax = (i * linePadding + 1) + ((linesPerPage * linePadding));
  string graphDef = "yaxis max 5 min -" + to_string(yMax) +
                    " size 10 \nxaxis min 0 max 20 size 8.5 \n";

  currParent = new File;
  currParent->name = argv[1];
  currParent->depth = 0;
  currParent->isDir = true;
  currParent->childrenPrinted = 0;
  currParent->parent = senti;
  currParent->size = -1;
  files.push_back(currParent);
  parentTrace.push_back(currParent);

  rootDepth = countChars(currParent->name, '/');
  lastDepth = rootDepth - 1;

  cerr << files[0]->name << endl;

  while (getline(cin, s)) {
    if (s[0] == '-') {
      f = new File;
      f->name = getNthWord(9, s);
      // cerr << getNthWord(5,s) << endl;
      f->size = stol(getNthWord(5, s));
      f->isDir = false;
      f->depth = currDepth;
      if (parentTrace.size() > 0) {
        f->parent = parentTrace[parentTrace.size() - 1];
      } else {
        f->parent = senti;
      }
      files.push_back(f);
    }
    if (s[0] == 't') {
      // do nothing with this for now
    }
    if (s[s.size() - 1] == ':') {
      currDepth = countChars(s, '/') - rootDepth;
      while (lastDepth >= currDepth && parentTrace.size() > 0) {
        parentTrace.pop_back();
        lastDepth--;
      }
      currParent = new File;
      if (parentTrace.size() > 0) {
        currParent->parent = parentTrace[parentTrace.size() - 1];
      } else {
        currParent->parent = senti;
      }
      currParent->isDir = true;
      currParent->size = -1;
      currParent->name = s;
      currParent->depth = currDepth;
      files.push_back(currParent);
      parentTrace.push_back(currParent);
      lastDepth = currDepth;
      currDepth++;
    }
  }
  for (i = 1; i < files.size(); i++) {
    files[i]->parent->children.push_back(files[i]);
  }
  printf("newgraph xaxis size 7 yaxis size 7\n");


  x = 0;
  y = 0;
  for(i = 0; i < files.size(); i++){
    if(files[i]->isDir){
      if(files[i]->parent != NULL){
        y = files[i]->parent->y - 10;
      }
      printf("newcurve cfill 1 0 0 marktype circle pts %d %d\n", x, y);
      files[i]->x = x;
      files[i]->y = y;
      if(files[i]->parent != NULL){
        printf("newline pts %d %d %d %d\n", x, y, files[i]->parent->x, files[i]->parent->y);
      }
    }else{
      y = files[i]->parent->y - 10;
      if(files[i]->parent->childrenPrinted < files[i]->parent->children.size()/2){
        x = files[i]->parent->x - (5 * files[i]->parent->childrenPrinted);
      }else{
        x = files[i]->parent->x + (5 * (files[i]->parent->childrenPrinted - (files[i]->parent->children.size()/2)));
      }
      files[i]->parent->childrenPrinted += 1;
      printf("newcurve cfill 0 1 0 marktype circle pts %d %d\n", x, y);
      files[i]->x = x;
      files[i]->y = y;
      printf("newline pts %d %d %d %d\n", x, y, files[i]->parent->x, files[i]->parent->y);
    }

  }

  /*mostly for debugging now, should be obsolete
  for (i = 0; i < files.size(); i++) {
    if (i == linesPerPage * pn) {
      if (i)
        printf("newpage\n");
      printf("newgraph\n");
      cout << graphDef;
      pn++;
    }
    offset = files[i]->depth;
    string tmpName = files[i]->name;
    tmpName += "     ";
    if (files[i]->parent != NULL)
      tmpName += files[i]->parent->name;
    int yPos =
        (-1) * ((i * linePadding) - ((linesPerPage * linePadding) * (pn - 1)));
    string color;
    color = "0 1 0";
    if (files[i]->isDir)
      color = "1 0 0";
    printf("newcurve cfill %s marktype box marksize 0.5 2 pts %d %d\n",
           color.c_str(), offset, yPos);
    printf("newstring hjl vjc rotate 0 fontsize %lf x %d y %d : %s\n", fontSize,
           offset, yPos, tmpName.c_str());
  }


  for(i = 0; i < senti->children.size(); i++){
    files[0]->children.push_back(senti->children[i]);
  }

  f = files[0];
  printf("newgraph\n yaxis min 0 max 10 nodraw xaxis nodraw\n");
  double xPos = (f->children.size()-1)/2.0;
  printf("newstring hjc vjb rotate 0 fontsize %d x %lf y %d : %s\n",
    fontSize, xPos, 5, files[0]->name.c_str());
  printf("newcurve marktype box color 1 1 1 pts %lf %d\n", xPos, 5);
  for(i = 0; i < f->children.size(); i++){
    printf("newcurve marktype box color 1 1 1 pts %d %d\n", i, 3);
    string n = f->children[i]->name.substr(f->name.size());
    n = n.substr(0, n.size()-1);
    printf("newstring hjl vjc rotate -45 fontsize %d x %d y %d : %s\n",
      fontSize, i, 3, n.c_str());
    printf("newline pts %d %d %lf 5\n", i, 3, xPos);
  }


  for(i = 1; i < files.size(); i++){
    if(files[i]->isDir){
      printf("newpage\n");
      f = files[i];
      printf("newgraph\n yaxis min 0 max 10 nodraw xaxis nodraw size 8\n");
      double xPos = (f->children.size()-1)/2.0;
      printf("newstring hjc vjb rotate 0 fontsize %d x %lf y %d : %s\n",
      fontSize, xPos, 5, f->name.c_str());
      printf("newcurve marktype box color 1 1 1 pts %lf %d\n", xPos, 5);
      for(j = 0; j < f->children.size(); j++){
        printf("newcurve marktype box color 1 1 1 pts %d %d\n", j, 3);
        string n;
        if(f->children[j]->isDir){
          n = f->children[j]->name.substr(f->name.size());
          n = n.substr(0, n.size()-1);
        }else{
          n = f->children[j]->name;
        }
        printf("newstring hjl vjc rotate -45 fontsize %d x %d y %d : %s\n",
          fontSize, j, 3, n.c_str());
        printf("newline pts %d %d %lf 5\n", j, 3, xPos);
      }
    }
  }*/

  return 0;
}
