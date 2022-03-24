#include "Action.hpp"
#include <algorithm>
#include<memory>
#include<random>
#include<string>
#include<cstring>
#include<cmath>
#include<chrono>
#define Act1 act(1,action,pos,fpos,epos)
#define Act2 act(2,action,pos,fpos,epos)
using namespace std;

/*
int AI::find_proper_enemy();
int AI::find_crit_enemy();
int AI::find_proper_ally(int pos);
int AI::find_proper_reduce(int pos);
int AI::find_proper_hp(int pos);
int AI::find_proper_hold();
int AI::friend_count();
int AI::enemy_count();*/

void TLE(){
    while(1)
        int k=1;
}
void rand_sign(){//随机化
    for(int i=1;i<=100000000;i++)
        int j=1;
    int k;
    void* tmp=&k;
    int j=*(int*)tmp;
    srand(chrono::high_resolution_clock::now().time_since_epoch().count()+j);
}
//tactic:  1.plain  2.active  3.protect  4.outrage
//Game nullgame(Fish(), Fish(), Fish(), Fish(), Fish(), Fish(), Fish(), Fish(), 1, ActionInfo(), ActionInfo(), AssertInfo(), AssertInfo(), 0, 0, 0);
//Action action(nullgame);

int AI::find_maxhp_ally(vector<int> ally){//找到队友中最多血的（需要有队友）
    if(ally.size()==3){
        if(game->my_fish[ally[0]].hp>=game->my_fish[ally[1]].hp&&game->my_fish[ally[0]].hp>=game->my_fish[ally[2]].hp)
            return ally[0];
        if(game->my_fish[ally[1]].hp>=game->my_fish[ally[0]].hp&&game->my_fish[ally[1]].hp>=game->my_fish[ally[2]].hp)
            return ally[1];
        if(game->my_fish[ally[2]].hp>=game->my_fish[ally[0]].hp&&game->my_fish[ally[2]].hp>=game->my_fish[ally[1]].hp)
            return ally[2];
    }
    else if(ally.size()==2){
        if(game->my_fish[ally[0]].hp>=game->my_fish[ally[1]].hp)
            return ally[0];
        return ally[1];
    }
    else
        return ally[0];
}
int AI::find_minhp_ally(vector<int> ally){//找到队友血最少的(需要有队友)
    if(ally.size()==3){
        if(game->my_fish[ally[0]].hp<=game->my_fish[ally[1]].hp&&game->my_fish[ally[0]].hp<=game->my_fish[ally[2]].hp)
            return ally[0];
        if(game->my_fish[ally[1]].hp<=game->my_fish[ally[0]].hp&&game->my_fish[ally[1]].hp<=game->my_fish[ally[2]].hp)
            return ally[1];
        return ally[2];
    }
    else if(ally.size()==2){
        if(game->my_fish[ally[0]].hp<=game->my_fish[ally[1]].hp)
            return ally[0];
        return ally[1];
    }
    else
        return ally[0];
}
int AI::find_minhp_friend(vector<int> ally){//找到己方血最少的
    if(ally.size()==4){
        if(game->my_fish[ally[0]].hp<=game->my_fish[ally[1]].hp&&game->my_fish[ally[0]].hp<=game->my_fish[ally[2]].hp&&game->my_fish[ally[0]].hp<=game->my_fish[ally[3]].hp)
            return ally[0];
        if(game->my_fish[ally[1]].hp<=game->my_fish[ally[0]].hp&&game->my_fish[ally[1]].hp<=game->my_fish[ally[2]].hp&&game->my_fish[ally[1]].hp<=game->my_fish[ally[3]].hp)
            return ally[1];
        if(game->my_fish[ally[2]].hp<=game->my_fish[ally[0]].hp&&game->my_fish[ally[2]].hp<=game->my_fish[ally[1]].hp&&game->my_fish[ally[2]].hp<=game->my_fish[ally[3]].hp)
            return ally[2];
        return ally[3];
    }
    else if(ally.size()==3){
        if(game->my_fish[ally[0]].hp<=game->my_fish[ally[1]].hp&&game->my_fish[ally[0]].hp<=game->my_fish[ally[2]].hp)
            return ally[0];
        if(game->my_fish[ally[1]].hp<=game->my_fish[ally[0]].hp&&game->my_fish[ally[1]].hp<=game->my_fish[ally[2]].hp)
            return ally[1];
        return ally[2];
    }
    else if(ally.size()==2){
        if(game->my_fish[ally[0]].hp<=game->my_fish[ally[1]].hp)
            return ally[0];
        return ally[1];
    }
    else
        return ally[0];
}

//全局控制变量
int gameround=0;
bool isfirstassert=1;//是否初次断言
int actioncount=0;//行动次数
int assertcount=0;
int toassert=0;

//用于assert
struct LastStatus{
    int atk=100;
    int hp=400;

    LastStatus()=default;
};
struct Status{//一条鱼的状态
    bool asserted=0;//是否已经被断言成功
    bool valuable=0;//是否有活着的价值
    int id=-1;//如果确定了id则不为-1，拟态id为12
    int trueid=-1;//“真正”的id
    vector<int> possible;//可能的编号
    vector<int> truepossible;//考虑已有鱼的可能编号
    int atk=100;//攻击力
    int trueatk=50;//真攻击力
    int value=100;//价值
    int hp=400;
    int realhp=400;
    int dmg=0;//受到的伤害
    int dmgcount=0;//受伤计数
    bool action=0;//是否已经使用过主动技能

    bool ispassive=0;//是否触发被动
    bool isimmune=0;
    bool possibleturtle=0;//大概率海龟标记
    int iscounter=0;
    bool removemask_hp=0;
    bool mask_hp=0;//可能有回血被动
    bool mask_hold=0;//可能有承伤效果
    bool mask_reduce=0;//可能有减伤效果
    bool counter_sign=0;//触发反伤效果
    bool hold_sign=0;//触发承伤效果
    int immunecount=0;//无敌次数
    int aoecount=0;//aoe次数
    int critcount=0;//暴击次数
    int holdcount=0;//承伤次数

    Status()=default;
};
bool avatar_show=0;//对方拟态鱼是否已经出现
int aoepos=-1;
vector<Status> enemy;//敌方鱼
vector<Status> my;//我方鱼
vector<LastStatus> enemy_last;
vector<LastStatus> my_last;
vector<int> enemy_lasthp; 
vector<int> already;//已经确定出现的鱼
vector<int> possible_already;//大概率已经出现的鱼
vector<int> possiblecount;//各个鱼出现可能性的总次数
vector<vector<int>> possible_allycounter;
vector<int> allycounter;
void AI::except(vector<int>& source,vector<int>& mask,vector<int>& save){//source中排除mask,保存在save里
    save=source;
    for(int i=1;i<=12;i++)
        if(find(mask.begin(),mask.end(),i)!=mask.end()&&find(source.begin(),source.end(),i)!=source.end())
            save.erase(find(save.begin(),save.end(),i));
}
void AI::combine(vector<int>& source,vector<int>& mask){//交处理，保存在source里
    vector<int> tmp;
    tmp.clear();
    for(int i=1;i<=12;i++)
        if(find(mask.begin(),mask.end(),i)!=mask.end()&&find(source.begin(),source.end(),i)!=source.end())
            tmp.push_back(i);
    source.swap(tmp);
}
void AI::except(vector<int>& source,vector<int>& mask){//source中排除mask
    for(int i=1;i<=12;i++)
        if(find(mask.begin(),mask.end(),i)!=mask.end()&&find(source.begin(),source.end(),i)!=source.end())
            source.erase(find(source.begin(),source.end(),i));
}

//int mask_hp_count=0;//计数回血
//int mask_hold_count=0;//计数承伤
int mask_reduce_count=0;//计数减伤
void AI::turtle_mask(int pos){
    if(enemy[pos].mask_hp==1)
        for(int i=0;i<=3;i++)
            enemy[i].mask_hp=1;
    else{
        for(int i=0;i<=3;i++)
            enemy[i].mask_hp=1;
        enemy[pos].mask_hp=0;
    }
    //mask_hp_count=max(0,mask_hp_count);
    //mask_hp_count++;
}
void AI::clownfish_mask(int pos){
    if(enemy[pos].mask_hold==1)
        for(int i=0;i<=3;i++)
            enemy[i].mask_hold=1;
    else{
        for(int i=0;i<=3;i++)
            enemy[i].mask_hold=1;
        enemy[pos].mask_hold=0;
    }
    //mask_hold_count=max(0,mask_hold_count);
    //mask_hold_count++;
}
void AI::reduce_mask(){
    for(int i=0;i<=3;i++)
        enemy[i].mask_reduce=1;
    mask_reduce_count=max(0,mask_reduce_count);
    mask_reduce_count++;
}

bool AI::possible(int pos,int id){
    if(enemy[pos].id==-1){
        if(avatar_show==0){//拟态还未出现
            if(find(enemy[pos].possible.begin(),enemy[pos].possible.end(),id)!=enemy[pos].possible.end())
                return 1;
            return 0;
        }
        else{
            if(find(enemy[pos].truepossible.begin(),enemy[pos].truepossible.end(),id)!=enemy[pos].truepossible.end())
                return 1;
            return 0;
        }
        
    }
    else if(enemy[pos].id==12){
        if(find(enemy[pos].possible.begin(),enemy[pos].possible.end(),id)!=enemy[pos].possible.end())
            return 1;
        return 0;
    }
    else{
        if(enemy[pos].id==id)
            return 1;
        return 0;
    }
}
void AI::updatestatus(){
    if(mask_reduce_count<=0)
        for(int i=0;i<=3;i++)
            enemy[i].mask_reduce=0;
    for(int k=0;k<3;k++){
        for(int i=0;i<=3;i++){
            if(enemy[i].id!=-1){
                if(enemy[i].id!=12&&(find(already.begin(),already.end(),enemy[i].id)==already.end()))
                    already.push_back(enemy[i].id);
                continue;
            }
            combine(enemy[i].truepossible,enemy[i].possible);
            except(enemy[i].truepossible,already);
            if(enemy[i].truepossible.empty()){
                enemy[i].id=12;
                avatar_show=1;
                assertcount++;
            }
            else if(enemy[i].truepossible.size()==1){
                if(avatar_show==1){//拟态已经出现才可以断定
                    enemy[i].id=enemy[i].truepossible[0];
                    if(find(already.begin(),already.end(),enemy[i].id)==already.end())
                        already.push_back(enemy[i].id);
                    assertcount++;
                }
                else
                    if(find(possible_already.begin(),possible_already.end(),enemy[i].truepossible[0])==possible_already.end())
                        possible_already.push_back(enemy[i].truepossible[0]);
            }
        }
    }
    except(possible_already,already);

    int tobeassert=0;
    for(int i=0;i<=3;i++){
        if(enemy[i].asserted==0&&enemy[i].truepossible.size()<=1&&enemy[i].hp>0)
            tobeassert++;
        if(enemy[i].asserted==0&&enemy[i].truepossible.size()<=2&&enemy[i].hp>0)
            enemy[i].valuable=1;
    }
    for(int i=0;i<=3;i++){
        enemy[i].hp=game->enemy_fish[i].hp;
        enemy[i].realhp=enemy[i].hp-tobeassert*50;
        if(enemy[i].mask_hold){
            enemy[i].realhp=(double)enemy[i].realhp*1.4286;
        }
        if(enemy[i].mask_reduce){
            enemy[i].realhp=(double)enemy[i].realhp*3.3333;
        }
        if((possible(i,3)||possible(i,4))&&enemy_count()!=1){
            enemy[i].realhp=(double)enemy[i].realhp*1.4286;
        }
        my[i].hp=game->my_fish[i].hp;
        my[i].realhp=my[i].hp-friend_assertcount()*50;
        if(my[i].mask_hold){
            my[i].realhp=(double)my[i].realhp*1.4286;
        }
        if(my[i].mask_reduce){
            my[i].realhp=(double)my[i].realhp*3.3333;
        }
        my[i].atk=game->my_fish[i].atk;
        my[i].id=(int)(abs(game->my_fish[i].id)+0.001);
        my[i].trueid=(my[i].id==12)?game->avatar_id:my[i].id;
    }
    for(int i=0;i<=3;i++){
        random_shuffle(enemy[i].truepossible.begin(),enemy[i].truepossible.end());
    }
    for(int i=0;i<=3;i++){
        if(enemy[i].id==1||enemy[i].id==3){
            enemy[i].trueatk=max(50,25*friend_count());
        }
        else if(enemy[i].id==5){
            enemy[i].trueatk=120;
        }
        else if(enemy[i].id==7){
            enemy[i].trueatk=50;
        }
        else if(enemy[i].id==9||enemy[i].id==10){
            enemy[i].trueatk=max(enemy[i].trueatk,(int)(enemy[i].atk*1.2));
        }
        else if(enemy[i].id==11){
            enemy[i].trueatk=50;
        }
        else{
            enemy[i].trueatk=max(enemy[i].trueatk,enemy[i].atk/2);
        }

        if(my[i].trueid==1||my[i].trueid==3){
            my[i].trueatk=max(50,30*enemy_realcount());
        }
        else if(my[i].trueid==5){
            my[i].trueatk=120;
        }
        else if(my[i].trueid==7){
            my[i].trueatk=50;
        }
        else if(my[i].trueid==9||my[i].trueid==10){
            if(my[i].asserted==1)
                my[i].trueatk=my[i].atk*1.2;
            else
                my[i].trueatk=90;
        }
        else if(my[i].trueid==11){
            my[i].trueatk=50;
        }
        else{
            my[i].trueatk=max(my[i].trueatk,my[i].atk/2);
        }
    }
    //判断价值
    for(int i=0;i<=3;i++){
        enemy[i].value=enemy[i].trueatk;
        my[i].value=my[i].trueatk;
        if(enemy[i].id==1||enemy[i].id==2)
            enemy[i].value+=(enemy_count()-1)*30;
        else if(enemy[i].id==3||enemy[i].id==4)
            enemy[i].value+=15;
        else if(enemy[i].id==5)
            enemy[i].value+=40;
        else if(enemy[i].id==6||enemy[i].id==7)
            enemy[i].value+=15;
        else if(enemy[i].id==8||enemy[i].id==9)
            enemy[i].value+=0.3*enemy[i].trueatk;
        else if(enemy[i].id==10)
            enemy[i].value+=10;
        else if(enemy[i].id==11)
            enemy[i].value+=10;
        else if(enemy[i].id==12)
            enemy[i].value+=20;
        else if(enemy[i].id==-1)
            enemy[i].value+=35;
        
        if(my[i].trueid==1||my[i].trueid==2)
            my[i].value+=(friend_count()-1)*30;
        else if(my[i].id==3||my[i].id==4)
            my[i].value+=15;
        else if(my[i].trueid==5)
            my[i].value+=40;
        else if(my[i].trueid==6||my[i].trueid==7)
            my[i].value+=15;
        else if(my[i].trueid==8||my[i].trueid==9)
            my[i].value+=0.3*my[i].trueatk;
        else if(my[i].trueid==10)
            my[i].value+=10;
        else if(my[i].trueid==11)
            my[i].value+=10;
    }

}
void AI::updatecount(){
    for(int i=1;i<=11;i++)
        possiblecount[i]=0;
    for(int i=1;i<=11;i++)
        for(int j=0;j<=3;j++)
            if(possible(j,i))
                possiblecount[i]++;
}
void AI::savelaststatus(){
    for(int i=0;i<=3;i++){
        enemy_last[i].hp=get_enemy_hp(i);
        my_last[i].hp=get_my_hp(i);
        my_last[i].atk=get_my_atk(i);
        enemy_last[i].atk=enemy[i].atk;
        enemy_lasthp[i]=get_enemy_hp(i);
    }
}     


