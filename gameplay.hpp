// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043
// uniqname: hgrobel
//  gameplay.cpp
//  project2
//

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <deque>
#include <queue>
#include <stack>
//#include <priority_queue>
#include "xcode_redirect.hpp"
#include "P2random.h"

using namespace std;

bool stats = false;
bool verbose = false;
bool median = false;
int num = 0;


class Game {
    
private:
    
    uint32_t curr_quiver;
    uint32_t quiver_capacity;
    uint32_t random_seed;
    uint32_t max_rand_distance;
    uint32_t max_rand_speed;
    uint32_t max_rand_health;

    int round = 0;
    bool dead = false;
    bool initialized_random = false;
   
    
    struct zombie{
        
        ~zombie(){};
        
        string name = "";
        uint32_t distance = 0;
        uint32_t speed = 0; //base speed
        uint32_t health = 0;
        int active_rounds = 1;
        bool active = false;
      
    };
   
    
    struct compare {
        
        bool operator ()(const zombie *z1, const zombie *z2) {

            
            uint32_t eta1 = (static_cast<uint32_t>(z1->distance) / static_cast<uint32_t>(z1->speed));
            uint32_t eta2 = (static_cast<uint32_t>(z2->distance) / static_cast<uint32_t>(z2->speed));
            
            if(eta1 != eta2) {
                return eta1 > eta2;
            }
            else if(z1->health != z2->health) {
                return z1->health > z2->health;
            }
            else {
                return z1->name > z2->name;
            }
        }
        
    };
    
    struct compare_lifetimes_min {
        bool operator ()(const zombie *z1, const zombie *z2) {
            
            if(z1->active_rounds != z2->active_rounds) {
                return z2->active_rounds < z1->active_rounds;
            }
            else {
                return z2->name < z1->name;
            }
            
        }
        
    };
    
    struct compare_lifetimes_max {
        bool operator ()(const zombie *z1, const zombie *z2) {
            
            
            if(z1->active_rounds != z2->active_rounds) {
                return z2->active_rounds > z1->active_rounds;
                
            }
            else {
                return z1->name > z2->name;
            }
            
            

        }
    };
    
    vector<zombie*> zombie_masterlist;
    priority_queue<zombie*, vector<zombie *>, compare> zombie_pq_living;
    priority_queue<zombie*, vector<zombie *>, compare_lifetimes_min> zomb_lives_min;
    priority_queue<zombie*, vector<zombie *>, compare_lifetimes_max> zomb_lives_max;
    vector<zombie*> killed_zombies;
//    priority_queue<int> lifetimes;
    zombie killer_zombie;
    priority_queue<int,vector<int>,less<int>> lower; // max heap to store the smaller half elements
    priority_queue<int,vector<int>,greater<int>> upper; // min heap to store the greater half elements
    string finalzombie;

    
public:
    
    bool game_won = false;
    
    void readHeader() {
 
        std::string line;
//        int linecount = 0;
        vector<string> input;
        getline(cin, line);
        cin >> line >> quiver_capacity;
        cin >> line >> random_seed;
        cin >> line >> max_rand_distance;
        cin >> line >> max_rand_speed;
        cin >> line >> max_rand_health;
//        cin >> line;

        
    }
    
    void readFile() {
                

        string line;
        int linecount = 0;
        bool is_random_round = false;
        int curr_round = 0;
        bool started = false;
        while(!dead) {
//            if(round > 1 && zombie_pq_living.size() == 0 && is_random_round == false) {
//                break;
//            }

            if(linecount == 0) {
                if(started == false) {
                    cin >> line;
                    cin >> line >> curr_round;
                    started = true;
                }
                if(curr_round != (round + 1)) {
                    is_random_round = true;
                    round++;
                    linecount++;
                    playRound();
                    continue;
                }
                
                round = curr_round;
                playRound(); //steps 1 and 2
                linecount++;
                started = false;
                continue;

            }

            
            //step 3: new zombies appear
            
            else if(linecount == 1 && is_random_round == false) {
                int second;
                cin >> line >> second;
//                string first = line.substr(0, line.find(' '));
//                string second = line.substr(first.length(), line.find(' '));
                linecount++;
                if(initialized_random == false) {
                    P2random::initialize(random_seed, max_rand_distance, max_rand_speed, max_rand_health);
                    initialized_random = true;
                }
                for(int i = 0; i < second; i++) {
                    generateRandom();
                }
                continue;
                
            }
            else if(linecount == 2 && is_random_round == false) {
                int second;
                uint32_t distance, speed, health;
                string name;
                cin >> line >> second;
                for(int i = 0; i < second; i++) {
                    cin >> name >> line >> distance >> line >> speed >> line >> health;
                    makeZombie(name, distance, speed, health);
                }
//                linecount = 0;
            }
            
            //complete steps 4 and 5 before moving onto next round
            //MAYBE UNCOMMENT THIS
            
            
            
            finishRound(); //not sure about this placement
            for(auto i : zombie_masterlist) {
                if(i->health != 0) {
                    i->active_rounds++;
                }
            }
//            for(uint32_t i = 0; i < zombie_masterlist.size(); i++) {
//                if(zombie_masterlist[i]->health != 0) {
//                    zombie_masterlist[i]->active_rounds++;
//                }
//
//            }
            cin >> std::ws;
            if(zombie_pq_living.empty() && !zombie_masterlist.empty() && cin.eof()) {
                break;
            }
            linecount = 0;
            is_random_round = false;
            
        }
        
    }
    
