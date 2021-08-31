#include "stdlib.h" // for rand
#include <deque>
#include <vector>
#include <ncurses.h>
#include <time.h>

using namespace std;

class Player
{
public:
    int x, y;
    Player(int width)
    {
        x = width / 2; // initialise Player in the middle of the "map"
        y = 0;
    }
};

class Lane
{
private:
    deque<bool> cars; // Series of "dots": if true there is a car in there, else nothing is there
public:
    Lane(int width)
    {
        for (int i = 0; i < width; i++)
        {
            cars.push_front(false); // no cars in the lane, to start with.
        }
    }
    void Move()
    {
        if (rand() % 10 == 1) // randomly create cars.
            cars.push_front(true);
        else
            cars.push_front(false);

        cars.pop_back(); // remove last car, so the lane stays constant in size.
    }
    bool CarExists(int pos)
    {
        return cars[pos];
    }
};

class Game
{
private:
    bool quit = false;
    int numberOfLanes;
    int width;
    int score = 0;
    Player *player;
    vector<Lane *> map;

    void InitGraphics()
    {
        // Using ncurses lib.
        initscr();            // init curses mode (allocs mem for present window, stdscr)
        noecho();             // inputs by user will not be displayed on screen
        cbreak();             // read character without having to press ENTER (disable line buffering)
        keypad(stdscr, TRUE); // enable arrow keys
        scrollok(stdscr, TRUE);
        nodelay(stdscr, TRUE); // non-blocking getchar for user input
    }
    void TearDownGraphics()
    {
        endwin(); // end curses mode. Failure to do so will result in weird terminal behaviour.
    }

public:
    Game(int w = 20, int h = 10)
    {
        numberOfLanes = h;
        width = w;
        for (int i = 0; i < numberOfLanes; i++)
        {
            map.push_back(new Lane(width));
        }
        player = new Player(width);
        srand(time(NULL));
        InitGraphics();
    }
    void Draw()
    {
        clear(); // clear screen
        for (int i = 0; i < numberOfLanes; i++)
        {
            for (int j = 0; j < width; j++)
            {
                if ((i == 0 || i == numberOfLanes - 1) && (j == 0 || j == width - 1))
                    addch('|');
                if (map[i]->CarExists(j) && i != 0 && i != numberOfLanes - 1)
                    // print car, but ignore first and last lanes so the player
                    // can start and finish on an empty lane.
                    addch('#');
                else if (player->x == j && player->y == i)
                    addch('o'); // print player
                else
                    addch(' ');
            }
            addch('\n');
        }
        attron(A_BOLD);
        printw("Score: %d", score);
        attroff(A_BOLD);
        refresh(); // Actual print on the screen.
        napms(50);
    }
    void Input()
    {
        int current = getch();
        if (current == KEY_DOWN || current == 'j')
            player->y++;
        if (current == KEY_UP || current == 'k')
            player->y--;
        if (current == KEY_LEFT || current == 'h')
            player->x--;
        if (current == KEY_RIGHT || current == 'l')
            player->x++;
        if (current == 'q')
            quit = true;
    }
    void Logic()
    {
        // Run this only for the lanes with cars.
        for (int i = 1; i < numberOfLanes - 1; i++)
        {
            if (rand() % 10 == 1)
                map[i]->Move(); // move lane.
            if (player->y == i && map[i]->CarExists(player->x))
                quit = true; // if there is a hit, stop.
            if (player->y == numberOfLanes - 1)
            {
                score++;
                player->y = 0; // return to the top
            }
        }
    }
    void Run()
    {
        while (!quit)
        {
            Input();
            Draw();
            Logic();
        }

        addstr("\nGAME OVER.");
        refresh();
    }
    void Stop()
    {
        TearDownGraphics();
    }
};

int main()
{

    int width = 50;  // size of lanes
    int height = 15; // 5 lanes
    Game game(width, height);
    game.Run();
    getchar();   // let the user see the end result.
    game.Stop(); // Clean-up of curses.
}