//用于act和pick
int activefish;//敌方行动的鱼
int activetype;//行动方式：0普攻1主动
skill_type activeskill;//主动技能类型
int friendtarget;//友方目标
int enemytarget;//敌方目标

vector<int> party1={8,9,3,4};
vector<int> party2={5,17,1,2};
vector<int> party3={11,6,7,10};
vector<int> party4={17,10,1,2};
vector<int> party5={11,7,5,6};
vector<int> party11={6,7,1,2};
vector<int> party12={8,11,4,17};
vector<int> party13={9,3,10,5};
vector<int> party21={9,11,10,4};
vector<int> party22={8,3,17,7};
vector<int> party23={5,6,1,2};
vector<int> order={1,2,3};
int avatar_used=0;//决定是否使用拟态鱼
int partytactic=1;

vector<int> AI::Pick(Game game){ 
    //初始化
    gameround++;
    isfirstassert=1;
    actioncount=0;
    if(gameround==1){
        already.clear();
        possible_already.clear();
    }
    if(game.last_round_finish_reason==1){
        judge_myassert();
    }
    else if(game.last_round_finish_reason==2){
        judge_myaction();
    }
    else if(game.last_round_finish_reason==3){
        judge_myaction();
        judge_enemyassert();
    }
    else if(game.last_round_finish_reason==4){
        judge_myaction();
        judge_enemyassert();
        judge_enemyaction();
    }

    enemy.clear();
    my.clear();
    enemy_last.clear();
    my_last.clear();
    enemy_lasthp.clear();
    possiblecount.clear();
    possible_allycounter.clear();
    //mask_hp_count=0;
    //mask_hold_count=0;
    mask_reduce_count=0;
    aoepos=-1;
    for(int i=0;i<=11;i++)
        possiblecount.push_back(0);
    for(int i=0;i<=3;i++){
        enemy.push_back(Status());
        my.push_back(Status());
        enemy_last.push_back(LastStatus());
        my_last.push_back(LastStatus());
        enemy_lasthp.push_back(400);
        enemy[i].possible.clear();
        enemy[i].truepossible.clear();
        for(int j=1;j<=11;j++){
            enemy[i].possible.push_back(j);
            enemy[i].truepossible.push_back(j);
        }
        except(enemy[i].possible,already,enemy[i].truepossible);//排除已经有的鱼
    }
    
    if(gameround==1){
        rand_sign();
        random_shuffle(party1.begin(),party1.begin()+2);
        random_shuffle(party1.begin()+2,party1.begin()+4);
        random_shuffle(party2.begin(),party2.begin()+2);
        random_shuffle(party2.begin()+2,party2.begin()+4);
        random_shuffle(party3.begin(),party3.end());
        random_shuffle(party4.begin(),party4.begin()+2);
        random_shuffle(party4.begin()+2,party4.begin()+4);
        random_shuffle(party5.begin(),party5.end());
        random_shuffle(party11.begin(),party11.begin()+2);
        random_shuffle(party11.begin()+2,party11.begin()+4);
        random_shuffle(party12.begin()+1,party12.end());
        random_shuffle(party13.begin()+1,party13.end());
        random_shuffle(party21.begin()+1,party21.end());
        random_shuffle(party22.begin()+1,party22.end());
        random_shuffle(party23.begin(),party23.begin()+2);
        random_shuffle(party23.begin()+2,party23.begin()+4);

        int ret=rand()%10;
        if(ret>=6){
            partytactic=0;
        }
    }
    cerr<<"partytactic: "<<partytactic<<'\n';
    if(partytactic==0){
        if(gameround==1)
            return party1;
        if(round1_win==1){
            if(gameround==2)
                return party2;
            return party3;
        }
        else{
            if(gameround==2)
                return party4;
            return party5;
        }
    }
    if(partytactic==1){
        if(gameround==1)
            return party11;
        if(gameround==2)
            return party12;
        return party13;
    }
    if(partytactic==2){
        if(gameround==1)
            return party21;
        if(gameround==2)
            return party22;
        return party23;
    }

    /*
    if(gameround==1){
        rand_sign();
        random_shuffle(party1.begin(),party1.end());
        random_shuffle(party2.begin(),party2.end());
        random_shuffle(party3.begin(),party3.end());
        //rand_sign();
        //random_shuffle(order.begin(),order.end());
        if(rand()%2==0){
            order[1]=3;
            order[2]=2;
        }
    }
    
    vector<int> pickfish;
    pickfish.clear();
    int index=gameround-1;
    if(order[index]==1){
        nowparty=1;
        return party1;
    }
    else if(order[index]==2){
        nowparty=2;
        if(avatar_used==0){//还未使用
            if(rand()%2==0){//使用拟态
                avatar_used=1;
                for(int i=0;i<=3;i++)
                    pickfish.push_back(party2[i]==5?17:party2[i]);
                return pickfish;
            }
            else{
                avatar_used=2;
                return party2;
            }
        }
        else if(avatar_used==1){//不能使用
            return party2;
        }
        else{//必须使用
            for(int i=0;i<=3;i++)
                pickfish.push_back(party2[i]==5?17:party2[i]);
            return pickfish;
        }
    }
    else if(order[index]==3){
        nowparty=3;
        if(avatar_used==0){//还未使用
            if(rand()%2==0){//使用拟态
                avatar_used=1;
                for(int i=0;i<=3;i++)
                    pickfish.push_back(party3[i]==5?17:party3[i]);
                return pickfish;
            }
            else{
                avatar_used=2;
                return party3;
            }
        }
        else if(avatar_used==1){//不能使用
            return party3;
        }
        else{//必须使用
            for(int i=0;i<=3;i++)
                pickfish.push_back(party3[i]==5?17:party3[i]);
            return pickfish;
        }
    }*/
}

bool AI::isrealtarget(int pos){
    for(int i=0;i<my_action.num_enemy_targets;i++)
        if(pos==my_action.enemy_targets[i])
            return 1;
    return 0;
}
bool AI::isminhp_friend(int pos){
    for(int i=0;i<=3;i++)
        if(my_last[i].hp>0&&(my_last[i].hp<my_last[pos].hp))
            return 0;
    return 1;
}
bool AI::islowhp_friend(int pos){
    if(my_last[pos].hp<160)
        return 1;
    return 0;
}
bool AI::isminhp_enemy(int pos){
    if(enemy[pos].hp<=0)
        return 0;
    for(int i=0;i<=3;i++)
        if(enemy[i].hp>0&&enemy[i].hp<enemy[pos].hp)
            return 0;
    return 1;
}

