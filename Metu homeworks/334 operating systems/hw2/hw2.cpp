#include "hw2_output.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <array>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>
#define STOP_ORD 3
#define CONT_ORD 2
#define BREAK_ORD 1
#define NO_ORD 0
using namespace std;
struct area
{
    int start_i;
    int start_j;
    int end_i;
    int end_j;
};

struct proper
{
    int id;
    int dim_i;
    int dim_j;
    int pos_coor_priv;
    time_t tm;
    pair<int, int> *place_point;
    int *gathered;
};

struct order
{
    time_t clk_order;
    string order;
};
struct place_to_smoke
{
    int i;
    int j;
    int smoke_count;
    int finished_smoking;
};

struct sneaky
{
    int id;
    time_t clk_smoker;
    int pos_coor_smoke;
    place_to_smoke *smoke_place;
};
sneaky *smoker_arg;

order *orders;
proper *arg;
vector<pair<int, area>> locked;            // locked areas // better to have
vector<pair<int, area>> locked_but_sneaky; // locked areas // better to have
vector<int> p_working;                     // will store ids of privates that are working
vector<int> p_smoking;                     // store the smokers ids
sem_t everybody_go_on;
sem_t everybody_went_on;
sem_t everybody_read;
sem_t order_ready;
int reader_count;
int active_thread_count;
pthread_mutex_t exit_mtx;
timeval start;
int reader = 0;
int **grid;
int order_now;
int order_count;
pthread_mutex_t private_mut;
void del_private(int p_id);
int wait_for_order_or_timeout(timespec until)
{
    sem_timedwait(&order_ready, &until);
    if (order_now != NO_ORD)
    {
        return order_now;
    }
    return NO_ORD;
}
int wait_for_order()
{
    if (!order_now)
    {
        sem_wait(&order_ready);
        return order_now;
    }
    return order_now;
}
timespec from_start(long ms)
{
    timespec t;
    t.tv_sec = start.tv_sec;
    t.tv_nsec = start.tv_usec * 1000;
    t.tv_nsec += ms * 1000000;
    while (t.tv_nsec >= 1000000000L)
    {
        t.tv_nsec -= 1000000000L;
        t.tv_sec++;
    }
    return t;
}
timespec from_now(long us)
{
    timeval now;
    gettimeofday(&now, NULL);
    timespec t;
    t.tv_sec = now.tv_sec;
    t.tv_nsec = now.tv_usec * 1000;
    t.tv_nsec += us * 1000;
    while (t.tv_nsec >= 1000000000L)
    {
        t.tv_nsec -= 1000000000L;
        t.tv_sec++;
    }
    return t;
}
timespec how_much_to_sleep(timespec till){
    timeval now;
    gettimeofday(&now, NULL);
    timespec t;
    t.tv_sec = now.tv_sec;
    t.tv_nsec = now.tv_usec * 1000;
    till.tv_sec -= t.tv_sec;
    till.tv_nsec -= t.tv_nsec;
    while(till.tv_nsec < 0){
        till.tv_nsec += 1000000000L;
        till.tv_sec--;
    }
    return till;
}
bool has_time_passed(timespec wut)
{
    timeval now;
    gettimeofday(&now, NULL);
    return wut.tv_sec < now.tv_sec || (wut.tv_sec == now.tv_sec && wut.tv_nsec < (now.tv_usec * 1000L));
}
int order_handle_working_proper(int x, pthread_mutex_t *m, int id)
{
    if (m)
        pthread_mutex_unlock(m);
    sem_post(&everybody_read);
    sem_wait(&everybody_go_on);
    if (x == STOP_ORD)
    {
        hw2_notify(PROPER_PRIVATE_STOPPED, id, 0, 0);
        sem_post(&everybody_went_on);
        pthread_exit(NULL);
    }
    if (x == BREAK_ORD)
    {
        del_private(id);
        hw2_notify(PROPER_PRIVATE_TOOK_BREAK, id, 0, 0);
        sem_post(&everybody_went_on);
        return 1;
    }
    if(x == CONT_ORD){
        sem_post(&everybody_went_on);
        return 0;
    }
}
int order_handle_idle_proper(int x, pthread_mutex_t *m, int id)
{
    if (m)
        pthread_mutex_unlock(m);
    sem_post(&everybody_read);
    sem_wait(&everybody_go_on);
    if (x == STOP_ORD)
    {
        hw2_notify(PROPER_PRIVATE_STOPPED, id, 0, 0);
        sem_post(&everybody_went_on);
        pthread_exit(NULL);
    }
    if (x == BREAK_ORD)
    {
        hw2_notify(PROPER_PRIVATE_TOOK_BREAK, id, 0, 0);
        sem_post(&everybody_went_on);
        return 1;
    }
    if(x == CONT_ORD){
        sem_post(&everybody_went_on);
        return 0;
    }

}
int order_handle_sneaky(int x, pthread_mutex_t *m, int id)
{
    if (x == STOP_ORD && m)
        pthread_mutex_unlock(m);
    sem_post(&everybody_read);
    sem_wait(&everybody_go_on);
    if (x == STOP_ORD)
    {
        hw2_notify(SNEAKY_SMOKER_STOPPED, id, 0, 0);
        sem_post(&everybody_went_on);
        pthread_exit(NULL);
    }
    sem_post(&everybody_went_on);
}
void in_break(int gid)
{
    int order_ = NO_ORD;
    while (!order_)
    {
        order_ = wait_for_order();
        if (order_ == BREAK_ORD)
        {
            order_ = NO_ORD;
            sem_post(&everybody_read);
            sem_wait(&everybody_go_on);
            sem_post(&everybody_went_on);
        }
    }
    if (order_ == CONT_ORD)
    {
        sem_post(&everybody_read);
        sem_wait(&everybody_go_on);
        hw2_notify(PROPER_PRIVATE_CONTINUED, gid, 0, 0);
        sem_post(&everybody_went_on);
        return;
    }
    if (order_ == STOP_ORD)
    {
        sem_post(&everybody_read);
        sem_wait(&everybody_go_on);
        hw2_notify(PROPER_PRIVATE_STOPPED, gid, 0, 0);
        sem_post(&everybody_went_on);
        pthread_exit(NULL);
    }
}
bool is_working(int p_id)
{
    for (int i = 0; i < p_working.size(); i++)
    {
        if (p_working[i] == p_id)
        {
            return true;
        }
    }
    return false;
}
void del_private(int p_id)
{
    vector<int>::iterator iter;
    vector<pair<int, area>>::iterator iter1;
    for (iter = p_working.begin(); iter != p_working.end(); iter++)
    {
        if (*iter == p_id)
        {
            p_working.erase(iter);
            break;
        }
    }
    for (iter1 = locked.begin(); iter1 != locked.end(); iter1++)
    {
        if (iter1->first == p_id)
        {
            locked.erase(iter1);
            break;
        }
    }
}

