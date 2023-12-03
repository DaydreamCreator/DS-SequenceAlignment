#include <cstdlib>
#include <iostream>
#include <string>
#include <algorithm>

// pointer reference at: https://www.geeksforgeeks.org/delete-a-linked-list-node-at-a-given-position/
// 2d array reference at: https://www.appsloveworld.com/cplus/100/621/error-cannot-initialize-return-object-of-type-int-with-an-lvalue-of-type-in
using namespace std;
 // linked list - work log
struct logNode{
  std::pair<int, int> index;     // x,y to identify the worklog
  vector<vector<int>> score_table;
  vector<vector<char>> direction_table;
  logNode* next;
};

class workLog{
public:
  unsigned int workLog_size;
  logNode *log;
public:
  // constructor
  workLog(){
    workLog_size = 0;
    log = NULL;
  }

  void addLog(logNode **log, vector<vector<int>> score_table, vector<vector<char>> direction_table, int x, int y){
    logNode *w = new logNode;   
    assert(w != NULL);
    w->score_table = score_table;
    w->direction_table = direction_table;
    (w->index).first = x;
    (w->index).second = y;

    w->next = *log;     // add new logNode to the head
    workLog_size ++;
    (*log) = w; 
  }

  bool removeLog(logNode **log, int x, int y){
    assert(workLog_size > 0 && *log != NULL);
    logNode *temp = *log;

    // if only one node  => for loop can save this
    if(workLog_size == 1){
      if((temp->index).first == x && (temp->index).second == y){ // FOUND
        (*log) = NULL;
        workLog_size --;
        return true;
      } else {  // NOT FOUND
        return false;
      }
    }

    logNode *last = NULL;
    while(temp != NULL){
      if((temp->index).first == x && (temp->index).second == y){
        if(temp->next != NULL){
          last->next = temp->next;
        } else {           // if is the last node
          last->next = NULL;  
        }
        delete temp;
        workLog_size --;
        return true;       // remove log successfully
      }
      last = temp;
      temp = temp->next;
    }
    return false;      
  }

  logNode *getLog(logNode **log, int x, int y){
    assert(*log != NULL && workLog_size != 0);
    logNode *temp = *log;

    for(int i = 0; i < workLog_size; i++){
      if((temp->index).first == x && (temp->index).second == y){
        return temp;
      }
      temp = temp->next;
    }
    return NULL;
  }

  void deleteWorkLog(logNode **log){
    assert(*log  !=  NULL); 

    logNode *temp = *log;
    logNode *last = NULL;
    while(temp != NULL){
      last = temp;
      temp = temp->next;
      delete last;
    }
    (*log) = NULL;
    workLog_size = 0;
  }

};