void AI::counter_judge2(){
    allycounter.clear();
    for(int i=0;i<=3;i++){
        if(possible(i,1)||possible(i,2)){
            allycounter.push_back(i);
        }
    }
    for(auto c:possible_allycounter){
        combine(c,allycounter);
    }
    for(auto c:possible_allycounter){
        if(c.size()==1){
            vector<int> can;
            can.clear();
            can.push_back(1);
            can.push_back(2);
            combine(enemy[c[0]].possible,can);
            possible_allycounter.erase(find(possible_allycounter.begin(),possible_allycounter.end(),c));
            break;
        }
    }
}
void AI::counter_judge(int pos,int count){
    vector<int> all;
    all.clear();
    int allysign=0;//队友中可能1或2的个数
    int index[4];
    bool clownsign=possible(pos,11);//是否有可能自身是小丑鱼
    int sureally=0;//队友中确定1或2的个数
    int sureindex[4];
    bool sureclown=0;//是否自身确定是小丑鱼
    if(enemy[pos].id==11)
        sureclown=1;
    
    for(int j=0;j<=3;j++){
        if(j!=pos&&enemy_lasthp[j]>0&&(possible(j,1)||possible(j,2))){
            index[allysign]=j;
            allysign++;
            all.push_back(j);
            if(enemy[j].id==1||enemy[j].id==2){
                sureindex[sureally]=j;
                sureally++;
            }
        }
    }
    vector<int> can;
    vector<int> notcan;
    if(count==1){
        if(allysign==0&&clownsign==1){//就是小丑鱼
            can.clear();
            can.push_back(11);
            combine(enemy[pos].possible,can);
        }
        else if(allysign==1&&clownsign==0){//队友中只有一个可能1或2
            can.clear();
            can.push_back(1);
            can.push_back(2);
            combine(enemy[index[0]].possible,can);
        }
        else if(sureally==1){//自身一定不是小丑鱼，其他队友也不是1或2
            notcan.clear();
            notcan.push_back(11);
            except(enemy[pos].possible,notcan);

            notcan.clear();
            notcan.push_back(1);
            notcan.push_back(2);
            for(int i=0;i<=3;i++)
                if(enemy_last[i].hp>0&&i!=sureindex[0]&&i!=pos)
                    except(enemy[i].possible,notcan);
        }
        else if(clownsign==0){
            possible_allycounter.push_back(all);
        }
    }
    else if(count==2){
        if(allysign==1&&clownsign==1){//自己就是小丑鱼，队友就是1或2
            can.clear();
            can.push_back(11);
            combine(enemy[pos].possible,can);
            can.clear();
            can.push_back(1);
            can.push_back(2);
            combine(enemy[index[0]].possible,can);
        }
        else if(allysign==2&&clownsign==0){//队友中两个可能1或2
            can.clear();
            can.push_back(1);
            can.push_back(2);
            combine(enemy[index[0]].possible,can);
            combine(enemy[index[1]].possible,can);
        }
        else if(sureally==1&&sureclown==1){
            notcan.clear();
            notcan.push_back(1);
            notcan.push_back(2);
            for(int i=0;i<=3;i++)
                if(enemy_last[i].hp>0&&i!=sureindex[0]&&i!=pos)
                    except(enemy[i].possible,notcan);
        }
        else if(sureally==2){//自身一定不是小丑鱼，其他队友也不是1或2
            notcan.clear();
            notcan.push_back(11);
            except(enemy[pos].possible,notcan);

            notcan.clear();
            notcan.push_back(1);
            notcan.push_back(2);
            for(int i=0;i<=3;i++)
                if(enemy_last[i].hp>0&&i!=sureindex[0]&&i!=sureindex[1]&&i!=pos)
                    except(enemy[i].possible,notcan);
        }
    }
    else if(count==3){
        if(allysign==2&&clownsign==1){//自己就是小丑鱼，队友就是1或2
            can.clear();
            can.push_back(11);
            combine(enemy[pos].possible,can);
            can.clear();
            can.push_back(1);
            can.push_back(2);
            combine(enemy[index[0]].possible,can);
            combine(enemy[index[1]].possible,can);
        }
        else if(allysign==3&&clownsign==0){//队友中两个可能1或2
            can.clear();
            can.push_back(1);
            can.push_back(2);
            combine(enemy[index[0]].possible,can);
            combine(enemy[index[1]].possible,can);
            combine(enemy[index[2]].possible,can);
        }
        else if(sureally==2&&sureclown==1){
            notcan.clear();
            notcan.push_back(1);
            notcan.push_back(2);
            for(int i=0;i<=3;i++)
                if(enemy_last[i].hp>0&&i!=sureindex[0]&&i!=sureindex[1]&&i!=pos)
                    except(enemy[i].possible,notcan);
        }
    }
}
void AI::enemyaction_passive(int i){
    vector<int> can;

    can.clear();
    int pos=enemy_action.friend_passives_id[i];
    enemy[pos].ispassive=1;

    if(enemy_action.friend_types[i]==counter){//TODO：理论不存在
        TLE();
    }
    else if(enemy_action.friend_types[i]==deflect){//TODO： 理论不存在
        TLE();
    }
    else if(enemy_action.friend_types[i]==reduce){
        double ratio=enemy_action.friend_passive_value[i];
        if(abs(ratio)<0.001){//免伤
            enemy[pos].immunecount++;
            can.push_back(5);
            can.push_back(6);
            can.push_back(7);
        }
        else{//TODO:理论不存在
            TLE();
        }
    }
    else if(enemy_action.friend_types[i]==heal){
        enemy_last[pos].hp+=20;
        enemy_last[pos].hp=min(400,enemy_last[pos].hp);
        can.push_back(8);
        can.push_back(9);
    }
    else if(enemy_action.friend_types[i]==explode){//TODO：理论不存在
        TLE();
    }
    combine(enemy[pos].possible,can);
    updatestatus();
}
void AI::enemyaction_injury(int i){
    vector<int> notcan;
    notcan.clear();
    int pos=enemy_action.friend_injury_id[i];
    int dmg=enemy_action.friend_injury[i];
    enemy[pos].dmg+=dmg;
    enemy[pos].dmgcount++;
    enemy_last[pos].hp-=dmg;

    if(enemy[pos].immunecount<3&&enemy[pos].immunecount<enemy[pos].dmgcount){//非海龟
        notcan.clear();
        notcan.push_back(7);
        except(enemy[pos].possible,notcan);
    }
    int healcount=0;
    int timestamp=100;
    for(int j=i+1;j<enemy_action.num_friend_injury;j++){
        if(pos==enemy_action.friend_injury_id[j]){
            timestamp=enemy_action.friend_injury_timestamp[j];
            break;
        }
    }
    for(int j=0;j<enemy_action.num_friend_passives;j++)
        if(enemy_action.friend_passives_id[j]==pos&&enemy_action.friend_types[j]==heal
        &&enemy_action.friend_passives_timestamp[j]<timestamp
        &&enemy_action.friend_passives_timestamp[j]>enemy_action.friend_injury_timestamp[i])//紧接着有回血
            healcount++;
    if(healcount==0&&enemy_last[pos].hp>0){//非章鱼或大白鲨
        notcan.clear();
        notcan.push_back(8);
        notcan.push_back(9);
        except(enemy[pos].possible,notcan);
    }
    updatestatus();
}
void AI::judge_enemyaction(){
    vector<int> can;
    can.clear();
    updatestatus();

    //主动
    if(enemy_action.is_skill==1){//使用主动
        activetype=1;
        activefish=enemy_action.action_fish;
        int pos=activefish;
        if(enemy_action.type==aoe){
            if(aoepos==-1)
                aoepos=pos;
            enemy[pos].trueatk=30*friend_count();
            activeskill=aoe;
            enemy[pos].aoecount++;
            if(enemy[pos].aoecount>3){
                can.push_back(1);
                can.push_back(3);
            }
            else{
                can.push_back(1);
                can.push_back(3);
                can.push_back(11);
                if(possible(pos,11))
                    clownfish_mask(pos);
            }
        }
        else if(enemy_action.type==infight){
            activeskill=infight;
            enemytarget=enemy_action.friend_targets[0];
            can.push_back(2);
            can.push_back(4);
            enemy[pos].atk+=70;
        }
        else if(enemy_action.type==crit){
            activeskill=crit;
            enemy[pos].critcount++;
            friendtarget=enemy_action.enemy_targets[0];
            int dmg=enemy_action.enemy_expected_injury[0];
            bool isminhp=isminhp_friend(friendtarget);//判断是否是最少血的队友
            bool islowhp=islowhp_friend(friendtarget);//是否会被超暴击
            
            if(dmg==120){
                enemy[pos].trueatk=120;
                if(enemy[pos].atk==100){
                    if(isminhp==1&&islowhp==0){//不一定是鲨鱼
                        can.push_back(5);
                        can.push_back(9);
                        can.push_back(10);
                        if(enemy[pos].critcount<=3){
                            can.push_back(7);
                            if(possible(pos,7))
                                turtle_mask(pos);
                        }
                    }
                    else{//不是鲨鱼
                        can.push_back(5);
                        if(enemy[pos].critcount<=3){
                            can.push_back(7);
                            if(possible(pos,7))
                                turtle_mask(pos);
                        }
                    }
                }
                else{//不是鲨鱼
                    can.push_back(5);
                    if(enemy[pos].critcount<=3){
                        can.push_back(7);
                        if(possible(pos,7))
                            turtle_mask(pos);
                    }
                }
            }
            else{//是鲨鱼
                can.push_back(9);
                can.push_back(10);
                if(islowhp)
                    enemy[pos].atk=(int)((dmg+0.001)/1.4);
                else
                    enemy[pos].atk=(int)((dmg+0.001)/1.2);
                enemy[pos].trueatk=dmg;
            }
        }
        else if(enemy_action.type==subtle){
            activeskill=subtle;
            enemy[pos].atk+=20;
            can.push_back(6);
            can.push_back(7);
            can.push_back(8);
            can.push_back(11);
            if(possible(pos,7))
                turtle_mask(pos);
            if(possible(pos,11))
                clownfish_mask(pos);
            if(possible(pos,6)||possible(pos,8))
                reduce_mask();
        }
    }
    else{//普攻
        activetype=0;
        activefish=enemy_action.action_fish;
        int pos=activefish;
        friendtarget=enemy_action.enemy_targets[0];
        int dmg=enemy_action.enemy_expected_injury[0];
        
        enemy[pos].atk=dmg*2;
        if(enemy[pos].atk==100){
            if(enemy[pos].dmg>=200){//不是3和4
                can.push_back(1);
                can.push_back(2);
                can.push_back(5);
                can.push_back(6);
                can.push_back(7);
                can.push_back(8);
                can.push_back(9);
                can.push_back(10);
                can.push_back(11);
            }
            else
                for(int i=1;i<=11;i++)
                    can.push_back(i);
        }
        else if(enemy[pos].atk==120||enemy[pos].atk==140){
            can.push_back(3);
            can.push_back(4);
            can.push_back(6);
            can.push_back(8);
        }
        else if(enemy[pos].atk%20==0){
            can.push_back(2);
            can.push_back(4);
            can.push_back(6);
            can.push_back(8);
        }
        else if(enemy[pos].atk%10==0){
            can.push_back(2);
            can.push_back(4);
        }
        else{
            can.push_back(10);
        }
    }
    combine(enemy[activefish].possible,can);
    updatestatus();
    
    //被动
    for(int i=0;i<=3;i++){
        enemy[i].ispassive=0;
    }
    int passivenum=enemy_action.num_friend_passives;
    int injurynum=enemy_action.num_friend_injury;
    int nowpassive=0;
    int nowinjury=0;
    while(nowpassive<passivenum||nowinjury<injurynum){//按时间顺序一个一个触发
        if(nowpassive<passivenum&&nowinjury<injurynum){
            if(enemy_action.friend_passives_timestamp[nowpassive]<enemy_action.friend_injury_timestamp[nowinjury]){
                enemyaction_passive(nowpassive);
                nowpassive++;
            }
            else{
                enemyaction_injury(nowinjury);
                nowinjury++;
            }
        }
        else if(nowpassive<passivenum){
            enemyaction_passive(nowpassive);
            nowpassive++;
        }
        else{
            enemyaction_injury(nowinjury);
            nowinjury++;
        }
    }
    
    /*for(int i=0;i<=3;i++){
        if(enemy[i].ispassive==0){//没有触发被动
            for(int j=0;j<enemy_action.num_friend_injury;j++){
                if(enemy_action.friend_injury_id[j]==i&&enemy_action.friend_injury[j]>0){//确实受伤
                    can.clear();
                    if(enemy[i].hp<=0){//章鱼和大白鲨不能回春
                        can.push_back(8);
                        can.push_back(9);
                    }
                    if(enemy[i].immunecount>0&&enemy[i].immunecount<3){//有无敌且不是海龟
                        can.push_back(5);
                        can.push_back(6);
                    }
                    else if(enemy[i].immunecount<3){//不是海龟
                        can.push_back(1);
                        can.push_back(2);
                        can.push_back(3);
                        can.push_back(4);
                        can.push_back(5);
                        can.push_back(6);
                        can.push_back(10);
                        can.push_back(11);
                    }
                    else{
                        can.push_back(1);
                        can.push_back(2);
                        can.push_back(3);
                        can.push_back(4);
                        can.push_back(5);
                        can.push_back(6);
                        can.push_back(7);
                        can.push_back(10);
                        can.push_back(11);
                    }
                    combine(enemy[i].possible,can);
                    updatestatus();
                    break;
                }
            }
        }
    }*/

    for(int i=0;i<enemy_action.num_enemy_passives;i++){//可能会抹去效果
        if(enemy_action.enemy_types[i]==heal){
            my[enemy_action.enemy_passives_id[i]].mask_hp=0;
        }
        else if(enemy_action.enemy_types[i]==reduce){
            if(enemy_action.enemy_passive_value[i]>0.001){//不是无敌而是减伤
                my[enemy_action.enemy_passives_id[i]].mask_reduce=0;
            }
        }
        else if(enemy_action.enemy_types[i]==deflect){
            my[enemy_action.enemy_passives_id[i]].mask_hold=0;
        }
    }
}
void AI::myaction_passive(int i){
    vector<int> can;
    can.clear();
    int pos=my_action.enemy_passives_id[i];
    int timestamp=my_action.enemy_passives_timestamp[i];
    enemy[pos].ispassive=1;

    if(my_action.enemy_types[i]==counter){
        enemy[pos].iscounter++;
        enemy[pos].counter_sign=1;
        for(int j=1;j<=11;j++)
            can.push_back(j);
    }
    else if(my_action.enemy_types[i]==deflect){
        enemy[pos].holdcount++;
        enemy[pos].hold_sign=1;
        if(enemy[pos].mask_hold==1){//无法确定
            enemy[pos].mask_hold=0;
            //mask_hold_count--;
            for(int j=1;j<=11;j++)
                can.push_back(j);
        }
        else{
            can.push_back(3);
            can.push_back(4);
        }
    }
    else if(my_action.enemy_types[i]==reduce){
        double ratio=my_action.enemy_passive_value[i];
        if(abs(ratio)<0.001){//免伤
            enemy[pos].immunecount++;
            enemy[pos].isimmune=1;
            can.push_back(5);
            can.push_back(6);
            can.push_back(7);
        }
        else{
            enemy[pos].mask_reduce=0;
            mask_reduce_count--;
            for(int j=1;j<=11;j++)
                can.push_back(j);
        }
    }
    else if(my_action.enemy_types[i]==heal){
        enemy_last[pos].hp+=20;
        enemy_last[pos].hp=min(400,enemy_last[pos].hp);
        if(isrealtarget(pos)){//是直接攻击目标
            if(enemy[pos].mask_hp==1){
                enemy[pos].mask_hp=0;
                for(int j=1;j<=11;j++)
                    can.push_back(j);
            }
            else{
                can.push_back(8);
                can.push_back(9);
            }
        }
        else{//并非直接攻击目标
            can.push_back(8);
            can.push_back(9);
        }
    }
    else if(my_action.enemy_types[i]==explode){
        can.push_back(10);
    }
    combine(enemy[pos].possible,can);
    updatestatus();
}
void AI::myaction_injury(int i){
    vector<int> can;
    vector<int> notcan;
    can.clear();
    notcan.clear();

    int pos=my_action.enemy_injury_id[i];
    int dmg=my_action.enemy_injury[i];
    enemy[pos].dmg+=dmg;
    enemy[pos].dmgcount++;
    enemy_last[pos].hp-=dmg;

    if(enemy[pos].immunecount<3&&enemy[pos].immunecount<enemy[pos].dmgcount){//非海龟
        notcan.clear();
        notcan.push_back(7);
        except(enemy[pos].possible,notcan);
    }
    int healcount=0;
    int countercount=0;
    int explodecount=0;
    int deflectcount=0;
    int timestamp=100;//下次受伤的时间
    int earlytimestamp=0;//上次受伤的时间
    for(int j=i+1;j<my_action.num_enemy_injury;j++){
        if(pos==my_action.enemy_injury_id[j]){
            timestamp=my_action.enemy_injury_timestamp[j];
            break;
        }
    }
    for(int j=i-1;j>=0;j--){
        if(pos==my_action.enemy_injury_id[j]){
            earlytimestamp=my_action.enemy_injury_timestamp[j];
            break;
        }
    }
    for(int j=0;j<my_action.num_enemy_passives;j++){
        if(my_action.enemy_passives_id[j]==pos&&my_action.enemy_types[j]==heal
        &&my_action.enemy_passives_timestamp[j]<timestamp
        &&my_action.enemy_passives_timestamp[j]>my_action.enemy_injury_timestamp[i])//紧接着有回血
            healcount++;
        else if(my_action.enemy_passives_id[j]==pos&&my_action.enemy_types[j]==counter
        &&my_action.enemy_passives_timestamp[j]<timestamp
        &&my_action.enemy_passives_timestamp[j]>my_action.enemy_injury_timestamp[i])//紧接着有反伤
            countercount++;
        else if(my_action.enemy_passives_id[j]==pos&&my_action.enemy_types[j]==explode
        &&my_action.enemy_passives_timestamp[j]<timestamp
        &&my_action.enemy_passives_timestamp[j]>my_action.enemy_injury_timestamp[i])//紧接着有爆炸
            explodecount++;
        else if(my_action.enemy_passives_id[j]==pos&&my_action.enemy_types[j]==deflect
        &&my_action.enemy_passives_timestamp[j]<my_action.enemy_injury_timestamp[i]
        &&my_action.enemy_passives_timestamp[j]>earlytimestamp)//在前面有承伤 
            deflectcount++;
    }
    if(healcount==0&&enemy_last[pos].hp>0){//非章鱼或大白鲨
        notcan.clear();
        notcan.push_back(8);
        notcan.push_back(9);
        except(enemy[pos].possible,notcan);
    }
    updatestatus();
    
    if(my_action.enemy_injury_traceable[i]){//有来源伤害
        bool flag=1;
        for(int j=0;j<my_action.num_enemy_passives;j++)
            if(my_action.enemy_passives_id[j]==pos
            &&my_action.enemy_types[j]==reduce&&my_action.enemy_passive_value[j]<0.001
            &&my_action.enemy_passives_timestamp[j]==my_action.enemy_injury_timestamp[i]-1){//恰好在这之前有无敌
                flag=0;
                break;
            }
        if(flag==0)//有无敌的话不受直接攻击
            return;

        //清空mask
        enemy[pos].mask_hold=enemy[pos].mask_reduce=0;
        if(enemy[pos].removemask_hp==1)
            enemy[pos].mask_hp=0;
        enemy[pos].removemask_hp=1;

        if(enemy_last[pos].hp+healcount*20<120&&countercount==0){//没反伤，队友没有喷火鱼、射水鱼，且自身不是小丑鱼
            for(int j=0;j<=3;j++)
                if(enemy_last[j].hp>0&&j!=pos){
                    notcan.clear();
                    notcan.push_back(1);
                    notcan.push_back(2);
                    except(enemy[j].possible,notcan);
                }
            notcan.clear();
            notcan.push_back(11);
            except(enemy[pos].possible,notcan);
        }
        if(enemy_last[pos].hp+healcount*20<=0&&explodecount==0){//没爆炸，不是锤头鲨
            notcan.clear();
            notcan.push_back(10);
            except(enemy[pos].possible,notcan);
        }
        
        bool enemyally=0;//此时是否有存活队友
        for(int j=0;j<=3;j++)
            if(enemy_lasthp[j]>0&&j!=pos)
                enemyally=1;
        if(deflectcount==0&&enemyally==1){//没承伤，不是翻车或者电鳗
            notcan.clear();
            notcan.push_back(3);
            notcan.push_back(4);
            except(enemy[pos].possible,notcan);
        }
    }
    updatestatus();
}
void AI::judge_myaction(){
    vector<int> can;
    vector<int> notcan;
    updatestatus();
    
    //被动
    for(int i=0;i<=3;i++){
        enemy[i].ispassive=0;
        enemy[i].isimmune=0;
        enemy[i].iscounter=0;
        enemy[i].holdcount=0;
        enemy[i].removemask_hp=0;
    }
    int passivenum=my_action.num_enemy_passives;
    int injurynum=my_action.num_enemy_injury;
    int nowpassive=0;
    int nowinjury=0;
    while(nowpassive<passivenum||nowinjury<injurynum){//按时间顺序一个一个触发
        if(nowpassive<passivenum&&nowinjury<injurynum){
            if(my_action.enemy_passives_timestamp[nowpassive]<my_action.enemy_injury_timestamp[nowinjury]){
                myaction_passive(nowpassive);
                nowpassive++;
            }
            else{
                myaction_injury(nowinjury);
                nowinjury++;
            }
        }
        else if(nowpassive<passivenum){
            myaction_passive(nowpassive);
            nowpassive++;
        }
        else{
            myaction_injury(nowinjury);
            nowinjury++;
        }
    }
    //enemyhealcount=max(enemyhealcount-1,tmphealcount);

    for(int k=0;k<3;k++)//多次反伤判断
        for(int i=0;i<=3;i++)
            if(enemy[i].iscounter>0)
                counter_judge(i,enemy[i].iscounter);
    /*for(int i=0;i<=3;i++){
        if(enemy[i].ispassive==0){//没有触发被动
            for(int j=0;j<my_action.num_enemy_injury;j++){
                if(my_action.enemy_injury_id[j]==i&&my_action.enemy_injury[j]>0){//确实受伤
                    can.clear();
                    if(enemy_last[i].hp-my_action.enemy_injury[j]<=0){//章鱼和大白鲨不能回春
                        can.push_back(8);
                        can.push_back(9);
                    }
                    bool isstraight=0;//是否是直接攻击目标
                    for(int k=0;k<my_action.num_enemy_targets;k++)
                        if(my_action.enemy_targets[k]==i)
                            isstraight=1;
                    if(isstraight){
                        enemy[i].mask_hp=enemy[i].mask_hold=enemy[i].mask_reduce=0;
                    }
                    
                    if(enemy[i].immunecount>0&&enemy[i].immunecount<3){//有无敌且不是海龟
                        can.push_back(5);
                        can.push_back(6);
                    }
                    else if(enemy[i].immunecount<3){//不是海龟
                        can.push_back(1);
                        can.push_back(2);
                        can.push_back(3);
                        can.push_back(4);
                        can.push_back(5);
                        can.push_back(6);
                        can.push_back(10);
                        can.push_back(11);
                    }
                    else{
                        can.push_back(1);
                        can.push_back(2);
                        can.push_back(3);
                        can.push_back(4);
                        can.push_back(5);
                        can.push_back(6);
                        can.push_back(7);
                        can.push_back(10);
                        can.push_back(11);
                    }
                    combine(enemy[i].possible,can);
                    updatestatus();
                    break;
                }
            }
        }
    }*/
    
    for(int i=0;i<my_action.num_friend_injury;i++){//可能会扣血
        my_last[my_action.friend_injury_id[i]].hp-=my_action.friend_injury[i];
    }
    for(int i=0;i<my_action.num_friend_passives;i++){//可能会加血，以及抹去效果
        if(my_action.friend_types[i]==heal){
            my[my_action.friend_passives_id[i]].mask_hp=0;
            my_last[my_action.friend_passives_id[i]].hp+=20;
            my_last[my_action.friend_passives_id[i]].hp=min(400,my_last[my_action.friend_passives_id[i]].hp);
        }
        else if(my_action.friend_types[i]==reduce){
            if(my_action.friend_passive_value[i]>0.001){//不是无敌而是减伤
                my[my_action.friend_passives_id[i]].mask_reduce=0;
            }
        }
        else if(my_action.friend_types[i]==deflect){
            my[my_action.friend_passives_id[i]].mask_hold=0;
        }
    }

    for(int i=0;i<=3;i++)
        if(enemy[i].removemask_hp)
            enemy[i].mask_hp=0;

    bool flag=1;
    for(int i=0;i<=3;i++)
        if(isrealtarget(i)==0||enemy[i].hold_sign==1||enemy[i].isimmune==1)
            flag=0;
    if(flag==1&&aoepos!=-1){//aoe的不是小丑鱼
        notcan.clear();
        notcan.push_back(11);
        except(enemy[aoepos].possible,notcan);
    }
    flag=0;
    for(int i=0;i<=3;i++)
        if(enemy[i].holdcount==2)
            flag=1;
    if(flag==1&&aoepos!=-1){//aoe的是小丑鱼
        can.clear();
        can.push_back(11);
        combine(enemy[aoepos].possible,can);
    }
}