bool is_smoking(int p_id)
{
    for (int i = 0; i < p_smoking.size(); i++)
    {
        if (p_smoking[i] == p_id)
        {
            return true;
        }
    }
    return false;
}
void del_smoker(int p_id)
{
    vector<int>::iterator iter;
    vector<pair<int, area>>::iterator iter1;
    for (iter = p_smoking.begin(); iter != p_smoking.end(); iter++)
    {
        if (*iter == p_id)
        {
            p_smoking.erase(iter);
            break;
        }
    }
    for (iter1 = locked_but_sneaky.begin(); iter1 != locked_but_sneaky.end(); iter1++)
    {
        if (iter1->first == p_id)
        {
            locked_but_sneaky.erase(iter1);
            break;
        }
    }
}

void *p_up_ciga(void *argu)
{

    int x, y; // this private's coordinates and i is for determining which starting point to use
    pair<int, area> a;
    proper *argv = (proper *)argu; // the coordinates
    int order_recieved;
    hw2_notify(PROPER_PRIVATE_CREATED, argv->id, 0, 0);
    for (int ass = 0; ass < argv->pos_coor_priv; ass++)
    {
    L2:
        timespec till_then = from_now(100);

        if (order_recieved = wait_for_order_or_timeout(till_then))
        {
            order_handle_idle_proper(order_recieved, NULL, argv->id);
            if (order_recieved == BREAK_ORD)
            {
                in_break(argv->id);
            }
        }
        while (is_working(argv->id) == false)
        { // check if private is working or waiting
            // mutex here

            vector<pair<int, area>>::iterator iter;
            vector<pair<int, area>>::iterator iter2;
            int can_work = 1;
            timespec till_then = from_now(100);

            if (order_recieved = wait_for_order_or_timeout(till_then))
            {
                order_handle_idle_proper(order_recieved, NULL, argv->id);
                if (order_recieved == BREAK_ORD)
                {
                    in_break(argv->id);
                    goto L2;
                }
            }
            if (pthread_mutex_trylock(&private_mut))
            {
                usleep(1000);
                continue;
            }
            till_then = from_now(100);
            if (order_recieved = wait_for_order_or_timeout(till_then))
            {
                order_handle_idle_proper(order_recieved, &private_mut, argv->id);
                if (order_recieved == BREAK_ORD)
                {
                    in_break(argv->id);
                    goto L2;
                }
            }
            // use iterator with for loop
            if (locked.size())
            {
                for (iter = locked.begin(); iter != locked.end(); iter++)
                {
                    till_then = from_now(100);
                    if (order_recieved = wait_for_order_or_timeout(till_then))
                    {
                        order_handle_idle_proper(order_recieved, &private_mut, argv->id);
                        if (order_recieved == BREAK_ORD)
                        {
                            in_break(argv->id);
                            goto L2;
                        }
                    }
                    if (!((argv->place_point[ass].first > iter->second.end_i) || (argv->place_point[ass].first + argv->dim_i - 1 < iter->second.start_i) ||
                          (argv->place_point[ass].second > iter->second.end_j) || (argv->place_point[ass].second + argv->dim_j - 1 < iter->second.start_j)))
                    // this checks if the place this possible area for private is outside of locked area
                    // and we do this for every locked area, if even one doesn't pass, this private can't work
                    {
                        can_work = 0;
                        break;
                    } // for this possible place for private we can't lock any areas, check other possible places
                }
            }
            if (can_work == 1 && locked_but_sneaky.size())
            {
                for (iter2 = locked_but_sneaky.begin(); iter2 != locked_but_sneaky.end(); iter2++)
                {
                    till_then = from_now(100);
                    if (order_recieved = wait_for_order_or_timeout(till_then))
                    {
                        order_handle_idle_proper(order_recieved, &private_mut, argv->id);
                        if (order_recieved == BREAK_ORD)
                        {
                            in_break(argv->id);
                            goto L2;
                        }
                    }
                    if (!((argv->place_point[ass].first > iter2->second.end_i) || (argv->place_point[ass].first + argv->dim_i < iter2->second.start_i) ||
                          (argv->place_point[ass].second > iter2->second.end_j) || (argv->place_point[ass].second + argv->dim_j < iter2->second.start_j)))
                    // this checks if the place this possible area for private is outside of locked area
                    // and we do this for every locked area, if even one doesn't pass, this private can't work
                    {
                        can_work = 0;
                        break;
                    } // for this possible place for private we can't lock any areas, check other possible places
                }
            }
            pthread_mutex_unlock(&private_mut);
            if (can_work == 1)
            {
                till_then = from_now(100);
                if (order_recieved = wait_for_order_or_timeout(till_then))
                {
                    order_handle_idle_proper(order_recieved, NULL, argv->id);
                    if (order_recieved == BREAK_ORD)
                    {
                        in_break(argv->id);
                        goto L2;
                    }
                }
                a.first = argv->id;
                a.second = {argv->place_point[ass].first, argv->place_point[ass].second, argv->place_point[ass].first + argv->dim_i - 1,
                            argv->place_point[ass].second + argv->dim_j - 1};
                p_working.push_back(argv->id);
                locked.push_back(a);
                x = a.second.start_j;
                y = a.second.start_i;
                hw2_notify(PROPER_PRIVATE_ARRIVED, argv->id, y, x);
                break;
            }
        }
        while (is_working(argv->id) == true)
        {
            // is working
            timespec till_then = from_now(100);
            if (order_recieved = wait_for_order_or_timeout(till_then))
            {
                order_handle_working_proper(order_recieved, NULL, argv->id);
                if (order_recieved == BREAK_ORD)
                {
                    in_break(argv->id);
                    goto L2;
                }
            }
            for (int i = a.second.start_i; i <= a.second.end_i; i++)
            {
                for (int j = a.second.start_j; j <= a.second.end_j; j++)
                {
                    timespec till_then = from_now(100);
                    if (order_recieved = wait_for_order_or_timeout(till_then))
                    {
                        order_handle_working_proper(order_recieved, NULL, argv->id);
                        if (order_recieved == BREAK_ORD)
                        {
                            in_break(argv->id);
                            goto L2;
                        }
                    }
                    while (grid[i][j] >= 1)
                    { // pick up cigarette
                        timespec till_then = from_now(argv->tm * 1000);
                        while (!has_time_passed(till_then))
                        {
                            if (order_recieved = wait_for_order_or_timeout(till_then))
                            {
                                order_handle_working_proper(order_recieved, NULL, argv->id);
                                if (order_recieved == BREAK_ORD)
                                {
                                    in_break(argv->id);
                                    goto L2;
                                }
                            }
                        }
                        grid[i][j] -= 1; // call gathered
                        hw2_notify(PROPER_PRIVATE_GATHERED, argv->id, i, j);
                    }
                }
            }
            del_private(argv->id);
            hw2_notify(PROPER_PRIVATE_CLEARED, argv->id, a.second.start_i, a.second.start_j);
            till_then = from_now(100);
            if (order_recieved = wait_for_order_or_timeout(till_then))
            {
                order_handle_idle_proper(order_recieved, NULL, argv->id);
                if (order_recieved == BREAK_ORD)
                {
                    in_break(argv->id);
                    goto L2;
                }
            }
        }
    }

    hw2_notify(PROPER_PRIVATE_EXITED, argv->id, a.second.start_i, a.second.start_j);

    pthread_mutex_lock(&exit_mtx);
    active_thread_count--;
    if (order_now)
    {
        sem_wait(&order_ready);
        sem_post(&everybody_read);
        sem_wait(&everybody_go_on);
        sem_post(&everybody_went_on);
    }
    pthread_mutex_unlock(&exit_mtx);
    pthread_exit(NULL);
}

