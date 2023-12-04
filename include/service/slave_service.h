#ifndef __SLAVE_SERVICE_H__
#define __SLAVE_SERVICE_H__
#include "service/abstract_service.h"
#include "model/ScoreMatrixTask.hpp"
#include "model/ScoreMatrixTaskResponse.hpp"
#include "model/TracebackTask.hpp"
#include "model/TracebackTaskResponse.hpp"
#include "model/workLog.hpp"
#include "nlohmann/json.hpp"

using namespace std;
using nlohmann::json;

class SlaveService : public AbstractService {
   public:
    SlaveService(AbstractController* controller) : AbstractService(controller) {}
    virtual void onInit() override;
    virtual void onNewMessage(std::string peer_id, const std::string& message,
                              bool is_binary) override;
    virtual void onConnectionEstablished(const std::string peer_id) override;
    virtual void onConnectionTerminated(const std::string peer_id) override;

    bool compute_score();   // compute the score matrix
    bool trace_back();  // TODO : return  

    workLog w;     // work log for scored matrix

   private:
    string matched_sequence_;    // different variable name with tbTaskResponse

    string sequence_row_
    string sequence_column_;

    vector<int> top_row_;
    vector<int> left_column_;
    vector<int> bottom_row_;
    vector<int> right_column_;
    int left_top_element_;

    int x_;
    int y_;
    int max_score_;
    int max_score_x_;
    int max_score_y_;

    int start_x_;
    int start_y_;
    int end_x_;
    int end_y_;


    int gap_open_;
    int match_score_;
    int mismatch_pentalty_;
    int gap_extra_;

    unsigned int smTaskLength = 12;
    unsigned int tbTaskLength = 5;

    void onScoreMatrixTask(string peer_id, json  &j);
    void onTracebackTask(string peer_id, json &j);

    // TODO
   
};

#endif