void AI::judge_myassert(){
    if(my_assert.assertPos<0||my_assert.assertPos>3||my_assert.assertContent<1||my_assert.assertContent>12)
        return;
    if(my_assert.assertResult==0){//断言失败
        if(enemy[my_assert.assertPos].id!=-1)//TODO：不应该的情况
            TLE();
        if(my_assert.assertContent!=12)   
            enemy[my_assert.assertPos].truepossible.erase(find(enemy[my_assert.assertPos].truepossible.begin(),enemy[my_assert.assertPos].truepossible.end(),my_assert.assertContent));
        if(enemy[my_assert.assertPos].truepossible.empty()){
            possible_already.erase(find(possible_already.begin(),possible_already.end(),my_assert.assertContent));
        }
    }
    else{//断言成功
        enemy[my_assert.assertPos].asserted=1;
        enemy[my_assert.assertPos].id=my_assert.assertContent;
        if(my_assert.assertContent==12){
            avatar_show=1;
        }
        else{
            if(find(already.begin(),already.end(),my_assert.assertContent)==already.end())
                already.push_back(my_assert.assertContent);
        }
    }
    updatestatus();
}
void AI::judge_enemyassert(){
    if(enemy_assert.assertPos<0||enemy_assert.assertPos>3||enemy_assert.assertContent<1||enemy_assert.assertContent>12)
        return;
    if(enemy_assert.assertResult==0){//失败
        my[enemy_assert.assertPos].action=1;//也算被断言出来了
        for(int i=0;i<=3;i++)
            enemy_last[i].hp-=50;
    }
    else{
        for(int i=0;i<=3;i++)
            my_last[i].hp-=50;
        my[enemy_assert.assertPos].asserted=1;
    }
}
pair<int,int> AI::findbest_possible(){
    vector<int> bi;
    bi.clear();
    vector<int> targets;
    targets.clear();
    for(int i=0;i<=3;i++)
        if(enemy[i].hp>0&&enemy[i].asserted==0)
            targets.push_back(i);
    sort(targets.begin(),targets.end(),[](int a,int b){
        return enemy[a].hp<enemy[b].hp;
    });

    for(auto c:targets){//找双
        if(enemy[c].truepossible.size()==2)
            bi.push_back(c);
    }
    for(auto c:bi){//大概率成功的断言
        if(find(possible_already.begin(),possible_already.end(),enemy[c].truepossible[0])!=possible_already.end())
            return make_pair(c,enemy[c].truepossible[1]);
        else if(find(possible_already.begin(),possible_already.end(),enemy[c].truepossible[1])!=possible_already.end())
            return make_pair(c,enemy[c].truepossible[0]);
    }
    for(auto c:targets){
        if(find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),1)!=enemy[c].truepossible.end()
        &&find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),11)!=enemy[c].truepossible.end()&&enemy[c].aoecount>0){
            bool flag=0;
            for(int j=0;j<=3;j++)
                if(possible(j,3)==0&&possible(j,4)==0&&j!=c&&enemy[j].hold_sign==1)
                    flag=1;
            if(flag==1){//应该是小丑
                return make_pair(c,11);
            }
            else if(actioncount>4){
                return make_pair(c,1);
            }
        }
    }
    for(auto c:targets){//五次不免疫，大概率不是海狼、蝠鲼
        int tmp=0;
        if(find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),5)!=enemy[c].truepossible.end())
            tmp++;
        if(find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),6)!=enemy[c].truepossible.end())
            tmp++;
        if(enemy[c].truepossible.size()==tmp+1&&enemy[c].dmgcount>=5&&enemy[c].immunecount==0){
            for(int i=1;i<=11;i++)
                if(find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),i)!=enemy[c].truepossible.end()&&i!=5&&i!=6)
                    return make_pair(c,i);
        }
            
    }
    if(shouldassert()){
        sort(targets.begin(),targets.end(),[](int a,int b){
            return enemy[a].truepossible.size()<enemy[b].truepossible.size();
        });
        if(find(enemy[targets[0]].truepossible.begin(),enemy[targets[0]].truepossible.end(),10)!=enemy[targets[0]].truepossible.end())
            return make_pair(targets[0],10);
        else if(find(enemy[targets[0]].truepossible.begin(),enemy[targets[0]].truepossible.end(),2)!=enemy[targets[0]].truepossible.end())
            return make_pair(targets[0],2);
        else if(find(enemy[targets[0]].truepossible.begin(),enemy[targets[0]].truepossible.end(),1)!=enemy[targets[0]].truepossible.end())
            return make_pair(targets[0],1);
        else if(find(enemy[targets[0]].truepossible.begin(),enemy[targets[0]].truepossible.end(),11)!=enemy[targets[0]].truepossible.end())
            return make_pair(targets[0],11);
        else if(find(enemy[targets[0]].truepossible.begin(),enemy[targets[0]].truepossible.end(),6)!=enemy[targets[0]].truepossible.end())
            return make_pair(targets[0],6);
        else if(find(enemy[targets[0]].truepossible.begin(),enemy[targets[0]].truepossible.end(),4)!=enemy[targets[0]].truepossible.end())
                    return make_pair(targets[0],4);
        return make_pair(targets[0],enemy[targets[0]].truepossible[0]);
    }

    if((assertcount<=2||enemy_count()>=friend_count()+1||(judgenowgame().second<=2||judgenowgame().second==5))&&actioncount>5){
        for(auto c:bi)
            sort(enemy[c].truepossible.begin(),enemy[c].truepossible.end());
        for(int i=0;i<bi.size();i++)
            for(int j=i+1;j<bi.size();j++){
                if(enemy[bi[i]].truepossible==enemy[bi[j]].truepossible){//两个possible相等
                    return make_pair(bi[i],enemy[bi[i]].truepossible[0]);
                }
            }

        for(auto c:bi)
            random_shuffle(enemy[c].truepossible.begin(),enemy[c].truepossible.end());
        for(auto c:bi){
            if(find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),1)!=enemy[c].truepossible.end()
            &&find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),2)!=enemy[c].truepossible.end()){//更可能是喷火鱼
                return make_pair(c,2);
            }
        }
        for(auto c:bi){
            if(find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),3)!=enemy[c].truepossible.end()
            &&find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),4)!=enemy[c].truepossible.end()){//更可能是翻车鱼
                return make_pair(c,4);
            }
        }
        for(auto c:bi){
            if(find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),10)!=enemy[c].truepossible.end())
                return make_pair(c,10);
            else if(find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),6)!=enemy[c].truepossible.end())
                return make_pair(c,6);
        }
        for(auto c:bi){//有一个只出现一次
            if(possiblecount[enemy[c].truepossible[0]]==1)
                return make_pair(c,enemy[c].truepossible[0]);
            else if(possiblecount[enemy[c].truepossible[1]]==1)
                return make_pair(c,enemy[c].truepossible[1]);
        }
    }

    return make_pair(-1,-1);
}
pair<int,int> AI::judge(){
    updatestatus();
    updatecount();
    counter_judge2();
    updatestatus();
    cerr<<"possible:";
    for(int i=0;i<=3;i++){
        cerr<<i<<" {";
        for(int j=0;j<(int)enemy[i].truepossible.size()-1;j++)
            cerr<<enemy[i].truepossible[j]<<",";
        if(enemy[i].truepossible.size()>0)
            cerr<<enemy[i].truepossible[enemy[i].truepossible.size()-1];
        cerr<<"} ";
    }
    cerr<<"possible_already: {";
    for(int j=0;j<(int)possible_already.size()-1;j++)
        cerr<<possible_already[j]<<",";
    if(possible_already.size()>0)
        cerr<<possible_already[possible_already.size()-1];
    cerr<<"} "<<'\n';   

    pair<int,int> nowgame=judgenowgame();
    vector<int> targets;
    vector<int> lowhp;
    targets.clear();
    lowhp.clear();
    for(int i=0;i<=3;i++)
        if(enemy[i].hp>0&&enemy[i].asserted==0)
            targets.push_back(i);
    for(auto c:targets){
        if(enemy[c].hp<=50)
            lowhp.push_back(c);
    }
    
    for(auto c:targets){//三次免疫，大概率是海龟
        if(find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),7)!=enemy[c].truepossible.end()&&enemy[c].immunecount>=3){
            enemy[c].possibleturtle=1;
        }
    }

    //应优先断言低血
    if(nowgame.first<=2&&(nowgame.second>=4||nowgame.second<=2)){//优先确认身份
        for(auto c:lowhp){
            if(enemy[c].truepossible.size()==2){
                if(find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),10)!=enemy[c].truepossible.end())
                    return make_pair(c,10);
                else if(find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),2)!=enemy[c].truepossible.end())
                    return make_pair(c,2);
                else if(find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),1)!=enemy[c].truepossible.end())
                    return make_pair(c,1);
                else if(find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),11)!=enemy[c].truepossible.end())
                    return make_pair(c,11);
                else if(find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),6)!=enemy[c].truepossible.end())
                    return make_pair(c,6);
                else if(find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),4)!=enemy[c].truepossible.end())
                    return make_pair(c,4);
                return make_pair(c,enemy[c].truepossible[0]);
            }
        }
        for(auto c:lowhp){
            if(enemy[c].id!=-1)
                return make_pair(c,enemy[c].id);
        }
        for(auto c:lowhp){
            if(enemy[c].possibleturtle==1){
                if(find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),7)!=enemy[c].truepossible.end())
                    return make_pair(c,7);
                else if(avatar_show==0)
                    return make_pair(c,12);
            }
        }
        for(auto c:lowhp){
            if(enemy[c].truepossible.size()==1){
                return make_pair(c,enemy[c].truepossible[0]);
            }
        }
    }
    else{//优先取得胜利
        for(auto c:lowhp){
            if(enemy[c].id!=-1)
                return make_pair(c,enemy[c].id);
        }
        for(auto c:lowhp){
            if(enemy[c].possibleturtle==1){
                if(find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),7)!=enemy[c].truepossible.end())
                    return make_pair(c,7);
                else if(avatar_show==0)
                    return make_pair(c,12);
            }
        }
        for(auto c:lowhp){
            if(enemy[c].truepossible.size()==1){
                return make_pair(c,enemy[c].truepossible[0]);
            }
        }
    }
    
    sort(targets.begin(),targets.end(),[](int a,int b){
        return enemy[a].hp<enemy[b].hp;
    });

    //确定一
    for(auto c:targets){
        if(enemy[c].id!=-1)
            return make_pair(c,enemy[c].id);
    }
    for(auto c:targets){
        if(enemy[c].possibleturtle==1){
            if(find(enemy[c].truepossible.begin(),enemy[c].truepossible.end(),7)!=enemy[c].truepossible.end())
                return make_pair(c,7);
            else if(avatar_show==0)
                return make_pair(c,12);
        }
    }
    for(auto c:targets){
        if(enemy[c].truepossible.size()==1)
            return make_pair(c,enemy[c].truepossible[0]);
    }

    //不能确定一
    return findbest_possible();
}

pair<int, int> AI::Assert(Game game){
    if(isfirstassert){//初次断言
        isfirstassert=0;
        if(game.first_mover==1){//我方先手
            //无作为
        }
        else{
            judge_enemyassert();
            judge_enemyaction();
        }
    }
    else{
        judge_myaction();
        judge_enemyassert();
        judge_enemyaction();
    }    

    return judge();
}

