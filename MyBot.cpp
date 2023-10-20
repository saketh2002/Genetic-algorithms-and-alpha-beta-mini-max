#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
#include <limits.h>
#include <algorithm> 
#include <time.h>
using namespace std;
using namespace Desdemona;

class MyBot: public OthelloPlayer
{
    public:
        /**
         * Initialisation routines here
         * This could do anything from open up a cache of "best moves" to
         * spawning a background processing thread. 
         */
        Turn turn;
        int depth;
        MyBot( Turn turn );

        /**
         * Play something 
         */
        virtual Move play( const OthelloBoard& board );
        float alphaBeta(OthelloBoard& board, int d, float alpha, float beta,clock_t start, Turn t);
        float evaluationFunc(OthelloBoard& board,Turn t);
    private:
};

MyBot::MyBot( Turn turn )
    : OthelloPlayer( turn )
{
    this->turn = turn;
}

Move MyBot::play( const OthelloBoard& board )
{   
    clock_t start,end;
    start = clock();
    list<Move> moves = board.getValidMoves(turn);
    list<Move>::iterator last_move = moves.begin();
    int b_count,r_count,d;
        b_count = board.getBlackCount();
        r_count = board.getRedCount();
    if(b_count+r_count < 50){
        d = 7;
    }
    else{
        d = 40;
    }
    for(int i = 3; i<d; i++)
    {
        this->depth = i;
        list<Move> moves = board.getValidMoves(turn);
        list<Move>::iterator it = moves.begin();
        OthelloBoard temp_board;
        float alpha = INT_MIN, beta = INT_MAX;
        list<Move>::iterator final_move = moves.begin();
        float val=0; 
        for (it = moves.begin(); it != moves.end(); it++){
            temp_board = board;
            temp_board.makeMove(turn, *it);
            Turn next_turn;
            next_turn = turn==BLACK?RED:BLACK;
            val = alphaBeta(temp_board, 1, alpha, beta, start , next_turn);
            if(val > alpha){
                alpha = val;
                final_move = it;
            }
            end = clock();
            if((float)(end-start)/CLOCKS_PER_SEC  > 1.9){
                return *final_move;
            }   
        }
        last_move = final_move;
    }
    return *last_move;
}

float MyBot::alphaBeta(OthelloBoard& board, int d, float alpha, float beta,clock_t start, Turn t){
    clock_t end = clock();
    if((float)(end-start)/CLOCKS_PER_SEC > 1.9){
        return INT_MIN;
    }
    if(d == depth) return evaluationFunc(board, t);

    list<Move> moves;
    OthelloBoard temp_board;
    list<Move>::iterator it;
    Turn curr_turn = t;
    //J is max node
    if(t == turn){
        t = t==BLACK?RED:BLACK;
        moves = board.getValidMoves(curr_turn);
        if(moves.size() == 0) return evaluationFunc(board , curr_turn);
        for(it = moves.begin(); it != moves.end(); it++){
            temp_board = board;
            temp_board.makeMove(curr_turn,*it);
            alpha = std::max(alpha, alphaBeta(temp_board, d+1, alpha, beta, start, t));
            if(alpha >= beta) return beta;
        }
        return alpha;
    }
    //J is min node
    else{
        t = t==BLACK?RED:BLACK;
        moves = board.getValidMoves( curr_turn );
        if(moves.size() == 0) return evaluationFunc(board , curr_turn);
        for(it = moves.begin(); it != moves.end(); it++){
            temp_board = board;
            temp_board.makeMove(curr_turn,*it);
            beta = std::min(beta, alphaBeta(temp_board, d+1, alpha, beta, start, t));
            if(alpha >= beta) return alpha;
        }
        return beta;
    }
}

float MyBot::evaluationFunc(OthelloBoard& board , Turn t){
    int b_count,r_count;
    //disc difference
    b_count = board.getBlackCount();
    r_count = board.getRedCount();
    int count_diff = (b_count-r_count);
    if (t == RED){
        count_diff = -count_diff;
    }
    //mobility
    int curr_move_no = b_count + r_count - 4;
    list<Move> moves1 = board.getValidMoves(BLACK);
    list<Move> moves2 = board.getValidMoves(RED);
    Turn opp_turn;
    opp_turn = (t == BLACK)? RED:BLACK;
    int move_diff;
    move_diff = (moves1.size()-moves2.size());
    if(t == RED)
        move_diff = -move_diff;
    //corners
    int corners[4][2] = {{0,0},{0,7},{7,0},{7,7}};
    int blackcorners = 0;
    int redcorners = 0;
    for(int i=0 ; i<4 ; i++){
        Turn temp = board.get(corners[i][0] , corners[i][1]);
        if(temp == BLACK) blackcorners++;
        else redcorners++;
    }
    int corner_val = (blackcorners - redcorners);
    if(t == RED) 
        corner_val = -corner_val;
    //parity 
    int parity_val = (60-(curr_move_no))%2?-1:1;
    if (move_diff == 0){
        if (count_diff == 0){
            return 10000;
        }
        if (count_diff > 0){
            return (10000+10*count_diff);
        }
        else{
        	return -(10000-10*count_diff);
        }
    }
    int startgame[8][8] = { {1000, -50, 1, 1, 1, 1, -50, 1000},
                        {-50, -100, 0, 0, 0, 0, -100, -50},
                        {1, 0, 0, 0, 0, 0, 0, 1},
                        {1, 0, 0, 0, 0, 0, 0, 1},
                        {1, 0, 0, 0, 0, 0, 0, 1},
                        {1, 0, 0, 0, 0, 0, 0, 1},
                        {-50, -100, 0, 0, 0, 0, -100, -50},
                        {1000, -50, 1, 1, 1, 1, -50, 1000}
                      };
    int midgame[8][8] = { {1000, -50, 10, 10, 10, 10, -50, 1000},
                        {-50, -100, 0, 0, 0, 0, -100, -50},
                        {10, 0, 5, 0, 0, 0, 5, 10},
                        {10, 0, 0, 5, 0, 5, 0, 10},
                        {10, 0, 0, 0, 5, 0, 0, 10},
                        {10, 0, 0, 5, 0, 5, 0, 10},
                        {-50, -100, 0, 0, 0, 0, -100, -50},
                        {1000, -50, 10, 10, 10, 10, -50, 1000}
                      };                 
    int v1 = 0;
    int v2 = 0;
    if(curr_move_no <= 34)
    {
        for (int i = 0; i < 8; i++) 
        {
            for (int j = 0; j < 8; j++) 
            {
                if(board.get(i,j) == t) v1 += startgame[i][j];
                if(board.get(i,j) == opp_turn) v2 += startgame[i][j];
            }
        }
        return ((v1-v2)+(20*move_diff)-count_diff + 1000*corner_val);
    }
    else
    {
        for (int i = 0; i < 8; i++) 
        {
            for (int j = 0; j < 8; j++) 
            {
                if(board.get(i,j) == t) v1 += midgame[i][j];
                if(board.get(i,j) == opp_turn) v2 += midgame[i][j];
            }
        }
        return ((v1-v2)+(move_diff)+count_diff+ 100*parity_val + 1000*corner_val);
    }   
}   
// The following lines are _very_ important to create a bot module for Desdemona

extern "C" {
    OthelloPlayer* createBot( Turn turn )
    {
        return new MyBot( turn );
    }

    void destroyBot( OthelloPlayer* bot )
    {
        delete bot;
    }
}
