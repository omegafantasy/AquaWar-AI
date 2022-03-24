#pragma once

#include "sdk/ai_client.hpp"
using namespace std;

class AI : public AIClient
{

private:
    bool attacked;
    int try_type;
    int try_pos;
    bool is_inited = false;
    bool *is_exposed;
    int *pickfish;

public:
    AI()
    {
        is_exposed = new bool[12];
        pickfish = new int[4];
    }
    ~AI()
    {
        delete[] is_exposed;
        delete[] pickfish;
    }

    vector<int> Pick(Game game);
    pair<int, int> Assert(Game game);
    Action Act(Game game);

    //行动
    int get_trueid(int pos);
    void act(int tactic,Action& action,int pos,int fpos,int epos);
    void action_spray(int tactic,Action& action,int pos,int fpos,int epos);
    void action_flame(int tactic,Action& action,int pos,int fpos,int epos);
    void action_eel(int tactic,Action& action,int pos,int fpos,int epos);
    void action_sunfish(int tactic,Action& action,int pos,int fpos,int epos);
    void action_barracuda(int tactic,Action& action,int pos,int fpos,int epos);
    void action_mobula(int tactic,Action& action,int pos,int fpos,int epos);
    void action_turtle(int tactic,Action& action,int pos,int fpos,int epos);
    void action_octupus(int tactic,Action& action,int pos,int fpos,int epos);
    void action_whiteshark(int tactic,Action& action,int pos,int fpos,int epos);
    void action_hammerhead(int tactic,Action& action,int pos,int fpos,int epos);
    void action_clownfish(int tactic,Action& action,int pos,int fpos,int epos);
    int find_pos(int id);//找到id鱼所在位置
    int find_maxhp_ally(vector<int> ally);
    int find_minhp_ally(vector<int> ally);
    int find_minhp_friend(vector<int> ally);

    //断言
    pair<int,int> findbest_possible();//双择一行为
    bool possible(int pos, int id);//判断pos位置的鱼是否有可能是id号鱼
    void except(vector<int>& source,vector<int>& mask);
    void combine(vector<int>& source,vector<int>& mask);
    void except(vector<int>& source,vector<int>& mask,vector<int>& save);
    void counter_judge(int pos,int count);//专门检测反击
    void counter_judge2();//统一检测反击

    void enemyaction_passive(int i);
    void enemyaction_injury(int i);
    void myaction_passive(int i);
    void myaction_injury(int i);
    void judge_myaction();
    void judge_enemyaction();
    void judge_myassert();
    void judge_enemyassert();
    pair<int,int> judge();

    bool isrealtarget(int pos);
    bool isminhp_friend(int pos);
    bool islowhp_friend(int pos);
    bool isminhp_enemy(int pos);
    void turtle_mask(int pos);//海龟的保护效果
    void clownfish_mask(int pos);//小丑鱼的保护效果
    void reduce_mask();//减伤70%效果

    void updatestatus();
    void updatecount();
    void savelaststatus();//保存该轮信息

    //行动
    pair<int,int> judgenowgame();//形势判断
    bool shouldassert();//是否一定要断言
    bool balanced();//场上血量是否均衡
    int find_best_aoe();
    int find_best_crit();
    int find_best_freecrit();
    pair<int,int> find_best_normal();
    int find_best_protect();
    int find_best_infight();
    void smartact(Action& action);
    pair<int,int> get_max_atk();//能打出最高伤害的鱼及其伤害
    int friend_hp();//友军hp总和
    int enemy_hp();//敌军hp总和
    int friend_count();//友军数
    int enemy_count();//敌军数
    int enemy_realcount();//真敌军数
    int friend_assertcount();//友军被断言总次数
    int enemy_assertcount();//敌军被断言总次数

    int find_proper_enemy();//找到合适的敌军目标
    int find_crit_enemy();
    int find_proper_ally(int pos);//合适的伤害友军目标
    int find_proper_reduce();
    int find_proper_hp(int pos);
    int find_proper_hold(int pos);
    int find_proper_infighttarget(int pos);
};