    void playRound() {
        
        
        if(verbose == true) {
            cout << "Round: " << round << '\n';
        }
        //step 1: refill quiver
        curr_quiver = quiver_capacity;
        //step 2: Existing zombies move forward. If reach player, attack
        
//        for(uint32_t i = 0; i < zombie_masterlist.size(); i++) {
        for(auto i : zombie_masterlist) {
//            zombie *z = zombie_masterlist[i];
            zombie *z = i;
            
            if(z->active == true) {
                z->distance = static_cast<uint32_t>(max(static_cast<int>(0), (static_cast<int>(z->distance) - static_cast<int>(z->speed))));
//                z->active_rounds++;
                //maybe uncomment that if issue with active rounds
                if(z->distance <= 0 && dead == false) {
                    dead = true;
                    killer_zombie.name = z->name;
                }
                
                if(verbose == true) {
                    cout << "Moved: " << z->name << " (distance: " << z->distance << ", speed: " << z->speed << ", health: " << z->health << ")" << '\n';
                }
            }
        }
        
        

        
    }
    
    void finishRound() {
        int destroyed = 0;
        //step 4: player shoots zombies
//        while(curr_quiver != 0) {
//        if(!zombie_pq_living.empty()) {
            while(!zombie_pq_living.empty() && curr_quiver != 0) {
                zombie *z = zombie_pq_living.top();
                if(z->health > curr_quiver) {
                    z->health -= curr_quiver;
                    curr_quiver = 0;
                    zombie_pq_living.pop();
                    zombie_pq_living.push(z);
                }
                else {
                    curr_quiver -= z->health;
                    z->health = 0;
                    zombie_pq_living.pop();
                    destroyed++;
                    z->active = false;
                    killed_zombies.push_back(z);
                    finalzombie = z->name;
                    
                    //median
                    if(upper.empty()) {
                        upper.push(z->active_rounds);
                    }
                    if(z->active_rounds >= upper.top()) {
                        upper.push(z->active_rounds);
                    }
                    else {
                        lower.push(z->active_rounds);
                    }
                    if(upper.size() - lower.size() == 2) {
                          lower.push(upper.top());
                          upper.pop();
                        
                    }
                    else if(lower.size() - upper.size() == 2) {
                          upper.push(lower.top());
                          lower.pop();
                    }
                    
                    if(verbose == true) {
                        cout << "Destroyed: " << z->name << " (distance: " << z->distance << ", speed: " << z->speed << ", health: " << z->health << ")" << '\n';
                        
                    }
                }
//
//                if(z->health > 0) {
//
//                }
                
//                else { // if zombie health is zero
//
//                    destroyed++;
//                    z->active = false;
//                    killed_zombies.push_back(z);
//                    finalzombie = z->name;
//
//                    //median
//                    if(upper.empty()) {
//                        upper.push(z->active_rounds);
//                    }
//                    if(z->active_rounds >= upper.top()) {
//                        upper.push(z->active_rounds);
//                    }
//                    else {
//                        lower.push(z->active_rounds);
//                    }
//                    if(upper.size() - lower.size() == 2) {
//                          lower.push(upper.top());
//                          upper.pop();
//
//                    }
//                    else if(lower.size() - upper.size() == 2) {
//                          upper.push(lower.top());
//                          lower.pop();
//                    }
//
//                    if(verbose == true) {
//                        cout << "Destroyed: " << z->name << " (distance: " << z->distance << ", speed: " << z->speed << ", health: " << z->health << ")" << '\n';
//
//                    }
//                }
                
            }
        
        
        //step 5: print stats
        if(median == true && destroyed > 0) {

            cout << "At the end of round " << round <<
            ", the median zombie lifetime is " << runningMedian() << '\n'; //might need to pass 'n'
        }
    }
    
