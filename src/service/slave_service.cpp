#include "service/slave_service.h"
#include "controller/abstract_controller.h"


using namespace std;
using nlohmann::json;

void SlaveService::onInit() {

}

void SlaveService::onNewMessage(std::string peer_id, const std::string& message,
                                bool is_binary) {
    // load msg from json file 
    json j;
    try{
        j = json::parse(message);
    } catch (const json::exception &e){
        return ;   // TODO
    }

    // identify the task type
    string object_type =  j["type"].template get<string>(); // j["type"] = "ScoreMatrixTask";
    if(object_type == "ScoreMatrixTask"){
        onScoreMatrixTask(peer_id, j);
    } else if (object_type == "TracebackTask"){
        onTracebackTask(peer_id, j);
    } else{
        return ;  
    }
  // not consider overlapping tasks problems

}

void SlaveService::onConnectionEstablished(const std::string peer_id) {}

void SlaveService::onConnectionTerminated(const std::string peer_id) {}



bool SlaveService::compute_score(){
    // todo: record new variables

    int row_len = sequence_row_.size();
    int col_len = sequence_column_.size();

    // generate score_table and direction_table, extra col and row are needed
    vector<vector<int>> score_table(row_len + 1, vector<int>(col_len + 1));
    vector<vector<char>> direction_table(row_len + 1, vector<char>(col_len + 1));

    // fill in the first row, first col and diagnol value from j
    score_table[0][0]= left_top_element_;

    if(row_len == top_row_.size() || col_len == left_column_.size())    // wrong data size
        return false;

    // temporarily copy vector in a cumbersome way

    for(int i = 1; i < row_len + 1; i ++) {
      score_table[i][0] = top_row_[i-1];
      direction_table[i][0] = 'u';
    }
    for(int j = 1; j < col_len + 1; j ++) {
      score_table[0][j] = left_column_[j-1];
      direction_table[0][j] = 'l';
    }
    
    cout<< score_table.size()<< endl;

    int i_max = 0;
    int j_max = 0;
    int max_value = 0;
    for(int i = 1; i < row_len + 1; i ++) {
      for(int j = 1; j < col_len + 1; j ++) {
        int diagonal_extra = sequence_row_.at(i-1) == sequence_column_.at(j-1) ? match_score_ : mismatch_pentalty_;
        int left_extra = direction_table[i][j-1] == 'l' ? (gap_extra_) : (gap_extra_ + gap_open_);
        int upp_extra = direction_table[i-1][j] == 'u'? (gap_extra_) : (gap_extra_ + gap_open_);

        int diag = score_table[i-1][j-1] + diagonal_extra;
        int left = score_table[i][j-1] + left_extra;
        int upp = score_table[i-1][j] + upp_extra;

        // fill in the score table
        score_table[i][j] = max(max(0, diag), max(left, upp));

        // fill in the direction table according to the score value
        if(score_table[i][j] == 0){
          direction_table[i][j] = '0';
        } else if (score_table[i][j] == diag){
          direction_table[i][j] = 'd';
        } else if (score_table[i][j] == left){
          direction_table[i][j] = 'l';
        } else {
          direction_table[i][j] = 'u';
        }

        if(score_table[i][j] > max_value){
          i_max = i;
          j_max = j;
          max_value = score_table[i][j];
        }
      }
    }

    // print score_table
    cout<< i_max << ", " << j_max << endl;
    cout<< max_value << endl;
    for(int j = 1; j < col_len + 1; j++){
      for(int i = 1; i < row_len + 1; i++){
        printf("%d", score_table[i][j]);
      }
      printf("\n");
    }
    // add the score_table and direction_table to the workLog
    w.addLog(&w.log, score_table, direction_table, x_, y_);

    // load the variable values
    bottom_row_.clear();
    for(int j = 1; j < col_len+1 ; j++){
      bottom_row_.push_back(score_table[row_len][j]);
    }
    
    right_column_.clear();
    for(int i = 1 ; i < row_len+1; i++){
        right_column_.push_back(score_table[i][col_len]);
    }

    max_score_ = max_value;
    max_score_x_ = i_max;
    max_score_y_ = j_max;

    return true;
}

bool SlaveService::trace_back(){
    string matched1;  // matched sequence in pattern
    string matched2;  // matched sequence in database
    int i = start_x_, j = start_y_;

    if(w.workLog_size <= 0){
      // raise error to the master
      return false;
    }
    logNode *l = w.getLog(&w.log, x_, y_);   // get the log according to x_ and y_
    if(l == NULL)
      return false;  // raise error to master

    vector<vector<int>> score_table = l->score_table;
    vector<vector<char>> direction_table = l->direction_table;


    while(score_table[i][j] != 0 && i > 0 && j > 0){      // TODO: check
      if(direction_table[i][j] == 'd'){
        matched1.insert(0, 1, sequence_row_.at(i-1));
        matched2.insert(0, 1, sequence_column_.at(j-1));
        i--; j--;
      } else if (direction_table[i][j] == 'l'){
        matched1.insert(0, 1, '-');
        matched2.insert(0, 1, sequence_column_.at(j-1));
        j--;
      } else if (direction_table[i][j] == 'u'){
        matched1.insert(0, 1, sequencze_row_.at(i-1));
        matched2.insert(0, 1, '-');
        i--;
      } else {
        break;
      }
    }
    // print tables
    cout << matched1 << endl;
    cout << matched2 << endl;

    response->end_x_ = i;
    response->end_y_ = y;
    matched_sequence_ = matched2;

    return true;
}

// cope with task
void SlaveService::onScoreMatrixTask(string peer_id, json  &j){
    auto request = make_shared<ScoreMatrixTask>();  
    request->loadFromJsonObject();
    assert(j.size() == smTaskLength); 

    sequence_row_ = request->sequence_row_;
    sequence_column_ = request->sequence_column_;
    top_row_ = request->top_row_;
    left_column_ = request->left_column_;
    left_top_element_ = request->left_top_element_;
    x_ = request->x_;
    y_ = request->y_;
    gap_open_ = request->gap_open_;
    match_score_ = request->match_score_;
    mismatch_pentalty_ = request->mismatch_pentalty_;
    gap_extra_ = request->gap_extra_;

    if(compute_score() == true){
        auto response = make_shared<ScoreMatrixTaskResponse>();

        response->bottom_row_ = bottom_row_;
        response->right_column_ = right_column_;
        response->x_ = x_;
        response->y_ = y_;
        response->max_score_ = max_score_;
        response->max_score_x_ = max_score_x_;
        response->max_score_y_ = max_score_y_;
        response->toJson();

        // TODO:  send?

    } else{
        return ;   // TODO
    }

}

void SlaveService::onTracebackTask(string peer_id, json &j){
    auto request = make_shared<TracebackTask>();
    request->loadFromJsonObject();
    assert(j.size() == tbTaskLength);

    x_ = request->x_;
    y_ = request->y_;
    start_x_ = request->start_x_;
    start_y_ = request->start_y_;

    if(trace_back() == true){
        auto response = make_shared<TracebackTaskResponse>();

        response->x_ = x_;
        response->y_ = y_;
        response->end_x_ = end_x_;
        response->end_y_ = end_y_;
        response->sequence_ = matched_sequence_;
        response->toJson();

        // TODO: send?
    } else {
        return ;  // TODO
    }
}