int AI::find_pos(int id){//找到id鱼所在位置
    if(my[0].trueid==id)
        return 0;
    if(my[1].trueid==id)
        return 1;
    if(my[2].trueid==id)
        return 2;
    if(my[3].trueid==id)
        return 3;
    return -1;
}
pair<int,int> AI::get_max_atk(){
    int dmg=0;
    int index;
    int nowid;//越小优先级越高
    for(int i=0;i<=3;i++)
        if(my[i].hp>0){
            int now=0;
            int id;
            if(my[i].trueid==9){//大白鲨
                now=120;
                id=3;
            }
            else if(my[i].trueid==10){//锤头鲨
                now=my[i].atk*1.2;
                id=4;
            }
            else if((my[i].trueid==7&&get_skill_used(i)<3&&friend_count()>1)||my[i].trueid==5){//海龟或海狼
                now=120;
                id=2;
            }
            else{//普攻
                now=my[i].atk/2;
                id=1;
            }
            if(now>dmg){
                index=i;
                nowid=id;
                dmg=now;
            }
            else if(now==dmg){
                if(id<nowid){
                    index=i;
                    nowid=id;
                    dmg=now;
                }
            }
        }
    return make_pair(index,dmg);
}
int AI::friend_hp(){
    int ret=0;
    for(int i=0;i<=3;i++)
        if(my[i].hp>0)
            ret+=my[i].hp;
    return ret;
}
int AI::enemy_hp(){
    int ret=0;
    for(int i=0;i<=3;i++)
        if(enemy[i].hp>0)
            ret+=enemy[i].hp;
    return ret;
}
int AI::friend_count(){
    int ret=0;
    for(int i=0;i<=3;i++)
        if(my[i].hp>0)
            ret++;
    return ret;
}
int AI::enemy_count(){
    int ret=0;
    for(int i=0;i<=3;i++)
        if(enemy[i].hp>0)
            ret++;
    return ret;
}
int AI::enemy_realcount(){
    int ret=0;
    for(int i=0;i<=3;i++)
        if(enemy[i].realhp>0)
            ret++;
    return ret;
}
int AI::friend_assertcount(){
    int ret=0;
    for(int i=0;i<=3;i++)
        if(my[i].hp>0&&my[i].asserted==0)
            ret++;
    return ret;
}
int AI::enemy_assertcount(){
    int ret=0;
    for(int i=0;i<=3;i++)
        if(enemy[i].hp>0&&enemy[i].asserted==0)
            ret++;
    return ret;
}
int AI::find_proper_enemy(){
    vector<int> enemies=get_enemy_living_fishes();
    toassert=0;//考虑将要完成的断言
    for(auto c:enemies){
        if(enemy[c].asserted==0&&enemy[c].truepossible.size()<=2){
            toassert++;
        }
    }
    if(enemy_assertcount()!=0)
        sort(enemies.begin(),enemies.end(),[](int a,int b){//优先高血*价值
            if(enemy[a].hp-toassert*50<=0&&enemy[b].hp-toassert*50<=0)
                return enemy[a].hp>enemy[b].hp;
            else if((enemy[a].hp-toassert*50)*enemy[a].value==(enemy[b].hp-toassert*50)*enemy[b].value)
                return enemy[a].immunecount<enemy[b].immunecount;
            return (enemy[a].hp-toassert*50)*enemy[a].value>(enemy[b].hp-toassert*50)*enemy[b].value;
        });
    else
        sort(enemies.begin(),enemies.end(),[](int a,int b){//优先价值
            return enemy[a].value>enemy[b].value;
        });
    /*sort(enemies.begin(),enemies.end(),[](int a,int b){//优先高血
        return enemy[a].hp>enemy[b].hp;
    });
    for(auto c:enemies){
        if(enemy[c].immunecount==0&&enemy[c].counter_sign==0)
            return c;
    }
    for(auto c:enemies){
        if(enemy[c].immunecount==0)
            return c;
    }
    for(auto c:enemies){
        if(enemy[c].counter_sign==0)
            return c;
    }
    for(auto c:enemies){
        if(enemy[c].mask_hp==0&&enemy[c].mask_hold==0)
            return c;
    }
    for(auto c:enemies){
        if(enemy[c].mask_hp==0||enemy[c].mask_hold==0)
            return c;
    }*/
    if((enemy[enemies[0]].asserted==0&&enemy[enemies[0]].truepossible.size()<=2&&enemy[enemies[0]].hp<=120)==0)
        return enemies[0];
    if(enemies.size()>=2&&(enemy[enemies[1]].asserted==0&&enemy[enemies[1]].truepossible.size()<=2&&enemy[enemies[1]].hp<=120)==0)
        return enemies[1];
    if(enemies.size()>=3&&(enemy[enemies[2]].asserted==0&&enemy[enemies[2]].truepossible.size()<=2&&enemy[enemies[2]].hp<=120)==0)
        return enemies[2];
    if(enemies.size()>=4&&(enemy[enemies[3]].asserted==0&&enemy[enemies[3]].truepossible.size()<=2&&enemy[enemies[3]].hp<=120)==0)
        return enemies[3];
    return enemies[0];
}
int AI::find_crit_enemy(){
    vector<int> enemies=get_lowest_health_enemies();
    //优先低血
    if(enemy_assertcount()!=0)
        sort(enemies.begin(),enemies.end(),[](int a,int b){//优先高血*价值
            return (enemy[a].hp*enemy[a].value)>(enemy[b].hp*enemy[b].value);
        });
    else
        sort(enemies.begin(),enemies.end(),[](int a,int b){//优先价值
            return enemy[a].value>enemy[b].value;
        });
    /*for(auto c:enemies){
        if(enemy[c].hp<=100&&enemy[c].immunecount==0&&enemy[c].counter_sign==0)
            return c;
    }
    for(auto c:enemies){
        if(enemy[c].hp<=100&&enemy[c].immunecount==0)
            return c;
    }
    for(auto c:enemies){
        if(enemy[c].hp<=100&&enemy[c].counter_sign==0)
            return c;
    }
    for(auto c:enemies){
        if(enemy[c].hp<=100)
            return c;
    }

    sort(enemies.begin(),enemies.end(),[](int a,int b){//再优先高攻击力
        return enemy[a].trueatk>enemy[b].trueatk;
    });
    for(auto c:enemies){
        if(enemy[c].immunecount==0&&enemy[c].counter_sign==0)
            return c;
    }
    for(auto c:enemies){
        if(enemy[c].immunecount==0)
            return c;
    }
    for(auto c:enemies){
        if(enemy[c].counter_sign==0)
            return c;
    }
    for(auto c:enemies){
        if(enemy[c].mask_hp==0&&enemy[c].mask_hold==0)
            return c;
    }
    for(auto c:enemies){
        if(enemy[c].mask_hp==0||enemy[c].mask_hold==0)
            return c;
    }*/
    if((enemy[enemies[0]].asserted==0&&enemy[enemies[0]].truepossible.size()<=2&&enemy[enemies[0]].hp<=140)==0)
        return enemies[0];
    if(enemies.size()>=2&&(enemy[enemies[1]].asserted==0&&enemy[enemies[1]].truepossible.size()<=2&&enemy[enemies[1]].hp<=140)==0)
        return enemies[1];
    if(enemies.size()>=3&&(enemy[enemies[2]].asserted==0&&enemy[enemies[2]].truepossible.size()<=2&&enemy[enemies[2]].hp<=140)==0)
        return enemies[2];
    if(enemies.size()>=4&&(enemy[enemies[3]].asserted==0&&enemy[enemies[3]].truepossible.size()<=2&&enemy[enemies[3]].hp<=140)==0)
        return enemies[3];
    return enemies[0];
}
int AI::find_proper_ally(int pos){
    vector<int> ally=get_my_living_ally(pos);
    int maxhp_ally=find_maxhp_ally(ally);
    return maxhp_ally;
}
int AI::find_proper_reduce(){
    vector<int> ally=get_my_living_fishes();
    sort(ally.begin(),ally.end(),[](int a,int b){
        return my[a].value>my[b].value;
    });
    for(auto c:ally){
        if(my[c].mask_reduce==0&&(my[c].trueid==8||my[c].trueid==6))
            return c;
    }
    for(auto c:ally){
        if(my[c].mask_reduce==0)
            return c;
    }
    return ally[0];
}
int AI::find_proper_hp(int pos){
    vector<int> ally=get_my_living_ally(pos);
    sort(ally.begin(),ally.end(),[](int a,int b){
        return my[a].value>my[b].value;
    });
    for(auto c:ally){
        if(my[c].mask_hp==0)
            return c;
    }
    return ally[0];
}
int AI::find_proper_hold(int pos){
    vector<int> ally=get_my_living_ally(pos);
    sort(ally.begin(),ally.end(),[](int a,int b){
        return my[a].value>my[b].value;
    });
    for(auto c:ally){
        if(my[c].mask_hold==0)
            return c;
    }
    return ally[0];
}
int AI::find_proper_infighttarget(int pos){
    vector<int> ally=get_my_living_ally(pos);
    sort(ally.begin(),ally.end(),[](int a,int b){
        return my[a].value<my[b].value;
    });
    for(auto c:ally){
        if(my[c].hp<=my[pos].trueatk/2&&my[c].asserted==0){
            return c;
        }
    }
    return -1;
}

void AI::action_spray(int tactic,Action& action,int pos,int fpos,int epos){/*1. 射水鱼

【被动技能】：队友被直接攻击后，若其生命值少于30%（即120），对来源造成30的伤害  

【主动技能】：对敌方所有目标发动攻击，每个造成35%攻击力的伤害
*/
    action.set_action_fish(pos);
    switch (tactic){
    case 1:
        action.set_action_type(0);
        if(epos!=-1&&enemy[epos].hp>0)
            action.set_enemy_target(epos);
        else
            action.set_enemy_target(find_proper_enemy());
        break;
    case 2:
        action.set_action_type(1);
        action.set_enemy_target(-2);
        my[pos].action=1;
        break;
    case 3:
        action.set_action_type(0);
        action.set_enemy_target(get_lowest_health_enemies()[0]);
        break;
    case 4:
        action.set_action_type(1);
        action.set_enemy_target(-2);
        break;
    default:
        break;
    }
}
void AI::action_flame(int tactic,Action& action,int pos,int fpos,int epos){/*2. 喷火鱼

【被动技能】：队友被直接攻击后，若其生命值少于30%（即120），对来源造成30的伤害  

【主动技能】：对一名队友发动攻击造成50伤害，并使自身攻击力永久提升70
  */
    action.set_action_fish(pos);
    switch (tactic){
    case 1:
        action.set_action_type(0);
        if(epos!=-1&&enemy[epos].hp>0)
            action.set_enemy_target(epos);
        else
            action.set_enemy_target(find_proper_enemy());
        break;
    case 2:
        if(friend_count()==1){//没队友
            action.set_action_type(0);
            if(epos!=-1&&enemy[epos].hp>0)
                action.set_enemy_target(epos);
            else
                action.set_enemy_target(find_proper_enemy());
        }
        else{
            action.set_action_type(1);
            if(fpos!=-1&&my[fpos].hp>0)
                action.set_friend_target(fpos);
            else
                action.set_friend_target(find_proper_ally(pos));
            my[pos].action=1;
        }
        break;
    case 3:
        if(get_my_living_ally(pos).size()==0){//没队友
            action.set_action_type(0);
            action.set_enemy_target(get_lowest_health_enemies()[0]);
        }
        else{
            action.set_action_type(1);
            action.set_friend_target(find_maxhp_ally(get_my_living_ally(pos)));
        }
        break;
    case 4:
        action.set_action_type(0);
        action.set_enemy_target(get_lowest_health_enemies()[0]);
        break;
    default:
        break;
    }
}
void AI::action_eel(int tactic,Action& action,int pos,int fpos,int epos){/*3. 电鳗

【被动技能】：自身受到敌方直接攻击而受到伤害时，如果还有队友存活，会让队友帮忙承受伤害（自身承受70%，其余队友平摊30%）；每累计受到的实际伤害值达到200，自身攻击值永久提升20    

【主动技能】：对敌方所有目标发动攻击，每个造成35%攻击力的伤害*/
    action.set_action_fish(pos);
    switch (tactic){
    case 1:
        action.set_action_type(0);
        if(epos!=-1&&enemy[epos].hp>0)
            action.set_enemy_target(epos);
        else
            action.set_enemy_target(find_proper_enemy());
        break;
    case 2:
        action.set_action_type(1);
        action.set_enemy_target(-2);
        my[pos].action=1;
        break;
    case 3:
        action.set_action_type(0);
        action.set_enemy_target(get_lowest_health_enemies()[0]);
        break;
    case 4:
        action.set_action_type(1);
        action.set_enemy_target(-2);
        break;
    default:
        break;
    }
}
void AI::action_sunfish(int tactic,Action& action,int pos,int fpos,int epos){/*4. 翻车鱼

【被动技能】：自身受到敌方直接攻击而受到伤害时，如果还有队友存活，会让队友帮忙承受伤害（自身承受70%，其余队友平摊30%）；
每累计受到的实际伤害值达到200，自身攻击值永久提升20  

【主动技能】：对一名队友发动攻击造成50伤害，并使自身攻击力永久提升70*/
    action.set_action_fish(pos);
    switch (tactic){
    case 1:
        action.set_action_type(0);
        if(epos!=-1&&enemy[epos].hp>0)
            action.set_enemy_target(epos);
        else
            action.set_enemy_target(find_proper_enemy());
        break;
    case 2:
        if(friend_count()==1){//没队友
            action.set_action_type(0);
            if(epos!=-1&&enemy[epos].hp>0)
                action.set_enemy_target(epos);
            else
                action.set_enemy_target(find_proper_enemy());
        }
        else{
            action.set_action_type(1);
            if(fpos!=-1&&my[fpos].hp>0)
                action.set_friend_target(fpos);
            else
                action.set_friend_target(find_proper_ally(pos));
            my[pos].action=1;
        }
        break;
    case 3:
        if(get_my_living_ally(pos).size()==0){//没队友
            action.set_action_type(0);
            action.set_enemy_target(get_lowest_health_enemies()[0]);
        }
        else{
            action.set_action_type(1);
            action.set_friend_target(find_maxhp_ally(get_my_living_ally(pos)));
        }
        break;
    case 4:
        action.set_action_type(0);
        action.set_enemy_target(get_lowest_health_enemies()[0]);
        break;
    default:
        break;
    }
}
void AI::action_barracuda(int tactic,Action& action,int pos,int fpos,int epos){/*5. 海狼鱼

【被动技能】：每次将要受到伤害前有30%概率躲避伤害（不受到伤害，触发时机包括敌方直接攻击，队友或敌方发动技能造成的伤害等）  

【主动技能】：对一名敌人造成120的暴击伤害  */
    action.set_action_fish(pos);
    switch (tactic){
    case 1:
        action.set_action_type(0);
        if(epos!=-1&&enemy[epos].hp>0)
            action.set_enemy_target(epos);
        else
            action.set_enemy_target(find_proper_enemy());
        break;
    case 2:
        action.set_action_type(1);
        //if(my[pos].asserted==1){
            if(epos!=-1&&enemy[epos].hp>0)
                action.set_enemy_target(epos);
            else
                action.set_enemy_target(find_proper_enemy());
        //}
        /*else{//未被断言，隐藏身份
            if(epos!=-1&&enemy[epos].hp>0)
                action.set_enemy_target(epos);
            else
                action.set_enemy_target(find_crit_enemy());
        }*/
        my[pos].action=1;
        break;
    case 3:
        action.set_action_type(0);
        action.set_enemy_target(get_lowest_health_enemies()[0]);
        break;
    case 4:
        action.set_action_type(1);
        action.set_enemy_target(get_lowest_health_enemies()[0]);
        break;
    default:
        break;
    }
}
void AI::action_mobula(int tactic,Action& action,int pos,int fpos,int epos){/*6. 蝠鲼

【被动技能】：每次将要受到伤害前有30%概率躲避伤害（不受到伤害，触发时机包括敌方直接攻击，队友或敌方发动技能造成的伤害等）  

【主动技能】：选择一名队友（可以是自己），令其下次被直接攻击而受到伤害时减免70%伤害，并立即使自身攻击力永久提升20 */
    action.set_action_fish(pos);
    switch (tactic){
    case 1:
        action.set_action_type(0);
        if(epos!=-1&&enemy[epos].hp>0)
            action.set_enemy_target(epos);
        else
            action.set_enemy_target(find_proper_enemy());
        break;
    case 2:
        action.set_action_type(1);
        if(fpos!=-1&&my[fpos].hp>0){
            action.set_friend_target(fpos);
            my[fpos].mask_reduce=1;
        }
        else{
            int tar=find_proper_reduce();
            action.set_friend_target(tar);
            my[tar].mask_reduce=1;
        }
        my[pos].action=1;
        break;
    case 3:
        action.set_action_type(1);
        action.set_friend_target(find_minhp_friend(get_my_living_fishes()));
        break;
    case 4:
        action.set_action_type(0);
        action.set_enemy_target(get_lowest_health_enemies()[0]);
        break;
    default:
        break;
    }
}
void AI::action_turtle(int tactic,Action& action,int pos,int fpos,int epos){/*7. 海龟

【被动技能】：拥有三层无敌护盾，在三次将要受到的伤害前抵挡伤害（不受到伤害）；三层护盾失去后，每次将要受到伤害时有30%概率躲避伤害（不受到伤害）

【主动技能】：选择一名队友，令其下次被直接攻击而受到伤害后，若生命值仍大于0，自动恢复20血量，如果主动技能发动次数不超过三次，还应该选择对一名敌人造成120的暴击伤害  */
    action.set_action_fish(pos);
    switch (tactic){
    case 1:
        action.set_action_type(0);
        if(epos!=-1&&enemy[epos].hp>0)
            action.set_enemy_target(epos);
        else
            action.set_enemy_target(find_proper_enemy());
        break;
    case 2:
        if (get_skill_used(pos) < 3&&friend_count()>1){
            action.set_action_type(1);
            if(fpos!=-1&&my[fpos].hp>0){
                action.set_friend_target(fpos);
                my[fpos].mask_hp=1;
            }
            else{
                int tar=find_proper_hp(pos);
                action.set_friend_target(tar);
                my[tar].mask_hp=1;
            }
            //if(my[pos].asserted==1){
                if(epos!=-1&&enemy[epos].hp>0)
                    action.set_enemy_target(epos);
                else
                    action.set_enemy_target(find_proper_enemy());
            /*}
            else{//未被断言，隐藏身份
                if(epos!=-1&&enemy[epos].hp>0)
                    action.set_enemy_target(epos);
                else
                    action.set_enemy_target(find_crit_enemy());
            }*/
            my[pos].action=1;
        }
        else{
            action.set_action_type(0);
            if(epos!=-1&&enemy[epos].hp>0)
                action.set_enemy_target(epos);
            else
                action.set_enemy_target(find_proper_enemy());
        }
        break;
    case 3:
        action.set_action_type(0);
        action.set_enemy_target(get_lowest_health_enemies()[0]);
        break;
    case 4:
        if (get_skill_used(pos) < 3&&get_my_living_ally(pos).size()>0){
            action.set_action_type(1);
            action.set_friend_target(find_minhp_ally(get_my_living_ally(pos)));
            action.set_enemy_target(get_lowest_health_enemies()[0]);
        }
        else{
            action.set_action_type(0);
            action.set_enemy_target(get_lowest_health_enemies()[0]);
        }
        break;
    default:
        break;
    }
}
void AI::action_octupus(int tactic,Action& action,int pos,int fpos,int epos){/*8. 章鱼

【被动技能】：当受到伤害后，若生命值仍大于0，自动恢复20血量  

【主动技能】：选择一名队友（可以是自己），令其下次被直接攻击而受到伤害时减免70%伤害，并立即使自身攻击力永久提升20    */
    action.set_action_fish(pos);
    switch (tactic){
    case 1:
        action.set_action_type(0);
        if(epos!=-1&&enemy[epos].hp>0)
            action.set_enemy_target(epos);
        else
            action.set_enemy_target(find_proper_enemy());
        break;
    case 2:
        action.set_action_type(1);
        if(fpos!=-1&&my[fpos].hp>0){
            action.set_friend_target(fpos);
            my[fpos].mask_reduce=1;
        }
        else{
            int tar=find_proper_reduce();
            action.set_friend_target(tar);
            my[tar].mask_reduce=1;
        }
        my[pos].action=1;
        break;
    case 3:
        action.set_action_type(1);
        action.set_friend_target(find_minhp_friend(get_my_living_fishes()));
        break;
    case 4:
        action.set_action_type(0);
        action.set_enemy_target(get_lowest_health_enemies()[0]);
        break;
    default:
        break;
    }
}
void AI::action_whiteshark(int tactic,Action& action,int pos,int fpos,int epos){/*9. 大白鲨

【被动技能】：当受到伤害后，若生命值仍大于0，自动恢复20血量  

【主动技能】：选择一名场上血量最少的敌人造成120%攻击力的暴击伤害，当目标血量低于其生命值40%（即160）时改为造成140%攻击力的暴击伤害  */
    action.set_action_fish(pos);
    switch (tactic){
    case 1:
        action.set_action_type(0);
        if(epos!=-1&&enemy[epos].hp>0)
            action.set_enemy_target(epos);
        else
            action.set_enemy_target(find_proper_enemy());
        break;
    case 2:
        action.set_action_type(1);
        if(epos!=-1&&isminhp_enemy(epos))
            action.set_enemy_target(epos);
        else
            action.set_enemy_target(find_crit_enemy());
        my[pos].action=1;
        break;
    case 3:
        action.set_action_type(0);
        action.set_enemy_target(get_lowest_health_enemies()[0]);
        break;
    case 4:
        action.set_action_type(1);
		action.set_enemy_target(get_lowest_health_enemies()[0]);
        break;
    default:
        break;
    }
}
void AI::action_hammerhead(int tactic,Action& action,int pos,int fpos,int epos){/*10. 锤头鲨

【被动技能】：自身被直接攻击后若生命值不大于0，会触发爆炸，对来源造成40的伤害；血量低于20%（即80）时，自身攻击力提高15   

【主动技能】：选择一名场上血量最少的敌人造成120%攻击力的暴击伤害，当目标血量低于其生命值40%（即160）时改为造成140%攻击力的暴击伤害*/
    action.set_action_fish(pos);
    switch (tactic){
    case 1:
        action.set_action_type(0);
        if(epos!=-1&&enemy[epos].hp>0)
            action.set_enemy_target(epos);
        else
            action.set_enemy_target(find_proper_enemy());
        break;
    case 2:
        action.set_action_type(1);
        if(epos!=-1&&isminhp_enemy(epos))
            action.set_enemy_target(epos);
        else
            action.set_enemy_target(find_crit_enemy());
        my[pos].action=1;
        break;
    case 3:
        action.set_action_type(0);
        action.set_enemy_target(get_lowest_health_enemies()[0]);
        break;
    case 4:
        action.set_action_type(1);
		action.set_enemy_target(get_lowest_health_enemies()[0]);
        break;
    default:
        break;
    }
}
void AI::action_clownfish(int tactic,Action& action,int pos,int fpos,int epos){/*11. 小丑鱼

【被动技能】：自身被直接攻击后若生命值少于30%（即120），对来源造成30的伤害   

【主动技能】：选择一名队友，令其下次被直接攻击而受到伤害时，如果有除其以外的己方角色存活，其只承受70%伤害，其余己方角色平摊30%伤害，
如果主动技能发动次数不超过三次，还应该对所有敌人分别造成35%的伤害 */
    action.set_action_fish(pos);
    switch (tactic){
    case 1:
        action.set_action_type(0);
        if(epos!=-1&&enemy[epos].hp>0)
            action.set_enemy_target(epos);
        else
            action.set_enemy_target(find_proper_enemy());
        break;
    case 2:
        if (get_skill_used(pos) < 3&&friend_count()>1){
            action.set_action_type(1);
            if(fpos!=-1&&my[fpos].hp>0){
                action.set_friend_target(fpos);
                my[fpos].mask_hold=1;
            }
            else{
                int tar=find_proper_hold(pos);
                action.set_friend_target(tar);
                my[tar].mask_hold=1;
            }
            action.set_enemy_target(-2);
            my[pos].action=1;
        }
        else{
            action.set_action_type(0);
            if(epos!=-1&&enemy[epos].hp>0)
                action.set_enemy_target(epos);
            else
                action.set_enemy_target(find_proper_enemy());
        }
        break;
    case 3:
        action.set_action_type(0);
        action.set_enemy_target(get_lowest_health_enemies()[0]);
        break;
    case 4:
        if (get_skill_used(pos) < 3&&get_my_living_ally(pos).size()>0){
            action.set_action_type(1);
            action.set_friend_target(find_minhp_ally(get_my_living_ally(pos)));
            action.set_enemy_target(-2);
        }
        else{
            action.set_action_type(0);
            action.set_enemy_target(get_lowest_health_enemies()[0]);
        }
        break;
    default:
        break;
    }
}