    int runningMedian() {
        
        if(upper.size()==lower.size()) {
            return(upper.top() + lower.top()) / (2);
        }
        else if(upper.size()>lower.size()) {
            return upper.top();
        }
        else {
            return lower.top();
        }
        
    }
    
    
    void createOutput() {
        
        if(dead == true) {
            cout << "DEFEAT IN ROUND " << round << "! "
            << killer_zombie.name << " ate your brains!" << '\n';
        }
        if(dead == false || zombie_pq_living.size() == 0) {
//            cout << "VICTORY IN ROUND " << round << "! "
//            << (zombie_masterlist.back())->name << " was the last zombie." << '\n';
            cout << "VICTORY IN ROUND " << round << "! "
            << finalzombie << " was the last zombie." << '\n';
        }
        if(stats == true) {
            cout << "Zombies still active: " << zombie_pq_living.size() << '\n';
            
            cout << "First zombies killed:" << '\n';
            
            int i = 0;
//                for(uint32_t j = 0; j < killed_zombies.size(); j++) {
//                    if(i < num) {
//                        cout << killed_zombies[j]->name << " " << (i + 1) << '\n';
//                        i++;
//                    }
//                }
            for(auto j : killed_zombies) {
                if(i < num) {
                    cout << j->name << " " << (i + 1) << '\n';
                    i++;
                }
            }

            
            cout << "Last zombies killed:" << '\n';
            i = num;
            int size = static_cast<int>(killed_zombies.size() - 1);
            if(static_cast<int>(size) < num) {
                i = static_cast<int>(size + 1);
            }
//            for(uint32_t j = size; j > -1; j--) {
//                cout << killed_zombies[static_cast<size_t>(j)]->name << " " << i << '\n';
//                i--;
//                if(j == 0) {
//                    break;
//                }
////                if(i == 0) {
////                    break;
////                }
//            }
            while(size >= 0 && i > 0) {
                cout << killed_zombies[static_cast<size_t>(size)]->name << " " << i << '\n';
                i--;
                size--;
            }
            
//            for(uint32_t i = 0; i < zombie_masterlist.size(); i++) {
//                zomb_lives_min.push(zombie_masterlist[i]);
//                zomb_lives_max.push(zombie_masterlist[i]);
//            }
            for(auto i : zombie_masterlist) {
                zomb_lives_min.push(i);
                zomb_lives_max.push(i);
            }
            
        
            cout << "Most active zombies:" << '\n';
            i = 0;
            while(!zomb_lives_max.empty() && i < num) {
                    cout << zomb_lives_max.top()->name << " " << zomb_lives_max.top()->active_rounds << '\n';
                    zomb_lives_max.pop();
                    i++;
            }
            
            
            cout << "Least active zombies:" << '\n';
            i = 0;
            while(!zomb_lives_min.empty() && i != num) {
                    cout << zomb_lives_min.top()->name << " " << zomb_lives_min.top()->active_rounds << '\n';
                    zomb_lives_min.pop();
                    i++;
            }
        }
        
        for(auto& zombie : zombie_masterlist) {
            delete zombie;
        }

    }
    
    void generateRandom() {
        
//        P2random::initialize(random_seed, max_rand_dist, max_rand_sp, max_rand_heal);
        std::string name  = P2random::getNextZombieName();
        uint32_t distance = P2random::getNextZombieDistance();
        uint32_t speed    = P2random::getNextZombieSpeed();
        uint32_t health   = P2random::getNextZombieHealth();
//        zombie_masterlist.resize(1);
        makeZombie(name, distance, speed, health);
        
    }
    
    
    void makeZombie(string n, uint32_t d,
                      uint32_t s, uint32_t h) {
        
//        zombie z;
        //does this need to be zombie *z = new zombie?
        zombie *z = new zombie;

        z->name = n;
        z->distance = d;
        z->speed = s;
        z->health = h;
        z->active = true;
        zombie_masterlist.push_back(z);
        zombie_pq_living.push(z);

        if(verbose == true) {
            cout << "Created: " << z->name << " (distance: " << z->distance << ", speed: " << z->speed << ", health: " << z->health << ")" << '\n';
        }
 
    }
        
    
    
};