void *litter(void *argu)
{
    int x, y; // this private's coordinates and i is for determining which starting point to use

    pair<int, area> a;
    sneaky *argv = (sneaky *)argu; // the coordinates
    hw2_notify(SNEAKY_SMOKER_CREATED, argv->id, 0, 0);
    int count_smoke_place = 0;
    int order_recieved;
    for (int butts = 0; butts < argv->pos_coor_smoke; butts++)
    {
        timespec till_then = from_now(100);

        if (order_recieved = wait_for_order_or_timeout(till_then))
        {
            order_handle_sneaky(order_recieved, NULL, argv->id);
        }
        while (is_smoking(argv->id) == false)
        { // check if private is working or waiting
            // mutex here
            till_then = from_now(100);

            if (order_recieved = wait_for_order_or_timeout(till_then))
            {
                order_handle_sneaky(order_recieved, NULL, argv->id);
            }
            if (pthread_mutex_trylock(&private_mut))
            {
                usleep(1000);
                continue;
            }
            till_then = from_now(100);

            if (order_recieved = wait_for_order_or_timeout(till_then))
            {
                order_handle_sneaky(order_recieved, &private_mut, argv->id);
            }
            vector<pair<int, area>>::iterator iter;
            vector<pair<int, area>>::iterator iter2;
            int can_smoke = 1;
            // use iterator with for loop
            if (locked.size())
            {
                for (iter = locked.begin(); iter != locked.end(); iter++)
                {
                    till_then = from_now(100);

                    if (order_recieved = wait_for_order_or_timeout(till_then))
                    {
                        order_handle_sneaky(order_recieved, &private_mut, argv->id);
                    }
                    if (!((((argv->smoke_place[butts].i) - 1) > iter->second.end_i) || (((argv->smoke_place[butts].i) + 1) < iter->second.start_i) ||
                          (((argv->smoke_place[butts].j) - 1) > iter->second.end_j) || (((argv->smoke_place[butts].j) + 1) < iter->second.start_j)))
                    // this checks if the place this possible area for private is outside of locked area
                    // and we do this for every locked area, if even one doesn't pass, this private can't work
                    {
                        can_smoke = 0;
                        break;
                    } // for this possible place for private we can't lock any areas, check other possible places
                }
            }
            if (can_smoke == 1 && locked_but_sneaky.size())
            {

                for (iter2 = locked_but_sneaky.begin(); iter2 != locked_but_sneaky.end(); iter2++)
                {
                    till_then = from_now(100);

                    if (order_recieved = wait_for_order_or_timeout(till_then))
                    {
                        order_handle_sneaky(order_recieved, &private_mut, argv->id);
                    }
                    if (iter2->second.start_i + 1 == argv->smoke_place[butts].i && iter2->second.start_j + 1 == argv->smoke_place[butts].j)
                    {
                        can_smoke = 0;
                        break;
                    }
                }
            }
            if (can_smoke == 1)
            {
                till_then = from_now(100);

                if (order_recieved = wait_for_order_or_timeout(till_then))
                {
                    order_handle_sneaky(order_recieved, &private_mut, argv->id);
                }
                a.first = argv->id;
                a.second = {argv->smoke_place[butts].i - 1, argv->smoke_place[butts].j - 1, argv->smoke_place[butts].i + 1,
                            argv->smoke_place[butts].j + 1};
                p_smoking.push_back(argv->id);
                locked_but_sneaky.push_back(a);
                x = a.second.start_j;
                y = a.second.start_i;
                hw2_notify(SNEAKY_SMOKER_ARRIVED, argv->id, y + 1, x + 1);
            }
            // end mutex
            pthread_mutex_unlock(&private_mut);
            till_then = from_now(100);

            if (order_recieved = wait_for_order_or_timeout(till_then))
            {
                order_handle_sneaky(order_recieved, NULL, argv->id);
            }
        }
        int smoke_cap = argv->smoke_place[butts].smoke_count;
        int count = 0;
        while (is_smoking(argv->id) == true)
        {
            // is smoking
            // sleep
            timespec till_then = from_now(argv->clk_smoker * 1000);
            while (!has_time_passed(till_then))
            {
                if (order_recieved = wait_for_order_or_timeout(till_then))
                {
                    order_handle_sneaky(order_recieved, NULL, argv->id);
                }
            }
            switch (count)
            {
            case 0:
                grid[y][x] += 1;
                hw2_notify(SNEAKY_SMOKER_FLICKED, argv->id, y, x);
                smoke_cap--;
                break;
            case 1:
                grid[y][x + 1] += 1;
                hw2_notify(SNEAKY_SMOKER_FLICKED, argv->id, y, x + 1);
                smoke_cap--;
                break;
            case 2:
                grid[y][x + 2] += 1;
                hw2_notify(SNEAKY_SMOKER_FLICKED, argv->id, y, x + 2);
                smoke_cap--;
                break;
            case 3:
                grid[y + 1][x + 2] += 1;
                hw2_notify(SNEAKY_SMOKER_FLICKED, argv->id, y + 1, x + 2);
                smoke_cap--;
                break;
            case 4:
                grid[y + 2][x + 2] += 1;
                hw2_notify(SNEAKY_SMOKER_FLICKED, argv->id, y + 2, x + 2);
                smoke_cap--;
                break;
            case 5:
                grid[y + 2][x + 1] += 1;
                hw2_notify(SNEAKY_SMOKER_FLICKED, argv->id, y + 2, x + 1);
                smoke_cap--;
                break;
            case 6:
                grid[y + 2][x] += 1;
                hw2_notify(SNEAKY_SMOKER_FLICKED, argv->id, y + 2, x);
                smoke_cap--;
                break;
            case 7:
                grid[y + 1][x] += 1;
                hw2_notify(SNEAKY_SMOKER_FLICKED, argv->id, y + 1, x);
                smoke_cap--;
                break;
            }
            count++;
            count = count % 8;
            if (smoke_cap == 0)
            {
                del_smoker(argv->id);
                hw2_notify(SNEAKY_SMOKER_LEFT, argv->id, 0, 0);
                till_then = from_now(100);

                if (order_recieved = wait_for_order_or_timeout(till_then))
                {
                    order_handle_sneaky(order_recieved, NULL, argv->id);
                }
                break;
            }
        }
    }
    hw2_notify(SNEAKY_SMOKER_EXITED, argv->id, 0, 0);
    pthread_mutex_lock(&exit_mtx);
    active_thread_count--;
    if (order_now)
    {
        sem_wait(&order_ready);
        sem_post(&everybody_read);
        sem_wait(&everybody_go_on);
        sem_post(&everybody_went_on);
    }
    pthread_mutex_unlock(&exit_mtx);
    pthread_exit(NULL);
}

