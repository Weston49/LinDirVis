#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

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

string getFileName(string s){ //this *should* work regarless of spaces in filenames, but may not if the filename is especially odd
  istringstream sin(s);
  string ret = "";
  int i = 0;
  while(sin >> s){
    i++;
    if(i >= 9){
      ret += s;
      ret += " ";
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

int calcNodeWidth(File* f){ //calculates how much horizontal space this node should take up in the tree drawing
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

/*Small note, currently directory sizes are 0, this can be fixed but for *most* practical purposes its lost in the noise*/
long long find_dir_sizes(File* f){ //adds up the sizes of all the files recursively and sets the dir file sizes accordingly
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

void draw_nodes(File *f, int x, int y){ //recursively finds the x and y positions each node will go, and draws the lines
  int i, offset;
  int spacing = 3;
  int fontSize = 12;
  int widthNeeded = 0;
  int vertOffset = 1;
  int curRightOffset = 0;
  vector<double> spacePercents;
  f->x = x;
  f->y = y;
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
  }
}

string get_extention(string s){ //get anything after the last '.' in a string and pretends that is the true file extention
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

unordered_map<string, int> get_file_dist_amt(vector<File*> files){ //gets all of the file extention amounts, does not sort them
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

unordered_map<string, int> get_file_dist_size(vector<File*> files){ //gets how many bytes each type of file takes up in total, also not sorted
  unordered_map<string, int> ret;
  int i;
  string tmp;
  for(i = 0; i < files.size(); i++){
    tmp = get_extention(files[i]->name);
    if(files[i]->isDir && tmp == "other"){
      //we do not count regular directories
    }else{
      if(ret.find(tmp) != ret.end()){
        ret.find(tmp)->second += files[i]->size;
      }else{
        ret.insert(make_pair(tmp,1));
      }
    }
  }
  return ret;
}


string get_random_color(string seed){ //gets the same color every time for a given string, so not really random but good enough
  string ret;
  int i;
  int seedVal = 0;
  ostringstream out;

  for(i = 0; i < seed.size(); i++){
    seedVal += seed[i];
  }

  srand(seedVal);
  int junk = rand(); //the val was always 0 for some reason so just throwing it away here
  double r = (double)rand() / RAND_MAX;
  double g = (double)rand() / RAND_MAX;
  double b = (double)rand() / RAND_MAX;
  
  out.precision(2);
  out << fixed << r << " " << g << " " << b;

  ret = out.str();

  return ret;
}

string get_readable_filesize(long bytes){
  string ret;
  if(bytes < 1000) return to_string(bytes) + " B";
  if(bytes < 1000000){
    ret = to_string(bytes / 1000.0);
    ret = ret.substr(0, ret.find('.')+3) + " KB";
    return ret;
  }
  if(bytes > 500000000){
    ret = to_string(bytes / 1000000000.0);
    ret = ret.substr(0, ret.find('.')+3) + " GB";
    return ret;
  }
  if(bytes < 1000000000){
    ret = to_string(bytes / 1000000.0);
    ret = ret.substr(0, ret.find('.')+3) + " MB";
    return ret;
  }
  ret = to_string(bytes / 1000000000.0);
  ret = ret.substr(0, ret.find('.')+3) + " GB";
  return ret;
}

int main(int argc, char **argv) {
  string s, bdStr, bfStr, extColor, rootStr, sizeStr;

  vector<string> v;
  vector<File *> files;
  vector<File *> parentTrace;

  File *currParent;
  File *f;
  File *biggestFile = new File;
  File *biggestDir = new File;
  File *senti = new File;
  senti->name = "///SENTI///";

  int i, j, rootDepth, currDepth, lastDepth, x, y, column, minX, mode, offset, filesFound;
  double tmpX, tmpY, bfPercent, bdPercent, fileDistOffset, extPercent;
  long long rootSize; //long long should be overkill for size in bytes, right?
                      //
  unordered_map<string, int> file_dist;
  unordered_map<string, int>::iterator umit;

  bool firstDir = true;
  bool firstFile = true;

  currDepth = 0;
  double fontSize = 14;

  if(argc > 2){
    mode = stoi(argv[2]);
  }else{
    mode = 31; //this will do everything by default
  }

  /*Setting up the root node as the filepath you enter*/
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

  biggestFile->size = 0;

  while (getline(cin, s)) {
    if(s.size() == 0) continue;
    if (s[0] == '-') { //does not handle solf links or hard links as of now, might cause weird behavior
      f = new File;
      f->name = getFileName(s);
      f->size = stol(getNthWord(5, s));
      f->isDir = false;
      f->depth = currDepth;
      if(f->size > biggestFile->size) biggestFile = f;
      if (parentTrace.size() > 0) {
        f->parent = parentTrace[parentTrace.size() - 1];
      } else {
        f->parent = senti;
      }
      files.push_back(f);
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
      currParent->size = 0;
      currParent->name = s;
      currParent->depth = currDepth;
      files.push_back(currParent);
      parentTrace.push_back(currParent);
      lastDepth = currDepth;
      currDepth++;
    }
  }

  /*Only the parent links have been set, this sets all the child links*/
  for (i = 1; i < files.size(); i++) {
    files[i]->totalChildrenWidth = -1;
    files[i]->parent->children.push_back(files[i]);
  }

  find_dir_sizes(files[0]);

  rootSize = files[0]->size;

  biggestDir->size = 0;

  for(i = 1; i < files.size(); i++){ //finds the biggest dir not including the root
    if(files[i]->isDir){
      if(biggestDir->size < files[i]->size) biggestDir = files[i];
    }
  }

  files[0]->totalChildrenWidth = calcNodeWidth(files[0]);

  minX = 0;

  /*FIRST PAGE File tree vizualization, my favorite part personally*/
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
  
  /*SECOND PAGE Largest file and directory information*/
  if(mode & (1 << 1)){
    if(mode & 1){
      printf("newpage\n");
    }
    printf("newgraph xaxis nodraw size 7 yaxis nodraw min -50\n");
    printf("newline poly pcfill 1 1 0 pts 0 0  1 0  1 10  0 10\n");

    bdPercent = 0;
    bdPercent = 0;

    if(biggestDir != NULL) bdPercent = (double)biggestDir->size / (double)rootSize;
    if(biggestFile != NULL) bfPercent = (double)biggestFile->size / (double)rootSize;
    if((double)rootSize == 0){
      bdPercent = 0;
      bfPercent = 0;
    }

    rootStr = "Root File: " + files[0]->name;
    sizeStr = "Total Size: " + get_readable_filesize(files[0]->size);
    printf("newstring hjl vjb fontsize %f x %d y %d : %s\n", fontSize, 0, 12, rootStr.c_str());
    printf("newline poly pcfill 1 1 0 pts 0.02 17  0.04 17  0.04 20  0.02 20\n");
    printf("newstring hjl vjb fontsize %f x %f y %d : %s\n", fontSize, 0.05, 17, sizeStr.c_str());


    printf("newline poly pcfill 1 0 1 pts 0.02 27  0.04 27  0.04 30  0.02 30\n");
    printf("newstring hjl vjb fontsize %f x %f y %d : %s\n", fontSize, 0.05, 27, "Largest Directory");
    printf("newline poly pcfill 0 1 1 pts 0.02 22  0.04 22  0.04 25  0.02 25\n");
    printf("newstring hjl vjb fontsize %f x %f y %d : %s\n", fontSize, 0.05, 22, "Largest File");

    bdStr = "No directory found";
    bfStr = "No file found";

    if(biggestDir != NULL) bdStr = biggestDir->name + " " + get_readable_filesize(biggestDir->size);
    if(biggestFile != NULL) bfStr = biggestFile->name + " " + get_readable_filesize(biggestFile->size);

    int rotateAmt = -25;
    if(bdStr.size() > 30) rotateAmt = -90;
    if(bfStr.size() > 30) rotateAmt = -90;

    printf("newline poly pcfill 1 0 1 pts 0 0  %f 0  %f 10  0 10\n", bdPercent, bdPercent);
    printf("newstring hjl vjt rotate %d fontsize %f x %f y %d : %s\n", rotateAmt, fontSize, (bdPercent/4)*3, 0, bdStr.c_str());
    printf("newline poly pcfill 0 1 1 pts 0 0  %f 0  %f 5  0 5\n", bfPercent, bfPercent);
    printf("newstring hjl vjt rotate %d fontsize %f x %f y %d : %s\n", rotateAmt, fontSize, bfPercent/2, 0, bfStr.c_str());
  }


  /*THIRD PAGE File ext dist by file count*/
  if(mode & (1 << 2)){
    if(mode & 3){
      printf("newpage\n");
    }
    printf("newgraph xaxis nodraw size 7 yaxis nodraw min -50\n");

    file_dist = get_file_dist_amt(files);

    filesFound = 0;
    for(umit = file_dist.begin(); umit != file_dist.end(); umit++){
      filesFound += umit->second;
    }
    
    fileDistOffset = 0;
    printf("newstring hjl vjb fontsize %f x 0 y 12 : %s\n", fontSize, "File Distribution by File Count");
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
      if((extPercent*100) < 10) tmpStr = "0" + tmpStr;
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

  /*FOURTH PAGE File ext dist by file sizes*/
  if(mode & (1 << 3)){
    if(mode & 7){
      printf("newpage\n");
    }
    printf("newgraph xaxis nodraw size 7 yaxis nodraw min -50\n");

    file_dist = get_file_dist_size(files);

    filesFound = 0;
    for(umit = file_dist.begin(); umit != file_dist.end(); umit++){
      filesFound += umit->second;
    }
    
    fileDistOffset = 0;
    printf("newstring hjl vjb fontsize %f x 0 y 12 : %s\n", fontSize, "File Distribution by File Size");
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
      if((extPercent*100) < 10) tmpStr = "0" + tmpStr;
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