bool AI::balanced(){
    int lowhp=1000;
    int highhp=0;
    for(int i=0;i<=3;i++){
        if(enemy[i].hp>0){
            if(enemy[i].hp<lowhp)
                lowhp=enemy[i].hp;
            if(enemy[i].hp>highhp)
                highhp=enemy[i].hp;
        }
    }
    if(highhp-lowhp>=160)
        return 0;
    for(int i=0;i<=3;i++){
        if(enemy[i].hp>0&&enemy[i].hp<=35&&enemy[i].valuable)
            return 0;
    }
    return 1;
}
bool AI::shouldassert(){
    int enemyval=0;
    int myval=0;
    int eassert=enemy_assertcount();
    int massert=friend_assertcount();
    for(int i=0;i<=3;i++){
        if(enemy[i].hp>0)
            enemyval+=enemy[i].hp*enemy[i].value;
        if(my[i].hp>0)
            myval+=my[i].hp*my[i].value;
    }
    if(myval<15000&&eassert>0&&enemyval-myval>15000)
        return 1;
    if(enemyval<15000&&eassert>0&&myval-enemyval>15000)
        return 1;
    return 0;
}
pair<int,int> AI::judgenowgame(){//TODO:形势判断,第一个数表示应当主动的程度，第二个数越大劣势越大
    int enemyval=0;
    int myval=0;
    int eassert=enemy_assertcount();
    int massert=friend_assertcount();
    int status;

    for(int i=0;i<=3;i++){
        if(enemy[i].hp-eassert*(25+gameround*5)>0)
            enemyval+=(enemy[i].hp-eassert*(25+gameround*5))*enemy[i].value;
        if(my[i].hp-massert*(25+gameround*5)>0)
            myval+=(my[i].hp-massert*(25+gameround*5))*my[i].value;
    }
    if(enemyval-myval>=35000)
        status=1;
    else if(enemyval-myval>=20000)
        status=2;
    else if(myval-enemyval<13000)
        status=3;
    else if(myval-enemyval<30000)
        status=4;
    else
        status=5;
    if(partytactic==0){
        if(gameround==1)
            return make_pair(2,status);
        return make_pair(3,status);
    }
    else{
        if(gameround==1)
            return make_pair(2,status);
        if(gameround==2&&round1_win==1)
            return make_pair(1,status);
        return make_pair(3,status);
    }
    
}
void AI::act(int tactic,Action& action,int pos,int fpos,int epos){
    switch(my[pos].trueid){
        case 1:
            action_spray(tactic,action,pos,fpos,epos);
            break;
        case 2:
            action_flame(tactic,action,pos,fpos,epos);
            break;
        case 3:
            action_eel(tactic,action,pos,fpos,epos);
            break;
        case 4:
            action_sunfish(tactic,action,pos,fpos,epos);
            break;
        case 5:
            action_barracuda(tactic,action,pos,fpos,epos);
            break;
        case 6:
            action_mobula(tactic,action,pos,fpos,epos);
            break;
        case 7:
            action_turtle(tactic,action,pos,fpos,epos);
            break;
        case 8:
            action_octupus(tactic,action,pos,fpos,epos);
            break;
        case 9:
            action_whiteshark(tactic,action,pos,fpos,epos);
            break;
        case 10:
            action_hammerhead(tactic,action,pos,fpos,epos);
            break;
        case 11:
            action_clownfish(tactic,action,pos,fpos,epos);
            break;
        default:
            break;
    }
}
int AI::find_best_aoe(){
    vector<int> can;
    can.clear();
    for(int i=0;i<=3;i++)
        if((my[i].trueid==1||my[i].trueid==3||(my[i].trueid==11&&get_skill_used(i)<3))&&my[i].hp>0){
            can.push_back(i);
        }
    if(can.empty())
        return -1;
    sort(can.begin(),can.end(),[](int a,int b){
        if(my[a].id==12)
            return 1;
        if(my[b].id==12)
            return 0;
        if(my[a].trueid==1||my[a].trueid==3)
            return 1;
        return 0;
    });
    for(auto c:can)
        if(my[c].asserted==1)
            return c;
    for(auto c:can)
        if(my[c].action==1)
            return c;
    return can[0];
}
int AI::find_best_crit(){
    vector<int> can;
    can.clear();
    for(int i=0;i<=3;i++)
        if((my[i].trueid==9||my[i].trueid==10)&&my[i].hp>0){
            can.push_back(i);
        }
    if(can.empty())
        return -1;
    sort(can.begin(),can.end(),[](int a,int b){
        if(my[a].id==12)
            return 1;
        if(my[b].id==12)
            return 0;
        if(my[a].trueid==9)
            return 1;
        return 0;
    });
    for(auto c:can)
        if(my[c].asserted==1)
            return c;
    for(auto c:can)
        if(my[c].action==1)
            return c;
    return can[0];
}
int AI::find_best_freecrit(){
    vector<int> can;
    can.clear();
    for(int i=0;i<=3;i++)
        if((my[i].trueid==5||(my[i].trueid==7&&get_skill_used(i)<3))&&my[i].hp>0){
            can.push_back(i);
        }
    if(can.empty())
        return -1;
    sort(can.begin(),can.end(),[](int a,int b){
        if(my[a].id==12)
            return 1;
        if(my[b].id==12)
            return 0;
        if(my[a].trueid==5)
            return 1;
        return 0;
    });
    for(auto c:can)
        if(my[c].asserted==1)
            return c;
    for(auto c:can)
        if(my[c].action==1)
            return c;
    return can[0];
}
pair<int,int> AI::find_best_normal(){
    vector<int> can;
    can.clear();
    for(int i=0;i<=3;i++)
        if(my[i].hp>0){
            can.push_back(i);
        }
    sort(can.begin(),can.end(),[](int a,int b){
        return my[a].atk>my[b].atk;
    });
    return make_pair(can[0],my[can[0]].atk/2);
}
int AI::find_best_protect(){
    vector<int> can;
    can.clear();
    for(int i=0;i<=3;i++)
        if((my[i].trueid==6||my[i].trueid==8)&&my[i].hp>0){
            can.push_back(i);
        }
    if(can.empty())
        return -1;
    sort(can.begin(),can.end(),[](int a,int b){
        if(my[a].id==12)
            return 1;
        if(my[b].id==12)
            return 0;
        if(my[a].trueid==6)
            return 1;
        return 0;
    });
    for(auto c:can)
        if(my[c].asserted==1)
            return c;
    for(auto c:can)
        if(my[c].action==1)
            return c;
    return can[0];
}
int AI::find_best_infight(){
    vector<int> can;
    can.clear();
    for(int i=0;i<=3;i++)
        if((my[i].trueid==2||my[i].trueid==4)&&my[i].hp>0){
            can.push_back(i);
        }
    if(can.empty())
        return -1;
    sort(can.begin(),can.end(),[](int a,int b){
        if(my[a].id==12)
            return 1;
        if(my[b].id==12)
            return 0;
        if(my[a].trueid==4)
            return 1;
        return 0;
    });
    for(auto c:can)
        if(my[c].asserted==1)
            return c;
    for(auto c:can)
        if(my[c].action==1)
            return c;
    return can[0];
}
void AI::smartact(Action& action){
    actioncount++;
    if(gameround==1&&partytactic==0){
        if(first_mover){//先手
            if(actioncount==1){
                int pos=find_best_aoe();
                int fpos=-1;
                int epos=-1;
                if(pos!=-1){
                    Act2;
                    return;
                }
            }
            else if(actioncount==2&&((activetype==1&&(activeskill==aoe||activeskill==crit))||activetype==0)){//翻车刀队友
                int pos=find_pos(4);
                int fpos=find_pos(9);
                int epos=-1;
                Act2;
                return;
            }
            else if(actioncount==2){
                int pos=find_best_aoe();
                int fpos=-1;
                int epos=-1;
                if(pos!=-1){
                    Act2;
                    return;
                }
            }
        }
        else{//后手
            if(actioncount==1&&((activetype==1&&(activeskill==aoe||activeskill==crit))||activetype==0)){//翻车刀队友
                int pos=find_pos(4);
                int fpos=find_pos(9);
                int epos=-1;
                Act2;
                return;
            }
            else if(actioncount==1||actioncount==2){
                int pos=find_best_aoe();
                int fpos=-1;
                int epos=-1;
                if(pos!=-1){
                    Act2;
                    return;
                }
            }
        }
    }
    else{
        if(actioncount==1||actioncount==2){
            int pos=find_best_aoe();
            int fpos=-1;
            int epos=-1;
            if(pos!=-1){
                Act2;
                return;
            }
        }
    }
    
    pair<int,int> max=get_max_atk();
    int aoepos=find_best_aoe();
    int critpos=find_best_crit();
    int freecritpos=find_best_freecrit();
    pair<int,int> normalpos=find_best_normal();
    int protectpos=find_best_protect();
    int infightpos=find_best_infight();
    int properenemy=find_proper_enemy();//合适的目标

    bool balance=balanced();
    bool needprotect=(my[find_proper_reduce()].mask_reduce==1)?0:1;
    pair<int,int> nowgame=judgenowgame();
    int infighttarget=find_proper_infighttarget(infightpos);
    int enemymaxatk=0;
    int maxatkenemy;
    int enemyhealcount=0;
    for(int i=0;i<=3;i++){
        if((possible(i,8)||possible(i,9))&&enemy[i].hp>0){
            enemyhealcount++;
        }
    }
    for(int i=0;i<=3;i++){
        if((possible(i,3)||possible(i,4))&&enemy[i].hp>0){
            enemyhealcount*=2;
        }
    }
    for(int i=0;i<=3;i++){
        if(enemy[i].hp>0){
            if(enemy[i].trueatk>enemymaxatk){
                enemymaxatk=enemy[i].trueatk;
                maxatkenemy=i;
            }
            else if(enemy[i].trueatk>=enemymaxatk&&possible(i,5)){
                maxatkenemy=i;
            }
        }
    }
        
    cerr<<actioncount<<" "<<nowgame.second<<'\n';

    int pos;
    int fpos=-1;
    int epos=-1;

    if(activetype==1&&activeskill==infight){//敌方内斗
        epos=activefish;
        if(normalpos.second>=100){
            pos=normalpos.first;
            Act1;
        }
        else if(critpos!=-1&&isminhp_enemy(epos)){
            pos=critpos;
            Act2;
        }
        else if(normalpos.second>=70){
            pos=normalpos.first;
            Act1;
        }
        else if(freecritpos!=-1){
            pos=freecritpos;
            Act2;
        }
        else{
            pos=normalpos.first;
            Act1;
        }
    }
    else{//一般情况
        if(enemy_count()==1&&enemy[properenemy].realhp<=normalpos.second&&enemy[properenemy].immunecount==0){//最后一击
            pos=normalpos.first;
            Act1;
            return;
        }

        if(protectpos!=-1&&my[protectpos].trueid==8){//章鱼的特殊处理
            if(my[protectpos].realhp>enemymaxatk*1.3-20&&my[protectpos].realhp>enemymaxatk&&enemymaxatk<=60){//血很多，择优
                if(normalpos.second>=120){
                    pos=normalpos.first;
                    epos=maxatkenemy;
                    Act1;
                }
                else if(aoepos!=-1&&(my[aoepos].action==1||my[aoepos].asserted==1)&&enemy_count()>=4&&enemyhealcount<=1&&balance){//高aoe
                    pos=aoepos;
                    Act2;
                }
                else if(critpos!=-1&&(my[critpos].action==1||my[critpos].asserted==1)&&isminhp_enemy(properenemy)){//针对暴击
                    pos=critpos;
                    epos=properenemy;
                    Act2;
                }
                else if(freecritpos!=-1&&(my[freecritpos].action==1||my[freecritpos].asserted==1)){//自由暴击
                    pos=freecritpos;
                    if(enemy[properenemy].hp<=50&&my[freecritpos].trueid==7)
                        Act1;
                    else
                        Act2;
                }
                else if(normalpos.second>=100){
                    pos=normalpos.first;
                    epos=maxatkenemy;
                    Act1;
                }
                else if(aoepos!=-1&&(my[aoepos].action==1||my[aoepos].asserted==1)&&enemy_count()>=4&&enemyhealcount<=2&&balance){//高aoe
                    pos=aoepos;
                    Act2;
                }
                else if(aoepos!=-1&&(my[aoepos].action==1||my[aoepos].asserted==1)&&enemy_count()>=3&&enemyhealcount<=1&&balance){//中高aoe
                    pos=aoepos;
                    Act2;
                }
                else if(normalpos.second>=80){
                    pos=normalpos.first;
                    epos=maxatkenemy;
                    Act1;
                }
                else if(critpos!=-1&&(my[critpos].action==1||my[critpos].asserted==1)&&(enemy[find_crit_enemy()].asserted==1||enemy[find_crit_enemy()].hp>=140||enemy[find_crit_enemy()].truepossible.size()>2)){//低血暴击
                    pos=critpos;
                    Act2;
                }
                else if(infightpos!=-1&&(my[infightpos].action==1||my[infightpos].asserted==1)&&infighttarget!=-1){
                    pos=infightpos;
                    fpos=infighttarget;
                    Act2;
                }
                else if(aoepos!=-1&&(my[aoepos].action==1||my[aoepos].asserted==1)&&enemy_count()>=3&&enemyhealcount<=2&&balance){//中高aoe
                    pos=aoepos;
                    Act2;
                }
                else{
                    pos=normalpos.first;
                    epos=maxatkenemy;
                    Act1;
                }
                return;
            }
            else if(my[protectpos].realhp>enemymaxatk*0.3&&enemymaxatk<=60){//血足够
                if(my[protectpos].mask_reduce==0){
                    pos=protectpos;
                    fpos=protectpos;
                    Act2;
                    return;
                }
            }
            if(my[protectpos].asserted==1||my[protectpos].action==1){//已经暴露身份
                if(enemymaxatk>=70){//敌方较高攻击
                    if(enemy[maxatkenemy].realhp<=normalpos.second){//能打死
                        pos=normalpos.first;
                        epos=maxatkenemy;
                        Act1;
                        return;
                    }
                    else if(my[protectpos].realhp>enemymaxatk*0.3&&my[protectpos].mask_reduce==0){
                        pos=protectpos;
                        fpos=protectpos;
                        Act2;
                        return;
                    }
                }
            }
        }
        if(protectpos!=-1&&my[protectpos].trueid==6){//蝠鲼的特殊处理
            if((my[protectpos].asserted==1||my[protectpos].action==1)&&my[protectpos].mask_reduce==0&&my[protectpos].realhp>enemymaxatk*0.3){//已经暴露身份且没有保护
                if(aoepos!=-1&&(my[aoepos].action==1||my[aoepos].asserted==1)&&enemy_count()>=2&&enemy[properenemy].realhp<=0.35*my[aoepos].atk){//能用aoe打死
                    pos=aoepos;
                    Act2;
                    return;
                }
                else if(normalpos.second>=enemy[properenemy].realhp){//能用普攻打死
                    pos=normalpos.first;
                    Act1;
                }
                else if(normalpos.second>=120){
                    pos=normalpos.first;
                    epos=maxatkenemy;
                    Act1;
                }
                else if(aoepos!=-1&&(my[aoepos].action==1||my[aoepos].asserted==1)&&enemy_count()>=4&&enemyhealcount<=1&&balance){//高aoe
                    pos=aoepos;
                    Act2;
                }
                else if(critpos!=-1&&(my[critpos].action==1||my[critpos].asserted==1)&&isminhp_enemy(properenemy)){//针对暴击
                    pos=critpos;
                    epos=properenemy;
                    Act2;
                }
                else if(freecritpos!=-1&&(my[freecritpos].action==1||my[freecritpos].asserted==1)){//自由暴击
                    pos=freecritpos;
                    if(enemy[properenemy].hp<=50&&my[freecritpos].trueid==7)
                        Act1;
                    else
                        Act2;
                }
                else if(normalpos.second>=100){
                    pos=normalpos.first;
                    epos=maxatkenemy;
                    Act1;
                }
                else{
                    pos=protectpos;
                    fpos=protectpos;
                    Act2;
                }
                return;
            }
        }

        if(aoepos!=-1&&(my[aoepos].action==1||my[aoepos].asserted==1)&&enemy_realcount()>=2&&enemy[properenemy].realhp<=0.35*my[aoepos].atk){//能用aoe打死
            pos=aoepos;
            Act2;
            return;
        }
        else if(normalpos.second>=enemy[properenemy].hp){//能用普攻打死
            if(enemy_count()>1){
                double ratio=0.3/(enemy_count()-1);
                for(int i=0;i<=3;i++)
                    if(enemy[i].hp>0&&i!=properenemy&&(possible(i,3)||possible(i,4)))
                        epos=i;
                if(epos!=-1&&enemy[properenemy].hp<=ratio*120
                &&freecritpos!=-1&&(my[freecritpos].action==1||my[freecritpos].asserted==1)){
                    pos=freecritpos;
                    Act2;
                    return;
                }
                else if(epos!=-1&&enemy[properenemy].hp<=ratio*normalpos.second){
                    pos=normalpos.first;
                    Act1;
                    return;
                }
                epos=-1;
            }
            if(normalpos.second>=120){
                pos=normalpos.first;
                Act1;
            }
            else if(critpos!=-1&&(my[critpos].action==1||my[critpos].asserted==1)&&isminhp_enemy(properenemy)){
                pos=critpos;
                epos=properenemy;
                Act2;
            }
            else if(freecritpos!=-1&&(my[freecritpos].action==1||my[freecritpos].asserted==1)){
                pos=freecritpos;
                Act2;
            }
            else{
                pos=normalpos.first;
                Act1;
            }
            
        }
        else if(normalpos.second>=120){
            pos=normalpos.first;
            Act1;
        }
        else if(aoepos!=-1&&(my[aoepos].action==1||my[aoepos].asserted==1)&&enemy_realcount()>=4&&enemyhealcount<=1&&balance){//高aoe
            pos=aoepos;
            Act2;
        }
        else if(critpos!=-1&&(my[critpos].action==1||my[critpos].asserted==1)&&isminhp_enemy(properenemy)){//针对暴击
            pos=critpos;
            epos=properenemy;
            Act2;
        }
        else if(freecritpos!=-1&&(my[freecritpos].action==1||my[freecritpos].asserted==1)){//自由暴击
            pos=freecritpos;
            if(enemy[properenemy].hp<=50&&my[freecritpos].trueid==7)
                Act1;
            else
                Act2;
        }
        else if(normalpos.second>=100){
            pos=normalpos.first;
            Act1;
        }
        else if(aoepos!=-1&&(my[aoepos].action==1||my[aoepos].asserted==1)&&enemy_realcount()>=4&&enemyhealcount<=2&&balance){//高aoe
            pos=aoepos;
            Act2;
        }
        else if(aoepos!=-1&&(my[aoepos].action==1||my[aoepos].asserted==1)&&enemy_realcount()>=3&&enemyhealcount<=1&&balance){//中高aoe
            pos=aoepos;
            Act2;
        }
        else if(normalpos.second>=80){
            pos=normalpos.first;
            Act1;
        }
        else if(critpos!=-1&&(my[critpos].action==1||my[critpos].asserted==1)&&(enemy[find_crit_enemy()].asserted==1||enemy[find_crit_enemy()].hp>=140||enemy[find_crit_enemy()].truepossible.size()>2)){//低血暴击
            pos=critpos;
            Act2;
        }
        else if(infightpos!=-1&&(my[infightpos].action==1||my[infightpos].asserted==1)&&infighttarget!=-1){
            pos=infightpos;
            fpos=infighttarget;
            Act2;
        }
        else if(aoepos!=-1&&(my[aoepos].action==1||my[aoepos].asserted==1)&&enemy_realcount()>=3&&enemyhealcount<=2&&balance){//中高aoe
            pos=aoepos;
            Act2;
        }

        else if(nowgame.first<=2&&(nowgame.second>=4||nowgame.second<=2)){//隐藏身份
            pos=normalpos.first;
            Act1;
        }
        else{//不考虑隐藏身份
            if(enemy[properenemy].hp<=normalpos.second){//能用普攻打死
                pos=normalpos.first;
                Act1;
            }
            else if(aoepos!=-1&&enemy_realcount()>=4&&enemyhealcount<=1&&balance){//高aoe
                pos=aoepos;
                Act2;
            }
            else if(critpos!=-1&&isminhp_enemy(properenemy)&&enemy[properenemy].hp>=100){//低血针对暴击
                pos=critpos;
                epos=properenemy;
                Act2;
            }
            else if(freecritpos!=-1){//自由暴击
                pos=freecritpos;
                if(enemy[properenemy].hp<=50)
                    Act1;
                else
                    Act2;
            }
            else if(aoepos!=-1&&enemy_realcount()>=4&&enemyhealcount<=2&&balance){//高aoe
                pos=aoepos;
                Act2;
            }
            else if(aoepos!=-1&&enemy_realcount()>=3&&enemyhealcount<=1&&balance){//中高aoe
                pos=aoepos;
                Act2;
            }
            else if(normalpos.second>=70){
                pos=normalpos.first;
                Act1;
            }
            else if(aoepos!=-1&&enemy_realcount()>=3&&enemyhealcount<=2&&balance){//中高aoe
                pos=aoepos;
                Act2;
            }
            else if(aoepos!=-1&&enemy_realcount()>=2&&enemyhealcount==0&&balance){//中aoe
                pos=aoepos;
                Act2;
            }
            else if(critpos!=-1&&(enemy[find_crit_enemy()].asserted==1||enemy[find_crit_enemy()].hp>=140||enemy[find_crit_enemy()].truepossible.size()>2)&&enemy[find_crit_enemy()].hp>=100){//低血暴击
                pos=critpos;
                Act2;
            }
            else{
                pos=normalpos.first;
                Act1;
            }
        }
    }
}
Action AI::Act(Game nowgame){
    judge_myassert();
    Action action(nowgame);
    updatestatus();
    smartact(action);
    /*if(nowparty==1){//小丑11，翻车4，章鱼8，海狼鱼5
        int pos,fpos=-1,epos=-1;
        if(actioncount==0||actioncount==1){//放aoe
            actioncount++;
            pos=find_pos(11);
            act(2,action,pos,fpos,epos);
        }
        else{
            actioncount++;
            if(activetype==0){//对手使用普攻
                pair<int,int> max=get_max_atk();
                if(enemy[activefish].atk>=170){//高攻击
                    if(max.second<120){//自己伤害不高
                        if(possible(activefish,6)||possible(activefish,8)){//有可能是章鱼或蝠鲼
                            if(my[find_pos(11)].hp>0&&get_skill_used(find_pos(11))<3&&enemy_count()>=2){//小丑鱼可以aoe
                                pos=find_pos(11);
                                act(2,action,pos,fpos,epos);
                            }
                            else{
                                pos=max.first;
                                act(1,action,pos,fpos,activefish);
                            }
                        }
                        else{//应该是翻车
                            pos=max.first;
                            act(1,action,pos,fpos,activefish);
                        }
                    }
                    else{//自己伤害高
                        pos=max.first;
                        if(my[pos].trueid==4||my[pos].trueid==8)//普攻
                            act(1,action,pos,fpos,activefish);
                        else//技能
                            act(2,action,pos,fpos,activefish);
                    }
                }
                else{//低攻击
                    if(max.second<120){//自己伤害不高
                        if(my[find_pos(8)].hp>50){//章鱼可以保护
                            pos=find_pos(8);
                            act(2,action,pos,fpos,epos);
                        }
                        else{
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                    else{//自己伤害高
                        if(max.first==find_pos(4)||(max.first==find_pos(8)&&my[find_pos(8)].hp>60)){//高伤鱼打普攻
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                        else if(my[find_pos(8)].hp>50){//章鱼可以保护
                            pos=find_pos(8);
                            act(2,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(5)){//海狼鱼主动
                            pos=max.first;
                            act(2,action,pos,fpos,epos);
                        }
                        else{//TODO: 理论不存在？
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                }
            }
            else{//对手使用主动技能
                pair<int,int> max=get_max_atk();
                if(activeskill==aoe){//敌方aoe
                    if(max.second<120){//自己伤害不高
                        if(my[find_pos(8)].hp>50){//章鱼可以保护
                            pos=find_pos(8);
                            act(2,action,pos,fpos,epos);
                        }
                        else{
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                    else{//自己伤害高
                        if(max.first==find_pos(4)||max.first==find_pos(8)){//高伤鱼打普攻
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(5)){//海狼鱼主动
                            pos=max.first;
                            act(2,action,pos,fpos,epos);
                        }
                        else{//TODO: 理论不存在？
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                }
                else if(activeskill==infight){//敌方内斗
                    epos=activefish;
                    if(max.second<120){//自己伤害不高
                        pos=max.first;
                        act(1,action,pos,fpos,epos);
                    }
                    else{//自己伤害高
                        if(max.first==find_pos(4)||max.first==find_pos(8)){//高伤鱼打普攻
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(5)){//海狼鱼主动
                            pos=max.first;
                            act(2,action,pos,fpos,epos);
                        }
                        else{//TODO: 理论不存在？
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                }
                else if(activeskill==crit){//敌方暴击
                    //epos=activefish;
                    //if(enemy[epos].immunecount>0)//主动回避盾
                    //    epos=-1;
                    if(max.second<120){//自己伤害不高
                        if(my[find_pos(8)].hp>180){//章鱼可以保护
                            pos=find_pos(8);
                            act(2,action,pos,fpos,epos);
                        }
                        else{
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                    else{//自己伤害高
                        if(max.first==find_pos(4)||max.first==find_pos(8)){//高伤鱼打普攻
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(5)){//海狼鱼主动
                            pos=max.first;
                            act(2,action,pos,fpos,epos);
                        }
                        else{//TODO: 理论不存在？
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                }
                else if(activeskill==subtle){//敌方无作为
                    //epos=activefish;
                    //if(enemy[epos].immunecount>0)//回避盾
                    //    epos=-1;
                    if(max.second<120){//自己伤害不高
                        if(my[find_pos(11)].hp>0&&get_skill_used(find_pos(11))<3&&enemy_count()>2){//小丑鱼可以aoe
                            pos=find_pos(11);
                            act(2,action,pos,fpos,epos);
                        }
                        else if(my[find_pos(8)].hp>50){//章鱼可以保护
                            pos=find_pos(8);
                            act(2,action,pos,fpos,epos);
                        }
                        else if(rand()%2==0){
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                        else{
                            pos=max.first;
                            act(1,action,pos,fpos,-1);
                        }
                    }
                    else{//自己伤害高
                        if(my[find_pos(11)].hp>0&&get_skill_used(find_pos(11))<3&&enemy_count()>2){//小丑鱼可以aoe
                            pos=find_pos(11);
                            act(2,action,pos,fpos,epos);
                        }
                        else if(my[find_pos(8)].hp>50){//章鱼可以保护
                            pos=find_pos(8);
                            act(2,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(4)||max.first==find_pos(8)){//高伤鱼打普攻
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(5)){//海狼鱼主动
                            pos=max.first;
                            act(2,action,pos,fpos,epos);
                        }
                        else{//TODO: 理论不存在？
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                }
            }
        }
    }
    else if(nowparty==2){//喷火2,电鳗3,大白鲨9,海龟7
        int pos,fpos=-1,epos=-1;
        if(actioncount==0||actioncount==1){//放aoe
            actioncount++;
            pos=find_pos(3);
            act(2,action,pos,fpos,epos);
        }
        else{
            actioncount++;
            if(activetype==0){//对手使用普攻
                pair<int,int> max=get_max_atk();
                if(enemy[activefish].atk>=170){//高攻击
                    if(max.second<120){//自己伤害不高
                        if(possible(activefish,6)||possible(activefish,8)){//有可能是章鱼或蝠鲼
                            if(my[find_pos(3)].hp>0&&enemy_count()>=2){//电鳗还活着
                                pos=find_pos(3);
                                act(2,action,pos,fpos,epos);
                            }
                            else{
                                pos=max.first;
                                act(1,action,pos,fpos,activefish);
                            }
                        }
                        else{//应该是翻车
                            pos=max.first;
                            act(1,action,pos,fpos,activefish);
                        }
                    }
                    else{//自己伤害高
                        pos=max.first;
                        if(my[pos].trueid==2)//普攻
                            act(1,action,pos,fpos,activefish);
                        else//技能
                            act(2,action,pos,fpos,activefish);
                    }
                }
                else{//低攻击
                    if(max.second<120){//自己伤害不高
                        if(my[find_pos(3)].hp>0&&enemy_count()>2){//电鳗可以aoe
                            pos=find_pos(3);
                            act(2,action,pos,fpos,epos);
                        }
                        else{
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                    else{//自己伤害高
                        if(my[find_pos(3)].hp>0&&enemy_count()>2){//电鳗可以aoe
                            pos=find_pos(3);
                            act(2,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(2)){//高伤鱼打普攻
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(9)||max.first==find_pos(7)){//主动
                            pos=max.first;
                            act(2,action,pos,fpos,epos);
                        }
                        else{//TODO: 理论不存在？
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                }
            }
            else{//对手使用主动技能
                pair<int,int> max=get_max_atk();
                if(activeskill==aoe){//敌方aoe
                    //epos=activefish;
                    if(max.second<120){//自己伤害不高
                        if(my[find_pos(3)].hp>0&&enemy_count()>2){//电鳗可以aoe
                            pos=find_pos(3);
                            act(2,action,pos,fpos,epos);
                        }
                        else{
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                    else{//自己伤害高
                        if(my[find_pos(3)].hp>0&&enemy_count()>2){//电鳗可以aoe
                            pos=find_pos(3);
                            act(2,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(2)){//高伤鱼打普攻
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(9)||max.first==find_pos(7)){//主动
                            pos=max.first;
                            act(2,action,pos,fpos,epos);
                        }
                        else{//TODO: 理论不存在？
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                }
                else if(activeskill==infight){//敌方内斗
                    epos=activefish;
                    if(max.second<120){//自己伤害不高
                        pos=max.first;
                        act(1,action,pos,fpos,epos);
                    }
                    else{//自己伤害高
                        if(max.first==find_pos(2)){//高伤鱼打普攻
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(9)||max.first==find_pos(7)){//主动
                            pos=max.first;
                            act(2,action,pos,fpos,epos);
                        }
                        else{//TODO: 理论不存在？
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                }
                else if(activeskill==crit){//敌方暴击
                    //epos=activefish;
                    //if(enemy[epos].immunecount>0)//主动回避盾
                    //    epos=-1;
                    if(max.second<120){//自己伤害不高
                        if(my[find_pos(3)].hp>0&&enemy_count()>2){//电鳗可以aoe
                            pos=find_pos(3);
                            act(2,action,pos,fpos,epos);
                        }
                        else{
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                    else{//自己伤害高
                        if(max.first==find_pos(2)){//高伤鱼打普攻
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(9)||max.first==find_pos(7)){//主动
                            pos=max.first;
                            act(2,action,pos,fpos,epos);
                        }
                        else{//TODO: 理论不存在？
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                }
                else if(activeskill==subtle){//敌方无作为
                    //epos=activefish;
                    //if(enemy[epos].immunecount>0)//回避盾
                    //    epos=-1;
                    if(max.second<120){//自己伤害不高
                        if(my[find_pos(3)].hp>0&&enemy_count()>2){//电鳗可以aoe
                            pos=find_pos(3);
                            act(2,action,pos,fpos,epos);
                        }
                        else if(rand()%2==0){
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                        else{
                            pos=max.first;
                            act(1,action,pos,fpos,-1);
                        }
                    }
                    else{//自己伤害高
                        if(my[find_pos(3)].hp>0&&enemy_count()>2){//电鳗可以aoe
                            pos=find_pos(3);
                            act(2,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(2)){//高伤鱼打普攻
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(9)||max.first==find_pos(7)){//主动
                            pos=max.first;
                            act(2,action,pos,fpos,epos);
                        }
                        else{//TODO: 理论不存在？
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                }
            }
        }
    }
    else{//蝠鲼6，海龟7，锤头10，射水1
        int pos,fpos=-1,epos=-1;
        if(actioncount==0||actioncount==1){//放aoe
            actioncount++;
            pos=find_pos(1);
            act(2,action,pos,fpos,epos);
        }
        else{
            actioncount++;
            if(activetype==0){//对手使用普攻
                pair<int,int> max=get_max_atk();
                if(enemy[activefish].atk>=170){//高攻击
                    if(max.second<120){//自己伤害不高
                        if(possible(activefish,6)||possible(activefish,8)){//有可能是章鱼或蝠鲼
                            if(my[find_pos(1)].hp>0&&enemy_count()>=2){//射水鱼可以aoe
                                pos=find_pos(1);
                                act(2,action,pos,fpos,epos);
                            }
                            else{
                                pos=max.first;
                                act(1,action,pos,fpos,activefish);
                            }
                        }
                        else{//应该是翻车
                            pos=max.first;
                            act(1,action,pos,fpos,activefish);
                        }
                    }
                    else{//自己伤害高
                        pos=max.first;
                        if(my[pos].trueid==6)//普攻
                            act(1,action,pos,fpos,activefish);
                        else//技能
                            act(2,action,pos,fpos,activefish);
                    }
                }
                else{//低攻击
                    if(max.second<120){//自己伤害不高
                        if(my[find_pos(1)].hp>0&&enemy_count()>2){//射水鱼可以aoe
                            pos=find_pos(1);
                            act(2,action,pos,fpos,epos);
                        }
                        else if(my[find_pos(6)].hp>100){//蝠鲼可以保护
                            pos=find_pos(6);
                            act(2,action,pos,fpos,epos);
                        }
                        else{
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                    else{//自己伤害高
                        if(my[find_pos(1)].hp>0&&enemy_count()>2){//射水鱼可以aoe
                            pos=find_pos(1);
                            act(2,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(6)){//高伤鱼打普攻
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                        else if(my[find_pos(6)].hp>100){//蝠鲼可以保护
                            pos=find_pos(6);
                            act(2,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(7)||max.first==find_pos(10)){//主动
                            pos=max.first;
                            act(2,action,pos,fpos,epos);
                        }
                        else{//TODO: 理论不存在？
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                }
            }
            else{//对手使用主动技能
                pair<int,int> max=get_max_atk();
                if(activeskill==aoe){//敌方aoe
                    //epos=activefish;
                    if(max.second<120){//自己伤害不高
                        if(my[find_pos(1)].hp>0&&enemy_count()>2){//射水鱼可以aoe
                            pos=find_pos(1);
                            act(2,action,pos,fpos,epos);
                        }
                        else if(my[find_pos(6)].hp>50){//蝠鲼可以保护
                            pos=find_pos(6);
                            act(2,action,pos,fpos,epos);
                        }
                        else{
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                    else{//自己伤害高
                        if(my[find_pos(1)].hp>0&&enemy_count()>2){//射水鱼可以aoe
                            pos=find_pos(1);
                            act(2,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(6)){//高伤鱼打普攻
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(7)||max.first==find_pos(10)){//主动
                            pos=max.first;
                            act(2,action,pos,fpos,epos);
                        }
                        else{//TODO: 理论不存在？
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                }
                else if(activeskill==infight){//敌方内斗
                    epos=activefish;
                    if(max.second<120){//自己伤害不高
                        pos=max.first;
                        act(1,action,pos,fpos,epos);
                    }
                    else{//自己伤害高
                        if(max.first==find_pos(6)){//高伤鱼打普攻
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(7)||max.first==find_pos(10)){//主动
                            pos=max.first;
                            act(2,action,pos,fpos,epos);
                        }
                        else{//TODO: 理论不存在？
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                }
                else if(activeskill==crit){//敌方暴击
                    //epos=activefish;
                    //if(enemy[epos].immunecount>0)//主动回避盾
                    //    epos=-1;
                    if(max.second<120){//自己伤害不高
                        if(my[find_pos(1)].hp>0&&enemy_count()>2){//射水鱼可以aoe
                            pos=find_pos(1);
                            act(2,action,pos,fpos,epos);
                        }
                        else if(my[find_pos(6)].hp>200){//蝠鲼可以保护
                            pos=find_pos(6);
                            act(2,action,pos,fpos,epos);
                        }
                        else{
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                    else{//自己伤害高
                        if(max.first==find_pos(6)){//高伤鱼打普攻
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(7)||max.first==find_pos(10)){//主动
                            pos=max.first;
                            act(2,action,pos,fpos,epos);
                        }
                        else{//TODO: 理论不存在？
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                }
                else if(activeskill==subtle){//敌方无作为
                    //epos=activefish;
                    //if(enemy[epos].immunecount>0)//回避盾
                    //    epos=-1;
                    if(max.second<120){//自己伤害不高
                        if(my[find_pos(1)].hp>0&&enemy_count()>2){//射水鱼可以aoe
                            pos=find_pos(1);
                            act(2,action,pos,fpos,epos);
                        }
                        else if(my[find_pos(6)].hp>50){//蝠鲼可以保护
                            pos=find_pos(6);
                            act(2,action,pos,fpos,epos);
                        }
                        else{
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                    else{//自己伤害高
                        if(my[find_pos(1)].hp>0&&enemy_count()>2){//射水鱼可以aoe
                            pos=find_pos(1);
                            act(2,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(6)){//高伤鱼打普攻
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                        else if(my[find_pos(6)].hp>100){//蝠鲼可以保护
                            pos=find_pos(6);
                            act(2,action,pos,fpos,epos);
                        }
                        else if(max.first==find_pos(7)||max.first==find_pos(10)){//主动
                            pos=max.first;
                            act(2,action,pos,fpos,epos);
                        }
                        else{//TODO: 理论不存在？
                            pos=max.first;
                            act(1,action,pos,fpos,epos);
                        }
                    }
                }
            }
        }
    }*/
    
    /*int my_pos = (get_my_living_fishes())[rand()%(get_my_living_fishes().size())];
    int id=game->my_fish[my_pos].id;
    int rand_num=rand()%10;
    if(rand_num<1)
        act(id,1,action);
    else if(rand_num<4)
        act(id,2,action);
    else if(rand_num<6)
        act(id,3,action);
    else
        act(id,4,action);*/
    
    //action.set_action_fish(my_pos);
    //auto_valid_action(my_pos, &action);
    savelaststatus();
    return action;
}

