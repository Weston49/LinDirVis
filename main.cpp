#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class File {
public:
  bool isDir;
  long long size;
  string name;
  File *parent;            // for now the root will point to NULL
  vector<File *> children; // should always be of size 0 when isDir is false
  int childrenPrinted;
  int x, y;
  int depth;
  bool isDrawn;
  int totalChildrenWidth;
  bool widthFound;
};

string getNthWord(int n, string s) { // if n is longer than the words in s, returns the last word
  istringstream sin(s);
  int i = 1;
  while (sin >> s) {
    if (i >= n)
      return s;
    i++;
  }
  return s;
}

string getFileName(string s){
  istringstream sin(s);
  string ret = "";
  int i = 0;
  while(sin >> s){
    i++;
    if(i >= 9){
      ret += s;
      ret += " "; //this is a janky way of doing this but for most files it should be fine enough for now
    }
  }
  if(ret[ret.size()-1] == ' ') ret.pop_back();
  return ret;
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

int calcNodeWidth(File* f){
  int width = 0;
  int i;
  if(f->widthFound) return f->totalChildrenWidth;
  f->widthFound = true;
  if(!f->isDir) {
    f->totalChildrenWidth = 1;
    return 1;
  }
  for(i = 0; i < f->children.size(); i++){
    width += calcNodeWidth(f->children[i]);
  }
  if(width == 0) width = 1;
  f->totalChildrenWidth = width;
  return width;
}

long long find_dir_sizes(File* f){
  int i;
  long long total = 0; 
  if(!f->isDir) return f->size;
  total += f->size;
  for(i = 0; i < f->children.size(); i++){
    if(f->children[i]->isDir){
      total += find_dir_sizes(f->children[i]);
    }else{
      total += f->children[i]->size;
    }
  }
  f->size = total;
  return total;
}

void draw_nodes(File *f, int x, int y){
  int i;
  int offset;
  int spacing = 3;
  int fontSize = 12;
  int widthNeeded = 0;
  int vertOffset = 1;
  vector<double> spacePercents;
  int curRightOffset = 0;
  f->x = x;
  f->y = y;
  if(!f->isDrawn){
    if(f->isDir){
      // printf("newcurve cfill 1 0 0 marktype circle pts %d %d\n", x, y);
      // printf("newstring hjr vjc rotate -45 fontsize %d x %d y %d : %d\n", fontSize, x, y, f->totalChildrenWidth);
    }else{
      // printf("newcurve cfill 0 1 0 marktype circle pts %d %d\n", x, y);
    }
  }else{

  }
  f->isDrawn = true;
  widthNeeded = f->totalChildrenWidth;
  for(i = 0; i < f->children.size(); i++){
    spacePercents.push_back((double)f->children[i]->totalChildrenWidth/(double)widthNeeded);
  }

  if(widthNeeded <= 0) widthNeeded = 1;
  for(i = 0; i < f->children.size(); i++){
    offset = x - (widthNeeded/2) + ((spacePercents[i]/2) * widthNeeded) + curRightOffset;
    draw_nodes(f->children[i], offset, y-vertOffset);
    printf("newline pts %d %d %d %d\n", x, y, offset, y-vertOffset);
    curRightOffset += spacePercents[i]*widthNeeded;
    // printf("newstring hjl vjc fontsize %d x %d y %d : %d\n", fontSize, offset, y-10, widthNeeded);
  }
}

string get_extention(string s){
  if(s[0] == '.') return "other"; //just makes things simpler, not ideal but works 
  string ret;
  int originalSize = s.size();
  if(s.find('.') == string::npos) return "other";
  reverse(s.begin(), s.end());
  ret = s.substr(0, s.find('.')+1);
  reverse(ret.begin(), ret.end());
  if(ret.size() == originalSize) return "other";
  return ret;
}

unordered_map<string, int> get_file_dist(vector<File*> files){
  unordered_map<string, int> ret;
  int i;
  string tmp;
  for(i = 0; i < files.size(); i++){
    tmp = get_extention(files[i]->name);
    if(files[i]->isDir && tmp == "other"){
      //we do not count regular directories
    }else{
      if(ret.find(tmp) != ret.end()){
        ret.find(tmp)->second++;
      }else{
        ret.insert(make_pair(tmp,1));
      }
    }
  }


  return ret;
}


string get_random_color(string seed){
  string ret;
  int i;
  int seedVal = 0;
  ostringstream out;

  for(i = 0; i < seed.size(); i++){
    seedVal += seed[i];
  }

  srand(seedVal);
  int junk = rand();
  double r = (double)rand() / RAND_MAX;
  double g = (double)rand() / RAND_MAX;
  double b = (double)rand() / RAND_MAX;
  
  out.precision(2);
  out << fixed << r << " " << g << " " << b;

  ret = out.str();
  // cerr << ret << endl;

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
  double tmpX;
  double tmpY;
  int column;
  long long rootSize;
  File *biggestFile;
  File *biggestDir;
  currDepth = 0;
  /* these params effect the sizes and styling of the graph */
  int linesPerPage = 30;
  int linePadding = 8;
  double fontSize = 14;
  double bfPercent;
  double bdPercent;
  string extColor;
  int minX;
  int mode;
  int offset;
  int filesFound;
  double fileDistOffset;
  double extPercent;
  unordered_map<string, int> file_dist;
  unordered_map<string, int>::iterator umit;
  string bdStr;
  string bfStr;
  pn = 0;
  int yMax = (i * linePadding + 1) + ((linesPerPage * linePadding));
  string graphDef = "yaxis max 5 min -" + to_string(yMax) +
                    " size 10 \nxaxis min 0 max 20 size 8.5 \n";



  if(argc > 2){
    mode = stoi(argv[2]);
  }else{
    mode = 31; //this will do everything by default
  }


  

  currParent = new File;
  string tmpStr = argv[1];
  if(tmpStr[tmpStr.size()-1] == '/') tmpStr.pop_back();
  currParent->name = tmpStr;
  currParent->depth = 0;
  currParent->isDir = true;
  currParent->childrenPrinted = 0;
  currParent->totalChildrenWidth = -1;
  currParent->isDrawn = false;
  currParent->parent = senti;
  currParent->size = 0;
  files.push_back(currParent);
  parentTrace.push_back(currParent);

  rootDepth = countChars(currParent->name, '/');
  lastDepth = 0;

  // cerr << files[0]->name << endl;
  
  while (getline(cin, s)) {
    if (s[0] == '-') { //does not handle solf links or hard links as of now, might cause weird behavior
      f = new File;
      f->name = getFileName(s);
      // cerr << getNthWord(5,s) << endl;
      f->size = stol(getNthWord(5, s));
      f->isDir = false;
      f->depth = currDepth;
      if(biggestFile == NULL || f->size > biggestFile->size) biggestFile = f;
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
      // cerr << "depth info: " << currDepth << endl;
      while (lastDepth >= currDepth && parentTrace.size() > 0) {
        // cerr << lastDepth << endl;
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
      currParent->size = 0;
      currParent->name = s;
      currParent->depth = currDepth;
      files.push_back(currParent);
      parentTrace.push_back(currParent);
      lastDepth = currDepth;
      currDepth++;
    }
  }

  for (i = 1; i < files.size(); i++) {
    files[i]->totalChildrenWidth = -1;
    files[i]->parent->children.push_back(files[i]);
    // cerr << files[i]->name << endl;
  }



  find_dir_sizes(files[0]);

  rootSize = files[0]->size;
  // cerr << rootSize << endl;

  for(i = 1; i < files.size(); i++){ //finds the biggest dir not including the root
    if(files[i]->isDir){
      if(biggestDir == NULL || biggestDir->size < files[i]->size) biggestDir = files[i];
    }
  }

  
  //recursively draw every node
  

  files[0]->totalChildrenWidth = calcNodeWidth(files[0]);

  bool firstDir = true;
  bool firstFile = true;
  
  minX = 0;

  
  /*FIRST PAGE TREE*/
  if(mode & (1)){
    printf("newgraph xaxis size 7 nodraw yaxis size 7 nodraw \n");
    draw_nodes(files[0], 0, 0);
    for(i = 0; i < files.size(); i++){
      x = files[i]->x;
      y = files[i]->y;
      if(x < minX) minX = x;
      if(files[i]->isDir){
        if(firstDir){
          printf("newcurve color 1 0 0 marktype circle label : Directory\n pts %d %d\n", x, y);
          firstDir = false;
        }else{
          printf("newcurve color 1 0 0 marktype circle pts %d %d\n", x, y);
        }
        // printf("newstring hjr vjc rotate -45 fontsize %d x %d y %d : %d\n", fontSize, x, y, f->totalChildrenWidth);
      }else{
        if(firstFile){
          printf("newcurve color 0 1 0 marktype circle label : File\n pts %d %d\n", x, y);
          firstFile = false;
        }else{
          printf("newcurve color 0 1 0 marktype circle pts %d %d\n", x, y);
        }
      }
    }
    if(biggestFile != NULL) printf("newcurve cfill 0 1 1 marktype box label : Largest File \npts %d %d\n", biggestFile->x, biggestFile->y);
    if(biggestDir != NULL) printf("newcurve cfill 1 0 1 marktype box label : Largest Directory \npts %d %d\n", biggestDir->x, biggestDir->y);

    printf("legend defaults fontsize 14 hjl vjt x %d y 0\n", minX);
  }
  


  /*SECOND PAGE OF INFORMATION*/
  if(mode & (1 << 1)){
    if(mode & 1){
      printf("newpage\n");
    }
    printf("newgraph xaxis nodraw size 7 yaxis nodraw min -50\n");
    printf("newline poly pfill 1 pts 0 0  1 0  1 10  0 10\n");


    if(biggestDir != NULL) bdPercent = (double)biggestDir->size / (double)rootSize;
    else bdPercent = 0;
    if(biggestFile != NULL) bfPercent = (double)biggestFile->size / (double)rootSize;
    else bfPercent = 0;


    string rootStr = "Root File: " + files[0]->name;
    string sizeStr = "Total Size: " + to_string(files[0]->size) + " bytes";
    printf("newstring hjl vjb fontsize %f x %d y %d : %s\n", fontSize, 0, 15, rootStr.c_str());
    printf("newstring hjl vjt fontsize %f x %d y %d : %s\n", fontSize, 0, 14, sizeStr.c_str());


    if(biggestDir != NULL) bdStr = biggestDir->name + " " + to_string(biggestDir->size) + " bytes";
    else bdStr = "No directory found";
    if(biggestFile != NULL) bfStr = biggestFile->name + " " + to_string(biggestFile->size) + " bytes";
    else bfStr = "No file found";

    int rotateAmt = -25;
    if(bdStr.size() > 30) rotateAmt = -90;
    if(bfStr.size() > 30) rotateAmt = -90;

    printf("newline poly pcfill 1 0 0 pts 0 0  %f 0  %f 10  0 10\n", bdPercent, bdPercent);
    printf("newstring hjl vjt rotate %d fontsize %f x %f y %d : %s\n", rotateAmt, fontSize, (bdPercent/4)*3, 0, bdStr.c_str());
    printf("newline poly pcfill 0 1 0 pts 0 0  %f 0  %f 5  0 5\n", bfPercent, bfPercent);
    printf("newstring hjl vjt rotate %d fontsize %f x %f y %d : %s\n", rotateAmt, fontSize, bfPercent/2, 0, bfStr.c_str());
  }


  /*THIRD PAGE OF INFORMATION*/
  if(mode & (1 << 2)){
    if(mode & 3){
      printf("newpage\n");
    }
    printf("newgraph xaxis nodraw size 7 yaxis nodraw min -50\n");

    file_dist = get_file_dist(files);

    filesFound = 0;
    for(umit = file_dist.begin(); umit != file_dist.end(); umit++){
      filesFound += umit->second;
    }
    
    fileDistOffset = 0;
    printf("newline poly pfill 1 pts 0 0  1 0  1 10  0 10\n");
    i = 0;
    column = 0;
    for(umit = file_dist.begin(); umit != file_dist.end(); umit++){
      extPercent = (double)umit->second / (double)filesFound;
      extColor = get_random_color(umit->first);
      printf("newline poly pcfill %s pts %f 0  %f 0  %f 10  %f 10\n",extColor.c_str(),fileDistOffset,extPercent+fileDistOffset,extPercent+fileDistOffset,fileDistOffset);
      tmpX = column*0.25;
      tmpY = (-1)*(i*5)-1;
      tmpStr = to_string(extPercent*100);
      if(tmpStr.size() > 5) tmpStr = tmpStr.substr(0, 5);
      tmpStr = "-- %" + tmpStr;
      if(extPercent >= 1) tmpStr = umit->first + " - %100";
      printf("newstring hjl vjt fontsize %f x %f y %f : %s\n", fontSize, tmpX+0.01, tmpY, umit->first.c_str());
      printf("newstring hjl vjt fontsize %f x %f y %f : %s\n", fontSize-2, tmpX+0.12, tmpY, tmpStr.c_str());
      printf("newcurve marktype box cfill %s pts %f %f\n",extColor.c_str(), tmpX, tmpY-1.5);
      fileDistOffset += extPercent;
      i++;
      if(i > 25){
        column++;
        i = 0;
      }
    }
  }




  return 0;
}