void *execute_orders(void *commands)
{
    order *argv = (order *)commands;
    order_now = NO_ORD;
    reader_count = 0;
    for (int i = 0; i < order_count; i++)
    {
        // wait till order time comes
        timespec order_time = from_start(argv[i].clk_order);
        if(!has_time_passed(order_time)){
            timespec waittime = how_much_to_sleep(order_time);
            nanosleep(&waittime,NULL); 
        }
        // order time
        pthread_mutex_lock(&exit_mtx);
        if (!argv[i].order.compare("break"))
        {
            order_now = BREAK_ORD;
        }
        else if (!argv[i].order.compare("continue"))
        {
            order_now = CONT_ORD;
        }
        else
        {
            order_now = STOP_ORD;
        }
        reader_count = active_thread_count;
        pthread_mutex_unlock(&exit_mtx);
        for (int j = 0; j < reader_count; j++)
        {
            sem_post(&order_ready);
        }
        for (int j = 0; j < reader_count; j++)
        {
            sem_wait(&everybody_read);
        }
        switch (order_now)
        {
        case STOP_ORD:
            hw2_notify(ORDER_STOP, 0, 0, 0);
            pthread_mutex_lock(&exit_mtx);
            active_thread_count = 0;
            pthread_mutex_unlock(&exit_mtx);
            break;

        case CONT_ORD:
            hw2_notify(ORDER_CONTINUE, 0, 0, 0);
            break;
        case BREAK_ORD:
            hw2_notify(ORDER_BREAK, 0, 0, 0);
            break;
        }
        order_now = NO_ORD;
        for (int j = 0; j < reader_count; j++)
        {
            sem_post(&everybody_go_on);
        }
        for (int j = 0; j < reader_count; j++)
        {
            sem_wait(&everybody_went_on);
        }
    }

    pthread_exit(NULL);
}