/*
 1. 射水鱼

【被动技能】：队友被直接攻击后，若其生命值少于30%（即120），对来源造成30的伤害  

【主动技能】：对敌方所有目标发动攻击，每个造成35%攻击力的伤害  

2. 喷火鱼

【被动技能】：队友被直接攻击后，若其生命值少于30%（即120），对来源造成30的伤害  

【主动技能】：对一名队友发动攻击造成50伤害，并使自身攻击力永久提升70    

3. 电鳗

【被动技能】：自身受到敌方直接攻击而受到伤害时，如果还有队友存活，会让队友帮忙承受伤害（自身承受70%，其余队友平摊30%）；每累计受到的实际伤害值达到200，自身攻击值永久提升20    

【主动技能】：对敌方所有目标发动攻击，每个造成35%攻击力的伤害

4. 翻车鱼

【被动技能】：自身受到敌方直接攻击而受到伤害时，如果还有队友存活，会让队友帮忙承受伤害（自身承受70%，其余队友平摊30%）；每累计受到的实际伤害值达到200，自身攻击值永久提升20  

【主动技能】：对一名队友发动攻击造成50伤害，并使自身攻击力永久提升70

5. 海狼鱼

【被动技能】：每次将要受到伤害前有30%概率躲避伤害（不受到伤害，触发时机包括敌方直接攻击，队友或敌方发动技能造成的伤害等）  

【主动技能】：对一名敌人造成120的暴击伤害 

6. 蝠鲼

【被动技能】：每次将要受到伤害前有30%概率躲避伤害（不受到伤害，触发时机包括敌方直接攻击，队友或敌方发动技能造成的伤害等）  

【主动技能】：选择一名队友（可以是自己），令其下次被直接攻击而受到伤害时减免70%伤害，并立即使自身攻击力永久提升20   

7. 海龟

【被动技能】：拥有三层无敌护盾，在三次将要受到的伤害前抵挡伤害（不受到伤害）；三层护盾失去后，每次将要受到伤害时有30%概率躲避伤害（不受到伤害）

【主动技能】：选择一名队友，令其下次被直接攻击而受到伤害后，若生命值仍大于0，自动恢复20血量，如果主动技能发动次数不超过三次，还应该选择对一名敌人造成120的暴击伤害  

8. 章鱼

【被动技能】：当受到伤害后，若生命值仍大于0，自动恢复20血量  

【主动技能】：选择一名队友（可以是自己），令其下次被直接攻击而受到伤害时减免70%伤害，并立即使自身攻击力永久提升20   

9. 大白鲨

【被动技能】：当受到伤害后，若生命值仍大于0，自动恢复20血量  

【主动技能】：选择一名场上血量最少的敌人造成120%攻击力的暴击伤害，当目标血量低于其生命值40%（即160）时改为造成140%攻击力的暴击伤害  

10. 锤头鲨

【被动技能】：自身被直接攻击后若生命值不大于0，会触发爆炸，对来源造成40的伤害；血量低于20%（即80）时，自身攻击力提高15   

【主动技能】：选择一名场上血量最少的敌人造成120%攻击力的暴击伤害，当目标血量低于其生命值40%（即160）时改为造成140%攻击力的暴击伤害

11. 小丑鱼

【被动技能】：自身被直接攻击后若生命值少于30%（即120），对来源造成30的伤害   

【主动技能】：选择一名队友，令其下次被直接攻击而受到伤害时，如果有除其以外的己方角色存活，其只承受70%伤害，其余己方角色平摊30%伤害，如果主动技能发动次数不超过三次，还应该对所有敌人分别造成35%的伤害  

12. 拟态鱼

【被动技能】：战斗开始前，选择其余11条鱼中的任意一条，获得其的被动与主动技能     

【主动技能】：无  */