int main()
{
    int private_count, grid_size_x, grid_size_y;

    pthread_mutex_init(&private_mut, NULL);
    sem_init(&everybody_go_on,0,0);
    sem_init(&everybody_went_on,0,0);
    sem_init(&everybody_read,0,0);
    sem_init(&order_ready,0,0);
    //////////////////// init grid
    cin >> grid_size_y >> grid_size_x;
    grid = new int *[grid_size_y];
    for (int i = 0; i < grid_size_y; i++)
    { // take cigarettes
        grid[i] = new int[grid_size_x];
        for (int j = 0; j < grid_size_x; j++)
        {
            cin >> grid[i][j];
        }
    }

    //////////////
    //////////////
    //////////////INIT PRIVATE

    private_count = 0;
    cin >> private_count; // private count
    active_thread_count = private_count;
    pthread_t privates[private_count];
    arg = new proper[private_count];

    for (int i = 0; i < private_count; i++)
    { // create threads for privates

        //      id              i               j           time            num of places (possible coordinates of private)
        cin >> arg[i].id >> arg[i].dim_i >> arg[i].dim_j >> arg[i].tm >> arg[i].pos_coor_priv;

        arg[i].place_point = new pair<int, int>[arg[i].pos_coor_priv];
        arg[i].gathered = new int[arg[i].pos_coor_priv];
        for (int p = 0; p < arg[i].pos_coor_priv; p++)
        { // take possible starting points
            //////      i                               j
            cin >> arg[i].place_point[p].first >> arg[i].place_point[p].second; // = possible starting points.                                       //
        }
    }
    //////////////////
    ///////////////// ORDERS
    order_count = 0;
    cin >> order_count;
    orders = new order[order_count];
    pthread_t order_thread;
    for (int i = 0; i < order_count; i++)
    {
        cin >> orders[i].clk_order >> orders[i].order;
    }

    ////////////////////
    //////////////////// SNEAKY GUYS

    int smoker_guy_count = 0;
    cin >> smoker_guy_count;
    active_thread_count += smoker_guy_count;
    pthread_t smokers_threads[smoker_guy_count];

    smoker_arg = new sneaky[smoker_guy_count];

    for (int i = 0; i < smoker_guy_count; i++)
    {

        cin >> smoker_arg[i].id >> smoker_arg[i].clk_smoker >> smoker_arg[i].pos_coor_smoke;

        smoker_arg[i].smoke_place = new place_to_smoke[smoker_arg[i].pos_coor_smoke];
        for (int p = 0; p < smoker_arg[i].pos_coor_smoke; p++)
        { // take possible starting points
            cin >> smoker_arg[i].smoke_place[p].i >> smoker_arg[i].smoke_place[p].j >> smoker_arg[i].smoke_place[p].smoke_count;
            // = possible starting points.
        }
    }
    hw2_init_notifier();
    gettimeofday(&start,NULL);
    if (pthread_create(&order_thread, NULL, &execute_orders, orders) != 0)
    {
        perror("order not created");
        return 1;
    }
    for (int i = 0; i < private_count; i++)
    {
        if (pthread_create(privates + i, NULL, &p_up_ciga, &arg[i]) != 0)
        {
            perror("private not created");
            return 1;
        }
    }
    for (int i = 0; i < smoker_guy_count; i++)
    {
        if (pthread_create(smokers_threads + i, NULL, &litter, &smoker_arg[i]) != 0)
        {
            perror("smoker not created");
            return 1;
        }
    }

    for (int i = 0; i < private_count; i++)
    {
        pthread_join(privates[i], NULL);
    }
    pthread_join(order_thread, NULL);
    for (int i = 0; i < smoker_guy_count; i++)
    {
        pthread_join(smokers_threads[i], NULL);
    }

    pthread_mutex_destroy(&private_mut);
    //////////////join somewhere here
    return 0;
}
